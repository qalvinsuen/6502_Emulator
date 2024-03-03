#include <stdio.h>
#include <stdlib.h>
#include "instructions.h"
// #include "cycles.h"

/*
    http://www.6502.org/users/obelisk/6502/architecture.html
    64Kb of Memory (8192 Bytes)
    16-bit Address Bus
    Little Endian
    256-Byte Page Size
    [$0000 - $00FF] Page Zero (RAM)
        Cache, for frequently referenced variables and uses special 1-byte addresses
    [$0100 - $01FF] Page One (RAM)
        System Stack
    [$0200 - $FFF9] User Programmable Memory (RAM/ROM)
    [$FFFA - $FFFF] Other Reserved (ROM)
        [$FFFA - $FFFB] Non-Maskable Interrupt Handler
        [$FFFC - $FFFD] Power-On Reset
        [$FFFE - $FFFF] Interrupt (BRK) Request Handler
    Note: The first two pages must be in RAM (holds program cache and stack) and the last page must be in ROM (holds reset)
*/

#define MAX_MEM (1024 * 64)

typedef struct Mem
{
    uint8_t Data[MAX_MEM];
    void Init(){
        for (int i; i < MAX_MEM; i++) Data[i] = 0;
    }
} MEM_6502;


typedef struct ALU
{
    
} ALU_6502;


typedef struct CPU
{   
    /*
        http://www.6502.org/users/obelisk/6502/registers.html
    */
    
    /*Special Registers*/
    uint16_t PC; //Program Counter
    uint8_t SP; //Stack Pointer
    uint8_t A; //Accumulator
    uint8_t X; //Index Register
    uint8_t Y; //Index Register
    
    /*Processor Status Flags*/
    uint8_t C : 1; //Carry
    uint8_t Z : 1; //Zero
    uint8_t I : 1; //Interrupt Disable
    uint8_t D : 1; //Decimal
    uint8_t B : 1; //Break
    uint8_t O : 1; //Overflow
    uint8_t N : 1; //Negative
    
    /*Address Bus Register */
    uint32_t ABR;
    
    /*For Testing*/
    int CycleCount = 0;
    
    void Reset(MEM_6502& mem){
        // [$FFFC - $FFFD] holds the power-on reset
        PC = 0xFFFC;
        // [$0100 - $01FF] holds the system stack, only need 8-bit address within the stack
        SP = 0x00;
        // Set registers to 0
        A = X = Y = 0;
        ABR = 0;
        // Set status flags to zero
        C = I = D = B = O = N = Z = 0;
        mem.Init();
    }
    
    /*
        http://www.6502.org/users/obelisk/6502/instructions.html
        Instruction Cycle for One Operation (Fetch, Decode, Execute, Store)
        Fetch
        - Using address from program counter (PC), fetch data from memory address
        Decode and Execute
        - Use enum defined in "instructions.h"
        - Execute behaviour defined in switch case statement
    */
    
    /*
        Fetch
            Use when fetching data (instruction and operand) from memory using the address in PC
    */
    uint8_t FetchByte(MEM_6502& mem){
        uint8_t data = mem.Data[PC];
        PC++;
        CycleCount++;
        return data;
    }
    /*
        Read
            Use when reading data from memory using address given as operand or read from memory
    */
    uint8_t ReadByte(MEM_6502& mem, uint16_t addr){
        uint8_t data = mem.Data[addr];
        CycleCount++;
        return data;
    }
    void SetFlagsLDA(){
        N = (A & 0b10000000) >> 7;
        Z = (A == 0);
    }
    void Execute(MEM_6502& mem){
        //FETCH
        Instruction INS = (Instruction) FetchByte(mem);
        //DECODE and EXECUTE
        switch (INS){
            case LDA_IM:
                // Load Accumulator
                A = FetchByte(mem);
                SetFlagsLDA();
                break;
            case LDA_ZP:
                ABR = 0x00FF & FetchByte(mem); //Clear ABR and store LSB
                A = ReadByte(mem, ABR);
                SetFlagsLDA();
                break;
            case LDA_AB:
                ABR = 0xFFFF & FetchByte(mem); //Clear ABR and store LSB
                ABR = ABR | (FetchByte(mem) << 8); //Store MSB
                A = ReadByte(mem, ABR);
                SetFlagsLDA();
                break;
            default:
                printf("Instruction Not Found\n");
                break;
        }
    }
    
} CPU_6502;

int main(){
    MEM_6502 mem;
    CPU_6502 cpu;
    cpu.Reset(mem);
    /*
        Load Example
        - Store value 0x3E at 8-bit address 0x08 (zero page address)
        - Store LDA_ZP instruction and 8-bit address 0x08 at power-on reset address
        - Use LDA_ZP to load the value from the zero page address into the accumulator
        - The operation should take 3 machines cycles, one per memory access
            (read opcode byte, read address byte, read byte at address)
    */
    int8_t val = 0x3E;
    int8_t zpaddr = 0x08;
    mem.Data[zpaddr] = val;
    mem.Data[0xFFFC] = (uint8_t) Instruction::LDA_ZP;
    mem.Data[0xFFFD] = zpaddr;
    cpu.Execute(mem);
    printf("The Accumulator value at is 0x%x.\n", cpu.A);
    printf("CPU running for %d machine cycles.\n", cpu.CycleCount);
    return 0;
}