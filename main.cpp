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
        Cache, for frequently referenced variables and uses 8-bit instead of 16-bit addresses
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
    uint16_t ABR;
    
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
    uint16_t FetchWord(MEM_6502& mem){
        return (FetchByte(mem) | (FetchByte(mem) << 8));
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
    uint16_t ReadWord(MEM_6502& mem, uint16_t addr){
        return (ReadByte(mem, addr) | (ReadByte(mem, addr+1) << 8));
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
                ABR = 0xFF & FetchByte(mem); //Clear ABR and store LSB
                A = ReadByte(mem, ABR);
                SetFlagsLDA();
                break;
            case LDA_ZPX:
                ABR = (0xFF & FetchByte(mem)) + X ; //Clear ABR and store LSB, then add value in X
                CycleCount++;
                A = ReadByte(mem, ABR);
                SetFlagsLDA();
                break;
            case LDA_AB:
                ABR = FetchWord(mem);
                A = ReadByte(mem, ABR);
                SetFlagsLDA();
                break;
            case JMP_AB:
                ABR = FetchWord(mem);
                PC = ABR;
                break;
            case JMP_IND:
                ABR = FetchWord(mem);
                PC = ReadWord(mem, ABR);
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
        Test Program
            JMP_AB  $0100
            JMP_IND $0200
    */
    // Power-On Reset, JMP
    mem.Data[0xFFFC] = (uint8_t) Instruction::JMP_AB;
    mem.Data[0xFFFD] = 0x00;
    mem.Data[0xFFFE] = 0x01;
    mem.Data[0x0100] = (uint8_t) Instruction::JMP_IND;
    mem.Data[0x0101] = 0x00;
    mem.Data[0x0102] = 0x02;
    mem.Data[0x0200] = 0x34;
    mem.Data[0x0201] = 0x12;
    cpu.Execute(mem);
    cpu.Execute(mem);
    printf("PC at 0x%x.\n", cpu.PC);
    printf("CPU ran for %d machine cycles.\n", cpu.CycleCount);
    return 0;
}