#include <stdio.h>
#include <string.h>


int main() {
    FILE *inputFile = fopen("input.txt", "r");
    FILE *outputFile = fopen("output.txt", "w");
    char lines[10][10], temp[10];
    int count = 0;

    if (inputFile == NULL || outputFile == NULL) {
        return 1;
    }

    while (fgets(lines[count], 10, inputFile)) {
        size_t len = strlen(lines[count]);
        if (len > 0 && lines[count][len - 1] == '\n') {
            lines[count][len - 1] = '\0';
        }
        count++;
        if (count >= 10) {
            break;
        }
    }
    fclose(inputFile);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(lines[i], lines[j]) > 0) {
                strcpy(temp, lines[i]);
                strcpy(lines[i], lines[j]);
                strcpy(lines[j], temp);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        fprintf(outputFile, "%s\n", lines[i]);
    }
    fclose(outputFile);
    return 0;
}
