/*!
 * @file setCurrent.ino
 * @brief This I2C 0-25mA DAC module can be used to output a current of 0-25mA.
 * @note The carrying capacity of this module is related to the external power supply voltage: range 18-24V, maximum 450R carrying capacity for 18V power supply, and 650R for 24V.
 * @n This demo is used for demonstration. Control the module to output the current of 10mA and save the config to make sure it is not lost when the module is powered up again.
 * @n The hardware connection in this demo:
 * @n 1. Disable  Macro #define I2C_PIN_REMAP_ENABLE I2C uses the default I2C pins, i.e., using the pin corresponding to the MCU hardware I2C.
 * @n 2. Enable  Macro #define I2C_PIN_REMAP_ENABLE I2C pin remapping macro, the way of connecting SCL and SDA pins is as shown in the following table:
 * @n ---------------------------------------------------------------------------------
 * @n |Module | UNO     | Leonardo  |  Mega2560 |     ESP32    |     M0    | microbit |
 * @n ---------------------------------------------------------------------------------
 * @n |SCL    |  3      |     3     |     3     |   SCL(26)(D3)|   SCL(3)  |  SCL(P8) |
 * @n |SDA    |  5      |     5     |     5     |   SDA(27)(D4)|   SDA(5)  |  SDA(P9) |
 * @n ---------------------------------------------------------------------------------
 * @note When using I2C pin remapping, besides the pins listed above, other IO pins of the MCU can also be selected
 * @note esp8266 does not support I2C pin remapping
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2022-03-02
 * @url https://github.com/DFRobot/DFRobot_GP8302
 */

#include "DFRobot_GP8302.h"


//#define I2C_PIN_REMAP_ENABLE  //Enable I2C pin remapping, remap it to the pins defined by I2C_SCL_PIN and I2C_SDA_PIN, if it's not enabled, the pins used by the hardware Wire object will be used by default.

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
   * @fn output
   * @brief Set the device to output the current of 0-25mA
   * @param current_mA  The output current value, range: 0-25mA
   * @return The DAC value corresponding to the output current value
   * @note calibration4_20mA After calibration, the output function will output the calibrated current value and return the calibrated DAC value
   */
  uint16_t dac = module.output(/*current_mA =*/10); //Control the DAC module to output the current of 10mA and return the DAC value corresponding to the current of 10mA
  Serial.print("DAC value: "); Serial.println(dac);

  /**
   * @fn output_mA
   * @brief Set DAC value to control the device to output the current of 0-25mA.
   * @param dac  Specific DAC value, range 0-0xFFF
   * @note DAC value range is 0-0xFFF, 0-0xFFF DAC value corresponds to the output current of 0-25mA, the formula of DAC value converting to actual current: Iout = (DAC/0xFFF)*25mA
   * @return The actual current, unit mA
   */
  //float current = module.output_mA(/*dac =*/0x666);  //Control the DAC module to output the current corresponding to a DAC value of 0x666 and return the current corresponding to the value, unit mA
  //Serial.print("Output current :"); Serial.print(current); Serial.println(" mA.");
  
  //module.store(); Serial.println("Save current configuration."); //Uncomment this line of code, and the current config above will be saved and will not be lost after power down.
}

void loop(){
  
}
