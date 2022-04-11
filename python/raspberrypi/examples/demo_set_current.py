# -*- coding:utf-8 -*-
from __future__ import print_function

'''!
  @file demo_set_current.py
  @brief   This I2C 0-25mA DAC module can be used to output a current of 0-25mA.
  @note The carrying capacity of this module is related to the external power supply voltage: range 18-24V, maximum 450R carrying capacity for 18V power supply, and 650R for 24V.
  @n This demo is used for demonstration. Control the module to output the current of 10mA and save the config to make sure it is not lost when the module is powered up again.
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
  @date  2022-03-03
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
    #I2C Redefine scl and sda pins
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
    @brief   Set DAC value to control the device to output the current of 0-25mA.
    @param current_mA  The output current value, range: 0-25mA
    @return The DAC value corresponding to the output current value
    @note calibration4_20mA After calibration, the output function will output the calibrated current value and return the calibrated DAC value
  '''
  #Control the DAC module to output the current of 10mA and return the DAC value corresponding to the current of 10mA
  dac = module.output(current_mA = 10)
  print("DAC value: 0x%x"%dac)
  
  #Control the DAC module to output the current corresponding to a DAC value of 0x666 and return the current corresponding to the value, unit mA
  '''
  current = module.output_mA(dac = 0x666)
  print("Output current : %.2f mA"%current)
  '''

  #Uncomment the code here, and the current config above will be saved and will not be lost after power down.
  #'''
  module.store()
  print("Save current configuration.")
  #'''
  

