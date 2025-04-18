// This example shows how to get root mean square (RMS) current and power
// readings from the ACS37800.

#include <ACS37800.h>
#include <Wire.h>

ACS37800 acs;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  acs.setBoardPololu(4);
  if (!acs.init())
  {
    while (1)
    {
      Serial.println(F("Failed to initialize ACS37800."));
      delay(1000);
    }
  }
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
