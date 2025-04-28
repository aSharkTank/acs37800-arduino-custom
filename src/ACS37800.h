// Copyright (C) Pololu Corporation.  See LICENSE.txt for details.

/// \file ACS37800.h
///
/// This is the main header file for the ACS37800 power monitoring library
/// for Arduino.
///
/// For more information about the library, see the main repository at:
/// https://github.com/pololu/acs37800-arduino

#pragma once

#include <Arduino.h>
#include <Wire.h>

class ACS37800
{
public:
  /// Creates a new ACS37800 object.
  ///
  /// The `address` parameter specifies the 7-bit I2C address to use, and it
  /// must match the address that the Motoron is configured to use.
  ACS37800(uint8_t address = 0x60, TwoWire * bus = &Wire)
    : bus(bus), address(address) {}

  /// Configures this object to use the specified I2C bus.
  /// The default bus is Wire, which is typically the first or only I2C bus on
  /// an Arduino.  To use Wire1 instead, you can write:
  /// ```{.cpp}
  /// mc.setBus(&Wire1);
  /// ```
  /// \param bus A pointer to a TwoWire object representing the I2C bus to use.
  void setBus(TwoWire * bus)
  {
    this->bus = bus;
  }

  /// Returns a pointer to the I2C bus that this object is configured to
  /// use.
  TwoWire * getBus()
  {
    return this->bus;
  }

  /// Configures this object to use the specified 7-bit I2C address.
  /// This must match the address that the ACS37800 is configured to use,
  /// which is a function of its EEPROM settings and its DIO_0 and DIO_1
  /// connections.
  void setAddress(uint8_t address)
  {
    this->address = address;
  }

  /// Returns the 7-bit I2C address that this object is configured to use.
  uint8_t getAddress()
  {
    return address;
  }

  /// Returns 0 if the last communication with the device was successful, or
  /// a non-zero error code if there was an error.
  uint8_t getLastError()
  {
    return lastError;
  }

  /// Configures this object to use the right calculation parameters for a
  /// Pololu ACS37800 isolated power monitor carrier board.
  ///
  /// The `rsense_kohm` argument should be the Rsense value of your board, in
  /// units of kilohms, which depends on the jumper settings of your board.
  /// See the "Voltage measurement ranges" section of your board's product page
  /// to determine the Rsense value.  Valid values are 1, 2, and 4.
   void setBoardPololu(uint8_t rsense_kohm)
  {
    icodesMult = 17873;
    icodesShift = 14;
    switch (rsense_kohm)
    {
    case 1:
      vcodesMult = 18623;
      vcodesShift = 9;
      pinstantMult = 1299;
      pinstantShift = 0;
      break;
    case 2:
      vcodesMult = 18627;
      vcodesShift = 10;
      pinstantMult = 10395;
      pinstantShift = 4;
      break;
    default:
    case 4:
      vcodesMult = 18637;
      vcodesShift = 11;
      pinstantMult = 325;
      pinstantShift = 0;
      break;
    }
  }

  /// Initializes this object.  This should be called before taking any readings.
  /// The return value is true if the initialization completed successfully.
  bool init()
  {
    // TODO: read some register to confirm we are talking to an ACS37800?

    // TODO: get or set current coarse gain?  What effect does that have?
    return true;
  }

  // Configures the sensor to use the specified number of samples for RMS and
  // power calculations.  Samples are taken at 32 kHz.
  // The count should be a number between 0 and 1023.
  // 1, 2, and 3 are treated the same as 4 by the ACS37800.
  // 0 means to take samples from one zero crossing to the next, instead of
  // taking a fixed number of samples.
  // This function only reads and writes from the shadow registers, not EEPROM,
  // so the settings applied will not be stored permanently.
  void setSampleCount(uint16_t count)
  {
    uint32_t reg = readReg(0x1F);
    if (getLastError()) { return; }

    if (count > 1023) { count = 1023; }

    // Clear N and BYPASS_N_EN, then set them if necessary.
    reg &= 0xFE003FFF;
    if (count) { reg |= ((uint32_t)1 << 24) | ((uint32_t)count << 14); }

    writeReg(0x1F, reg);
  }

  /// Reads the root mean square (RMS) voltage and current measurements from
  /// the sensor, converts them to mV and mA respectively, and stores them in
  /// the rmsVoltageMillivolts and rmsCurrentMilliamps members.
  void readRMSVoltageAndCurrent()
  {
    uint32_t reg = readReg(0x20);
    uint16_t vrms = (uint16_t)reg;
    uint16_t irms = (uint16_t)(reg >> 16);
    rmsVoltageMillivolts = (int32_t)vrms * vcodesMult >> vcodesShift >> 1;
    rmsCurrentMilliamps = (int32_t)irms * icodesMult >> icodesShift >> 1;
  }

  /// Reads the active and reactive power from the sensor, converts both to
  /// units of mW, and stores them in the activePowerMilliwatts and
  /// reactivePowerMilliwatts members.
  void readActiveAndReactivePower()
  {
    uint32_t reg = readReg(0x21);
    int16_t pactive = (int16_t)reg;
    int16_t pimag = (int16_t)(reg >> 16);
    activePowerMilliwatts = (int32_t)pactive * pinstantMult >> pinstantShift;
    reactivePowerMilliwatts = (int32_t)pimag * pinstantMult >> pinstantShift;
  }

  /// Reads the apparent power from the sensor and returns it in mW.
  int32_t readApparentPowerMilliwatts()
  {
    // Note: maybe this function should also store the power factor and
    // other things in register 0x22.
    uint32_t reg = readReg(0x22);
    uint16_t papparent = (uint16_t)reg;
    apparentPowerMilliwatts = (int32_t)papparent * pinstantMult >> pinstantShift >> 1;
    return apparentPowerMilliwatts;
  }

  /// Reads the instantaneous voltage and current measurements from the sensor
  /// (VCODES and ICODES), converts them to mV and mA respectively, and stores
  /// them in the instVoltageMillivolts and instCurrentMilliamps members.
  void readInstVoltageAndCurrent()
  {
    uint32_t reg = readReg(0x2A);
    int16_t vcodes = (int16_t)reg;
    int16_t icodes = (int16_t)(reg >> 16);
    instVoltageMillivolts = (int32_t)vcodes * vcodesMult >> vcodesShift;
    instCurrentMilliamps = (int32_t)icodes * icodesMult >> icodesShift;
  }

  /// Reads the instananeous power measurement (PINSTANT) from the sensor
  /// and returns its value converted to milliwatts (mW).
  int32_t readInstPowerMilliwatts()
  {
    int16_t pinstant = (int16_t)readReg(0x2C);
    instPowerMilliwatts = (int32_t)pinstant * pinstantMult >> pinstantShift;
    return instPowerMilliwatts;
  }

  /// Reads the root mean square (RMS) voltage measurement from the sensor
  /// and returns its value converted to millivolts (mV).
  ///
  /// If you need the current and the voltage, it is more efficient to use
  /// readRMSVoltageAndCurrent() instead.
  int32_t readRMSVoltageMillivolts()
  {
    readRMSVoltageAndCurrent();
    return rmsVoltageMillivolts;
  }

  /// Reads the root mean square (RMS) current measurement from the sensor
  /// and returns its value converted to millivolts (mV).
  ///
  /// If you need the current and the voltage, it is more efficient to use
  /// readRMSVoltageAndCurrent() instead.
  int32_t readRMSCurrentMilliamps()
  {
    readRMSVoltageAndCurrent();
    return rmsCurrentMilliamps;
  }

  /// Reads the active power from the sensor and returns its value converted to
  /// milliwatts.
  ///
  /// If you need the active and reactive power, it is better to use
  /// readActiveAndReactivePower().
  int32_t readActivePowerMilliwatts()
  {
    readActiveAndReactivePower();
    return activePowerMilliwatts;
  }

  /// Reads the reactive (imaginary) power from the sensor and returns its
  /// value converted to milliwatts.
  ///
  /// If you need the active and reactive power, it is better to use
  /// readActiveAndReactivePower().
  int32_t readReactivePowerMilliwatts()
  {
    readActiveAndReactivePower();
    return reactivePowerMilliwatts;
  }

  /// Reads the instantaneous voltage measurement from the sensor,
  /// and returns its value converted to millivolts (mV).
  ///
  /// If you need the current and the voltage, it is more efficient to use
  /// readInstVoltageAndCurrent() instead.
  int32_t readInstVoltageMillivolts()
  {
    readInstVoltageAndCurrent();
    return instVoltageMillivolts;
  }

  /// Reads the instantaneous current measurement from the sensor
  /// and returns its value converted to milliamps (mA).
  ///
  /// If you need the current and the voltage, it is more efficient to use
  /// readInstVoltageAndCurrent() instead.
  int32_t readInstCurrentMilliamps()
  {
    readInstVoltageAndCurrent();
    return instCurrentMilliamps;
  }

  /// Sets the 7-bit I2C device address of the sensor.
  ///
  /// The new address does not take effect until the sensor is power cycled.
  void setI2CAddress(uint8_t address)
  {
    writeReg(0x2F, 0x4F70656E);  // Write the access code
    if (lastError) { return; }
    uint32_t reg = readReg(0x0F);
    if (lastError) { return; }
    reg = (reg & ~(uint32_t)0x3FC) | (1 << 9) | (address & 0x7F << 2);
    writeReg(0x1F, reg);
    if (lastError) { return; }
    writeReg(0x2F, 0);  // Disable access.  // TODO: test that this works
    // TODO: do we need a delay here to give it time to finish writing EEPROM?
  }

  /// Reads a sensor register and returns its value.
  uint32_t readReg(uint8_t reg)
  {
    bus->beginTransmission(address);
    bus->write(reg);
    lastError = bus->endTransmission();
    if (lastError) { return 0; }

    uint8_t byteCount = bus->requestFrom(address, (uint8_t)4);
    if (byteCount != 4)
    {
      lastError = 50;
      return 0;
    }

    uint32_t value = bus->read();
    value |= (uint32_t)bus->read() << 8;
    value |= (uint32_t)bus->read() << 16;
    value |= (uint32_t)bus->read() << 24;
    return value;
  }

  /// Writes to a sensor register.
  void writeReg(uint8_t reg, uint32_t value)
  {
    bus->beginTransmission(address);
    bus->write(reg);
    bus->write(value & 0xFF);
    bus->write(value >> 8 & 0xFF);
    bus->write(value >> 16 & 0xFF);
    bus->write(value >> 24 & 0xFF);
    lastError = bus->endTransmission();
  }

  int32_t instVoltageMillivolts;
  int32_t instCurrentMilliamps;
  int32_t instPowerMilliwatts;

  int32_t rmsVoltageMillivolts;
  int32_t rmsCurrentMilliamps;
  int32_t activePowerMilliwatts;
  int32_t reactivePowerMilliwatts;
  int32_t apparentPowerMilliwatts;

private:
  TwoWire * bus;
  uint8_t address;

  uint8_t lastError = 0;

  uint16_t vcodesMult = 1, icodesMult = 1, pinstantMult = 1;
  uint8_t vcodesShift = 0, icodesShift = 0, pinstantShift = 0;
};
