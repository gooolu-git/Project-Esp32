# 🌱 ESP32 IoT Smart Monitoring & Control System  
### **DHT11 | Soil Moisture | Water Level | Firebase RTDB | Motor & LED Control**

This project uses an **ESP32 Dev Kit** with multiple sensors to monitor environmental conditions and control devices remotely using **Firebase Realtime Database**.  
It features real-time syncing, auto WiFi reconnect, cloud-based motor control, and clean non-blocking loop timing.

---

## 📌 **Features**
- 🌡️ DHT11 Temperature & Humidity Monitoring  
- 🌱 Soil Moisture Sensor (Analog)  
- 💧 Water Level Sensor (Analog)  
- ⚡ Motor/Relay Control (Active LOW)  
- 💡 LED Control from Firebase  
- ☁️ Real-time Firebase RTDB Sync  
- 🔄 Automatic WiFi Reconnection  
- 🛡️ Supports Firebase Authentication (Email & Password)

---

## 📁 **Project Structure**
```
/src
  ├── main.cpp
/platformio.ini
/README.md
```

---

## 🧠 **How It Works**
The ESP32 performs the following tasks:

### **1️⃣ Reads sensors every 1 second**
- DHT11 → temperature & humidity  
- Moisture sensor → analog value  
- Water level sensor → analog value  

### **2️⃣ Uploads sensor readings to Firebase**
```
/ESP32/Moisture
/ESP32/WaterLevel
/ESP32/Temperature
/ESP32/Humidity
```

### **3️⃣ Reads remote control commands**
```
/ESP32/commands/motor  → ON / OFF  
/ESP32/commands/led    → ON / OFF
```

### **4️⃣ Controls devices**
- Relay (motor) — **Active LOW**  
- ESP32 built-in LED — **Active HIGH**

---

## 🔐 **Required Firebase Structure**

```
ESP32/
 ├── commands/
 │     ├── motor : "ON"
 │     └── led   : "OFF"
 ├── Moisture : 532
 ├── WaterLevel : 271
 ├── Temperature : 27.4
 └── Humidity : 66
```

---



## 🚀 Installation **

### **1️⃣ Install PlatformIO**
VS Code → Extensions → “PlatformIO IDE” → Install

### **2️⃣ Create a new ESP32 project**
```
PlatformIO > New Project > Board: ESP32 Dev Module > Framework: Arduino
```

### **3️⃣ Add the above `platformio.ini`**

### **4️⃣ Replace `src/main.cpp` with the provided code**

### **5️⃣ Upload the code**
```
PlatformIO: Upload (→ arrow button)
```

---

## 🔥 Firebase Database Rules (Recommended)

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

---

## 🧪 Testing Commands

| Path | Value |
|------|-------|
| `/ESP32/commands/motor` | `"ON"` or `"OFF"` |
| `/ESP32/commands/led` | `"ON"` or `"OFF"` |

---





