#ifndef WEATHER_PROCESSING_H
#define WEATHER_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../cjson/cJSON.h"

// Define constants
#define API_KEY "50fe3f0f4010bb987bff53d51ee641c4"
#define API_URL "https://api.openweathermap.org/data/2.5/forecast?lat=24.8607&lon=67.0011&appid=" API_KEY

// Function prototypes

/**
 * Callback function to handle data received from API calls.
 * @param contents - Pointer to the data received.
 * @param size - Size of a single data element.
 * @param nmemb - Number of data elements.
 * @param userp - Pointer to the user-provided memory where data will be written.
 * @return The total number of bytes handled.
 */
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

/**
 * Check for duplicate entries in the processed CSV file.
 * @param processed_file - File pointer to the processed CSV file.
 * @param timestamp - Timestamp to check for duplicates.
 * @return 1 if a duplicate is found, 0 otherwise.
 */
int check_duplicate(FILE *processed_file, const char *timestamp);

/**
 * Parse weather data in JSON format and process it into a CSV file.
 * @param data - JSON string containing the weather data.
 */
void parse_weather_data(const char *data);

#endif // WEATHER_PROCESSING_H

