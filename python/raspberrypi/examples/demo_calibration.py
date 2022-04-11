# -*- coding:utf-8 -*-
from __future__ import print_function

'''!
  @file demo_set_current.py
  @brief   Calibrate the current within 4-20mA
  @note The carrying capacity of this module is related to the external power supply voltage: range 18-24V, maximum 450R carrying capacity for 18V power supply, and 650R for 24V.
  @n The hardware connection in this demo:
  @n 1. Disable  I2C pin remapping, i.e., I2C_PIN_REMAP_ENABLE = 0, the I2C pin of Raspberry Pi is connected currently, i.e., the (3, 2) pin in BCM code
  @n ------------------------
  @n |Module | raspberry pi |
  @n ------------------------
  @n |SCL    |     3(BCM)   |
  @n |SDA    |     2(BCM)   |
  @n ------------------------
  @n 2. Enable  I2C pin remapping, i.e., I2C_PIN_REMAP_ENABLE = 1, the way of connecting SCL and SDA pins is as shown in the following table.
  @n ------------------------
  @n |Module | raspberry pi |
  @n ------------------------
  @n |SCL    |     5(BCM)   |
  @n |SDA    |     6(BCM)   |
  @n ------------------------
  @note When using I2C pin remapping, besides the pins listed above, other IO pins of Raspberry Pi can also be selected
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license     The MIT License (MIT)
  @author [Arya](xue.peng@dfrobot.com)
  @version  V1.0
  @date  2022-03-16
  @url https://github.com/DFRobot/DFRobot_GP8302
'''

import sys
import os
import time

sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
from DFRobot_GP8302 import *
#Enable I2C pin remapping
#I2C_PIN_REMAP_ENABLE = 1 
#Disable I2C pin remapping
I2C_PIN_REMAP_ENABLE = 0


if I2C_PIN_REMAP_ENABLE:
  I2C_SCL_PIN = 5  # 5 is the io pin of raspberry pi ,which is BCM coding  
  I2C_SDA_PIN = 6  # 6 is the io pin of raspberry pi ,which is BCM coding  

module = DFRobot_GP8302()

if __name__ == "__main__":
  print("I2C to 0-25 mA analog current moudle initialization ... ", end=" ")
  status = 0
  if I2C_PIN_REMAP_ENABLE:
    #I2C scl and sda pins redefine
    status = module.begin(scl = I2C_SCL_PIN, sda = I2C_SDA_PIN)
  else:
    #Default to use the pins used by the Raspberry Pi hardware I2C1, and scl = 3(BCM) & sda = 2(BCM)
    status = module.begin()
  if status != 0:
    print("failed. Error code: %d" %status)
    print("Error Code: ")
    print("    1: _scl or _sda pin is invaild.")
    print("    2: Device not found, please check if the device is connected.")
    while 1:
      time.sleep(1)

  print("done!")
  
  '''!
    @brief   Calibrate the current within 4-20mA
    @param dac_4   Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 4mA generally fluctuates at about 655, the actual value needs to be tested by the user in actual applications
    @param dac_20  Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 20mA generally fluctuates at about 3277, the actual value needs to be tested by the user in actual applications
    @note The method of obtaining the DAC value corresponding to the current of 4mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 655, the actual DAC value is the one input into the instrument when the measured current is 4mA.
    @n The method of obtaining the DAC value corresponding to the current of 20mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 3277, the actual DAC value is the one input into the instrument when the measured current is 20mA.
    @note Parameter dac_4 and dac_20 should meet the conditions: dac_4 < dac_20, after the calibration is enabled, output function will output the calibrated current value
  '''
  module.calibration4_20mA(dac_4 = 655, dac_20 = 3277)
  
  '''!
    @brief   Set DAC value to control the device to output the current of 0-25mA.
    @param current_mA  The output current value, range: 0-25mA
    @return The DAC value corresponding to the output current value
    @note calibration4_20mA After calibration, the output function will output the calibrated current value and return the calibrated DAC value
  '''
  #Control the DAC module to output the current of 10mA and return the DAC value corresponding to the current of 10mA
  dac = module.output(current_mA = 10)
  print("DAC value: 0x%x"%dac)
  
  

