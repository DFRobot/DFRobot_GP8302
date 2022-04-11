/*!
 * @file DFRobot_GP8302.h
 * @brief This I2C to 0-25mA DAC module has the following features:
 * @n 1. Require an external power supply, range: 18-24V, maximum carrying capacity of 450R for 18V power supply, and 650R for 24V.
 * @n 2. Output DC within 0-25mA.
 * @n 3. It can control the output current with an I2C interface, the I2C address is default to be 0x58.
 * @n 4. The output current config will be lost after the module is powered down. Save the config if you want to use it for the next power-up.
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2022-02-28
 * @url https://github.com/DFRobot/DFRobot_GP8302
 */
#ifndef DFROBOT_GP8302_H
#define DFROBOT_GP8302_H

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <Wire.h>


class DFRobot_GP8302{
public:
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
   */
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

protected:
  void startSignal(void);
  void stopSignal(void);
  uint8_t recvAck(uint8_t ack);
  uint8_t sendByte(uint8_t data, uint8_t ack = 0, uint8_t bits = 8, bool flag = true);
private:
  uint16_t _digital;
  int _scl;
  int _sda;
  uint8_t _twiaddr;
  bool _calibration;
  uint16_t _dac_4;
  uint16_t _dac_20;
};
#endif
