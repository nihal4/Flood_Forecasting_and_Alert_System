#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <termios.h>  // For terminal settings (to hide password input)

#ifdef _WIN32
#include <windows.h>  // For Windows
#else
#include <unistd.h>   // For macOS and Unix-based systems
#endif


////////////// CONSTANT ////////////////////

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

#define CONVENTION_FACTOR 0.7 // to convert unit to meter
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

////////////////// END /////////////////////////

////////////// STRCUTRE FOR COMPLEX DATA HANDELING //////////////////////

// structre to store data for user register and login
typedef struct
{
    char username[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char location[MAX_LOCATION_LENGTH];
} User;

// strcutre to store data for admin register and login
typedef struct
{
    char adminID[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} Admin;

// strcuture for store data while forcast for user
typedef struct
{
    char location[MAX_LOCATION_LENGTH];
    float temperature;
    float rainfall;
    float waterLevel;
    char alertStatus[10];
} ForecastData;

// strcture for store envermental data while taking input
typedef struct
{
    float rainfall;
    float temperature;
    char location[50];
} EnvironmentalData;

//////////////////////////// END /////////////////////////////

////////////////////// INITIALIZING ///////////////////////////

// setting array for admin
Admin admins[MAX_USERS];
int adminCount = 0; // to track number of admin created

// setting array for user
User users[MAX_USERS];
int userCount = 0; // to track number of user created

// array for envermental data
EnvironmentalData data[MAX_DATA_ENTRIES];
int count = 0; // to track number of data entered

const char *input_file = "data.txt";             // File for environmental data
const char *prediction_file = "predictions.txt"; // File for predictions

////////////////// END ////////////////////////

///////////// EXTRA DEPENDCY CUSTOM FUNCTION ///////////

// Function to clear the terminal screen
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // macOS/Linux
#endif
}

// delay on screen
void delay()
{
#ifdef _WIN32
    Sleep(1000); // Windows: Delay for 2000 milliseconds (2 seconds)
#else
    sleep(1); // macOS/Unix: Delay for 2 seconds
#endif
}

// Function for case-insensitive comparison of strings
int compareLocations(const char *location1, const char *location2)
{
    return strcasecmp(location1, location2); // Case-insensitive comparison
}
///////////////// END ///////////////////////////

///////////////////////// 232-35-048/////////////////////////////////////////////////////////////////////////////////////////////

//////////// code for load and saving file/////////////////

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
    printf("\033[1;32m"); // Green for success
    printf("Data saved successfully to data.txt.\n");
    printf("\033[0m"); // Reset color
    
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
    printf("\033[1;32m"); // Green for success
    printf("Data loaded successfully. %d entries read.\n", count);
    printf("\033[0m"); // Reset color
    
    return count; // Return the number of entries loaded
}
///////////////////// END////// END//////////////////////////////

//////// CODE FOR TAKE INPUT////////////

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
    printf("\033[1;34m"); // Blue for input prompts
    printf("Enter location: ");
    printf("\033[0m"); // Reset color

    printf("\033[1;33m");
    scanf("%s", newLocation);
    printf("\033[0m");

    // Check if location already exists in the loaded data (case-insensitive)
    for (int i = 0; i < existingCount; i++)
    {
        if (compareLocations(existingData[i].location, newLocation) == 0)
        {
            printf("\033[1;31m"); // Red for error
            printf("Location '%s' already exists.\n", newLocation);
            printf("\033[0m"); // Reset color
            locationExists = 1;
            break;
        }
    }

    // If location doesn't exist, proceed with data input
    if (!locationExists)
    {
        strcpy(data[*count].location, newLocation);
        printf("\033[1;34m"); // Blue for input prompts
        printf("Enter rainfall (in mm): ");
        printf("\033[0m"); // Reset color

        printf("\033[1;33m");
        scanf("%f", &data[*count].rainfall);
        printf("\033[0m");

        printf("\033[1;34m"); // Blue for input prompts
        printf("Enter temperature (in degrees Celsius): ");
        printf("\033[0m"); // Reset color

        printf("\033[1;33m");
        scanf("%f", &data[*count].temperature);
        printf("\033[0m");

        printf("\033[1;32m"); // Green for success message
        printf("Data inputted successfully for %s.\n", data[*count].location);
        printf("\033[0m"); // Reset color

        // Append the new data to the file
        FILE *file = fopen("data.txt", "a");
        if (file != NULL)
        {
            fprintf(file, "%s %.2f %.2f\n", data[*count].location, data[*count].rainfall, data[*count].temperature);
            fclose(file);
        }
        else
        {
            printf("\033[1;31m"); // Red for error
            printf("Error saving data.\n");
            printf("\033[0m"); // Reset color
        }

        (*count)++; // Increment count of entries
    }
}


///////////////// END //////////////

///////////// FOR UPDATE ENV DATA /////////////
void updateData(EnvironmentalData *data, int count)
{
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
    printf("\033[1;34m"); // Blue color for prompt
    printf("Enter the location name to update: ");
    printf("\033[0m"); // Reset color

    printf("\033[1;33m");
    scanf("%s", newLocation);
    printf("\033[0m");

    // Check if location already exists in the loaded data (case-insensitive)
    for (int i = 0; i < existingCount; i++)
    {
        if (compareLocations(existingData[i].location, newLocation) == 0)
        {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1)
    {
        // Update the existing data for the found location
        printf("\033[1;32m"); // Green for success
        printf("Updating data for %s...\n", data[foundIndex].location);
        printf("\033[0m"); // Reset color
        printf("\033[1;34m");
        printf("Enter new rainfall (in mm): ");
        printf("\033[0m");
        printf("\033[1;33m");
        scanf("%f", &data[foundIndex].rainfall);
        printf("\033[1;0m");
        printf("\033[1;34m");
        printf("Enter new temperature (in degrees Celsius): ");
        printf("\033[0m");
        printf("\033[1;33m");
        scanf("%f", &data[foundIndex].temperature);
        printf("\033[0m");
        printf("\033[1;32m"); // Green for success
        printf("Data updated successfully for %s.\n", data[foundIndex].location);
        printf("\033[0m"); // Reset color

        // Open the file for writing (this will overwrite the existing file)
        FILE *file = fopen("data.txt", "w");
        if (file == NULL)
        {
            printf("\033[1;31m"); // Red for error
            printf("Error opening file for writing.\n");
            printf("\033[0m"); // Reset color
            return;
        }

        // Write all data back to the file after updating
        for (int i = 0; i < count; i++)
        {
            fprintf(file, "%s %.2f %.2f\n", data[i].location, data[i].rainfall, data[i].temperature);
        }

        fclose(file);
        printf("\033[1;32m"); // Green for success
        printf("Data saved successfully to the file.\n");
        printf("\033[0m"); // Reset color
    }
    else
    {
        printf("\033[1;31m"); // Red for error
        printf("Location '%s' not found in the data.\n", newLocation);
        printf("\033[0m"); // Reset color
    }
}


//////////////////////////// END ///////////////////////////

///////////////// SHOW FORCAST TO USER LOCATION /////////////////

void showForecast(const char *location)
{
    FILE *file = fopen("predictions.txt", "r");
    if (file == NULL)
    {
        printf("\033[1;31m"); // Red for error message
        printf("Error opening predictions file.\n");
        printf("\033[0m"); // Reset color
        return;
    }

    ForecastData data;
    int found = 0;

    // Read each line of the file and search for the location
    while (fscanf(file, "%s %f %f %f %s", data.location, &data.rainfall, &data.temperature, &data.waterLevel, data.alertStatus) != EOF)
    {
        if (strcasecmp(data.location, location) == 0)
        { // Case-insensitive comparison
            clearScreen();
            found = 1;

            // Forecast section with green for location name and blue for other details
            printf("\033[1;32m"); // Green for location name
            printf("Forecast for location: %s\n", data.location);
            printf("\033[1;34m"); // Blue for other data
            printf("Temperature: %.2f°C\n", data.temperature);
            printf("Rainfall: %.2f mm\n", data.rainfall);
            printf("Water Level: %.2f m\n", data.waterLevel);
            
            // Color change for alert status: red for "ON", green for "OFF"
            if (strcasecmp(data.alertStatus, "ON") == 0)
            {
                printf("\033[1;31m"); // Red for alert ON
            }
            else
            {
                printf("\033[1;32m"); // Green for alert OFF
            }
            printf("Alert: %s\n", data.alertStatus);
            printf("\033[0m"); // Reset color
            break;
        }
    }

    if (!found)
    {
        // If location not found, show error in red
        printf("\033[1;31m"); // Red for error message
        printf("Location '%s' not found in the forecast data.\n", location);
        printf("\033[0m"); // Reset color
    }

    fclose(file);
}


//////////////////// SHOW FORCAST TO SEARCH LOCATION //////////////
void searchOtherLocation()
{
    char location[MAX_LOCATION_LENGTH];

    // Enhance the input prompt color with blue
    printf("\033[1;34m"); // Blue for the prompt text
    printf("Enter location to search: ");
    printf("\033[0m"); // Reset color

    // Using scanf for input
    scanf("%s", location);

    // Call showForecast with cyan text color for the output
    printf("\033[1;36m"); // Cyan for the forecast output
    showForecast(location);
    printf("\033[0m"); // Reset color after displaying forecast
}


////////////// END //////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 232-35-002 /////////////////////////////////////////////////////////////////

////////////////// PREDICTION ALGORITHOM//////////////////////

// Function to calculate water level based on the given formula
float calculate_water_level(float rainfall, float temperature)
{
    float water_level = (ALPHA * rainfall) + (BETA * temperature) + GAMMA;
    return water_level * CONVENTION_FACTOR;
}

///////////////////////// END /////////////////////////////

////////////////// UPDATE PREDICTIONS WITH NEW DATA//////////////////////

// Function to update predictions and alert status in a new prediction file
void update_predictions(const char *input_file, const char *output_file)
{
    FILE *file = fopen(input_file, "r");
    if (file == NULL)
    {
        printf("\033[1;31m"); // Red for error message
        printf("Error: Could not open prediction file.\n");
        printf("\033[0m"); // Reset color
        return;
    }

    // Open a new file for writing the predictions
    FILE *prediction_file = fopen(output_file, "w");
    if (prediction_file == NULL)
    {   
        printf("\033[1;31m"); // Red for error message
        printf("Error: Could not create prediction file.\n");
        printf("\033[0m"); // Reset color
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

//////////////////////////// END ///////////////////////////////////////

//////////////// DISPLAY ALERT FOR ADMIN ///////////////////////////////////////

// Function to display the alert table from the prediction file
void view_alert(const char *prediction_file)
{
    FILE *file = fopen(prediction_file, "r");
    if (file == NULL)
    {
        printf("\033[1;31m"); // Red for error message
        printf("Error: Could not open prediction file.\n");
        printf("\033[0m"); // Reset color
        return;
    }

    char area_name[50], alert_status[10];
    float rainfall, temperature, predicted_water_level;

    // Print the table header
    printf("\033[1;34m"); // Blue for header text
    printf("%-15s %-15s %-15s %-25s %-10s\n", "Area", "Rainfall(mm)", "Temperature(C)", "Predicted Water Level(M)", "Alert");
    printf("--------------------------------------------------------------------------------\n");
    printf("\033[0m"); // Reset color

    // Reading each line from the prediction file
    while (fscanf(file, "%s %f %f %f %s", area_name, &rainfall, &temperature, &predicted_water_level, alert_status) == 5)
    {
        // Color code the alert status (Green for safe, Red for alert)
        if (strcmp(alert_status, "OFF") == 0)
        {
            printf("\033[1;32m"); // Green for safe
        }
        else if (strcmp(alert_status, "ON") == 0)
        {
            printf("\033[1;31m"); // Red for alert
        }

        // Print the formatted data row
        printf("%-15s %-15.2f %-15.2f %-25.2f %-10s\n", area_name, rainfall, temperature, predicted_water_level, alert_status);

        printf("\033[0m"); // Reset color after each row
    }

    fclose(file);
}



//////////////////////////////// END ///////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// 232-35-076//////////////////////////////////////////////////////////////////////////////////

///////////////////// LOGIN REGISTER LOAD FILE  ////////////////////

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
                printf("\033[1;33m"); // Yellow for warning message
                printf("Maximum number of admins reached.\n");
                printf("\033[0m"); // Reset color
                break;
            }
        }
        fclose(file);
        printf("\033[1;32m"); // Green for success message
        printf("Admins loaded successfully.\n");
        printf("\033[0m"); // Reset color
    }
    else
    {
        printf("\033[1;31m"); // Red for error message
        printf("Error loading admins from file.\n");
        printf("\033[0m"); // Reset color
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

///////////////////////// END ////////////////////////////

///////////////////LOGIN REGISTER SAVE FILE/////////////////
// save admin
void saveAdminToFile()
{
    FILE *file = fopen("admins.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "%s %s\n", admins[adminCount].adminID, admins[adminCount].password);
        fclose(file);
        printf("\033[1;32m"); // Green for success message
        printf("Admin data saved successfully!\n");
        printf("\033[0m"); // Reset color
    }
    else
    {
        printf("\033[1;31m"); // Red for error message
        printf("Error saving admin data.\n");
        printf("\033[0m"); // Reset color
    }
}


// User part
void saveUserToFile()
{
    FILE *file = fopen("users.txt", "a");
    if (file != NULL)
    {
        fprintf(file, "%s %s %s\n", users[userCount].username, users[userCount].password, users[userCount].location);
        fclose(file);
        printf("\033[1;32m"); // Green for success message
        printf("User data saved successfully!\n");
        printf("\033[0m"); // Reset color
    }
    else
    {
        printf("\033[1;31m"); // Red for error message
        printf("Error saving user data.\n");
        printf("\033[0m"); // Reset color
    }
}


///////////////////////// END //////////////////////

//////////////////////// REGISTER ADMIN AND USER////////////////////

// Function to register a new admin
void registerAdmin()
{
    char adminID[MAX_NAME_LENGTH];
    int adminIDTaken = 0;

    // UI Styling for admin registration
    printf("\033[1;34m"); // Blue color for the registration title
    printf("*********************************\n");
    printf("**     ADMIN REGISTRATION      **\n");
    printf("*********************************\n");
    printf("\033[0m"); // Reset color

    printf("\033[1;33m"); // Yellow for input prompts
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
        printf("\033[1;31m"); // Red for error
        printf("Admin ID '%s' is already taken. Please choose a different admin ID.\n", adminID);
        printf("\033[0m"); // Reset color
        return; // Exit the function if the admin ID is taken
    }

    // If admin ID is not taken, proceed with registration
    strcpy(admins[adminCount].adminID, adminID);
    printf("Enter password: ");
    scanf("%s", admins[adminCount].password);

    saveAdminToFile();
    adminCount++;

    printf("\033[1;32m"); // Green for success message
    printf("Admin registered successfully!\n");
    printf("\033[0m"); // Reset color
}


// Function to register a new user
void registerUser()
{
    char username[MAX_NAME_LENGTH];
    int usernameTaken = 0;

    // UI Styling for user registration
    printf("\033[1;34m"); // Blue color for the registration title
    printf("*********************************\n");
    printf("**    USER REGISTRATION        **\n");
    printf("*********************************\n");
    printf("\033[0m"); // Reset color

    printf("\033[1;33m"); // Yellow for input prompts
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
        printf("\033[1;31m"); // Red for error
        printf("Username '%s' is already taken. Please choose a different username.\n", username);
        printf("\033[0m"); // Reset color
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

    printf("\033[1;32m"); // Green for success message
    printf("User registered successfully!\n");
    printf("\033[0m"); // Reset color
}


/////////////////////////////// END //////////////////////////////

///////////////////// LOGIN USER AND ADMIN////////////////////////

int authenticateAdmin()
{
    char adminID[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    
    // UI Styling for the login page
    printf("\033[1;34m"); // Blue color for the login title
    printf("*********************************\n");
    printf("**        ADMIN LOGIN          **\n");
    printf("*********************************\n");
    printf("\033[0m"); // Reset color

    printf("\033[1;33m"); // Yellow color for input prompts
    printf("Enter admin ID: ");
    scanf("%s", adminID);
    printf("Enter password: ");
    scanf("%s", password);

    // Iterate through all admins and check for matching ID and password
    for (int i = 0; i < adminCount; i++)
    {
        if (strcmp(adminID, admins[i].adminID) == 0 && strcmp(password, admins[i].password) == 0)
        {
            printf("\033[1;32m"); // Green color for success
            printf("Admin authentication successful! Welcome, Admin %s.\n", adminID);
            printf("\033[0m"); // Reset color
            return 1; // Authentication successful
        }
    }

    printf("\033[1;31m"); // Red color for failure
    printf("Invalid admin ID or password. Please try again.\n");
    printf("\033[0m"); // Reset color
    return 0; // Authentication failed
}


// login user
int authenticateUser()
{
    char username[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    printf("\033[1;34m"); // Blue color for login title
    printf("*********************************\n");
    printf("**        USER LOGIN           **\n");
    printf("*********************************\n");
    printf("\033[0m"); // Reset color

    printf("\033[1;33m"); // Yellow color for input prompts
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password); // No change to how password is handled

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0)
        {
            printf("\033[1;32m"); // Green color for success
            printf("Login successful! Welcome, %s.\n", username);
            printf("\033[0m"); // Reset color
            return 1; // User authenticated
        }
    }

    printf("\033[1;31m"); // Red color for failure
    printf("Invalid username or password. Please try again.\n");
    printf("\033[0m"); // Reset color
    return 0; // Authentication failed
}


//////////////////// END /////////////////////////////////////

//////////// ADMIN AND USER MAIN MENU ////////////////////////

// admin menu
void adminMenu()
{
    int choice;

    do
    {
        clearScreen();
        printf("\033[1;35m"); // Magenta color for the title
        printf("********************************************\n");
        printf("**               ADMIN MENU              **\n");
        printf("********************************************\n");
        printf("\033[0m"); // Reset color
        printf("\n");

        printf("\033[1;34m"); // Blue color for options
        printf("1. Input environmental data\n");
        printf("2. Update predictions and alerts\n");
        printf("3. View alert status\n");
        printf("4. Update existing environmental data\n");
        printf("5. Add New Admin\n");
        printf("6. Logout\n");
        printf("\033[0m"); // Reset color
        printf("\n");

        // Prompt for admin choice
        printf("\033[1;33m"); // Yellow color for the prompt
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\033[0m"); // Reset color

        switch (choice)
        {
        case 1:
            clearScreen();
            printf("\033[1;32m"); // Green for action
            printf("Inputting environmental data...\n");
            printf("\033[0m");
            inputData(data, &count); // Take environmental data as input
            break;
        case 2:
            clearScreen();
            printf("\033[1;32m"); // Green for action
            printf("Updating predictions and alerts...\n");
            printf("\033[0m");
            update_predictions(input_file, prediction_file); // Update predictions and alert status
            printf("\033[1;32m"); // Green for success
            printf("Predictions and alerts updated.\n");
            printf("\033[0m");
            break;
        case 3:
            clearScreen();
            printf("\033[1;32m"); // Green for action
            printf("Viewing alert status...\n");
            printf("\033[0m");
            view_alert(prediction_file); // Display the alert table
            break;
        case 4:
            clearScreen();
            printf("\033[1;32m"); // Green for action
            printf("Updating existing environmental data...\n");
            printf("\033[0m");
            updateData(data, count); // Update existing environmental data
            break;
        case 5:
            clearScreen();
            printf("\033[1;32m"); // Green for action
            printf("Adding new admin...\n");
            printf("\033[0m");
            registerAdmin();
            break;
        case 6:
            clearScreen();
            printf("\033[1;33m"); // Yellow for logging out
            printf("Logging out...\n");
            printf("\033[0m");
            return; // Exit the loop and return to the main menu or authentication
        default:
            printf("\033[1;31m"); // Red for error
            printf("Invalid choice. Please try again.\n");
            printf("\033[0m");
        }

        if (choice != 6)
        {
            printf("\n\033[1;36m"); // Cyan for continue prompt
            printf("Press Enter to continue...");
            getchar(); // Consume the newline character from previous input
            getchar(); // Wait for user to press Enter before clearing
            printf("\033[0m"); // Reset color
        }
    } while (choice != 6);
}


// after login
void userMenu()
{
    int choice;

    do
    {
        clearScreen();
        printf("\033[1;36m"); // Cyan color for the title
        printf("********************************************\n");
        printf("**               USER MENU               **\n");
        printf("********************************************\n");
        printf("\033[0m"); // Reset color
        printf("\n");

        printf("\033[1;34m"); // Blue color for the options
        printf("1. Show forecast for my location\n");
        printf("2. Search for forecast in other location\n");
        printf("3. Logout\n");
        printf("\033[0m"); // Reset color
        printf("\n");

        // Prompt user for their choice
        printf("\033[1;33m"); // Yellow color for the prompt
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\033[0m"); // Reset color

        switch (choice)
        {
        case 1:
            clearScreen();
            printf("\033[1;32m"); // Green for showing the forecast
            printf("Displaying forecast for your location...\n");
            printf("\033[0m");
            showForecast(users[userCount - 1].location); // Using last registered user for simplicity
            break;
        case 2:
            clearScreen();
            printf("\033[1;32m"); // Green for searching another location
            printf("Searching for forecast in another location...\n");
            printf("\033[0m");
            searchOtherLocation();
            break;
        case 3:
            clearScreen();
            printf("\033[1;33m"); // Yellow for logout message
            printf("Logging out...\n");
            printf("\033[0m");
            return; // Exit the menu
        default:
            printf("\033[1;31m"); // Red for invalid input
            printf("Invalid choice. Please try again.\n");
            printf("\033[0m");
        }

        if (choice != 3)
        {
            printf("\n\033[1;36m"); // Cyan color for continue prompt
            printf("Press Enter to continue...");
            getchar(); // Consume the newline character from previous input
            getchar(); // Wait for user to press Enter before clearing
            printf("\033[0m"); // Reset color
        }
    } while (choice != 3);
}


//////////////////////// END //////////////////////////////////

////////////////////// MAIN UI /////////////////////////////
void loginPage()
{
    int choice;

    do
    {
        clearScreen();
        printf("\033[1;32m"); // Green color for the title
        printf("********************************************\n");
        printf("**       WELCOME TO THE LOGIN SYSTEM      **\n");
        printf("********************************************\n");
        printf("\033[0m"); // Reset color
        printf("\n");
        printf("\033[1;34m"); // Blue color for options
        printf("1. Admin Login\n");
        printf("2. User Login\n");
        printf("3. Register as User\n");
        printf("4. Exit Program\n");
        printf("\033[0m"); // Reset color
        printf("\n");
        
        // Prompt user for their choice
        printf("\033[1;33m"); // Yellow color for the prompt
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\033[0m"); // Reset color

        switch (choice)
        {
        case 1:
            clearScreen();
            if (authenticateAdmin())
            {
                printf("\033[1;32m"); // Green for success
                printf("\nAdmin login successful!\n");
                printf("\033[0m");
                adminMenu();
            }
            else
            {
                printf("\033[1;31m"); // Red for error
                printf("Invalid admin credentials.\n");
                printf("\033[0m");
            }
            break;
        case 2:
            clearScreen();
            if (authenticateUser())
            {
                printf("\033[1;32m"); // Green for success
                printf("\nUser login successful!\n");
                printf("\033[0m");
                userMenu();
            }
            else
            {
                printf("\033[1;31m"); // Red for error
                printf("Invalid user credentials.\n");
                printf("\033[0m");
            }
            break;
        case 3:
            clearScreen();
            registerUser();
            break;
        case 4:
            printf("\033[1;33m"); // Yellow for exit message
            printf("Exiting program. Goodbye!\n");
            printf("\033[0m");
            exit(0);
        default:
            printf("\033[1;31m"); // Red for invalid input
            printf("Invalid choice. Please try again.\n");
            printf("\033[0m");
        }

        if (choice != 4)
        {
            delay();
        }
    } while (choice != 4);
}

//////////////////// END /////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////// MAIN FUCTION //////////////////////////////

int main()
{

    loadAdminFromFile();
    loadUsersFromFile();

    clearScreen();
    loginPage();

    return 0;
}