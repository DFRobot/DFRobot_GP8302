/*!
 * @file calibration.ino
 * @brief Calibrate the current within 4-20mA
 * @note The carrying capacity of this module is related to the external power supply voltage: range 18-24V, maximum 450R carrying capacity for 18V power supply, and 650R for 24V.
 * @n The hardware connection in this demo:
 * @n 1. Disable  Macro #define I2C_PIN_REMAP_ENABLE I2C uses the default I2C pins, i.e., using the pin corresponding to the MCU hardware I2C.
 * @n 2. Enable  Macro #define I2C_PIN_REMAP_ENABLE I2C pin remapping macro, the way of connecting SCL and SDA pins is as shown in the following table:
 * @n ---------------------------------------------------------------------------------
 * @n |Module | UNO     | Leonardo  |  Mega2560 |     ESP32    |     M0    | microbit |
 * @n ---------------------------------------------------------------------------------
 * @n |SCL    |  3      |     3     |     3     |   SCL(26)(D3)|   SCL(3)  |  SCL(P8) |
 * @n |SDA    |  5      |     5     |     5     |   SDA(27)(D4)|   SDA(5)  |  SDA(P9) |
 * @n ---------------------------------------------------------------------------------
 * @note When using I2C pin remapping, besides the pins listed above, other IO pins of the MCU can also be selected.
 * @note esp8266 does not support I2C pin remapping
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2022-03-16
 * @url https://github.com/DFRobot/DFRobot_GP8302
 */

#include "DFRobot_GP8302.h"


//#define I2C_PIN_REMAP_ENABLE  //Enable I2C pin remapping, remap it to the pins defined by I2C_SCL_PIN and I2C_SDA_PIN, if it’s not enabled, the pins used by the hardware Wire object will be used by default.

#ifdef I2C_PIN_REMAP_ENABLE
#if defined(ESP32)
#define I2C_SCL_PIN   26 
#define I2C_SDA_PIN   27
#elif defined(ARDUINO_BBC_MICROBIT)
#define I2C_SCL_PIN   8 
#define I2C_SDA_PIN   9
#else
#define I2C_SCL_PIN   3 
#define I2C_SDA_PIN   5
#endif
#endif

DFRobot_GP8302 module;

void setup(){
  Serial.begin(115200);
  
  while(!Serial){
    //Wait for USB serial port to connect. Needed for native USB port only
  }

  Serial.print("I2C to 0-25 mA analog current moudle initialization ... ");
  
#ifdef I2C_PIN_REMAP_ENABLE
  uint8_t status = module.begin(/* scl = */I2C_SCL_PIN, /*sda = */I2C_SDA_PIN);  //I2C scl and sda pins redefine
#else
  uint8_t status = module.begin(); // Default to use the pins used by the MCU hardware I2C Wire object
#endif

  if(status != 0){
    Serial.print("failed. Error code: ");
    Serial.println(status);
    Serial.println("Error Code: ");
    Serial.println("\t1: _scl or _sda pin is invaild.");
    Serial.println("\t2: Device not found, please check if the device is connected.");
    while(1) yield();
  }
  Serial.println("done!");
  
  /**
   * @fn calibration4_20mA
   * @brief Calibrate the current within 4-20mA
   * @param dac_4   Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 4mA generally fluctuates at about 655, the actual value needs to be tested by the user in actual applications
   * @param dac_20  Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 20mA generally fluctuates at about 3277, the actual value needs to be tested by the user in actual applications
   * @note The method of obtaining the DAC value corresponding to the current of 4mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 655, the actual DAC value is the one input into the instrument when the measured current is 4mA.
   * @n The method of obtaining the DAC value corresponding to the current of 20mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 3277, the actual DAC value is the one input into the instrument when the measured current is 20mA.
   * @note Parameter dac_4 and dac_20 should meet the conditions: dac_4 < dac_20, after the calibration is enabled, output function will output the calibrated current value
   */
  module.calibration4_20mA(/*dac_4 =*/655, /*dac_20 =*/3277);
  /**
   * @fn output
   * @brief Set the device to output the current of 0-25mA
   * @param current_mA  The output current value, range: 0-25mA
   * @return The DAC value corresponding to the output current value
   * @note calibration4_20mA After calibration, the output function will output the calibrated current value and return the calibrated DAC value
   */
  uint16_t dac = module.output(/*current_mA =*/10); //Control the DAC module to output the current of 10mA and return the DAC value corresponding to the current of 10mA
  Serial.print("DAC value: "); Serial.println(dac);

  
}

void loop(){
  
}
