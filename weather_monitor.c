#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

#define API_KEY "50fe3f0f4010bb987bff53d51ee641c4"
// Updated API URL for 5-day forecast (using latitude and longitude for Karachi)
#define API_URL "https://api.openweathermap.org/data/2.5/forecast?lat=24.8607&lon=67.0011&appid=" API_KEY

// Callback function to handle the data received by API call through libcurl
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    char *data = (char *)userp;
    strncat(data, contents, total_size);  // Append received data to a string
    return total_size;
}

int check_duplicate(FILE *processed_file, const char *timestamp) {
    char line[1024];
    rewind(processed_file);  // Reset file pointer to the beginning of the file
    while (fgets(line, sizeof(line), processed_file)) {
        if (strstr(line, timestamp)) {
            return 1;  // Duplicate found
        }
    }
    return 0;  // No duplicate
}

void parse_weather_data(const char *data) {
    // Parse the JSON data
    cJSON *json_data = cJSON_Parse(data);
    if (json_data == NULL) {
        fprintf(stderr, "Error parsing JSON data: %s\n", cJSON_GetErrorPtr());
        return;
    }

    // Extract the forecast list from JSON
    cJSON *list = cJSON_GetObjectItem(json_data, "list");
    if (!cJSON_IsArray(list)) {
        fprintf(stderr, "Error: Unable to extract forecast data.\n");
        cJSON_Delete(json_data);
        return;
    }

    // Open the processed CSV file for writing (append mode)
    FILE *processed_file = fopen("processed_data.csv", "a");
    if (!processed_file) {
        fprintf(stderr, "Error opening processed data file.\n");
        cJSON_Delete(json_data);
        return;
    }

    // Write headers to the CSV if it's the first time writing
    if (ftell(processed_file) == 0) {
        fprintf(processed_file, "Timestamp,Temperature (C),Humidity (%),Feels Like (C),Description\n");
    }

    // Iterate through each forecast entry (3-hour intervals)
    int num_entries = cJSON_GetArraySize(list);
    for (int i = 0; i < num_entries; i++) {
        cJSON *entry = cJSON_GetArrayItem(list, i);
        
        // Extract the timestamp (dt_txt)
        cJSON *timestamp_item = cJSON_GetObjectItem(entry, "dt_txt");
        if (!cJSON_IsString(timestamp_item)) continue;
        const char *timestamp = timestamp_item->valuestring;

        // Check if this timestamp already exists in the CSV file (to avoid duplicates)
        if (check_duplicate(processed_file, timestamp)) {
            printf("Duplicate data for %s found. Skipping...\n", timestamp);
            continue;
        }

        // Extract weather data (temperature, humidity, feels like, description)
        cJSON *main_data = cJSON_GetObjectItem(entry, "main");
        cJSON *temp = cJSON_GetObjectItem(main_data, "temp");
        cJSON *humidity = cJSON_GetObjectItem(main_data, "humidity");
        cJSON *feels_like = cJSON_GetObjectItem(main_data, "feels_like");

        cJSON *weather_array = cJSON_GetObjectItem(entry, "weather");
        cJSON *weather_item = cJSON_GetArrayItem(weather_array, 0);
        cJSON *description = cJSON_GetObjectItem(weather_item, "description");

        if (temp && humidity && feels_like && description) {
            // Convert temperature from Kelvin to Celsius
            float temp_celsius = temp->valuedouble - 273.15;
            float feels_like_celsius = feels_like->valuedouble - 273.15;

            // Write the processed data to the CSV
            fprintf(processed_file, "%s,%.2f,%.2f,%.2f,%s\n", 
                    timestamp, 
                    temp_celsius, 
                    humidity->valuedouble, 
                    feels_like_celsius, 
                    description->valuestring);
            printf("Processed data for %s saved.\n", timestamp);
        }
    }

    // Close the processed CSV file
    fclose(processed_file);

    // Free the cJSON object after use
    cJSON_Delete(json_data);
}

int main() {
    CURL *curl;
    CURLcode res;
    char data[4096] = {0};  // Buffer to store the API response

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request (forecast data)
        curl_easy_setopt(curl, CURLOPT_URL, API_URL);

        // Set the callback function to handle the data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the response (for debugging purposes)
            printf("Response Data: %s\n", data);

            // Parse the JSON data and process it
            parse_weather_data(data);
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    // Global cleanup
    curl_global_cleanup();

    return 0;
}
