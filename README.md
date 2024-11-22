# **ENVIRONMENTAL MONITORING SYSTEM**

An **Environmental Monitoring System** that retrieves real-time weather data for Karachi using the **OpenWeatherMap API**. The system processes, stores, and analyzes the data, generating alerts for significant environmental conditions.

---

## **Features**
- **Real-Time Data Retrieval**: Fetches weather data such as temperature, humidity, and weather descriptions using OpenWeatherMap API.
- **Data Processing**:
  - Converts raw JSON data into a structured CSV format.
  - Eliminates duplicate entries based on timestamps.
- **Alerts**: Generates weather-related alerts for critical conditions.
- **Automation**: Automates the data retrieval and processing workflow using shell scripts.

---

## **Installation**

### **Prerequisites**
- **GCC Compiler**
- **libcurl** for API requests
- **cjson** for retreive data from json
- **jq** for JSON processing

### **Steps**
1. **Clone the repository**:
   ```bash
   git clone https://github.com/sumbalzehra88/environmental-monitoring-system-in-c.git
   cd environmental-monitoring-system-in-c
2. **Install Dependencies:**:
     - **Install libcurl**: 
       ```bash
       sudo apt update
       sudo apt install libcurl4-openssl-dev
     - **Install jq**:
         ```bash
         sudo apt install jq
     - **Install JSON**:
         ```bash
          sudo apt install cjson

3. **Compile the Project:**:
   ```bash
   gcc -o main src/main.c cjson/cJSON.c -Iinclude -Icjson -lcurl
4. **Run the Program**:
   ```bash
   ./main
5. **Run the Shell Script**:
   - **for weather_automation**:
     ```bash
     chmod +x weather_automation.sh
     ./weather_automation.sh
   - **for generate_alerts**:
     ```bash
     chmod +x alerts.sh
     ./alerts.sh
### **Usage**

- **API Data Retrieval**: Fetches 5-day weather forecasts for Karachi in JSON format.
- **Alerts**:
  - **Notifications**: Alerts are printed on the terminal.
  - **Future Feature**: Email notifications for severe weather conditions.

---

### **Contributors**
- **Khadija Sehar**  
- **Sumbal Zehra**  
- **Aamnah Aatif**

---

### **Key Improvements**
- Support for multiple cities or dynamic location input.
- Visualization of weather trends using graphs.
- Enhanced error handling for API failures.
