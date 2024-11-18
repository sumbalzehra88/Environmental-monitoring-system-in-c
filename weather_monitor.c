#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

#define API_KEY "50fe3f0f4010bb987bff53d51ee641c4"
#define API_URL "https://api.openweathermap.org/data/2.5/weather?lat=24.8607&lon=67.0011&appid=" API_KEY
#define BUFFER_SIZE 4096

// Structure for handling the response data
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function for handling data received by libcurl
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + total_size + 1);
    if (ptr == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->memory[mem->size] = 0;

    return total_size;
}

// Function to parse weather data
void parse_weather_data(const char *data) {
    cJSON *json_data = cJSON_Parse(data);  // Parse the JSON response string

    if (json_data == NULL) {
        fprintf(stderr, "Error parsing JSON data: %s\n", cJSON_GetErrorPtr());
        return;
    }

    // Extract main data for temperature, humidity, and feels like
    cJSON *main_data = cJSON_GetObjectItem(json_data, "main");
    cJSON *temp = cJSON_GetObjectItem(main_data, "temp");
    cJSON *humidity = cJSON_GetObjectItem(main_data, "humidity");
    cJSON *feels_like = cJSON_GetObjectItem(main_data, "feels_like");

    // Extract weather description
    cJSON *weather = cJSON_GetObjectItem(json_data, "weather");
    cJSON *weather_item = cJSON_GetArrayItem(weather, 0);
    cJSON *weather_description = cJSON_GetObjectItem(weather_item, "description");

    if (temp != NULL && humidity != NULL && feels_like != NULL && weather_description != NULL) {
        // Print the extracted data
        printf("Temperature: %.2f°C\n", temp->valuedouble - 273.15);
        printf("Humidity: %.2f%%\n", humidity->valuedouble);
        printf("Feels Like: %.2f°C\n", feels_like->valuedouble - 273.15);
        printf("Weather: %s\n", weather_description->valuestring);

        // Save the raw data to a file
        FILE *raw_file = fopen("raw_data.json", "a");
        if (raw_file != NULL) {
            fprintf(raw_file, "%s\n", data);  // Store the raw JSON data
            fclose(raw_file);
        }

        // Save the processed data to a CSV file
        FILE *processed_file = fopen("processed_data.csv", "a");
        if (processed_file != NULL) {
            fprintf(processed_file, "%.2f,%.2f,%.2f,%s\n", 
                    temp->valuedouble - 273.15,  // Temperature in Celsius
                    humidity->valuedouble,       // Humidity
                    feels_like->valuedouble - 273.15,  // Feels like in Celsius
                    weather_description->valuestring); // Weather description
            fclose(processed_file);
        }
    }

    cJSON_Delete(json_data);  // Free the cJSON object after use
}

int main() {
    CURL *curl;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Start with a 1-byte buffer
    chunk.size = 0;            // Initially, buffer is empty

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, API_URL);

        // Set callback function to handle data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        // Pass the chunk to the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the response for debugging purposes
            printf("Response: %s\n", chunk.memory);

            // Parse the JSON response to extract specific data
            parse_weather_data(chunk.memory);
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Free the memory allocated for the response
    if (chunk.memory) {
        free(chunk.memory);
    }

    // Cleanup CURL
    curl_global_cleanup();

    return 0;
}
