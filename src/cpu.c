#include "cpu.h"
#include "cpu_opcodes.h"
#include "emulator.h"

static byte execute(struct CPU* cpu, const struct Instruction* instr);
static void interrupt(struct CPU* cpu, InterruptType type);

// CPUBus access functions
static byte fetch_byte(struct CPU* cpu);
static word fetch_word(struct CPU* cpu);
static byte read_byte(const struct CPU* cpu, word address);
static word read_word(const struct CPU* cpu, word address);
static void write_byte(struct CPU* cpu, word address, byte value);
static void write_word(struct CPU* cpu, word address, word value);
static void push_byte(struct CPU* cpu, byte value);
static void push_word(struct CPU* cpu, word value);
static byte pull_byte(struct CPU* cpu);
static word pull_word(struct CPU* cpu);

static void set_zn(struct CPU* cpu, byte reg);
static word sp_to_address(const struct CPU* cpu);
static byte page_crossed(word addr1, word addr2);

// Address functions
static word immediate(struct CPU* cpu);
static word zero_page(struct CPU* cpu);
static word zero_page_x(struct CPU* cpu);
static word zero_page_y(struct CPU* cpu);
static word absolute(struct CPU* cpu);
static word absolute_x(struct CPU* cpu);
static word absolute_y(struct CPU* cpu);
static word indirect(struct CPU* cpu);
static word indirect_x(struct CPU* cpu);
static word indirect_y(struct CPU* cpu);

// Operation helper functions
static void load_register(struct CPU* cpu, word address, byte* reg);
static void and(struct CPU* cpu, word address);
static void ora(struct CPU* cpu, word address);
static void eor(struct CPU* cpu, word address);
static void branch_if(struct CPU* cpu, byte test, byte expected);
static void compare(struct CPU* cpu, byte operand, byte value);
static void adc(struct CPU* cpu, byte operand);
static void sbc(struct CPU* cpu, byte operand);
static byte asl(struct CPU* cpu, byte operand);
static byte lsr(struct CPU* cpu, byte operand);
static byte rol(struct CPU* cpu, byte operand);
static byte ror(struct CPU* cpu, byte operand);
static void push_status(struct CPU* cpu);
static void pull_status(struct CPU* cpu);

void init_cpu(struct Emulator* emulator) {
    struct CPU* cpu = &emulator->cpu;
    cpu->bus = &emulator->cpu_bus;
    reset_cpu(&emulator->cpu);
}

void reset_cpu(struct CPU* cpu) {
    cpu->pc = read_word(cpu, RESET_VECTOR);
    cpu->sp = STACK_RESET;
    cpu->a = cpu->x = cpu->y = 0;
    cpu->status.c = cpu->status.z = cpu->status.d = cpu->status.b = cpu->status.v = cpu->status.n = 0;
    cpu->status.i = 1;
}

void execute_cpu(struct CPU* cpu) {
    cpu->cycles++;
    if(cpu->skip_cycles-- > 1)
        return;
    cpu->skip_cycles = 0;

    if(cpu->pending_nmi) {
        interrupt(cpu, NMI);
        cpu->pending_nmi = cpu->pending_irq = 0;
        return;
    }
    if(cpu->pending_irq) {
        interrupt(cpu, IRQ);
        cpu->pending_nmi = cpu->pending_irq = 0;
        return;
    }

    const byte opcode = fetch_byte(cpu);
    const Instruction* instr = &INSTRUCTIONS[opcode];
    cpu->skip_cycles += execute(cpu, instr);
}

void interrupt_cpu(struct CPU* cpu, InterruptType type) {
    switch (type) {
        case IRQ:
            cpu->pending_irq = 1;
            break;
        case NMI:
            cpu->pending_nmi = 1;
            break;
    }
}

static byte execute(struct CPU* cpu, const struct Instruction* instr) {
    word address;
    switch (instr->address_mode) {
        case IMM: address = immediate(cpu);   break;
        case ZPG: address = zero_page(cpu);   break;
        case ZPX: address = zero_page_x(cpu); break;
        case ZPY: address = zero_page_y(cpu); break;
        case ABS: address = absolute(cpu);    break;
        case ABX: address = absolute_x(cpu);  break;
        case ABY: address = absolute_y(cpu);  break;
        case IND: address = indirect(cpu);    break;
        case IDX: address = indirect_x(cpu);  break;
        case IDY: address = indirect_y(cpu);  break;
        default:  address = 0; // Ignore
    }

    switch (instr->operation) {
        case LDA: {
            load_register(cpu, address, &cpu->a);
            break;
        }
        case LDX: {
            load_register(cpu, address, &cpu->x);
            break;
        }
        case LDY: {
            load_register(cpu, address, &cpu->y);
            break;
        }
        case STA: {
            write_byte(cpu, address, cpu->a);
            break;
        }
        case STX: {
            write_byte(cpu, address, cpu->x);
            break;
        }
        case STY: {
            write_byte(cpu, address, cpu->y);
            break;
        }
        case TSX: {
            cpu->x = cpu->sp;
            set_zn(cpu, cpu->x);
            break;
        }
        case TXS: {
            cpu->sp = cpu->x;
            break;
        }
        case PHA: {
            push_byte(cpu, cpu->a);
            break;
        }
        case PLA: {
            cpu->a = pull_byte(cpu);
            set_zn(cpu, cpu->a);
            break;
        }
        case PHP: {
            push_status(cpu);
            break;
        }
        case PLP: {
            pull_status(cpu);
            break;
        }
        case JMP: {
            cpu->pc = address;
            break;
        }
        case JSR: {
            push_word(cpu, cpu->pc - 1);
            cpu->pc = address;
            break;
        }
        case RTS: {
            cpu->pc = pull_word(cpu) + 1;
            break;
        }
        case AND: {
            and(cpu, address);
            break;
        }
        case ORA: {
            ora(cpu, address);
            break;
        }
        case EOR: {
            eor(cpu, address);
            break;
        }
        case BIT: {
            const byte value = read_byte(cpu, address);
            cpu->status.z = !(cpu->a & value);
            cpu->status.n = (value & NEGATIVE_BIT) != 0;
            cpu->status.v = (value & OVERFLOW_BIT) != 0;
            break;
        }
        case TAX: {
            cpu->x = cpu->a;
            set_zn(cpu, cpu->x);
            break;
        }
        case TAY: {
            cpu->y = cpu->a;
            set_zn(cpu, cpu->y);
            break;
        }
        case TXA: {
            cpu->a = cpu->x;
            set_zn(cpu, cpu->a);
            break;
        }
        case TYA: {
            cpu->a = cpu->y;
            set_zn(cpu, cpu->a);
            break;
        }
        case INX: {
            cpu->x++;
            set_zn(cpu, cpu->x);
            break;
        }
        case INY: {
            cpu->y++;
            set_zn(cpu, cpu->y);
            break;
        }
        case DEX: {
            cpu->x--;
            set_zn(cpu, cpu->x);
            break;
        }
        case DEY: {
            cpu->y--;
            set_zn(cpu, cpu->y);
            break;
        }
        case INC: {
            const byte value = read_byte(cpu, address) + 1;
            write_byte(cpu, address, value);
            set_zn(cpu, value);
            break;
        }
        case DEC: {
            const byte value = read_byte(cpu, address) - 1;
            write_byte(cpu, address, value);
            set_zn(cpu, value);
            break;
        }
        case BEQ: {
            branch_if(cpu, cpu->status.z, 1);
            break;
        }
        case BNE: {
            branch_if(cpu, cpu->status.z, 0);
            break;
        }
        case BCC: {
            branch_if(cpu, cpu->status.c, 0);
            break;
        }
        case BCS: {
            branch_if(cpu, cpu->status.c, 1);
            break;
        }
        case BMI: {
            branch_if(cpu, cpu->status.n, 1);
            break;
        }
        case BPL: {
            branch_if(cpu, cpu->status.n, 0);
            break;
        }
        case BVS: {
            branch_if(cpu, cpu->status.v, 1);
            break;
        }
        case BVC: {
            branch_if(cpu, cpu->status.v, 0);
            break;
        }
        case CLC: {
            cpu->status.c = 0;
            break;
        }
        case SEC: {
            cpu->status.c = 1;
            break;
        }
        case CLD: {
            cpu->status.d = 0;
            break;
        }
        case SED: {
            cpu->status.d = 1;
            break;
        }
        case CLI: {
            cpu->status.i = 0;
            break;
        }
        case SEI: {
            cpu->status.i = 1;
            break;
        }
        case CLV: {
            cpu->status.v = 0;
            break;
        }
        case ADC: {
            const byte operand = read_byte(cpu, address);
            adc(cpu, operand);
            break;
        }
        case SBC: {
            const byte operand = read_byte(cpu, address);
            sbc(cpu, operand);
            break;
        }
        case CMP: {
            const byte operand = read_byte(cpu, address);
            compare(cpu, operand, cpu->a);
            break;
        }
        case CPX: {
            const byte operand = read_byte(cpu, address);
            compare(cpu, operand, cpu->x);
            break;
        }
        case CPY: {
            const byte operand = read_byte(cpu, address);
            compare(cpu, operand, cpu->y);
            break;
        }
        case ASL: {
            if (instr->address_mode == ACC) {
                cpu->a = asl(cpu, cpu->a);
            } else {
                const byte operand = read_byte(cpu, address);
                const byte value = asl(cpu, operand);
                write_byte(cpu, address, value);
            }
            break;
        }
        case LSR: {
            if (instr->address_mode == ACC) {
                cpu->a = lsr(cpu, cpu->a);
            } else {
                const byte operand = read_byte(cpu, address);
                const byte value = lsr(cpu, operand);
                write_byte(cpu, address, value);
            }
            break;
        }
        case ROL: {
            if (instr->address_mode == ACC) {
                cpu->a = rol(cpu, cpu->a);
            } else {
                const byte operand = read_byte(cpu, address);
                const byte value = rol(cpu, operand);
                write_byte(cpu, address, value);
            }
            break;
        }
        case ROR: {
            if (instr->address_mode == ACC) {
                cpu->a = ror(cpu, cpu->a);
            } else {
                const byte operand = read_byte(cpu, address);
                const byte value = ror(cpu, operand);
                write_byte(cpu, address, value);
            }
            break;
        }
        case NOP: {
            break;
        }
        case BRK: {
            push_word(cpu, cpu->pc + 1);
            push_status(cpu);
            cpu->pc = read_word(cpu, IRQ_VECTOR);
            cpu->status.b = 1;
            cpu->status.i = 1;
            break;
        }
        case RTI: {
            pull_status(cpu);
            cpu->pc = pull_word(cpu);
            break;
        }
        default: {
            break;
        }
    }
    return instr->cycles;
}

static void interrupt(struct CPU* cpu, InterruptType type) {
    if(cpu->status.i)
        return;
    push_word(cpu, cpu->pc);
    push_status(cpu);
    cpu->status.i = 1;

    switch (type) {
        case IRQ:
            cpu->pc = read_word(cpu, IRQ_VECTOR);
            break;
        case NMI:
            cpu->pc = read_word(cpu, NMI_VECTOR);
            break;
    }
    cpu->skip_cycles += 6;
}

static byte fetch_byte(struct CPU* cpu) {
    return read_memory(cpu->bus, cpu->pc++);
}

static word fetch_word(struct CPU* cpu) {
    const word lo = fetch_byte(cpu);
    const word hi = fetch_byte(cpu);
    return lo | (hi << 8);
}

static byte read_byte(const struct CPU* cpu, word address) {
    return read_memory(cpu->bus, address);
}

static word read_word(const struct CPU* cpu, word address) {
    const word lo = read_byte(cpu, address);
    const word hi = read_byte(cpu, address + 1);
    return lo | (hi << 8);
}

static void write_byte(struct CPU* cpu, word address, byte value) {
    write_memory(cpu->bus, address, value);
}

static void write_word(struct CPU* cpu, word address, word value) {
    write_byte(cpu, address, value & 0xff);
    write_byte(cpu, address + 1, value >> 8);
}

static void push_byte(struct CPU* cpu, byte value) {
    write_byte(cpu, sp_to_address(cpu), value);
    cpu->sp--;
}

static void push_word(struct CPU* cpu, word value) {
    push_byte(cpu, value >> 8);
    push_byte(cpu, value & 0xff);
}

static byte pull_byte(struct CPU* cpu) {
    cpu->sp++;
    return read_memory(cpu->bus, sp_to_address(cpu));
}

static word pull_word(struct CPU* cpu) {
    const word value = read_word(cpu, sp_to_address(cpu) + 1);
    cpu->sp += 2;
    return value;
}

static void set_zn(struct CPU* cpu, byte reg) {
    cpu->status.z = (reg == 0);
    cpu->status.n = (reg & NEGATIVE_BIT) != 0;
}

static word sp_to_address(const struct CPU* cpu) {
    return STACK_BASE | (word)cpu->sp;
}

static byte page_crossed(word addr1, word addr2) {
    return (addr1 & 0xff00) != (addr2 & 0xff00);
}

static word immediate(struct CPU* cpu) {
    return cpu->pc++;
}

static word zero_page(struct CPU* cpu) {
    return fetch_byte(cpu);
}

static word zero_page_x(struct CPU* cpu) {
    return fetch_byte(cpu) + cpu->x;
}

static word zero_page_y(struct CPU* cpu) {
    return fetch_byte(cpu) + cpu->y;
}

static word absolute(struct CPU* cpu) {
    return fetch_word(cpu);
}

static word absolute_x(struct CPU* cpu) {
    const word abs_address = fetch_word(cpu);
    const word abs_address_x = fetch_word(cpu) + cpu->x;
    if (page_crossed(abs_address, abs_address_x)) {
        cpu->skip_cycles++;
    }
    return abs_address_x;
}

static word absolute_y(struct CPU* cpu) {
    const word abs_address = fetch_word(cpu);
    const word abs_address_y = fetch_word(cpu) + cpu->y;
    if (page_crossed(abs_address, abs_address_y)) {
        cpu->skip_cycles++;
    }
    return abs_address_y;
}

static word indirect(struct CPU* cpu) {
    const word address = absolute(cpu);
    return read_word(cpu, address);
}

static word indirect_x(struct CPU* cpu) {
    const word zpg_address = zero_page_x(cpu);
    return read_word(cpu, zpg_address);
}

static word indirect_y(struct CPU* cpu) {
    const word zpg_address = zero_page(cpu);
    const word ind_address = read_word(cpu, zpg_address);
    const word ind_address_y = ind_address + cpu->y;
    if (page_crossed(ind_address, ind_address_y)) {
        cpu->skip_cycles++;
    }
    return ind_address_y;
}


static void load_register(struct CPU* cpu, word address, byte* reg) {
    *reg = read_byte(cpu, address);
    set_zn(cpu, *reg);
}

static void and(struct CPU* cpu, word address) {
    cpu->a &= read_byte(cpu, address);
    set_zn(cpu, cpu->a);
}

static void ora(struct CPU* cpu, word address) {
    cpu->a |= read_byte(cpu, address);
    set_zn(cpu, cpu->a);
}

static void eor(struct CPU* cpu, word address) {
    cpu->a ^= read_byte(cpu, address);
    set_zn(cpu, cpu->a);
}

static void branch_if(struct CPU* cpu, byte test, byte expected) {
    const sbyte offset = (sbyte) fetch_byte(cpu);
    if (test != expected) return;
    const word old_pc = cpu->pc;
    cpu->pc += offset;
    if (page_crossed(old_pc, cpu->pc)) {

    }
}

static void compare(struct CPU* cpu, byte operand, byte value) {
    cpu->status.n = ((value - operand) & NEGATIVE_BIT) != 0;
    cpu->status.z = value == operand;
    cpu->status.c = value >= operand;
}

static void adc(struct CPU* cpu, byte operand) {
    const byte same_signs = ((cpu->a ^ operand) & NEGATIVE_BIT) == 0;
    const word sum = cpu->a + operand + cpu->status.c;
    cpu->a = (byte)(sum & 0xff);
    set_zn(cpu, cpu->a);
    cpu->status.c = sum > 0xff;
    cpu->status.v = same_signs && ((cpu->a ^ operand) & NEGATIVE_BIT);
}

static void sbc(struct CPU* cpu, byte operand) {
    adc(cpu, ~operand);
}

static byte asl(struct CPU* cpu, byte operand) {
    cpu->status.c = (operand & NEGATIVE_BIT) != 0;
    const byte result = operand << 1;
    set_zn(cpu, result);
    return result;
}

static byte lsr(struct CPU* cpu, byte operand) {
    cpu->status.c = (operand & CARRY_BIT) != 0;
    const byte result = operand >> 1;
    set_zn(cpu, result);
    return result;
}

static byte rol(struct CPU* cpu, byte operand) {
    const byte new_first = cpu->status.c ? CARRY_BIT : 0;
    cpu->status.c = (operand & NEGATIVE_BIT) != 0;
    const byte result = (operand << 1) | new_first;
    set_zn(cpu, result);
    return result;
}

static byte ror(struct CPU* cpu, byte operand) {
    const byte old_first = (operand & CARRY_BIT) != 0;
    operand >>= 1;
    if(cpu->status.c) {
        operand |= NEGATIVE_BIT;
    }
    cpu->status.c = old_first;
    set_zn(cpu, operand);
    return operand;
}

static void push_status(struct CPU* cpu) {
    push_byte(cpu, cpu->status.value | BREAK_BIT | UNUSED_BIT);
}

static void pull_status(struct CPU* cpu) {
    cpu->status.value = pull_byte(cpu);
    cpu->status.b = cpu->status.u = 0;
}