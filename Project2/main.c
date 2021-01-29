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
    int temps = size;
    while (temps > TSIZE) {
	      temps /= 2;
        int tempi = (label[0] + label[strlen(label) - 1]) % temps;
        int probe = 3;
        while (symbolTable[tempi].name != NULL) {
            if (strcmp(symbolTable[tempi].name, label) == 0) {
                return tempi;
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
            return index;
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
    if (symbolTable[index].name != NULL) {
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

static const char instructionTable[59][6] = {"ADD","ADDF","ADDR","AND","CLEAR","COMP","COMPF","COMPR","DIV","DIVF","DIVR","FIX","FLOAT","HIO","J","JEQ","JGT","JLT","JSUB","LDA","LDB","LDCH","LDF","LDL","LDS","LDT","LDX","LPS","MUL","MULF","MULR","NORM","OR","RD","RMO","RSUB","SHIFTL","SHIFTR","SIO","SSK","STA","STB","STCH","STF","STI","STL","STS","STSW","STT","STX","SUB","SUBF","SUBR","SVC","TD","TIO","TIX","TIXR","WD"};

int instructionCode(int index) {
    switch (index) {
        case 0 : return 0x18;
        case 1 : return 0x58;
        case 2 : return 0x90;
        case 3 : return 0x40;
        case 4 : return 0xB4;
        case 5 : return 0x28;
        case 6 : return 0x88;
        case 7 : return 0xA0;
        case 8 : return 0x24;
        case 9 : return 0x64;
        case 10 : return 0x9C;
        case 11 : return 0xC4;
        case 12 : return 0xC0;
        case 13 : return 0xF4;
        case 14 : return 0x3C;
        case 15 : return 0x30;
        case 16 : return 0x34;
        case 17 : return 0x38;
        case 18 : return 0x48;
        case 19 : return 0x00;
        case 20 : return 0x68;
        case 21 : return 0x50;
        case 22 : return 0x70;
        case 23 : return 0x08;
        case 24 : return 0x6C;
        case 25 : return 0x74;
        case 26 : return 0x04;
        case 27 : return 0xD0;
        case 28 : return 0x20;
        case 29 : return 0x60;
        case 30 : return 0x98;
        case 31 : return 0xC8;
        case 32 : return 0x44;
        case 33 : return 0xD8;
        case 34 : return 0xAC;
        case 35 : return 0x4C;
        case 36 : return 0xA4;
        case 37 : return 0xA8;
        case 38 : return 0xF0;
        case 39 : return 0xEC;
        case 40 : return 0x0C;
        case 41 : return 0x78;
        case 42 : return 0x54;
        case 43 : return 0x80;
        case 44 : return 0xD4;
        case 45 : return 0x14;
        case 46 : return 0x7C;
        case 47 : return 0xE8;
        case 48 : return 0x84;
        case 49 : return 0x10;
        case 50 : return 0x1C;
        case 51 : return 0x5C;
        case 52 : return 0x94;
        case 53 : return 0xB0;
        case 54 : return 0xE0;
        case 55 : return 0xF8;
        case 56 : return 0x2C;
        case 57 : return 0xB8;
        case 58 : return 0xDC;
        default : return -1;
    }
}

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
    /*
    if (argc != 2) {
        printf("USAGE: %s <filename>\n", argv[0]);
        return 1;
    }
    */

    FILE* inputFile;

    inputFile = fopen(argv[1], "r");
    //inputFile = fopen("test1.sic", "r");
    //inputFile = fopen("example.txt", "r");

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

    //PASS 1
    initTable();

    while (fgets(line, 1024, inputFile)) {
        lineCtr++;
        
        char *tempLine = calloc(strlen(line) + 1, sizeof(char));
        strcpy(tempLine, line);
        char *otherTempLine = calloc(strlen(line) + 1, sizeof(char));
        strcpy(otherTempLine, line);

        if (locCtr >= 0x8000) {
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
        char *operand = calloc(strlen(otherTempLine), sizeof(char));
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
                }
                else {
                    strcpy(proName, "");
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
                char *tempString = calloc(strlen(operand) + 1, sizeof(char));
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
		        continue;
            }
            operand = strtok(operand, " ");
            if (opcodeFlag == 4) {
                for (int i = 0; i < strlen(operand); i++) {
                    if (operand[i] < 48 || operand[i] > 57) {
                        char *error = "WORD operand contains at least one invalid digit!";
                        assemblyError(lineCtr, otherTempLine, error, 16);
                    }
                }
                if (abs(atoi(operand)) >= 0x800000) {
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
    
    //printTable();

    //PASS 2
    char *record = calloc(60, sizeof(char));
    char recordLoc[7];
    int recordLen = 0;
    int firstIns = 0;
    int insFlag = 0;
    lineCtr = 0;
    comCtr = 0;
    locCtr = 0;

    inputFile = fopen(argv[1], "r");
    //inputFile = fopen("test1.sic", "r");
    //inputFile = fopen("example.txt", "r");

    FILE* outputFile;
    char *filename = calloc(strlen(argv[1]), sizeof(char));
    strcpy(filename, argv[1]);
    //filename = strtok(filename, ".");
    strcat(filename, ".obj");
    outputFile = fopen(filename, "w");

    while (fgets(line, 1024, inputFile)) {
        lineCtr++;

        if (line[0] == 35) {
            comCtr++;
            continue;
        }

        char *tempLine = calloc(strlen(line) + 1, sizeof(char));
        strcpy(tempLine, line);
        char *label = calloc(MAXSTRLEN, sizeof(char));
        char *opcode = calloc(MAXSTRLEN, sizeof(char));
        char *operand = calloc(strlen(line), sizeof(char));
        char *token = strtok(line, " \t");
        int symbolFlag = 0;
        int opcodeFlag = 0;

        if ((line[0] >= 65) && (line[0] <= 90)) {
            symbolFlag = 1;
        }
        if (symbolFlag) {
            label = token;
            token = strtok(NULL, " \t");
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

        if ((lineCtr - comCtr) == 1) {
            if (!label || opcodeFlag != 1) {
                strcpy(proName, "NULL");
            }
            char header[20];
            char startStr[7];
            char proLenStr[7];

            header[0] = 'H';
            for (int i = 0; i < 6; i++) {
                if (i >= strlen(proName)) {
                    header[i + 1] = ' ';
                    continue;
                }
                header[i + 1] = proName[i];
            }
            sprintf(startStr, "%06X", start);
            for (int i = 0; i < 6; i++) {
                header[i + 7] = startStr[i];
            }
            sprintf(proLenStr, "%06X", proLen);
            for (int i = 0; i < 6; i++) {
                header[i + 13] = proLenStr[i];
            }
            header[19] = '\0';
            locCtr = start;
            fprintf(outputFile, "%s\n", header);
            sprintf(recordLoc, "%06X", locCtr);
            if (opcodeFlag == 1) {
                memset(record, 0, strlen(record));
                recordLen = 0;
                continue;
            }
        }
        
        if (opcodeFlag == 2) {
            if (strlen(record) > 0) {
                char text[70];
                text[0] = 'T';
                for (int i = 0; i < 6; i++) {
                    text[i + 1] = recordLoc[i];
                }
                char recordLenStr[3];
                sprintf(recordLenStr, "%02X", recordLen);
                for (int i = 0; i < 2; i++) {
                    text[i + 7] = recordLenStr[i];
                }
                for (int i = 0; i < strlen(record); i++) {
                    text[i + 9] = record[i];
                }
                text[strlen(record) + 9] = '\0';
                fprintf(outputFile, "%s\n", text);
                memset(text, 0, sizeof(text));
                memset(record, 0, strlen(record));
                sprintf(recordLoc, "%06X", locCtr);
                recordLen = 0;
            }

            char *temps = calloc(7, sizeof(char));
            int tempi = search(operand);
            if (!symbolTable[tempi].name) {
                fclose(outputFile);
                remove(filename);
                char *error = "Symbol in END operand not found in symbol table!";
                assemblyError(lineCtr, tempLine, error, 22);
            }

            char end[8];
            char firstInsStr[7];

            end[0] = 'E';
            sprintf(firstInsStr, "%06X", firstIns);
            for (int i = 0; i < 6; i++) {
                end[i + 1] = firstInsStr[i];
            }
            end[7] = '\0';
            fprintf(outputFile, "%s\n", end);
            break;
        }

        int objCode = 0;
        int objLen = 0;
        char *objCodeStr;
        char *objLenStr = calloc(2, sizeof(char));
        
        if (opcodeFlag == 3) {
            char text[70];
            if (strlen(record) > 0) {
                text[0] = 'T';
                for (int i = 0; i < 6; i++) {
                    text[i + 1] = recordLoc[i];
                }
                char recordLenStr[3];
                sprintf(recordLenStr, "%02X", recordLen);
                for (int i = 0; i < 2; i++) {
                    text[i + 7] = recordLenStr[i];
                }
                for (int i = 0; i < strlen(record); i++) {
                    text[i + 9] = record[i];
                }
                text[strlen(record) + 9] = '\0';
                fprintf(outputFile, "%s\n", text);
                memset(text, 0, sizeof(text));
                memset(record, 0, strlen(record));
                sprintf(recordLoc, "%06X", locCtr);
                recordLen = 0;
            }
            char *tempString = calloc(strlen(operand) + 1, sizeof(char));
            for (int i = 2; i < (strlen(operand) - 1); i++) {
                strncat(tempString, &operand[i], 1);
            }
            if (operand[0] == 67) {
                objLen = strlen(tempString);
                objCodeStr = calloc(2 * objLen, sizeof(char));
                char *otherTempString = calloc(2, sizeof(char));
                if (objLen > 0xFF) {
                    fclose(outputFile);
                    remove(filename);
                    char *error = "Character string length exceeds 255 bytes!";
                    assemblyError(lineCtr, tempLine, error, 23);
                }
                for (int i = 0; i < objLen; i++) {
                    int tempValue = tempString[i];
                    sprintf(otherTempString, "%02X", tempValue);
                    strcat(objCodeStr, otherTempString);
                }
                objCode = (int)strtol(objCodeStr, NULL, 16);
                while (objLen > 0x1E) {
                    locCtr += 30;
                    sprintf(objLenStr, "%02X", 30);
                    objLen -= 30;
                    text[0] = 'T';
                    for (int i = 0; i < 6; i++) {
                        text[i + 1] = recordLoc[i];
                    }
                    for (int i = 0; i < 2; i++) {
                        text[i + 7] = objLenStr[i];
                    }
                    for (int i = 0; i < 60; i++) {
                        text[i + 9] = objCodeStr[i];
                    }
                    text[69] = '\0';
                    objCodeStr += 60;
                    fprintf(outputFile, "%s\n", text);
                    memset(text, 0, sizeof(text));
                    sprintf(recordLoc, "%06X", locCtr);
                }
                locCtr += objLen;
                sprintf(objLenStr, "%02X", objLen);
                text[0] = 'T';
                for (int i = 0; i < 6; i++) {
                    text[i + 1] = recordLoc[i];
                }
                for (int i = 0; i < 2; i++) {
                    text[i + 7] = objLenStr[i];
                }
                for (int i = 0; i < (2 * objLen); i++) {
                    text[i + 9] = objCodeStr[i];
                }
                text[(2 * objLen) + 9] = '\0';
                objCodeStr += 2 * objLen;
                fprintf(outputFile, "%s\n", text);
                memset(text, 0, sizeof(text));
                sprintf(recordLoc, "%06X", locCtr);
                continue;
            }
            else {
                objLen = (strlen(tempString) / 2) + (strlen(tempString) % 2);
                if (objLen > 0xFF) {
                    fclose(outputFile);
                    remove(filename);
                    char *error = "Hex string length exceeds 255 bytes!";
                    assemblyError(lineCtr, tempLine, error, 24);
                }
                if (strlen(tempString) % 2 != 0) {
                    char *otherTempString = calloc(strlen(tempString), sizeof(char));
                    strcpy(otherTempString, tempString);
                    strcpy(tempString, "0");
                    strcat(tempString, otherTempString);
                    free(otherTempString);
                }
                objCode = (int)strtol(tempString, NULL, 16);
                objCodeStr = calloc(objLen, sizeof(char));
                strcpy(objCodeStr, tempString);
                while (objLen > 0x1E) {
                    locCtr += 30;
                    sprintf(objLenStr, "%02X", 30);
                    objLen -= 30;
                    text[0] = 'T';
                    for (int i = 0; i < 6; i++) {
                        text[i + 1] = recordLoc[i];
                    }
                    for (int i = 0; i < 2; i++) {
                        text[i + 7] = objLenStr[i];
                    }
                    for (int i = 0; i < 60; i++) {
                        text[i + 9] = objCodeStr[i];
                    }
                    text[69] = '\0';
                    objCodeStr += 60;
                    fprintf(outputFile, "%s\n", text);
                    memset(text, 0, sizeof(text));
                    sprintf(recordLoc, "%06X", locCtr);
                }
                locCtr += objLen;
                sprintf(objLenStr, "%02X", objLen);
                text[0] = 'T';
                for (int i = 0; i < 6; i++) {
                    text[i + 1] = recordLoc[i];
                }
                for (int i = 0; i < 2; i++) {
                    text[i + 7] = objLenStr[i];
                }
                for (int i = 0; i < (2 * objLen); i++) {
                    text[i + 9] = objCodeStr[i];
                }
                text[(2 * objLen) + 9] = '\0';
                objCodeStr += 2 * objLen;
                fprintf(outputFile, "%s\n", text);
                memset(text, 0, sizeof(text));
                sprintf(recordLoc, "%06X", locCtr);
                continue;
            }
        }
        else if (opcodeFlag == 4) {
            operand = strtok(operand, " ");
            locCtr += 3;
            objLen = 3;
            objCode = atoi(operand);
            objCodeStr = calloc(2 * objLen, sizeof(char));
            sprintf(objCodeStr, "%06X", objCode);
            recordLen += objLen;
            strcat(record, objCodeStr);
        }
        else if (opcodeFlag == 5) {
            operand = strtok(operand, " ");
            locCtr += atoi(operand);
            if (strlen(record) > 0) {
                char text[70];
                text[0] = 'T';
                for (int i = 0; i < 6; i++) {
                    text[i + 1] = recordLoc[i];
                }
                char recordLenStr[3];
                sprintf(recordLenStr, "%02X", recordLen);
                for (int i = 0; i < 2; i++) {
                    text[i + 7] = recordLenStr[i];
                }
                for (int i = 0; i < strlen(record); i++) {
                    text[i + 9] = record[i];
                }
                text[strlen(record) + 9] = '\0';
                fprintf(outputFile, "%s\n", text);
                memset(text, 0, sizeof(text));
                memset(record, 0, strlen(record));
                recordLen = 0;
            }
            sprintf(recordLoc, "%06X", locCtr);
            continue;
        }
        else if (opcodeFlag == 6) {
            operand = strtok(operand, " ");
            locCtr += (atoi(operand) * 3);
            if (strlen(record) > 0) {
                char text[70];
                text[0] = 'T';
                for (int i = 0; i < 6; i++) {
                    text[i + 1] = recordLoc[i];
                }
                char recordLenStr[3];
                sprintf(recordLenStr, "%02X", recordLen);
                for (int i = 0; i < 2; i++) {
                    text[i + 7] = recordLenStr[i];
                }
                for (int i = 0; i < strlen(record); i++) {
                    text[i + 9] = record[i];
                }
                text[strlen(record) + 9] = '\0';
                fprintf(outputFile, "%s\n", text);
                memset(text, 0, sizeof(text));
                memset(record, 0, strlen(record));
                recordLen = 0;
            }
            sprintf(recordLoc, "%06X", locCtr);
            continue;
        }
        else if (opcodeFlag == 7 || opcodeFlag == 8) {
            printf("\nPass 2 is unable to handle RESR or EXPORTS opcodes (for now)\n");
            fclose(outputFile);
            remove(filename);
            exit(-1);
        }
        else {
            if (insFlag == 0) {
                firstIns = locCtr;
                insFlag = 1;
            }
            int errorFlag = 0;
            operand = strtok(operand, " ");
            locCtr += 3;
            objLen = 3;
            sprintf(objLenStr, "%02X", objLen);
            int instCode;
            char tempString[3];
            for (int i = 0; i < sizeof(instructionTable); i++) {
                if (strcmp(opcode, instructionTable[i]) == 0) {
                    instCode = instructionCode(i);
                    break;
                }
            }
            sprintf(tempString, "%02X", instCode);
            objCodeStr = calloc(2 * objLen, sizeof(char));
            strcat(objCodeStr, tempString);
            if (instCode == 0x90 || instCode == 0xB4 || instCode == 0xA0 || instCode == 0x9C || instCode == 0x98 || instCode == 0xAC || instCode == 0xA4 || instCode == 0xA8 || instCode == 0x94 || instCode == 0xB0 || instCode == 0xB8) {
                printf("\nPass 2 is unable to handle instructions that use registers (for now)\n");
                fclose(outputFile);
                remove(filename);
                exit(-1);
            }
            else if (instCode == 0xC4 || instCode == 0xC0 || instCode == 0xF4 || instCode == 0xC8 || instCode == 0x4C || instCode == 0xF0 || instCode == 0xF8) {
                if (operand) {
                    fclose(outputFile);
                    remove(filename);
                    char *error = "Given instruction must not have an operand!";
                    assemblyError(lineCtr, tempLine, error, 25);
                }
                strcat(objCodeStr, "0000");
            }
            else {
                int commaCtr = 0;
                int commaInd = 0;
                for (int i = 0; i < strlen(operand); i++) {
                    if (operand[i] == ',') {
                        commaInd = i;
                        errorFlag = 0;
                        commaCtr++;
                    }
                    if (commaCtr > 1) {
                        errorFlag = 1;
                    }
                    if (i == strlen(operand) - 1 && operand[i] == ',') {
                        errorFlag = 1;
                    }
                }
                if (commaCtr == 1 && operand[strlen(operand) - 1] != 'X') {
                    errorFlag = 1;
                }
                if (errorFlag) {
                    fclose(outputFile);
                    remove(filename);
                    char *error = "Invalid formatting of operand for given instruction!";
                    assemblyError(lineCtr, tempLine, error, 26);
                }
                if (!operand) {
                    fclose(outputFile);
                    remove(filename);
                    char *error = "Given instruction must have an operand!";
                    assemblyError(lineCtr, tempLine, error, 27);
                }
                if (commaCtr == 1) {
                    char *temps = calloc(7, sizeof(char));
                    memcpy(temps, &operand[0], commaInd);
                    int tempi = search(temps);
                    if (!symbolTable[tempi].name) {
                        fclose(outputFile);
                        remove(filename);
                        char *error = "Symbol in operand not found in symbol table!";
                        assemblyError(lineCtr, tempLine, error, 28);
                    }
                    objCode = symbolTable[search(temps)].address + 0x8000;
                    sprintf(temps, "%04X", objCode);
                    strcat(objCodeStr, temps);
                }
                else {
                    char *temps = calloc(7, sizeof(char));
                    int tempi = search(operand);
                    if (!symbolTable[tempi].name) {
                        fclose(outputFile);
                        remove(filename);
                        char *error = "Symbol in operand not found in symbol table!";
                        assemblyError(lineCtr, tempLine, error, 29);
                    }
                    objCode = symbolTable[tempi].address;
                    sprintf(temps, "%04X", objCode);
                    strcat(objCodeStr, temps);
                }
            }
            recordLen += objLen;
            strcat(record, objCodeStr);
        }
        if (strlen(record) == 60) {
            char text[70];
            text[0] = 'T';
            for (int i = 0; i < 6; i++) {
                text[i + 1] = recordLoc[i];
            }
            char recordLenStr[3];
            sprintf(recordLenStr, "%02X", recordLen);
            for (int i = 0; i < 2; i++) {
                text[i + 7] = recordLenStr[i];
            }
            for (int i = 0; i < 60; i++) {
                text[i + 9] = record[i];
            }
            text[69] = '\0';
            fprintf(outputFile, "%s\n", text);
            memset(text, 0, sizeof(text));
            memset(record, 0, strlen(record));
            sprintf(recordLoc, "%06X", locCtr);
            recordLen = 0;
        }
    }
    fclose(outputFile);
    for (int i = 0; i < size; i++) {
        if (symbolTable[i].name != NULL) {
            free(symbolTable[i].name);
        }
    }
    free(symbolTable);
    return 0;
}
