# -*- coding:utf-8 -*-
from __future__ import print_function

'''!
  @file DFRobot_GP8302.py
  @brief This I2C to 0-25mA DAC module has the following features:
  @n 1. Require an external power supply, range: 18-24V, maximum carrying capacity of 450R for 18V power supply, and 650R for 24V.
  @n 2. Output DC within 0-25mA.
  @n 3. It can control the output current with an I2C interface, the I2C address is default to be 0x58.
  @n 4. The output current config will be lost after the module is powered down. Save the config if you want to use it for the next power-up.
  @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
  @license     The MIT License (MIT)
  @author [Arya](xue.peng@dfrobot.com)
  @version  V1.0
  @date  2022-03-02
  @url https://github.com/DFRobot/DFRobot_GP8302
'''

import sys
import time
import RPi.GPIO as GPIO

class DFRobot_GP8302:
  ## The default I2C address of the I2C to current DAC module
  GP8302_DEF_I2C_ADDR         =    0x58
  ## Configure current sensor register   
  GP8302_CONFIG_CURRENT_REG   =    0x02 
  ## Current resolution: 12 bits, 0x0FFF  
  GP8302_CURRENT_RESOLUTION   =    0x0FFF 
  ## Maximum conversion current: 25mA
  GP8302_MAX_CURRENT          =    25 
  ## Store function timing start head    
  GP8302_STORE_TIMING_HEAD    =    0x02  
  ## The initial address for entering store timing 
  GP8302_STORE_TIMING_ADDR    =    0x10  
  ## The command 1 to enter store timing 
  GP8302_STORE_TIMING_CMD1    =    0x03 
  ## The command 2 to enter store timing  
  GP8302_STORE_TIMING_CMD2    =    0x00 
   ## Interval delay time in storing: 10ms, more than 7ms  
  GP8302_STORE_TIMING_DELAY   =    0.01    
  ## Total I2C communication cycle 5us
  I2C_CYCLE_TOTAL             =    0.000005 
  ## The first half cycle of the total I2C communication cycle 2us    
  I2C_CYCLE_BEFORE            =    0.000002
  ## The second half cycle of the total I2C communication cycle 3us      
  I2C_CYCLE_AFTER             =    0.000003      
 
  _digital = 0
  ## The io pin of raspberry pi: 3 (BCM)
  _scl     = 3
  ## The io pin of raspberry pi: 2 (BCM)
  _sda     = 2
  _addr    = GP8302_DEF_I2C_ADDR

  _dac_4       = 0
  _dac_20      = 0
  _calibration = False
  
  def __init__(self):
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)

  def begin(self, scl = -1, sda = -1):
    '''!
      @brief   Remap the I2C pins of the MCU connected to the DAC module, and initialize the module
      @param scl  IO port pin of Raspberry Pi
      @param sda  IO port pin of Raspberry Pi
      @return  The value for init status
      @retval  0    Init succeeded
      @retval  1    The I2C pin of the MCU connected to the DAC module is invalid.
      @retval  2    Device not found, please check if the connection is correct
    '''
    if scl >= 0:
      self._scl = scl
    if sda >= 0:
      self._sda = sda
    
    if self._scl < 0 or self._sda < 0:
      #print("_scl or _sda pin is invaild.")
      return 1

    GPIO.setup(self._scl, GPIO.OUT, initial = GPIO.HIGH)
    GPIO.setup(self._sda, GPIO.OUT, initial = GPIO.HIGH)
    #GPIO.output(self._scl, GPIO.HIGH)
    #GPIO.output(self._sda, GPIO.HIGH)

    self._start_signal()
    if self._send_byte(self._addr << 1) != 0:
      #print("Device not found, please check if the device is connected.")
      self._stop_signal()
      return 2
    self._stop_signal()
    return 0

  def calibration4_20mA(self, dac_4 = 655, dac_20 = 3277):
    '''!
      @brief   Calibrate the current within 4-20mA
      @param dac_4   Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 4mA generally fluctuates at about 655, the actual value needs to be tested by the user in actual applications
      @param dac_20  Range 0-0xFFF, the calibration is invalid if the value is out of range, the DAC value corresponding to current of 20mA generally fluctuates at about 3277, the actual value needs to be tested by the user in actual applications
      @note The method of obtaining the DAC value corresponding to the current of 4mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 655, the actual DAC value is the one input into the instrument when the measured current is 4mA.
      @n The method of obtaining the DAC value corresponding to the current of 20mA in actual applications: use output_mA(uint16_t dac) function, pass the DAC parameter fluctuating at about 3277, the actual DAC value is the one input into the instrument when the measured current is 20mA.
      @note Parameter dac_4 and dac_20 should meet the conditions: dac_4 < dac_20, after the calibration is enabled, output function will output the calibrated current value
    '''
    if dac_4 >= dac_20 or dac_20 > self.GP8302_CURRENT_RESOLUTION:
      return None
    self._dac_4       = dac_4
    self._dac_20      = dac_20
    self._calibration = True

  
  def output_mA(self, dac):
    '''!
      @brief   Set DAC value to control the device to output the current of 0-25mA.
      @param dac  Specific DAC value, range 0-0xFFF
      @note DAC value range is 0-0xFFF, 0-0xFFF DAC value corresponds to the output current of 0-25mA, the formula of DAC value converting to actual current: Iout = (DAC/0xFFF)*25mA
      @return Calculate current value (may be different from the actual measured current), unit mA
    '''
    self._digital = dac & self.GP8302_CURRENT_RESOLUTION
    self._start_signal()
    self._send_byte(self._addr<<1)
    self._send_byte(self.GP8302_CONFIG_CURRENT_REG)
    self._send_byte((self._digital << 4) & 0xF0)
    self._send_byte((self._digital >> 4)&0xFF)
    self._stop_signal()

    return float((self._digital/(self.GP8302_CURRENT_RESOLUTION * 1.0))*self.GP8302_MAX_CURRENT)
  
  def output(self, current_mA):
    '''!
      @brief   Set DAC value to control the device to output the current of 0-25mA.
      @param current_mA  The output current value, range: 0-25mA
      @return The DAC value corresponding to the output current value
      @note calibration4_20mA After calibration, the output function will output the calibrated current value and return the calibrated DAC value
    '''
    if current_mA < 0:
      current_mA = 0
    if current_mA > self.GP8302_MAX_CURRENT:
      current_mA = self.GP8302_MAX_CURRENT
    if self._calibration and current_mA >= 4 and current_mA <= 20:
      self._digital = self._dac_4 + ((current_mA - 4)*(self._dac_20 - self._dac_4))//(20 - 4)
      if ((self._dac_4 + ((current_mA - 4)*(self._dac_20 - self._dac_4))/(20 - 4)) - self._digital) * 10 >= 5:
        self._digital += 1
    else:
      self._digital = (current_mA * self.GP8302_CURRENT_RESOLUTION)//self.GP8302_MAX_CURRENT
      if((((current_mA * self.GP8302_CURRENT_RESOLUTION)/(self.GP8302_MAX_CURRENT*1.0)) - self._digital)*10 >= 5):
        self._digital += 1
    self.output_mA(self._digital)
    return self._digital


  def store(self):
    '''!
      @brief   Save the current config, after the config is saved successfully, it will be enabled when the module is powered down and restarts.
    '''
    self._start_signal()
    self._send_byte(self.GP8302_STORE_TIMING_HEAD, 0, 3, False)
    self._stop_signal()
    self._start_signal()
    self._send_byte(self.GP8302_STORE_TIMING_ADDR)
    self._send_byte(self.GP8302_STORE_TIMING_CMD1)
    self._stop_signal()

    self._start_signal()
    self._send_byte(self._addr<<1, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2, 1)
    self._stop_signal()

    time.sleep(self.GP8302_STORE_TIMING_DELAY)

    self._start_signal()
    self._send_byte(self.GP8302_STORE_TIMING_HEAD, 0, 3, False)
    self._stop_signal()
    self._start_signal()
    self._send_byte(self.GP8302_STORE_TIMING_ADDR)
    self._send_byte(self.GP8302_STORE_TIMING_CMD2)
    self._stop_signal()



  def _start_signal(self):
    GPIO.output(self._scl, GPIO.HIGH)
    GPIO.output(self._sda, GPIO.HIGH)
    time.sleep(self.I2C_CYCLE_BEFORE)
    GPIO.output(self._sda, GPIO.LOW)
    time.sleep(self.I2C_CYCLE_AFTER)
    GPIO.output(self._scl, GPIO.LOW)
    time.sleep(self.I2C_CYCLE_TOTAL)
  
  def _stop_signal(self):
    GPIO.output(self._sda, GPIO.LOW)
    time.sleep(self.I2C_CYCLE_BEFORE)
    GPIO.output(self._scl, GPIO.HIGH)
    time.sleep(self.I2C_CYCLE_TOTAL)
    GPIO.output(self._sda, GPIO.HIGH)
    time.sleep(self.I2C_CYCLE_TOTAL)

  def _recv_ack(self, ack = 0):
    ack_ = 0
    error_time = 0
    GPIO.setup(self._sda, GPIO.IN)
    time.sleep(self.I2C_CYCLE_BEFORE)
    GPIO.output(self._scl, GPIO.HIGH)
    time.sleep(self.I2C_CYCLE_AFTER)
    while GPIO.input(self._sda) != ack:
      time.sleep(0.000001)
      error_time += 1
      if error_time > 250:
        break
    ack_ = GPIO.input(self._sda)
    time.sleep(self.I2C_CYCLE_BEFORE)
    GPIO.output(self._scl, GPIO.LOW)
    time.sleep(self.I2C_CYCLE_AFTER)
    GPIO.setup(self._sda, GPIO.OUT)
    return ack_

  def _send_byte(self, data, ack = 0, bits = 8, flag = True):
      i = bits
      data = data & 0xFF
      while i > 0:
        i -= 1
        if data & (1 << i):
          GPIO.output(self._sda, GPIO.HIGH)
        else:
          GPIO.output(self._sda, GPIO.LOW)
        time.sleep(self.I2C_CYCLE_BEFORE)
        GPIO.output(self._scl, GPIO.HIGH)
        time.sleep(self.I2C_CYCLE_TOTAL)
        GPIO.output(self._scl, GPIO.LOW)
        time.sleep(self.I2C_CYCLE_AFTER)
      if flag:
        return self._recv_ack(ack)
      else:
        GPIO.output(self._sda, GPIO.LOW)
        GPIO.output(self._scl, GPIO.HIGH)
        return ack
    
