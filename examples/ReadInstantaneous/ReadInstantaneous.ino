// This example shows how to get instantaneous voltage, current,
// and power readings from the ACS37800.

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
}

void loop()
{
  acs.readInstVoltageAndCurrent();
  acs.readInstPower();
  Serial.print(acs.instVoltageMillivolts);
  Serial.print(F(" mV, "));
  Serial.print(acs.instCurrentMilliamps);
  Serial.print(F(" mA, "));
  Serial.print(acs.instPowerMilliwatts);
  Serial.println(F(" mW"));
  delay(250);
}
