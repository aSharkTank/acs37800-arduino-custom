// This example shows how to get root mean square (RMS) voltage,
// current and power readings from the ACS37800.

#include <ACS37800.h>
#include <Wire.h>

ACS37800 acs;

void setup()
{
  Wire.begin();
  acs.setBoardPololu(4);

  // RMS readings are calculating by taking voltage and current
  // readings from some number of samples.  The sample count can
  // be any number from 4 to 1023, or you can pass 0 to take
  // samples from one voltage zero-crossing to the next.
  acs.setSampleCount(0);
}

void loop()
{
  acs.readRMSVoltageAndCurrent();
  acs.readActiveAndReactivePower();
  acs.readApparentPowerMilliwatts();
  Serial.print(acs.rmsVoltageMillivolts);
  Serial.print(F(" mV, "));
  Serial.print(acs.rmsCurrentMilliamps);
  Serial.print(F(" mA, "));
  Serial.print(acs.activePowerMilliwatts);
  Serial.print(F(" mW, "));
  Serial.print(acs.reactivePowerMilliwatts);
  Serial.print(F(" mW, "));
  Serial.print(acs.apparentPowerMilliwatts);
  Serial.println(F(" mW"));
  delay(250);
}
