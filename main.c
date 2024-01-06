#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    char opcode[10];
    char format;
} InstructionInfo;


InstructionInfo instructions[] = {
    {"ADD", 'R'}, {"SUB", 'R'}, {"AND", 'R'}, {"OR", 'R'},
    {"ADDI", 'I'}, {"ANDI", 'I'}, {"SLL", 'I'},
    {"BEQ", 'I'}, {"BNE", 'I'}, {"J", 'J'}
};


char* hexToBinary(char hex) {
    switch (hex) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return NULL;
    }
}

char* hexToBinaryStr(char* hex) {
    int len = strlen(hex);
    char* binary = malloc(len * 4 + 1);
    binary[0] = '\0';

    for (int i = 0; i < len; ++i) {
        strcat(binary, hexToBinary(hex[i]));
    }

    return binary;
}


char* binaryToHex(char* binary) {
    int len = strlen(binary);
    char* hex = malloc(len / 4 + 1);
    hex[0] = '\0';

    for (int i = 0; i < len; i += 4) {
        char nibble[5];
        strncpy(nibble, binary + i, 4);
        nibble[4] = '\0';
        int decimal = strtol(nibble, NULL, 2);
        sprintf(hex + strlen(hex), "%X", decimal);
    }

    return hex;
}


char getInstructionFormat(char* opcode) {
    for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); ++i) {
        if (strcmp(instructions[i].opcode, opcode) == 0) {
            return instructions[i].format;
        }
    }
    return '\0';
}

char* convertToMachineCode(char* opcode, char* operands, int address) {
    char format = getInstructionFormat(opcode);
    char* machineCode = malloc(9); 

    if (format == 'R') {
       
        char rd[6], rs[6], rt[6];
        int numItems = sscanf(operands, "%5[^,], %5[^,], %5s", rd, rs, rt);
        
        if (numItems == 3) {
           
            int rdReg = atoi(rd);
            int rsReg = atoi(rs);
            int rtReg = atoi(rt);

           
            sprintf(machineCode, "%02X%02X%02X000000", rsReg, rtReg, rdReg);
        } else {
          
            printf("Error: Invalid format for R-type instruction '%s'.\n", opcode);
            free(machineCode);
            return NULL;
        }
    } else if (format == 'I') {
        
        char rt[6], rs[6], imm[16];
        int numItems = sscanf(operands, "%5[^,], %5[^,], %[^\n]", rt, rs, imm);

        if (numItems == 3) {
            
            int rtReg = atoi(rt);
            int rsReg = atoi(rs);
            int immediate = atoi(imm);

           
            sprintf(machineCode, "%02X%02X%04X", rsReg, rtReg, immediate);
        } else {
            
            printf("Error: Invalid format for I-type instruction '%s'.\n", opcode);
            free(machineCode);
            return NULL;
        }
    } else if (format == 'J') {
      
        char target[26];
        int numItems = sscanf(operands, "%[^\n]", target);

        if (numItems == 1) {
        
            int targetAddr = atoi(target);

           
            int relativeOffset = targetAddr - address;
            
            relativeOffset >>= 2;

            
            sprintf(machineCode, "02X%06X", relativeOffset);
        } else {
            
            printf("Error: Invalid format for J-type instruction '%s'.\n", opcode);
            free(machineCode);
            return NULL;
        }
    } else {
      
        printf("Error: Unsupported instruction '%s'.\n", opcode);
        free(machineCode);
        return NULL;
    }

    return machineCode;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source_file.asm>\n", argv[0]);
        return 1;
    }

    FILE* sourceFile = fopen(argv[1], "r");
    if (!sourceFile) {
        printf("Error opening source file.\n");
        return 1;
    }

    
    char outputFileName[100];
    snprintf(outputFileName, sizeof(outputFileName), "%s.obj", argv[1]);
    FILE* objFile = fopen(outputFileName, "w");
    if (!objFile) {
        printf("Error creating output file.\n");
        fclose(sourceFile);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int address = 0x00400000;

   
    while (fgets(line, MAX_LINE_LENGTH, sourceFile) != NULL) {
        char opcode[10], operands[50];
        int numItems = sscanf(line, "%s %[^\n]", opcode, operands);

        if (numItems == 2) {
          
            char* machineCode = convertToMachineCode(opcode, operands, address);
            if (machineCode != NULL) {
               
                fprintf(objFile, "%08X: %s\n", address, machineCode);
                address += 4;  
                free(machineCode);
            }
        }
    }

    fclose(sourceFile);
    fclose(objFile);

    printf("Assembler completed successfully. Output file: %s\n", outputFileName);

    return 0;
}
