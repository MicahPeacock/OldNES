#include "cpu.h"
#include "cpu_opcodes.h"

// Memory access functions
static byte fetch_byte(CPU* cpu, const Memory* memory);
static word fetch_word(CPU* cpu, const Memory* memory);
static byte read_byte(word address, const Memory* memory);
static word read_word(word address, const Memory* memory);
static void write_byte(word address, byte value, Memory* memory);
static void write_word(word address, word value, Memory* memory);
static void push_byte(CPU* cpu, byte value, Memory* memory);
static void push_word(CPU* cpu, word value, Memory* memory);
static byte pull_byte(CPU* cpu, const Memory* memory);
static word pull_word(CPU* cpu, const Memory* memory);

static void set_zn(CPU* cpu, byte reg);
static word sp_to_address(const CPU* cpu);
static byte page_crossed(word addr1, word addr2);

// Address functions
static word immediate(CPU* cpu);
static word zero_page(CPU* cpu, const Memory* memory);
static word zero_page_x(CPU* cpu, const Memory* memory);
static word zero_page_y(CPU* cpu, const Memory* memory);
static word absolute(CPU* cpu, const Memory* memory);
static word absolute_x(CPU* cpu, const Memory* memory);
static word absolute_y(CPU* cpu, const Memory* memory);
static word indirect(CPU* cpu, const Memory* memory);
static word indirect_x(CPU* cpu, const Memory* memory);
static word indirect_y(CPU* cpu, const Memory* memory);

// Operation helper functions
static void load_register(CPU* cpu, word address, byte* reg, const Memory* memory);
static void and(CPU* cpu, word address, const Memory* memory);
static void ora(CPU* cpu, word address, const Memory* memory);
static void eor(CPU* cpu, word address, const Memory* memory);
static void branch_if(CPU* cpu, byte test, byte expected, const Memory* memory);
static void compare(CPU* cpu, byte operand, byte value);
static void adc(CPU* cpu, byte operand);
static void sbc(CPU* cpu, byte operand);
static byte asl(CPU* cpu, byte operand);
static byte lsr(CPU* cpu, byte operand);
static byte rol(CPU* cpu, byte operand);
static byte ror(CPU* cpu, byte operand);
static void push_status(CPU* cpu, Memory* memory);
static void pull_status(CPU* cpu, const Memory* memory);

void reset_cpu(CPU* cpu) {
    cpu->pc = RESET_VECTOR;
    cpu->sp = STACK_RESET;
    cpu->a = cpu->x = cpu->y = 0;
    cpu->status.c = cpu->status.z = cpu->status.d = cpu->status.b = cpu->status.v = cpu->status.n = 0;
    cpu->status.i = 1;
}

byte execute(CPU* cpu, Memory* memory) {
    const byte opcode = fetch_byte(cpu, memory);
    const Instruction instr = INSTRUCTIONS[opcode];

    word address;
    switch (instr.address_mode) {
        case IMM: address = immediate(cpu);           break;
        case ZPG: address = zero_page(cpu, memory);   break;
        case ZPX: address = zero_page_x(cpu, memory); break;
        case ZPY: address = zero_page_y(cpu, memory); break;
        case ABS: address = absolute(cpu, memory);    break;
        case ABX: address = absolute_x(cpu, memory);  break;
        case ABY: address = absolute_y(cpu, memory);  break;
        case IND: address = indirect(cpu, memory);    break;
        case IDX: address = indirect_x(cpu, memory);  break;
        case IDY: address = indirect_y(cpu, memory);  break;
        default:  address = 0; // Ignore
    }

    switch (instr.operation) {
        case ADC: {
            const byte operand = read_byte(address, memory);
            adc(cpu, operand);
            break;
        }
        case AND: {
            and(cpu, address, memory);
            break;
        }
        case ASL: {
            if (instr.address_mode == ACC) {
                cpu->a = asl(cpu, cpu->a);
            } else {
                const byte operand = read_byte(address, memory);
                const byte value = asl(cpu, operand);
                write_byte(address, value, memory);
            }
            break;
        }
        case BCC: {
            branch_if(cpu, cpu->status.c, 0, memory);
            break;
        }
        case BCS: {
            branch_if(cpu, cpu->status.c, 1, memory);
            break;
        }
        case BEQ: {
            branch_if(cpu, cpu->status.z, 1, memory);
            break;
        }
        case BIT: {
            const byte value = read_byte(address, memory);
            cpu->status.z = !(cpu->a & value);
            cpu->status.n = (value & NEGATIVE_BIT) != 0;
            cpu->status.v = (value & OVERFLOW_BIT) != 0;
            break;
        }
        case BMI: {
            branch_if(cpu, cpu->status.n, 1, memory);
            break;
        }
        case BNE: {
            branch_if(cpu, cpu->status.z, 0, memory);
            break;
        }
        case BPL: {
            branch_if(cpu, cpu->status.n, 0, memory);
            break;
        }
        case BRK: {
            push_word(cpu, cpu->pc + 1, memory);
            push_status(cpu, memory);
            cpu->pc = read_word(IRQ_VECTOR, memory);
            cpu->status.b = 1;
            cpu->status.i = 1;
            break;
        }
        case BVC: {
            branch_if(cpu, cpu->status.v, 0, memory);
            break;
        }
        case BVS: {
            branch_if(cpu, cpu->status.v, 1, memory);
            break;
        }
        case CLC: {
            cpu->status.c = 0;
            break;
        }
        case CLD: {
            cpu->status.d = 0;
            break;
        }
        case CLI: {
            cpu->status.i = 0;
            break;
        }
        case CLV: {
            cpu->status.v = 0;
            break;
        }
        case CMP: {
            const byte operand = read_byte(address, memory);
            compare(cpu, operand, cpu->a);
            break;
        }
        case CPX: {
            const byte operand = read_byte(address, memory);
            compare(cpu, operand, cpu->x);
            break;
        }
        case CPY: {
            const byte operand = read_byte(address, memory);
            compare(cpu, operand, cpu->y);
            break;
        }
        case DEC: {
            const byte value = read_byte(address, memory) - 1;
            write_byte(address, value, memory);
            set_zn(cpu, value);
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
        case EOR: {
            eor(cpu, address, memory);
            break;
        }
        case INC: {
            const byte value = read_byte(address, memory) + 1;
            write_byte(address, value, memory);
            set_zn(cpu, value);
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
        case JMP: {
            cpu->pc = address;
            break;
        }
        case JSR: {
            push_word(cpu, cpu->pc - 1, memory);
            cpu->pc = address;
            break;
        }
        case LDA: {
            load_register(cpu, address, &cpu->a, memory);
            break;
        }
        case LDX: {
            load_register(cpu, address, &cpu->x, memory);
            break;
        }
        case LDY: {
            load_register(cpu, address, &cpu->y, memory);
            break;
        }
        case LSR: {
            if (instr.address_mode == ACC) {
                cpu->a = lsr(cpu, cpu->a);
            } else {
                const byte operand = read_byte(address, memory);
                const byte value = lsr(cpu, operand);
                write_byte(address, value, memory);
            }
            break;
        }
        case NOP: {
            break;
        }
        case ORA: {
            ora(cpu, address, memory);
            break;
        }
        case PHA: {
            push_byte(cpu, cpu->a, memory);
            break;
        }
        case PHP: {
            push_status(cpu, memory);
            break;
        }
        case PLA: {
            cpu->a = pull_byte(cpu, memory);
            set_zn(cpu, cpu->a);
            break;
        }
        case PLP: {
            pull_status(cpu, memory);
            break;
        }
        case ROL: {
            if (instr.address_mode == ACC) {
                cpu->a = rol(cpu, cpu->a);
            } else {
                const byte operand = read_byte(address, memory);
                const byte value = rol(cpu, operand);
                write_byte(address, value, memory);
            }
            break;
        }
        case ROR: {
            if (instr.address_mode == ACC) {
                cpu->a = ror(cpu, cpu->a);
            } else {
                const byte operand = read_byte(address, memory);
                const byte value = ror(cpu, operand);
                write_byte(address, value, memory);
            }
            break;
        }
        case RTI: {
            pull_status(cpu, memory);
            cpu->pc = pull_word(cpu, memory);
            break;
        }
        case RTS: {
            cpu->pc = pull_word(cpu, memory) + 1;
            break;
        }
        case SBC: {
            const byte operand = read_byte(address, memory);
            sbc(cpu, operand);
            break;
        }
        case SEC: {
            cpu->status.c = 1;
            break;
        }
        case SED: {
            cpu->status.d = 1;
            break;
        }
        case SEI: {
            cpu->status.i = 1;
            break;
        }
        case STA: {
            write_byte(address, cpu->a, memory);
            break;
        }
        case STX: {
            write_byte(address, cpu->x, memory);
            break;
        }
        case STY: {
            write_byte(address, cpu->y, memory);
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
        case TSX: {
            cpu->x = cpu->sp;
            set_zn(cpu, cpu->x);
            break;
        }
        case TXS: {
            cpu->sp = cpu->x;
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
        case XXX: {
            break;
        }
    }
    return instr.cycles;
}

static byte fetch_byte(CPU* cpu, const Memory* memory) {
    return read_memory(memory, cpu->pc++);
}

static word fetch_word(CPU* cpu, const Memory* memory) {
    const byte lo = fetch_byte(cpu, memory);
    const byte hi = fetch_byte(cpu, memory);
    return lo | (hi << 8);
}

static byte read_byte(word address, const Memory* memory) {
    return read_memory(memory, address);
}

static word read_word(word address, const Memory* memory) {
    const byte lo = read_byte(address, memory);
    const byte hi = read_byte(address + 1, memory);
    return lo | (hi << 8);
}

static void write_byte(word address, byte value, Memory* memory) {
    write_memory(memory, address, value);
}

static void write_word(word address, word value, Memory* memory) {
    write_byte(address, value & 0xff, memory);
    write_byte(address + 1, value >> 8, memory);
}

static void push_byte(CPU* cpu, byte value, Memory* memory) {
    write_byte(sp_to_address(cpu), value, memory);
    cpu->sp--;
}

static void push_word(CPU* cpu, word value, Memory* memory) {
    push_byte(cpu, value >> 8, memory);
    push_byte(cpu, value & 0xff, memory);
}

static byte pull_byte(CPU* cpu, const Memory* memory) {
    cpu->sp++;
    return read_memory(memory, sp_to_address(cpu));
}

static word pull_word(CPU* cpu, const Memory* memory) {
    const word value = read_word(sp_to_address(cpu) + 1, memory);
    cpu->sp += 2;
    return value;
}

static void set_zn(CPU* cpu, byte reg) {
    cpu->status.z = (reg == 0);
    cpu->status.n = (reg & NEGATIVE_BIT) != 0;
}

static word sp_to_address(const CPU* cpu) {
    return STACK_BASE | (word) cpu->sp;
}

static byte page_crossed(word addr1, word addr2) {
    return (addr1 & 0xff00) != (addr2 & 0xff00);
}

static word immediate(CPU* cpu) {
    return cpu->pc++;
}

static word zero_page(CPU* cpu, const Memory* memory) {
    return fetch_byte(cpu, memory);
}

static word zero_page_x(CPU* cpu, const Memory* memory) {
    return fetch_byte(cpu, memory) + cpu->x;
}

static word zero_page_y(CPU* cpu, const Memory* memory) {
    return fetch_byte(cpu, memory) + cpu->y;
}

static word absolute(CPU* cpu, const Memory* memory) {
    return fetch_word(cpu, memory);
}

static word absolute_x(CPU* cpu, const Memory* memory) {
    const word abs_address = fetch_word(cpu, memory);
    const word abs_address_x = fetch_word(cpu, memory) + cpu->x;
    if (page_crossed(abs_address, abs_address_x)) {

    }
    return abs_address_x;
}

static word absolute_y(CPU* cpu, const Memory* memory) {
    const word abs_address = fetch_word(cpu, memory);
    const word abs_address_y = fetch_word(cpu, memory) + cpu->y;
    if (page_crossed(abs_address, abs_address_y)) {

    }
    return abs_address_y;
}

static word indirect(CPU* cpu, const Memory* memory) {
    const word address = absolute(cpu, memory);
    return read_word(address, memory);
}

static word indirect_x(CPU* cpu, const Memory* memory) {
    const word zpg_address = zero_page_x(cpu, memory);
    return read_word(zpg_address, memory);
}

static word indirect_y(CPU* cpu, const Memory* memory) {
    const word zpg_address = zero_page(cpu, memory);
    const word ind_address = read_word(zpg_address, memory);
    const word ind_address_y = ind_address + cpu->y;
    if (page_crossed(ind_address, ind_address_y)) {

    }
    return ind_address_y;
}


static void load_register(CPU* cpu, word address, byte* reg, const Memory* memory) {
    *reg = read_byte(address, memory);
    set_zn(cpu, *reg);
}

static void and(CPU* cpu, word address, const Memory* memory) {
    cpu->a &= read_byte(address, memory);
    set_zn(cpu, cpu->a);
}

static void ora(CPU* cpu, word address, const Memory* memory) {
    cpu->a |= read_byte(address, memory);
    set_zn(cpu, cpu->a);
}

static void eor(CPU* cpu, word address, const Memory* memory) {
    cpu->a ^= read_byte(address, memory);
    set_zn(cpu, cpu->a);
}

static void branch_if(CPU* cpu, byte test, byte expected, const Memory* memory) {
    const sbyte offset = (sbyte) fetch_byte(cpu, memory);
    if (test != expected) return;
    const word old_pc = cpu->pc;
    cpu->pc += offset;
    if (page_crossed(old_pc, cpu->pc)) {

    }
}

static void compare(CPU* cpu, byte operand, byte value) {
    cpu->status.n = ((value - operand) & NEGATIVE_BIT) != 0;
    cpu->status.z = value == operand;
    cpu->status.c = value >= operand;
}

static void adc(CPU* cpu, byte operand) {
    const byte same_signs = ((cpu->a ^ operand) & NEGATIVE_BIT) == 0;
    const word sum = cpu->a + operand + cpu->status.c;
    cpu->a = (byte)(sum & 0xff);
    set_zn(cpu, cpu->a);
    cpu->status.c = sum > 0xff;
    cpu->status.v = same_signs && ((cpu->a ^ operand) & NEGATIVE_BIT);
}

static void sbc(CPU* cpu, byte operand) {
    adc(cpu, ~operand);
}

static byte asl(CPU* cpu, byte operand) {
    cpu->status.c = (operand & NEGATIVE_BIT) != 0;
    const byte result = operand << 1;
    set_zn(cpu, result);
    return result;
}

static byte lsr(CPU* cpu, byte operand) {
    cpu->status.c = (operand & CARRY_BIT) != 0;
    const byte result = operand >> 1;
    set_zn(cpu, result);
    return result;
}

static byte rol(CPU* cpu, byte operand) {
    const byte new_first = cpu->status.c ? CARRY_BIT : 0;
    cpu->status.c = (operand & NEGATIVE_BIT) != 0;
    const byte result = (operand << 1) | new_first;
    set_zn(cpu, result);
    return result;
}

static byte ror(CPU* cpu, byte operand) {
    const byte old_first = (operand & CARRY_BIT) != 0;
    operand >>= 1;
    if(cpu->status.c) {
        operand |= NEGATIVE_BIT;
    }
    cpu->status.c = old_first;
    set_zn(cpu, operand);
    return operand;
}

static void push_status(CPU* cpu, Memory* memory) {
    push_byte(cpu, cpu->status.value | BREAK_BIT | UNUSED_BIT, memory);
}

static void pull_status(CPU* cpu, const Memory* memory) {
    cpu->status.value = pull_byte(cpu, memory);
    cpu->status.b = cpu->status.u = 0;
}