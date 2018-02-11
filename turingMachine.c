#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ALPHABETSIZE 256
#define TAPELENGTH 256
#define HALT 26 //equivalent to the EOF character in UNIX

typedef uint16_t state_t;
typedef uint16_t tapelen_t;
typedef uint8_t symbol_t;

typedef struct
{
        symbol_t newSymbol[ALPHABETSIZE];
        int8_t leftRight[ALPHABETSIZE]; //left right stay
        uint8_t newState[ALPHABETSIZE];
}instruction_t;

symbol_t tape[TAPELENGTH];
instruction_t instTable[128];
tapelen_t headPos;
state_t stateRegister;
tapelen_t viewWidth = 64;

void executeInstruction(instruction_t inst);
void loadInstructionSet(instruction_t* instTab, const uint8_t* str);
void loadInstructionSetFile(instruction_t* instTab, FILE* fp);
void initMachine();
void dumpTape(tapelen_t view);

uint16_t main(int argc, char* argv[])
{

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i],"-wide") == 0)
        {
            viewWidth = 128;
        }
    }

    FILE* fp = fopen("copy.tur","r");

    initMachine();

    dumpTape(viewWidth);

    loadInstructionSetFile(instTable, fp);
    
    while(stateRegister != HALT)
    {
            executeInstruction(instTable[stateRegister]);
    }

    dumpTape(viewWidth);
    return 0;
}

void executeInstruction(instruction_t inst)
{
        symbol_t origSymbol = tape[headPos];
        tape[headPos] = inst.newSymbol[origSymbol];
        headPos += inst.leftRight[origSymbol];
        stateRegister = inst.newState[origSymbol];
}

void loadInstructionSet(instruction_t* instTab, const uint8_t* instructionFile)
{
    state_t state = 0;
    symbol_t startSymbol;
    uint16_t filePtr = 0;
    while(instructionFile[filePtr] != 0)
    {
        const uint8_t* temp = instructionFile+filePtr;
        if(instructionFile[filePtr] == '{')
        {
            sscanf(temp,"{%d,", &state);
        }
        else if(instructionFile[filePtr] == '[')
        {
            sscanf(temp,"[%c", &startSymbol);
            sscanf(temp,"[%*c,%c,%d,%d]", &instTab[state].newSymbol[startSymbol],&instTab[state].leftRight[startSymbol], &instTab[state].newState[startSymbol]);
        }
        filePtr++;
    }
}

void loadInstructionSetFile(instruction_t* instTab, FILE* instructionFilePtr)
{
    uint8_t instructionFileStr[1024];
    fscanf(instructionFilePtr,"%s", instructionFileStr);
    const uint8_t* instructionFile = instructionFileStr;
    loadInstructionSet(instTab, instructionFile);
}

void initMachine()
{
        for(tapelen_t i = 0; i < TAPELENGTH; i++)
        {
                tape[i] = '0';
        }

        stateRegister = 0;
        headPos = 128;
        tape[headPos]='1';
        tape[headPos-1]='1';
        tape[headPos-2]='1';
        tape[headPos-3]='1';
}

void dumpTape(tapelen_t view)
{
        tapelen_t edge = view / 2;
        for(tapelen_t i = headPos-edge; i < headPos+edge; i++)
        {
                if(i == headPos) printf("%d", stateRegister);
                else printf(" ");
        }
        printf("\n");
        for(tapelen_t i = headPos-edge; i < headPos+edge; i++)
        {
                if(i == headPos) printf("V");
                else printf(" ");
        }
        printf("\n");
        for(tapelen_t i = headPos-edge; i < headPos+edge; i++)
        {
                printf("%c", tape[i]);
        }
        printf("\n");
}