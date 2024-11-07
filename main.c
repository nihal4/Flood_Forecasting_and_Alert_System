#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHA 0.5
/*
This represents the sensitivity of water levels to rainfall.
In Bangladesh, heavy monsoon rains can rapidly raise river levels.
An approximate value might range between 0.5 to 1.5 meters per mm of rainfall,
depending on the location and catchment area.
*/
#define BETA 0.01
/*
While temperature directly affects evaporation rates,
it may not have as large an effect on immediate water levels in rivers,
but it does affect reservoir levels over time. For Bangladesh,
a small coefficient might be reasonable, such as 0.01 to 0.05 meters per degree Celsius.
*/

#define GAMMA 5
/*
This constant can represent an average base level of water in the river
or reservoir when rainfall and temperature effects are minimal.
In Bangladesh, baseline levels vary by river and season,
but for the monsoon season, a typical baseline could range from 5 to 10 meters
*/
#define THRESHOLD_WATER_LEVEL 19
/*
The Danger Level is defined as 19.05 meters,
indicating a threshold beyond which the water level may pose risks, such as flooding.
*/

#define CONVENTION_FACTOR 0.7    // to convert unit to meter
/*
If the model outputs a water level in "units" and,
on average, 1 model unit corresponds to 1.5 meters
at a particular station on the Brahmaputra, then the conversion
factor would be 1.5 meters per unit. but we are setting to 0.7 for 
our easier calculation.
*/

#define MAX_DATA_ENTRIES 100 // the maximum number of location data i can enter.

#define MAX_USERS 100          // value max number of user name for admin and user
#define MAX_NAME_LENGTH 50     // maximum size for name for admin and user
#define MAX_PASSWORD_LENGTH 50 // maximum password size;
#define MAX_LOCATION_LENGTH 50 // maximum location name size;

// strcutre for 
typedef struct
{
    char username[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char location[MAX_LOCATION_LENGTH];
} User;

typedef struct
{
    char adminID[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} Admin;

// Admin admin;

Admin admins[MAX_USERS];
int adminCount = 0;

User users[MAX_USERS];
int userCount = 0;

// for forcast
//#define MAX_LOCATION_LENGTH 50
// #define MAX_DATA_ENTRIES 100

// Structure to hold forecast data
typedef struct
{
    char location[MAX_LOCATION_LENGTH];
    float temperature;
    float rainfall;
    float waterLevel;
    char alertStatus[10]; // "On" or "Off"
} ForecastData;

// Function to clear the terminal screen
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // macOS/Linux
#endif
}

// Function for case-insensitive comparison of strings
int compareLocations(const char *location1, const char *location2)
{
    return strcasecmp(location1, location2); // Case-insensitive comparison
}

// part of noor
//  Structure to hold environmental data
typedef struct
{
    float rainfall;    // Rainfall in mm
    float temperature; // Temperature in degrees Celsius
    char location[50]; // Location name
} EnvironmentalData;

EnvironmentalData data[MAX_DATA_ENTRIES];        // Array to store environmental data
int count = 0;                                   // To track number of entries
const char *input_file = "data.txt";             // File for environmental data
const char *prediction_file = "predictions.txt"; // File for predictions

// Function to save environmental data to a file
void saveDataToFile(const EnvironmentalData *data, int count)
{
    FILE *file = fopen("data.txt", "a"); // Open file in append mode
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
    FILE *file = fopen("data.txt", "r"); // Changed to consistent file name
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

// EnvironmentalData existingData[MAX_DATA_ENTRIES];

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
    for (int i = 0; i < existingCount; i++)
    {
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

// update data
//  Function for updating existing environmental data
void updateData(EnvironmentalData *data, int count)
{
    // char locationToEdit[50];
    int foundIndex = -1;

    char newLocation[50];

    // Load existing data from file
    EnvironmentalData existingData[MAX_DATA_ENTRIES];
    int existingCount = loadDataFromFile(existingData);

    // Copy existing data into the current array (if needed for other operations)
    for (int i = 0; i < existingCount; i++)
    {
        data[i] = existingData[i];
    }
    count = existingCount;

    // Get location input from user
    printf("Enter the location name to update: ");
    scanf("%s", newLocation);

    // Check if location already exists in the loaded data (case-insensitive)
    for (int i = 0; i < existingCount; i++)
    {
        if (compareLocations(existingData[i].location, newLocation) == 0)
        {
            // printf("Location '%s' already exists.\n", newLocation);
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1)
    {
        // Update the existing data for the found location
        printf("Updating data for %s...\n", data[foundIndex].location);
        printf("Enter new rainfall (in mm): ");
        scanf("%f", &data[foundIndex].rainfall);
        printf("Enter new temperature (in degrees Celsius): ");
        scanf("%f", &data[foundIndex].temperature);
        printf("Data updated successfully for %s.\n", data[foundIndex].location);

        // Open the file for writing (this will overwrite the existing file)
        FILE *file = fopen("data.txt", "w");
        if (file == NULL)
        {
            printf("Error opening file for writing.\n");
            return;
        }

        // Write all data back to the file after updating
        for (int i = 0; i < count; i++)
        {
            fprintf(file, "%s %.2f %.2f\n", data[i].location, data[i].rainfall, data[i].temperature);
        }

        fclose(file);
        printf("Data saved successfully to the file.\n");
    }
    else
    {
        printf("Location '%s' not found in the data.\n", newLocation);
    }
}

////part of nihal
// Function to calculate water level based on the given formula
float calculate_water_level(float rainfall, float temperature)
{
    float water_level = (ALPHA * rainfall) + (BETA * temperature) + GAMMA;
    return water_level * CONVENTION_FACTOR;
}

// Function to update predictions and alert status in a new prediction file
void update_predictions(const char *input_file, const char *output_file)
{
    FILE *file = fopen(input_file, "r");
    if (file == NULL)
    {
        printf("Error: Could not open input file.\n");
        return;
    }

    // Open a new file for writing the predictions
    FILE *prediction_file = fopen(output_file, "w");
    if (prediction_file == NULL)
    {
        printf("Error: Could not create prediction file.\n");
        fclose(file);
        return;
    }

    char area_name[50];
    float rainfall, temperature, predicted_water_level;
    char alert_status[10]; // To store "Alert ON" or "Alert OFF"

    // Reading each line from the input file
    while (fscanf(file, "%s %f %f", area_name, &rainfall, &temperature) == 3)
    {
        // Calculate water level using the formula
        predicted_water_level = calculate_water_level(rainfall, temperature);

        // Decide alert status based on the water level threshold
        if (predicted_water_level > THRESHOLD_WATER_LEVEL)
        {
            strcpy(alert_status, "ON");
        }
        else
        {
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
void view_alert(const char *prediction_file)
{
    FILE *file = fopen(prediction_file, "r");
    if (file == NULL)
    {
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
    while (fscanf(file, "%s %f %f %f %s", area_name, &rainfall, &temperature, &predicted_water_level, alert_status) == 5)
    {
        // Print the formatted data row
        printf("%-15s %-15.2f %-15.2f %-25.2f %-10s\n", area_name, rainfall, temperature, predicted_water_level, alert_status);
    }

    fclose(file);
}

// login_ UI afrim part
void loadAdminFromFile()
{
    FILE *file = fopen("admins.txt", "r");
    if (file != NULL)
    {
        // Read all admins from file
        while (fscanf(file, "%s %s", admins[adminCount].adminID, admins[adminCount].password) != EOF)
        {
            adminCount++; // Increment the count of loaded admins
            if (adminCount >= MAX_USERS)
            {
                printf("Maximum number of admins reached.\n");
                break;
            }
        }
        fclose(file);
    }
    else
    {
        printf("Error loading admins from file.\n");
    }
}

void loadUsersFromFile()
{
    FILE *file = fopen("users.txt", "r");
    if (file != NULL)
    {
        while (fscanf(file, "%s %s %s", users[userCount].username, users[userCount].password, users[userCount].location) != EOF)
        {
            userCount++;
        }
        fclose(file);
    }
}

// save admin
void saveAdminToFile()
{
    FILE *file = fopen("admins.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "%s %s\n", admins[adminCount].adminID, admins[adminCount].password);
        fclose(file);
    }
    else
    {
        printf("Error saving admin data.\n");
    }
}

// Function to register a new admin
void registerAdmin()
{
    char adminID[MAX_NAME_LENGTH];
    int adminIDTaken = 0;

    printf("Registering new Admin...\n");
    printf("Enter admin ID: ");
    scanf("%s", adminID);

    // Check if the admin ID already exists
    for (int i = 0; i < adminCount; i++)
    {
        if (strcmp(admins[i].adminID, adminID) == 0)
        {
            adminIDTaken = 1;
            break;
        }
    }

    if (adminIDTaken)
    {
        printf("Admin ID '%s' is already taken. Please choose a different admin ID.\n", adminID);
        return; // Exit the function if the admin ID is taken
    }

    // If admin ID is not taken, proceed with registration
    strcpy(admins[adminCount].adminID, adminID);
    printf("Enter password: ");
    scanf("%s", admins[adminCount].password);

    saveAdminToFile();
    adminCount++;
    printf("Admin registered successfully!\n");
}

int authenticateAdmin()
{
    char adminID[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    printf("Enter admin ID: ");
    scanf("%s", adminID);
    printf("Enter password: ");
    scanf("%s", password);

    // Iterate through all admins and check for matching ID and password
    for (int i = 0; i < adminCount; i++)
    {
        if (strcmp(adminID, admins[i].adminID) == 0 && strcmp(password, admins[i].password) == 0)
        {
            return 1; // Authentication successful
        }
    }

    return 0; // Authentication failed
}

// admin menu
void adminMenu()
{
    int choice;

    do
    {
        clearScreen();
        printf("\nAdmin Menu:\n");
        printf("1. Input environmental data\n");
        printf("2. Update predictions and alerts\n");
        printf("3. View alert status\n");
        printf("4. Update existing environmental data\n");
        printf("5. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            clearScreen();
            inputData(data, &count); // Take environmental data as input
            break;
        case 2:
            clearScreen();
            update_predictions(input_file, prediction_file); // Update predictions and alert status
            printf("Predictions and alerts updated.\n");
            break;
        case 3:
            clearScreen();
            view_alert(prediction_file); // Display the alert table
            break;
        case 4:
            clearScreen();
            updateData(data, count); // Update existing environmental data
            break;
        case 5:
            printf("Logging out...\n");
            return; // Exit the loop and return to the main menu or authentication
        default:
            printf("Invalid choice. Please try again.\n");
        }

        if (choice != 5)
        {
            printf("\nPress Enter to continue...");
            getchar(); // Consume the newline character from previous input
            getchar(); // Wait for user to press Enter before clearing
        }
    } while (choice != 5);
}

// User part
void saveUserToFile()
{
    FILE *file = fopen("users.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "%s %s %s\n", users[userCount].username, users[userCount].password, users[userCount].location);
        fclose(file);
    }
    else
    {
        printf("Error saving user data.\n");
    }
}

// register user

// Function to register a new user
void registerUser()
{
    char username[MAX_NAME_LENGTH];
    int usernameTaken = 0;

    printf("Registering new User...\n");
    printf("Enter username: ");
    scanf("%s", username);

    // Check if the username already exists
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            usernameTaken = 1;
            break;
        }
    }

    if (usernameTaken)
    {
        printf("Username '%s' is already taken. Please choose a different username.\n", username);
        return; // Exit the function if the username is taken
    }

    // If username is not taken, proceed with registration
    strcpy(users[userCount].username, username);
    printf("Enter password: ");
    scanf("%s", users[userCount].password);
    printf("Enter location: ");
    scanf("%s", users[userCount].location);

    saveUserToFile();
    userCount++;
    printf("User registered successfully!\n");
}

// login user
int authenticateUser()
{
    char username[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
        {
            return 1; // User authenticated
        }
    }
    return 0; // Authentication failed
}

void showForecast(const char *location)
{
    FILE *file = fopen("predictions.txt", "r");
    if (file == NULL)
    {
        printf("Error opening predictions file.\n");
        return;
    }

    ForecastData data;
    int found = 0;

    // Read each line of the file and search for the location
    while (fscanf(file, "%s %f %f %f %s", data.location, &data.temperature, &data.rainfall, &data.waterLevel, data.alertStatus) != EOF)
    {
        if (strcasecmp(data.location, location) == 0)
        { // Case-insensitive comparison
            clearScreen();
            found = 1;
            printf("Forecast for location: %s\n", data.location);
            printf("Temperature: %.2f°C\n", data.temperature);
            printf("Rainfall: %.2f mm\n", data.rainfall);
            printf("Water Level: %.2f m\n", data.waterLevel);
            printf("Alert: %s\n", data.alertStatus);
            break;
        }
    }

    if (!found)
    {
        printf("Location '%s' not found in the forecast data.\n", location);
    }

    fclose(file);
}

void searchOtherLocation()
{
    char location[MAX_LOCATION_LENGTH];
    printf("Enter location to search: ");
    scanf("%s", location);
    showForecast(location);
}

// after login
void userMenu()
{
    int choice;

    do
    {
        clearScreen();
        printf("\nUser Menu:\n");
        printf("1. Show forecast for my location\n");
        printf("2. Search for forecast in other location\n");
        printf("3. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            clearScreen();
            showForecast(users[userCount - 1].location); // Using last registered user for simplicity
            break;
        case 2:
            clearScreen();
            searchOtherLocation();
            break;
        case 3:
            return;
        default:
            printf("Invalid choice. Please try again.\n");
        }

        if (choice != 3)
        {
            printf("\nPress Enter to continue...");
            getchar(); // Consume the newline character from previous input
            getchar(); // Wait for user to press Enter before clearing
        }
    } while (choice != 3);
}

void loginPage()
{
    int choice;

    do
    {
        clearScreen();
        printf("Welcome to the Login System!\n");
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Register as Admin\n");
        printf("4. Register as User\n");
        printf("5. Exit program\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            clearScreen();
            if (authenticateAdmin())
            {
                // clearScreen();
                adminMenu();
            }
            else
            {
                printf("Invalid admin credentials.\n");
            }
            break;
        case 2:
            clearScreen();
            if (authenticateUser())
            {
                clearScreen();
                userMenu();
            }
            else
            {
                printf("Invalid user credentials.\n");
            }
            break;
        case 3:
            clearScreen();
            registerAdmin();
            break;
        case 4:
            clearScreen();
            registerUser();
            break;
        case 5:
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }

        if (choice != 5)
        {
            printf("\nPress Enter to continue...");
            getchar(); // Consume the newline character from previous input
            getchar(); // Wait for user to press Enter before clearing
        }
    } while (choice != 5);
}

int main()
{
    // Load data from files if needed
    loadAdminFromFile();
    loadUsersFromFile();

    clearScreen();
    loginPage();

    return 0;
}
