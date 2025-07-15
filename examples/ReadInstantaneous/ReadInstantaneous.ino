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
}

void loop()
{
  acs.readInstVoltageAndCurrent();
  acs.readInstPowerMilliwatts();
  Serial.print(acs.instVoltageMillivolts);
  Serial.print(F(" mV, "));
  Serial.print(acs.instCurrentMilliamps);
  Serial.print(F(" mA, "));
  Serial.print(acs.instPowerMilliwatts);
  Serial.println(F(" mW"));
  delay(250);
}
