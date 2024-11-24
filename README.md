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
- **bc** for handling float variables

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
         sudo apt-get install jq
     - **Install JSON**:
         ```bash
          sudo apt install cjson
     - **Install bc**:
         ```bash
          sudo apt install bc
      - **Install gdb**:
         ```bash
          sudo apt install gdb

3. **Compile the Project:**:
   ```bash
   gcc -o main main.c -lcurl -lcjson
4. **Run the Program**:
   ```bash
   ./main
5. **Run the Shell Script**:
     ```bash
     chmod +x weather_automation.sh
     ./weather_automation.sh
6. **Run the Data Log File**:
     ```bash
     cat weather_data_log.txt
7. **Run the Cron Data Log File**:
     ```bash
     cat cron_weather_data_log.txt
   
### **Usage**

- **API Data Retrieval**: Fetches 5-day weather forecasts for Karachi in JSON format.
- **Alerts/Notifictions**: generate alerts/notifications when certain conditions meet. 

---

### **Contributors**
- **Khadija Sehar**  
- **Sumbal Zehra**  
- **Aamnah Aatif**

---

### **Key Improvements**
- Support for multiple cities or dynamic location input.
- Visualization of weather trends using graphs.
- Support for the email/sms notifications/ alerts.
- Enhanced error handling for API failures.
