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
static word zero_page(CPU* cpu, const Memory* memory);
static word zero_page_x(CPU* cpu, const Memory* memory);
static word zero_page_y(CPU* cpu, const Memory* memory);
static word absolute(CPU* cpu, const Memory* memory);
static word absolute_x(CPU* cpu, const Memory* memory);
static word absolute_y(CPU* cpu, const Memory* memory);
static word indirect_x(CPU* cpu, const Memory* memory);
static word indirect_y(CPU* cpu, const Memory* memory);

// Operation helper functions
static void load_register(CPU* cpu, word address, byte* reg, const Memory* memory);
static void and(CPU* cpu, word address, const Memory* memory);
static void ora(CPU* cpu, word address, const Memory* memory);
static void eor(CPU* cpu, word address, const Memory* memory);
static void branch_if(CPU* cpu, byte test, byte expected);
static void compare(CPU* cpu, byte operand, byte value);
static void adc(CPU* cpu, byte operand, const Memory* memory);
static void sbc(CPU* cpu, byte operand, const Memory* memory);
static void asl(CPU* cpu, byte operand, const Memory* memory);
static void lsr(CPU* cpu, byte operand, const Memory* memory);
static void rol(CPU* cpu, byte operand, const Memory* memory);
static void ror(CPU* cpu, byte operand, const Memory* memory);
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

    switch (instr.operation) {
        case ADC:
            break;
        case AND:
            break;
        case ASL:
            break;
        case BCC:
            break;
        case BCS:
            break;
        case BEQ:
            break;
        case BIT:
            break;
        case BMI:
            break;
        case BNE:
            break;
        case BPL:
            break;
        case BRK:
            break;
        case BVC:
            break;
        case BVS:
            break;
        case CLC:
            break;
        case CLD:
            break;
        case CLI:
            break;
        case CLV:
            break;
        case CMP:
            break;
        case CPX:
            break;
        case CPY:
            break;
        case DEC:
            break;
        case DEX:
            break;
        case DEY:
            break;
        case EOR:
            break;
        case INC:
            break;
        case INX:
            break;
        case INY:
            break;
        case JMP:
            break;
        case JSR:
            break;
        case LDA:
            break;
        case LDX:
            break;
        case LDY:
            break;
        case LSR:
            break;
        case NOP:
            break;
        case ORA:
            break;
        case PHA:
            break;
        case PHP:
            break;
        case PLA:
            break;
        case PLP:
            break;
        case ROL:
            break;
        case ROR:
            break;
        case RTI:
            break;
        case RTS:
            break;
        case SBC:
            break;
        case SEC:
            break;
        case SED:
            break;
        case SEI:
            break;
        case STA:
            break;
        case STX:
            break;
        case STY:
            break;
        case TAX:
            break;
        case TAY:
            break;
        case TSX:
            break;
        case TXS:
            break;
        case TXA:
            break;
        case TYA:
            break;
        case XXX:
            break;
    }
    return instr.cycles;
}

static word sp_to_address(const CPU* cpu) {
    return STACK_BASE | (word) cpu->sp;
}

static byte page_crossed(word addr1, word addr2) {
    return (addr1 & 0xff00) != (addr2 & 0xff00);
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

static word indirect_x(CPU* cpu, const Memory* memory) {
    return 0;
}

static word indirect_y(CPU* cpu, const Memory* memory) {
    return 0;
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

static void branch_if(CPU* cpu, byte test, byte expected) {

}

static void compare(CPU* cpu, byte operand, byte value) {

}

static void adc(CPU* cpu, byte operand, const Memory* memory) {

}

static void sbc(CPU* cpu, byte operand, const Memory* memory) {

}

static void asl(CPU* cpu, byte operand, const Memory* memory) {

}

static void lsr(CPU* cpu, byte operand, const Memory* memory) {

}

static void rol(CPU* cpu, byte operand, const Memory* memory) {

}

static void ror(CPU* cpu, byte operand, const Memory* memory) {

}

static void push_status(CPU* cpu, Memory* memory) {
    push_byte(cpu, cpu->status.value | BREAK_BIT | UNUSED_BIT, memory);
}

static void pull_status(CPU* cpu, const Memory* memory) {
    cpu->status.value = pull_byte(cpu, memory);
    cpu->status.b = cpu->status.u = 0;
}