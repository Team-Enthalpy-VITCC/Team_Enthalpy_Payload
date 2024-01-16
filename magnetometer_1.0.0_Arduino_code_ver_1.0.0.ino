#include <Wire.h>  // Include Wire library for I2C
#include <SD.h>  // Include SD library
#include <RH_ASK.h>  // Include RadioHead Amplitude Shift Keying Library
#include <SPI.h>  // Include SPI library needed for the RadioHead library

// HMC5883L
#define HMC5883L_Address 0x1E  // Define the HMC5883L address

// SD Card
#define SD_ChipSelectPin 4  // SD card CS pin

// ZETA 433 S0
#define ZETA_SDN 2  // ZETA SDN pin
#define ZETA_IRQ 3  // ZETA IRQ pin

// Create an instance of the RadioHead ASK object
RH_ASK rf_driver;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize I2C communication for HMC5883L
  Wire.begin();

  // Initialize SD card
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD card initialization failed!");
    while(1);  // Halt program execution if SD card initialization fails
  }

  // Initialize ZETA 433 S0
  pinMode(ZETA_SDN, OUTPUT);
  pinMode(ZETA_IRQ, INPUT);
  digitalWrite(ZETA_SDN, LOW);  // Exit low power mode

  // Initialize RF 433MHz transmitter
  if (!rf_driver.init()) {
    Serial.println("RF driver initialization failed!");
    while(1);  // Halt program execution if RF driver initialization fails
  }
}

void loop() {
  // Read data from HMC5883L
  Wire.beginTransmission(HMC5883L_Address);
  Wire.write(0x03);  // Send register address
  if (Wire.endTransmission() != 0) {
    Serial.println("Error communicating with HMC5883L");
    return;
  }
  
  Wire.requestFrom(HMC5883L_Address, 6);
  if (6 <= Wire.available()) {
    int x = Wire.read() << 8;  // X-axis value
    x |= Wire.read();
    int z = Wire.read() << 8;  // Z-axis value
    z |= Wire.read();
    int y = Wire.read() << 8;  // Y-axis value
    y |= Wire.read();

    // TODO: Process or store values as needed
  } else {
    Serial.println("Error reading data from HMC5883L");
    return;
  }

  // Write data to SD card
  File dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    // TODO: Write data to file
    dataFile.close();
  } else {
    Serial.println("Error opening data.txt");
    return;
  }

  // Read data from SD card
  File dataFileRead = SD.open("data.txt");
  if (dataFileRead) {
    while (dataFileRead.available()) {
      char data = dataFileRead.read();
      // Transmit data using ZETA 433 S0
      rf_driver.send((uint8_t *)&data, sizeof(data));
      rf_driver.waitPacketSent();
      Serial.println("Message sent!");
    }
    dataFileRead.close();
  } else {
    Serial.println("Error opening data.txt");
    return;
  }
}
