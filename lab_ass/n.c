#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *inputFile, *outputFile;
    char ch;
    long fileSize;

    inputFile = fopen("input.txt", "w");
    fprintf(inputFile, "Hello World! This is a test.");
    fclose(inputFile);

    inputFile = fopen("input.txt", "r");
    outputFile = fopen("output.txt", "w");

    fseek(inputFile, 0, SEEK_END);
    fileSize = ftell(inputFile);

    for (long i = fileSize - 1; i >= 0; i--) {
        fseek(inputFile, i, SEEK_SET);
        ch = fgetc(inputFile);
        fputc(ch, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
