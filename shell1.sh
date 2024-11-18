#!/bin/bash

# API details
API_URL="https://api.openweathermap.org/data/2.5/weather?lat=24.8607&lon=67.0011&appid=50fe3f0f4010bb987bff53d51ee641c4"

# File paths
raw_data_file="raw_data.json"
processed_data_file="processed_data.csv"

# Step 1: Retrieve data from API
echo "Retrieving weather data..."
curl -s "$API_URL" -o "$raw_data_file"

if [ $? -ne 0 ]; then
    echo "Failed to retrieve data."
    exit 1
fi

echo "Weather data saved to $raw_data_file."

# Step 2: Process the data with jq
echo "Processing weather data..."

# Extract fields using jq
TEMP=$(jq '.main.temp' "$raw_data_file")
HUMIDITY=$(jq '.main.humidity' "$raw_data_file")
FEELS_LIKE=$(jq '.main.feels_like' "$raw_data_file")
DESCRIPTION=$(jq -r '.weather[0].description' "$raw_data_file")

# Convert temperature from Kelvin to Celsius
TEMP_C=$(echo "$TEMP - 273.15" | bc)
FEELS_LIKE_C=$(echo "$FEELS_LIKE - 273.15" | bc)

# Save processed data to a CSV file
if [ ! -f "$PROCESSED_DATA_FILE" ]; then
    echo "Temperature (C),Humidity (%),Feels Like (C),Description" > "$processed_data_file"
fi

echo "$TEMP_C,$HUMIDITY,$FEELS_LIKE_C,$DESCRIPTION" >> "$processed_data_file"

echo "Processed data saved to $processed_data_file."