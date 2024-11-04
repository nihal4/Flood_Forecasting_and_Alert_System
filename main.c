#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHA 0.5   // Example constant for rainfall coefficient
#define BETA  0.01   // Example constant for temperature coefficient
#define GAMMA 5   // Example constant for base water level
#define THRESHOLD_WATER_LEVEL 19  // Example threshold for flood alert
#define CONVENTION_FACTOR 0.7

// Function to calculate water level based on the given formula
float calculate_water_level(float rainfall, float temperature) {
    float water_level =(ALPHA * rainfall) + (BETA * temperature) + GAMMA;
    return  water_level * CONVENTION_FACTOR;

}

// Function to update predictions and alert status in a new prediction file
void update_predictions(const char* input_file, const char* output_file) {
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Error: Could not open input file.\n");
        return;
    }

    // Open a new file for writing the predictions
    FILE *prediction_file = fopen(output_file, "w");
    if (prediction_file == NULL) {
        printf("Error: Could not create prediction file.\n");
        fclose(file);
        return;
    }

    char area_name[50];
    float rainfall, temperature, predicted_water_level;
    char alert_status[10];  // To store "Alert ON" or "Alert OFF"

    // Reading each line from the input file
    while (fscanf(file, "%s %f %f", area_name, &rainfall, &temperature) == 3) {
        // Calculate water level using the formula
        predicted_water_level = calculate_water_level(rainfall, temperature);

        // Decide alert status based on the water level threshold
        if (predicted_water_level > THRESHOLD_WATER_LEVEL) {
            strcpy(alert_status, "ON");
        } else {
            strcpy(alert_status, "OFF");
        }

        // Write the updated data to the prediction file
        fprintf(prediction_file, "%s %.2f %.2f %.2f %s\n", area_name, rainfall, temperature, predicted_water_level, alert_status);
    }

    // Close both files
    fclose(file);
    fclose(prediction_file);
}

// Function to display the alert table from the prediction file
void view_alert(const char* prediction_file) {
    FILE *file = fopen(prediction_file, "r");
    if (file == NULL) {
        printf("Error: Could not open prediction file.\n");
        return;
    }

    char area_name[50], alert_status[10];
    float rainfall, temperature, predicted_water_level;

    // Print the table header
    printf("%-15s %-15s %-15s %-25s %-10s\n", "Area", "Rainfall(mm)", "Temperature(C)", "Predicted Water Level(M)", "Alert");

    // Line separator for the table header
    printf("-------------------------------------------------------------------\n");

    // Reading each line from the prediction file
    while (fscanf(file, "%s %f %f %f %s", area_name, &rainfall, &temperature, &predicted_water_level, alert_status) == 5) {
        // Print the formatted data row
        printf("%-15s %-15.2f %-15.2f %-25.2f %-10s\n", area_name, rainfall, temperature, predicted_water_level, alert_status);
    }

    fclose(file);
}

int main() {
    const char *input_file = "data.txt";       // File containing area name, rainfall, and temperature
    const char *prediction_file = "predictions.txt"; // File to store predictions

    // First, update the predictions and write them to the prediction file
    update_predictions(input_file, prediction_file);

    // Then, display the predictions in a table format
    view_alert(prediction_file);

    return 0;
}
