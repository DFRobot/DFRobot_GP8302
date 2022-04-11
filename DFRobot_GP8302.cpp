/*!
 * @file DFRobot_GP8302.cpp
 * @brief This I2C 0-25mA DAC module has the following features:
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

#include "DFRobot_GP8302.h"

#ifndef DBG

#if 0  //< Change 0 to 1 to open debug macro and check program debug information
#define DBG(...) {Serial.print("["); Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

#endif 
#define GP8302_DEF_I2C_ADDR                 0x58  ///< The default I2C address of the I2C current DAC module
#define GP8302_CONFIG_CURRENT_REG           0x02  ///< Configure current sensor register
#define GP8302_CURRENT_RESOLUTION           0x0FFF///< Current resolution: 12 bits, 0x0FFF
#define GP8302_MAX_CURRENT                  25    ///< Maximum switching current: 25mA
#define GP8302_STORE_TIMING_HEAD            0x02  ///< Store function timing start head
#define GP8302_STORE_TIMING_ADDR            0x10  ///< The first address for entering store timing
#define GP8302_STORE_TIMING_CMD1            0x03  ///< The command 1 to enter store timing
#define GP8302_STORE_TIMING_CMD2            0x00  ///< The command 2 to enter store timing
#define GP8302_STORE_TIMING_DELAY           10    ///< Store procedure interval delay time: 10ms, more than 7ms
#define I2C_CYCLE_TOTAL                     5     ///< Total I2C communication cycle
#define I2C_CYCLE_BEFORE                    2     ///< The first half cycle of the total I2C communication cycle
#define I2C_CYCLE_AFTER                     3     ///< The second half cycle of the total I2C communication cycle


 DFRobot_GP8302::DFRobot_GP8302()
   :_digital(0), _scl(SCL), _sda(SDA), _twiaddr(GP8302_DEF_I2C_ADDR),_calibration(false){}

uint8_t DFRobot_GP8302::begin(int scl, int sda){
  if(scl >= 0) _scl = scl;
  if(sda >= 0) _sda = sda;
  if(_scl < 0 || _sda < 0){
    DBG("_scl or _sda pin is invaild.");
    return 1;
  }
  pinMode(_scl, OUTPUT);
  pinMode(_sda, OUTPUT);
  digitalWrite(_scl, HIGH);
  digitalWrite(_sda, HIGH);
  
  startSignal();
  if (sendByte(_twiaddr << 1) != 0){
    DBG("Device not found, please check if the device is connected.");
    stopSignal();
    return 2;
  }
  stopSignal();
  return 0;
}

void DFRobot_GP8302::calibration4_20mA(uint16_t dac_4, uint16_t dac_20){
  if((dac_4 >= dac_20) || (dac_20 > GP8302_CURRENT_RESOLUTION)) return;
  _dac_4 = dac_4;
  _dac_20 = dac_20;
  _calibration = true;
}

float DFRobot_GP8302::output_mA(uint16_t dac){
  if(dac > GP8302_CURRENT_RESOLUTION) return _digital;
  _digital = dac & GP8302_CURRENT_RESOLUTION;
  startSignal();
  sendByte(_twiaddr<<1);
  sendByte(GP8302_CONFIG_CURRENT_REG);
  sendByte((_digital << 4) & 0xF0);
  sendByte((_digital >> 4)&0xFF);
  stopSignal();

  DBG(_digital, HEX);
  return float((_digital/(GP8302_CURRENT_RESOLUTION * 1.0))*GP8302_MAX_CURRENT);
}

uint16_t DFRobot_GP8302::output(float current_mA){
  if(current_mA < 0) current_mA = 0;
  if(current_mA > GP8302_MAX_CURRENT) current_mA = GP8302_MAX_CURRENT;

  if(_calibration && (current_mA >= 4) && (current_mA <= 20)){
    _digital = _dac_4 + ((current_mA - 4)*(_dac_20 - _dac_4)/(20-4));
    DBG(_digital);
    DBG(((_dac_4 + ((current_mA - 4)*(_dac_20 - _dac_4)/((20-4)*1.0))) - _digital)*10);
    DBG((_dac_4 + ((current_mA - 4)*(_dac_20 - _dac_4)/((20-4)*1.0))));
    if(((_dac_4 + ((current_mA - 4)*(_dac_20 - _dac_4)/((20-4)*1.0))) - _digital)*10 >= 5) _digital += 1;
    
  }else{
    _digital = (current_mA * GP8302_CURRENT_RESOLUTION)/GP8302_MAX_CURRENT;
    if((((current_mA * GP8302_CURRENT_RESOLUTION)/(GP8302_MAX_CURRENT*1.0)) - _digital)*10 >= 5) _digital += 1;
    DBG(_digital);
    DBG(((current_mA * GP8302_CURRENT_RESOLUTION)/(GP8302_MAX_CURRENT*1.0)));
    DBG((((current_mA * GP8302_CURRENT_RESOLUTION)/(GP8302_MAX_CURRENT*1.0)) - _digital)*10);
  }
  
  output_mA(_digital);
  return _digital;
}

void DFRobot_GP8302::store(){
  startSignal();
  sendByte(GP8302_STORE_TIMING_HEAD, 0, 3, false);
  stopSignal();
  startSignal();
  sendByte(GP8302_STORE_TIMING_ADDR);
  sendByte(GP8302_STORE_TIMING_CMD1);
  stopSignal();
  
  startSignal();
  sendByte(_twiaddr<<1, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  sendByte(GP8302_STORE_TIMING_CMD2, 1);
  stopSignal();

  delay(GP8302_STORE_TIMING_DELAY);

  startSignal();
  sendByte(GP8302_STORE_TIMING_HEAD, 0, 3, false);
  stopSignal();
  startSignal();
  sendByte(GP8302_STORE_TIMING_ADDR);
  sendByte(GP8302_STORE_TIMING_CMD2);
  stopSignal();

}

void DFRobot_GP8302::startSignal(void){
  digitalWrite(_scl,HIGH);
  digitalWrite(_sda,HIGH);
  delayMicroseconds(I2C_CYCLE_BEFORE);
  digitalWrite(_sda,LOW);
  delayMicroseconds(I2C_CYCLE_AFTER);
  digitalWrite(_scl,LOW);
  delayMicroseconds(I2C_CYCLE_TOTAL);
}

void DFRobot_GP8302::stopSignal(void){
  digitalWrite(_sda,LOW);
  delayMicroseconds(I2C_CYCLE_BEFORE);
  digitalWrite(_scl,HIGH);
  delayMicroseconds(I2C_CYCLE_TOTAL);
  digitalWrite(_sda,HIGH);
  delayMicroseconds(I2C_CYCLE_TOTAL);
}


uint8_t DFRobot_GP8302::sendByte(uint8_t data, uint8_t ack, uint8_t bits, bool flag){
  for(int i=bits-1; i>=0;i--){  
    if(data & (1<<i)){
      digitalWrite(_sda,HIGH);//Change the status of sda level during scl low level, and it lasts for some time
    }else{
      digitalWrite(_sda,LOW);
    }
    delayMicroseconds(I2C_CYCLE_BEFORE); 
    digitalWrite(_scl,HIGH);
    delayMicroseconds(I2C_CYCLE_TOTAL);
    //while(digitalRead(_scl) == 0){
        //delayMicroseconds(1);
    //}
    digitalWrite(_scl,LOW);
    delayMicroseconds(I2C_CYCLE_AFTER); 
  }
  if(flag) return recvAck(ack);
  else {
    digitalWrite(_sda,LOW);//
    digitalWrite(_scl,HIGH);//
    return 0;
  }
}


uint8_t DFRobot_GP8302::recvAck(uint8_t ack){
  uint8_t ack_=0;
  uint16_t errorTime = 0;
  pinMode(_sda,INPUT_PULLUP);
  digitalWrite(_sda,HIGH);
  delayMicroseconds(I2C_CYCLE_BEFORE);
  digitalWrite(_scl,HIGH);
  delayMicroseconds(I2C_CYCLE_AFTER);
  while(digitalRead(_sda) != ack){
      delayMicroseconds(1);
      errorTime++;
      if(errorTime > 250) break;
  }
  ack_=digitalRead(_sda);
  delayMicroseconds(I2C_CYCLE_BEFORE);
  digitalWrite(_scl,LOW);
  delayMicroseconds(I2C_CYCLE_AFTER);
  pinMode(_sda,OUTPUT);
  return ack_;
}

