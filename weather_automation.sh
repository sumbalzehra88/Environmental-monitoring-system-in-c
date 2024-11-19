#!/bin/bash

# API details (Updated for 5-day forecast)
API_URL="https://api.openweathermap.org/data/2.5/forecast?lat=24.8607&lon=67.0011&appid=50fe3f0f4010bb987bff53d51ee641c4"

# File paths
raw_data_file="raw_data.json"
processed_data_file="processed_data.csv"

# Step 1: Create the CSV file with headers if it doesn't exist
if [ ! -f "$processed_data_file" ]; then
    echo "Timestamp,Temperature (C),Humidity (%),Feels Like (C),Description" > "$processed_data_file"
fi

# Step 2: Retrieve data from the API (forecast data)
echo "Retrieving forecast weather data..."
curl -s "$API_URL" -o "$raw_data_file"

if [ $? -ne 0 ]; then
    echo "Failed to retrieve data."
    exit 1
fi

echo "Weather data saved to $raw_data_file."

# Step 3: Process the data with jq (Extract data for each 3-hour interval)
echo "Processing weather data..."

# Loop through each forecast entry (every 3 hours)
for i in $(seq 0 23); do
    # Extract timestamp for each entry (every 3 hours)
    TIMESTAMP=$(jq -r ".list[$i].dt_txt" "$raw_data_file")

    # Check if the timestamp already exists in the CSV
    if grep -q "$TIMESTAMP" "$processed_data_file"; then
        echo "Duplicate data for $TIMESTAMP found. Skipping..."
        continue
    fi

    # Extract temperature, humidity, feels_like, and description
    TEMP=$(jq ".list[$i].main.temp" "$raw_data_file")
    HUMIDITY=$(jq ".list[$i].main.humidity" "$raw_data_file")
    FEELS_LIKE=$(jq ".list[$i].main.feels_like" "$raw_data_file")
    DESCRIPTION=$(jq -r ".list[$i].weather[0].description" "$raw_data_file")

    # Check if values are empty and set defaults if needed
    if [ -z "$TEMP" ]; then TEMP=0; fi
    if [ -z "$FEELS_LIKE" ]; then FEELS_LIKE=0; fi
    if [ -z "$HUMIDITY" ]; then HUMIDITY=0; fi
    if [ -z "$DESCRIPTION" ]; then DESCRIPTION="unknown"; fi

    # Convert temperature from Kelvin to Celsius using awk
    TEMP_C=$(echo "$TEMP - 273.15" | awk '{print int($1*10)/10}')
    FEELS_LIKE_C=$(echo "$FEELS_LIKE - 273.15" | awk '{print int($1*10)/10}')

    # Save processed data to the CSV file
    echo "$TIMESTAMP,$TEMP_C,$HUMIDITY,$FEELS_LIKE_C,$DESCRIPTION" >> "$processed_data_file"
    echo "Processed data for $TIMESTAMP saved to $processed_data_file."
done

echo "Weather data collection for 24 hourly readings completed."

