#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TSIZE 20
#define MAXSTRLEN 8

int size;
int load;

typedef struct {
    char *name;
    int address;
} symbol;

symbol *symbolTable;

void initTable() {
    size = TSIZE;
    symbolTable = calloc(size, sizeof(symbol));
}

void resizeTable() {
    size *= 2;
    symbolTable = realloc(symbolTable, size * sizeof(symbol));
    if (!symbolTable) {
        printf("\nPROGRAM ERROR: attempted to resize table but resulted in a null pointer!\n");
        for (int i = 0; i < size; i++) {
            if (symbolTable[i].name != NULL) {
                free(symbolTable[i].name);
            }
        }
        free(symbolTable);
        exit(-3);
    }
}

int hash(char *name) {
    return (name[0] + name[strlen(name) - 1]) % size;
}

int search(char* label) {
    for (int temps = size / 2; temps > TSIZE; temps /= 2) {
        int tempi = (label[0] + label[strlen(label) - 1]) % temps;
        int probe = 3;
        while (symbolTable[tempi].name != NULL) {
            if (strcmp(symbolTable[tempi].name, label) == 0) {
                return -1;
            }
            tempi += (probe * probe);
            tempi %= temps;
            probe++;
            if ((probe - 3) > temps / 2) {
                break;
            }
        }
    }
    int index = hash(label);
    int probe = 3;
    while (symbolTable[index].name != NULL) {
        if (strcmp(symbolTable[index].name, label) == 0) {
            return -1;
        }
        index += (probe * probe);
        index %= size;
        probe++;
    }
    return index;
}

int insert(char* label, int loc) {
    if (!label) {
        printf("\nPROGRAM ERROR: attempted to add NULL symbol to table!\n");
        for (int i = 0; i < size; i++) {
            if (symbolTable[i].name != NULL) {
                free(symbolTable[i].name);
            }
        }
        free(symbolTable);
        exit(-2);
    }
    int index = search(label);
    if (index == -1) {
        return 0;
    }
    if (((float)load / (float)size) > 0.5) {
        resizeTable();
    }
    symbolTable[index].name = calloc(MAXSTRLEN, sizeof(char));
    for (int i = 0; i < strlen(label); i++) {
        symbolTable[index].name[i] = label[i];
    }
    symbolTable[index].address = loc;
    load++;
    return 1;
}

void printTable() {
    printf("\n");
    for (int i = 0; i < size; i++) {
        if (symbolTable[i].name == NULL) {
            printf("%-6d\t------\t----\n", i);
        }
        else {
            printf("%-6d\t%-6s\t%04X\n", i, symbolTable[i].name, symbolTable[i].address);
        }
    }
}

static const char instructionTable[59][6] = {"ADD","ADDF","ADDR","AND","CLEAR","COMP","COMPF","COMPR","DIV","DIVF","DIVR","FIX","FLOAT","HIO","J","JEQ","JGT","JLT","JSUB","LDA","LDB","LDCH","LDF","LDL","LDS","LDT","LDX","LPS","MUL","MULF","MULR","NORM","OR","RD","RMO","RSUB","SHIFTL","SHIFTR","SIO","SSK","STA","STCH","STF","STI","STL","STS","STSW","STB","STT","STX","SUB","SUBF","SUBR","SVC","TD","TIO","TIX","TIXR","WD"};

void assemblyError(int lineCtr, char line[1024], char *message, int exitStatus) {
    printf("\nASSEMBLY ERROR:\n");
    printf("%s\n", strtok(line, "\n"));
    printf("Line %d: %s\n", lineCtr, message);
    for (int i = 0; i < size; i++) {
        if (symbolTable[i].name != NULL) {
            free(symbolTable[i].name);
        }
    }
    free(symbolTable);
    exit(exitStatus);
}

int directiveFlag(char *string) {
    if (strcmp(string, "START") == 0) {
        return 1;
    }
    else if (strcmp(string, "END") == 0) {
        return 2;
    }
    else if (strcmp(string, "BYTE") == 0) {
        return 3;
    }
    else if (strcmp(string, "WORD") == 0) {
        return 4;
    }
    else if (strcmp(string, "RESB") == 0) {
        return 5;
    }
    else if (strcmp(string, "RESW") == 0) {
        return 6;
    }
    else if (strcmp(string, "RESR") == 0) {
        return 7;
    }
    else if (strcmp(string, "EXPORTS") == 0) {
        return 8;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE* inputFile;

    inputFile = fopen(argv[1], "r");

    if (!inputFile) {
        printf("\nPROGRAM ERROR: %s could not be read.\n", argv[1]);
        exit(-1);
    }

    char line[1024];
    char proName[7];
    int locCtr = 0;
    int lineCtr = 0;
    int comCtr = 0;
    int endCtr = 0;
    int start = 0;
    int proLen = 0;

    initTable();
    printf("Symbol table initialized.\n\n");

    while (fgets(line, 1024, inputFile)) {
        lineCtr++;
        
        char *tempLine = calloc(strlen(line) + 1, sizeof(char));
        strcpy(tempLine, line);
        char *otherTempLine = calloc(strlen(line) + 1, sizeof(char));
        strcpy(otherTempLine, line);

        if (locCtr >= 32768) {
            char *error = "Maximum memory exceeded!";
            assemblyError(lineCtr, otherTempLine, error, 1);
        }

        if (strlen(line) == 0 || !strtok(line, " \t\r\n")) {
            char *error = "Empty line detected!";
            assemblyError(lineCtr, otherTempLine, error, 2);
        }
        if (line[0] == 35) {
            comCtr++;
            continue;
        }

        char *label = calloc(MAXSTRLEN, sizeof(char));
        char *opcode = calloc(MAXSTRLEN, sizeof(char));
        char *operand = calloc(MAXSTRLEN, sizeof(char));
        int opcodeFlag = 0;
        int symbolFlag = 0;

        if ((tempLine[0] >= 65) && (tempLine[0] <= 90)) {
            symbolFlag = 1;
        }
        if ((tempLine[0] != 32 && tempLine[0] != 9) && !symbolFlag) {
            char *error = "Symbol does not start with an alpha character!";
            assemblyError(lineCtr, otherTempLine, error, 3);
        }

        char *token = strtok(tempLine, " \t");
        if (symbolFlag) {
            label = token;
            token = strtok(NULL, " \t");
            if (directiveFlag(label) != 0) {
                char *error = "Symbol matches a directive!";
                assemblyError(lineCtr, otherTempLine, error, 4);
            }
            if (strlen(label) > 6) {
                char *error = "Symbol length is too long!";
                assemblyError(lineCtr, otherTempLine, error, 5);
            }
            for (int i = 0; i < strlen(label); i++) {
                if (label[i] == 9 || label[i] == 32 || label[i] == 36 || label[i] == 33 || label[i] == 61 || label[i] == 43 || label[i] == 45 || label[i] == 40 || label[i] == 41 || label[i] == 64) {
                  char *error = "Symbol contains at least one invalid character!";
                assemblyError(lineCtr, otherTempLine, error, 6);
                }
            }
            if (!token) {
                char *error = "Symbol defined without opcode!";
                assemblyError(lineCtr, otherTempLine, error, 7);
            }
        }
        else {
            label = NULL;
        }
        
        opcode = token;
        token = strtok(NULL, "\t");
        operand = token;
        opcode = strtok(opcode, "\r\n");
        operand = strtok(operand, "\r\n");
        opcodeFlag = directiveFlag(opcode);

        if (opcodeFlag > 0 && !operand) {
            char *error = "Directive defined without operand!";
            assemblyError(lineCtr, otherTempLine, error, 8);    
        }
        if ((lineCtr - comCtr) == 1) {
            if (opcodeFlag == 1) {
                for (int i = 0; i < strlen(operand); i++) {
                    if ((operand[i] < 48) || (operand[i] > 57 && operand[i] < 65) || (operand[i] > 70)) {
                        char *error = "Starting address contains at least one invalid digit!";
                        assemblyError(lineCtr, otherTempLine, error, 9);
                    }
                }
                start = (int)strtol(operand, NULL, 16);
                locCtr = start;
                if (symbolFlag) {
                    insert(label, locCtr);
                    strcpy(proName, label);
                    printf("%-6s\t%04X\n", label, locCtr);
                }
                continue;
            }
        }
        else if (opcodeFlag == 1) {
            char *error = "Invalid use of START directive!";
            assemblyError(lineCtr, otherTempLine, error, 10);
        }
        if (symbolFlag) {
            if (!insert(label, locCtr)) {
                char *error = "Duplicate symbol detected!";
                assemblyError(lineCtr, otherTempLine, error, 11);
            }
            printf("%-6s\t%04X\n", label, locCtr);
        }
        if (opcodeFlag != 2) {
            if (opcodeFlag == 3) {
                if ((operand[0] != 67 && operand[0] != 88) || operand[1] != 39) {
                    char *error = "Unable to register BYTE operand as character/hex!";
                    assemblyError(lineCtr, otherTempLine, error, 12);
                }
                int quoteCtr = 1;
                for (int i = 2; i < strlen(operand); i++) {
                    if (operand[i] == 39) {
                        quoteCtr++;
                    }
                }
                if (quoteCtr != 2 || operand[strlen(operand) - 1] != 39) {
                    char *error = "Invalid quoting in BYTE operand!";
                    assemblyError(lineCtr, otherTempLine, error, 13);
                }
                char *tempString = calloc(MAXSTRLEN, sizeof(char));
                for (int i = 2; i < (strlen(operand) - 1); i++) {
                    strncat(tempString, &operand[i], 1);
                }
                if (!tempString) {
                    char *error = "No data between quotes in BYTE operand detected!";
                    assemblyError(lineCtr, otherTempLine, error, 14);
                }
                if (operand[0] == 67) {
                    locCtr += strlen(tempString);
                }
                else {
                    for (int i = 0; i < strlen(tempString); i++) {
                        if ((tempString[i] < 48) || (tempString[i] > 57 && tempString[i] < 65) || (tempString[i] > 70)) {
                            char *error = "Quoted data in BYTE operand contains at least one invalid digit!";
                            assemblyError(lineCtr, otherTempLine, error, 15);
                        }
                    }
                    locCtr += (strlen(tempString) / 2) + (strlen(tempString) % 2);
                }
                free(tempString);
            }
            else if (opcodeFlag == 4) {
                for (int i = 0; i < strlen(operand); i++) {
                    if (operand[i] < 48 || operand[i] > 57) {
                        char *error = "WORD operand contains at least one invalid digit!";
                        assemblyError(lineCtr, otherTempLine, error, 16);
                    }
                }
                if (abs(atoi(operand)) >= 8388608) {
                    char *error = "WORD operand exceeds word size!";
                    assemblyError(lineCtr, otherTempLine, error, 17);
                }
                locCtr += 3;
            }
            else if (opcodeFlag == 5) {
                for (int i = 0; i < strlen(operand); i++) {
                    if (operand[i] < 48 || operand[i] > 57) {
                        char *error = "RESB operand contains at least one invalid digit!";
                        assemblyError(lineCtr, otherTempLine, error, 18);
                    }
                }
                locCtr += atoi(operand);
            }
            else if (opcodeFlag == 6) {
                for (int i = 0; i < strlen(operand); i++) {
                    if (operand[i] < 48 || operand[i] > 57) {
                        char *error = "RESW operand contains at least one invalid digit!";
                        assemblyError(lineCtr, otherTempLine, error, 19);
                    }
                }
                locCtr += (atoi(operand)*3);
            }
            else if (opcodeFlag == 7 || opcodeFlag == 8) {
                locCtr += 3;
            }
            else {
                int instructionFlag = 0;
                for (int i = 0; i < sizeof(instructionTable); i++) {
                    if (strcmp(opcode, instructionTable[i]) == 0) {
                        instructionFlag++;
                        break;
                    }
                }
                if (!instructionFlag) {
                    char *error = "Opcode is not a valid directive nor instruction!";
                    assemblyError(lineCtr, otherTempLine, error, 20);
                }
                locCtr += 3;
            }
        }
        else {
            proLen = locCtr - start;
            endCtr++;
        }
    }

    if (endCtr != 1) {
        char *error = "The amount of END statements is not 1!";
        assemblyError(-1, " ", error, 21);
    }
    fclose(inputFile);
    
    printf("\nSymbol table finalized. Program length: %X \nPass 1 complete.\n", proLen);
    //printTable();

    for (int i = 0; i < size; i++) {
        if (symbolTable[i].name != NULL) {
            free(symbolTable[i].name);
        }
    }
    free(symbolTable);
    return 0;
}