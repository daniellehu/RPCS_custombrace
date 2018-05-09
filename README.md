# RPCS_custombrace
Rapid Prototyping for Computer Systems, Spring 2018 | Repository for the custom pressure brace system

The code used in the final demo can be found in Control2 folder. This can be loaded onto an Adafruit Bluefruit Feather M0 board.

# Arduino IDE setup
    1. Add https://adafruit.github.io/arduino-board-index/package_adafruit_index.json to Boards Manager URLs option in preferences
    1. Install Arduino SAMD Boards (32-bits ARM Cortex-M0+) by Arduino in Boards Manager
    1. Install Adafruit SAMD Boards by Adafruit in Boards Manager

     https://learn.adafruit.com/add-boards-arduino-v164/setup
     https://learn.adafruit.com/adafruit-feather-m0-basic-proto/using-with-arduino-ide


# Libraries used
    * Bluetooth LE Module - Adafruit BLE SPI Friend 
      https://github.com/adafruit/Adafruit_BluefruitLE_nRF51
    * Arduino Wire

# Content of each folder:

    * Control2 - Code used for final demo
    * Datasheets - All related datasheets for the HW components

    [Code used in the experimentation process]
    * PIDControl - A simple feedback loop control
    * ReadFromSensor - Read pressure and temperature values from I2C digital sensor
    * bleuart_cmdmode - Adafruit BLE example
    * control_working - Simple control for regulation pressure in air bag
    * integration_demo - First time putting HW components together without actual control loop
    * motor - Code to operate motor
    * readSendBle - Simulating a regulation process with bluetooth reporting (no control loop)
    * valve - Code to operate valve
