#include <stdio.h>
#include <unistd.h>  // For usleep (or use Sleep on Windows)

void loadingDotsAnimation(int duration, char str[10])
{
    printf("\033[1;32m");  // Set text color to green (optional for styling)
    printf("%s", str);

    for (int t = 0; t < duration; t++)  // Duration controls how long the animation runs
    {
        // Print dots based on the current cycle
        for (int dot = 1; dot <= 3; dot++)
        {
            printf(".");
            fflush(stdout);  // Force the output to display immediately
            sleep(1);  // Sleep for 500 milliseconds (0.5 seconds)
        }

        // Erase the dots by printing backspaces (to reset the line)
        for (int dot = 1; dot <= 3; dot++)
        {
            printf("\b \b");  // Print backspace and a space to clear
        }
    }

    printf("\033[0m");  // Reset text color
    printf("\nLoading complete!\n");
}

int main()
{
    loadingDotsAnimation(2, "nihal");  // Display loading dots animation for 10 cycles
    return 0;
}

