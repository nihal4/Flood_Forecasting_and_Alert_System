#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ALPHA 0.5   // Example constant for rainfall coefficient
#define BETA  0.01   // Example constant for temperature coefficient
#define GAMMA 5   // Example constant for base water level
#define THRESHOLD_WATER_LEVEL 19  // Example threshold for flood alert
#define CONVENTION_FACTOR 0.7 //to convert unit to meter

#define MAX_DATA_ENTRIES 10 // Maximum number of data entries

// Function to clear the terminal screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear");  // macOS/Linux
    #endif
}

// Function for case-insensitive comparison of strings
int compareLocations(const char *location1, const char *location2) {
    return strcasecmp(location1, location2);  // Case-insensitive comparison
}

//part of noor
// Structure to hold environmental data
typedef struct
{
    float rainfall;    // Rainfall in mm
    float temperature; // Temperature in degrees Celsius
    char location[50]; // Location name
} EnvironmentalData;

// Function to save environmental data to a file
void saveDataToFile(const EnvironmentalData *data, int count)
{
    FILE *file = fopen("data.txt", "a");  // Open file in append mode
    if (file == NULL)
    {
        printf("Error opening file for writing.\n");
        return;
    }
    // Write each entry to the file
    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s %.2f %.2f\n", data[i].location, data[i].rainfall, data[i].temperature);
    }
    fclose(file);
    printf("Data saved successfully to data.txt.\n");
}

// Function to load environmental data from a file
int loadDataFromFile(EnvironmentalData *data)
{
    FILE *file = fopen("data.txt", "r");  // Changed to consistent file name
    if (file == NULL)
    {
        printf("Error opening file for reading. File may not exist.\n");
        return 0; // Indicate no data loaded
    }

    int count = 0;
    // Read data until end of file or maximum entries
    while (fscanf(file, "%s %f %f", data[count].location, &data[count].rainfall, &data[count].temperature) == 3)
    {
        count++;
        if (count >= MAX_DATA_ENTRIES)
            break; // Prevent overflow
    }

    fclose(file);
    printf("Data loaded successfully. %d entries read.\n", count);
    return count; // Return the number of entries loaded
}


// Modified inputData function with location existence check
// Function to input data and append to the file
void inputData(EnvironmentalData *data, int *count)
{
    char newLocation[50];
    int locationExists = 0;

    // Load existing data from file
    EnvironmentalData existingData[MAX_DATA_ENTRIES];
    int existingCount = loadDataFromFile(existingData);

    // Copy existing data into the current array (if needed for other operations)
    for (int i = 0; i < existingCount; i++) {
        data[i] = existingData[i];
    }
    *count = existingCount;

    // Get location input from user
    printf("Enter location: ");
    scanf("%s", newLocation);

    // Check if location already exists in the loaded data (case-insensitive)
    for (int i = 0; i < existingCount; i++)
    {
        if (compareLocations(existingData[i].location, newLocation) == 0)
        {
            printf("Location '%s' already exists.\n", newLocation);
            locationExists = 1;
            break;
        }
    }

    // If location doesn't exist, proceed with data input
    if (!locationExists)
    {
        strcpy(data[*count].location, newLocation);
        printf("Enter rainfall (in mm): ");
        scanf("%f", &data[*count].rainfall);
        printf("Enter temperature (in degrees Celsius): ");
        scanf("%f", &data[*count].temperature);
        printf("Data inputted successfully for %s.\n", data[*count].location);

        // Append the new data to the file
        FILE *file = fopen("data.txt", "a");
        if (file != NULL)
        {
            fprintf(file, "%s %.2f %.2f\n", data[*count].location, data[*count].rainfall, data[*count].temperature);
            fclose(file);
        }
        else
        {
            printf("Error saving data.\n");
        }

        (*count)++; // Increment count of entries
    }
}

//update data
// Function for updating existing environmental data
void updateData(EnvironmentalData *data, int count) {
    char locationToEdit[50];
    int foundIndex = -1;

    // Prompt user to input the location they want to update
    printf("Enter the location name to update: ");
    scanf("%s", locationToEdit);

    // Search for the location in the data
    for (int i = 0; i < count; i++) {
        if (compareLocations(data[i].location, locationToEdit) == 0) {
            foundIndex = i;  // Location found, store the index
            break;
        }
    }

    if (foundIndex != -1) {
        // Update the existing data for the found location
        printf("Updating data for %s...\n", data[foundIndex].location);
        printf("Enter new rainfall (in mm): ");
        scanf("%f", &data[foundIndex].rainfall);
        printf("Enter new temperature (in degrees Celsius): ");
        scanf("%f", &data[foundIndex].temperature);
        printf("Data updated successfully for %s.\n", data[foundIndex].location);

        // Open the file for writing (this will overwrite the existing file)
        FILE *file = fopen("data.txt", "w");
        if (file == NULL) {
            printf("Error opening file for writing.\n");
            return;
        }

        // Write all data back to the file after updating
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %.2f %.2f\n", data[i].location, data[i].rainfall, data[i].temperature);
        }

        fclose(file);
        printf("Data saved successfully to the file.\n");
    } else {
        printf("Location '%s' not found in the data.\n", locationToEdit);
    }
}



////part of nihal
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
    EnvironmentalData data[MAX_DATA_ENTRIES];
    int count = 0;
    int choice;
    const char *input_file = "data.txt";
    const char *prediction_file = "predictions.txt";

    printf("Welcome to the Flood Prediction System\n");

    do {
        clearScreen();  // Clear the screen at the beginning of every loop iteration

        printf("\nMenu:\n");
        printf("1. Input environmental data\n");
        printf("2. Update predictions and alerts\n");
        printf("3. View alert status\n");
        printf("4. Update existing environmental data\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                clearScreen();
                inputData(data, &count);  // Take environmental data as input
                break;
            case 2:
                clearScreen();
                update_predictions(input_file, prediction_file);  // Update predictions and alert status
                printf("Predictions and alerts updated.\n");
                break;
            case 3:
                clearScreen();
                view_alert(prediction_file);  // Display the alert table
                break;
            case 4:
                clearScreen();
                updateData(data, count);  // Update existing environmental data
                break;
            case 5:
                printf("Exiting the system.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

        if (choice != 5) {
            printf("\nPress Enter to continue...");
            getchar();  // Consume the newline character from previous input
            getchar();  // Wait for user to press Enter before clearing
        }

    } while (choice != 5);

    return 0;
}
