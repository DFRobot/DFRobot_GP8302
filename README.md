# DFRobot_GP8302

* [中文版](./README_CN.md)

This I2C to 0-25mA DAC module can be used for users to output a current of 0-25mA. It has the following features:
1. Require an external power supply, range: 18-24V, maximum carrying capacity of 450R for 18V power supply, and 650R for 24V.
2. Output DC within 0-25mA.
3. It can control the output current with an I2C interface, the I2C address is default to be 0x58.
4. The output current config will be lost after the module is powered down. Save the config if you want to use it for the next power-up.


![产品效果图](./resources/images/DFR0972.png) 

## Product Link（[www.dfrobot.com](www.dfrobot.com)）
    SKU: DFR0972 

## Table of Contents
* [Summary](#summary)
* [Installation](#installation)
* [Methods](#methods)
* [Compatibility](#compatibility)
* [History](#history)
* [Credits](#credits)

## Summary
The Arduino library is provided for the I2C 0-25mA DAC module to set and save the output current config of the module. And the library has the following functions:
1. Set the output current of 0-25mA directly;
2. Output the corresponding voltage by setting the DAC range of 0-0xFFF;
3. Save the current config(Will not be lost after power down).

## Installation

There two methods: 
1. To use this library, first download the library file, paste it into the \Arduino\libraries directory, then open the examples folder and run the demo in the folder.
2. Search the DFRobot_GP8302 library from the Arduino Software Library Manager and download it.

## Methods

```C++
  /**
   * @fn DFRobot_GP8302
   * @brief DFRobot_GP8302 constructor, construct a DFRobot_GP8302 class object or class object pointer.
   * @return NONE
   */
  DFRobot_GP8302();
  /**
   * @fn begin
   * @brief Remap the I2C pins of the MCU connected to the DAC module, and initialize the module
   * @param scl  IO port pin of the MCU
   * @param sda  IO port pin of the MCU
   * @note When scl and sda use the default formal parameter, they are the pins corresponding to MCU hardware I2C
   * @return The value for init status
   * @retval  0    Init succeeded
   * @retval  1    The I2C pin of the MCU connected to the DAC module is invalid.
   * @retval  2    Device not found, please check if the connection is correct.
   */
  uint8_t begin(int scl = -1, int sda = -1);

  /**
   * @fn calibration4_20mA
   * @brief Calibrate the current within 4-20mA
   * @param dac_4   Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 4mA generally fluctuates at about 655, the actual value needs to be tested by the user in actual applications
   * @param dac_20  Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 20mA generally fluctuates at about 3277, the actual value needs to be tested by the user in actual applications
   * @note The method of obtaining the DAC value corresponding to the current of 4mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 655, the actual DAC value is the one input into the instrument when the measured current is 4mA.
   * @n The method of obtaining the DAC value corresponding to the current of 20mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 3277, the actual DAC value is the one input into the instrument when the measured current is 20mA.
   * @note Parameter dac_4 and dac_20 should meet the conditions: dac_4 < dac_20, after the calibration is enabled, output function will output the calibrated current value
  void calibration4_20mA(uint16_t dac_4 = 655, uint16_t dac_20 = 3277);
  /**
   * @fn output_mA
   * @brief Set DAC value to control the device to output the current of 0-25mA.
   * @param dac  Specific DAC value, range 0-0xFFF
   * @note DAC value range is 0-0xFFF, 0-0xFFF DAC value corresponds to the output current of 0-25mA, the formula of DAC value converting to actual current: Iout = (DAC/0xFFF)*25mA
   * @return Calculate current value (may be different from the actual measured current), unit mA
   */
  float output_mA(uint16_t dac);
  /**
   * @fn output
   * @brief Control the device to output the current of 0-25mA
   * @param current_mA  The output current value, range: 0-25mA
   * @return The DAC value corresponding to the output current value
   * @note calibration4_20mA After calibration, the output function will output the calibrated current value and return the calibrated DAC value
   */
  uint16_t output(float current_mA);
  /**
   * @fn store
   * @brief Save the current config, after the config is saved successfully, it will be enabled when the module is powered down and restarts.
   */
  void store();
```
## Compatibility

MCU                |  Work Well    | Work Wrong   | Untested    | Remarks
------------------ | :----------: | :----------: | :---------: | -----
Arduino Uno        |       √       |              |             | 
Mega2560           |      √       |              |             | 
Leonardo           |      √       |              |             | 
ESP32              |      √       |              |             | 
ESP8266            |      √       |              |             | 
micro:bit          |      √       |              |             | 
FireBeetle M0      |      √       |              |             | 

## History

- 2022/03/03 - Version 1.0.0 released.

## Credits

Written by Arya(xue.peng@dfrobot.com), 2022. (Welcome to our [website](https://www.dfrobot.com/))

