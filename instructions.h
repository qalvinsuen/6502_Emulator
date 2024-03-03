/*
    http://www.6502.org/users/obelisk/6502/instructions.html
    Addressing Modes
    - Immediate     = IM    Provide 8-bit constant in instruction (immediate)
    - Zero Page     = ZP    Provide zero page 8-bit address to value in instruction (indirect)
    - Zero Page, X  = ZPX   Provide zero page 8-bit address to value in instruction and add content of X register to it (indirect)
    - Zero Page, Y  = ZPY   Provide zero page 8-bit address to value in instruction and add content of Y register to it (indirect)
    - Absolute      = AB    Provide 16-bit address to value in instruction (direct)
    - Absolute, X   = ABX   Provide 16-bit address to value in instruction and add content of X register to it (direct)
    - Absolute, Y   = ABY   Provide 16-bit address to value in instruction and add content of Y register to it (direct)
    - Indirect      = IND   Provide 16-bit address to 16-bit address in instruction (indirect)
*/
enum Instruction {
    // Load Operations
    LDA_IM = 0xA9,
    LDA_ZP = 0xA5,
    LDA_ZPX = 0xB5,
    LDA_AB = 0xAD,
    LDX,
    LDY,
    // Store Operations
    STA,
    STX,
    STY,
    // Register Transfers
    TAX,
    TAY,
    TXA,
    TYA,
    // Stack Operations
    TSX,
    TXS,
    PHA,
    PHP,
    PLA,
    PPL,
    // Logical Operations
    AND,
    EOR,
    ORA,
    BIT,
    // Arithmetric Operations
    ADC,
    SBC,
    CMP,
    CPX,
    CPY,
    // Increments and Decrements
    IMC,
    INX,
    INY,
    DEC,
    DEX,
    DEY,
    // Shifts
    ASL,
    LSR,
    ROL,
    ROR,
    // Jumps and Calls
    JMP,
    JSR,
    RTS,
    // Branches
    BCC,
    BCS,
    BEQ,
    BMI,
    BNE,
    BPL,
    BVC,
    BVS,
    // Status Flag Changes
    CLC,
    CLD,
    CLI,
    CLV,
    SEC,
    SED,
    SEI,
    // System Functions
    BRK,
    NOP,
    RTI,
};