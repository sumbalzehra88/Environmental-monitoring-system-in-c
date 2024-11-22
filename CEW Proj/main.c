#include "header.h"

struct Memory {
    char *data;
    size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->data, mem->size + total_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->data[mem->size] = '\0';

    return total_size;
}

int check_duplicate(FILE *processed_file, const char *timestamp) {
    char line[1024];
    rewind(processed_file);
    while (fgets(line, sizeof(line), processed_file)) {
        if (strstr(line, timestamp)) {
            return 1;
        }
    }
    return 0;
}

void parse_weather_data(const char *data) {
    cJSON *json_data = cJSON_Parse(data);
    if (json_data == NULL) {
        fprintf(stderr, "Error parsing JSON data: %s\n", cJSON_GetErrorPtr());
        return;
    }

    cJSON *list = cJSON_GetObjectItem(json_data, "list");
    if (!cJSON_IsArray(list)) {
        fprintf(stderr, "Error: Unable to extract forecast data.\n");
        cJSON_Delete(json_data);
        return;
    }

    FILE *processed_file = fopen("processed_data.csv", "a");
    if (!processed_file) {
        fprintf(stderr, "Error opening processed data file.\n");
        cJSON_Delete(json_data);
        return;
    }

    if (ftell(processed_file) == 0) {
        fprintf(processed_file, "Timestamp,Temperature (C),Humidity (%%),Feels Like (C),Description\n");
    }

    int num_entries = cJSON_GetArraySize(list);
    for (int i = 0; i < num_entries; i++) {
        cJSON *entry = cJSON_GetArrayItem(list, i);

        cJSON *timestamp_item = cJSON_GetObjectItem(entry, "dt_txt");
        if (!cJSON_IsString(timestamp_item)) continue;
        const char *timestamp = timestamp_item->valuestring;

        if (check_duplicate(processed_file, timestamp)) {
            printf("Duplicate data for %s found. Skipping...\n", timestamp);
            continue;
        }

        cJSON *main_data = cJSON_GetObjectItem(entry, "main");
        cJSON *temp = cJSON_GetObjectItem(main_data, "temp");
        cJSON *humidity = cJSON_GetObjectItem(main_data, "humidity");
        cJSON *feels_like = cJSON_GetObjectItem(main_data, "feels_like");

        cJSON *weather_array = cJSON_GetObjectItem(entry, "weather");
        cJSON *weather_item = cJSON_GetArrayItem(weather_array, 0);
        cJSON *description = cJSON_GetObjectItem(weather_item, "description");

        if (temp && humidity && feels_like && description) {
            float temp_celsius = temp->valuedouble - 273.15;
            float feels_like_celsius = feels_like->valuedouble - 273.15;

            fprintf(processed_file, "%s,%.2f,%.2f,%.2f,%s\n", 
                    timestamp, 
                    temp_celsius, 
                    humidity->valuedouble, 
                    feels_like_celsius, 
                    description->valuestring);
            printf("Processed data for %s saved.\n", timestamp);
        }
    }

    fclose(processed_file);
    cJSON_Delete(json_data);
}

int main() {
    CURL *curl;
    CURLcode res;

    struct Memory chunk = {NULL, 0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, API_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Response Data: %s\n", chunk.data);
            parse_weather_data(chunk.data);
        }

        curl_easy_cleanup(curl);
        free(chunk.data);
    }

    curl_global_cleanup();

    return 0;
}