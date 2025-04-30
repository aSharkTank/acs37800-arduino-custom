// This example sketch provides an interactive utility you can use to set the
// I2C address for an ACS37800 sensor.
//
// After uploading this sketch to your Arduino, open the Serial Monitor
// (from the Tools menu of the Arduino IDE).  The top of the serial monitor
// contains a box where you can type commands and send them to
// this sketch by pressing Ctrl + Enter.
//
// By default, most of the commands in this sketch use address 96 (0x60).
// If your ACS37800 is on a different address, send "t" followed by the address
// (in decimal) to change the sketch to talk to that address instead.
// For example, "t17" changes the sketch to use target address 17 (0x11).
//
// If you're not sure what address your ACS37800 is using, you can use the "s"
// command to do a simple scan of the bus to see which addresses have devices
// responding.
//
// To assign an I2C address to an ACS37800, send "a" followed by the address
// (in decimal).  For example, "a17" sets the address of the ACS37800 to
// decimal 17.
//
// To make the ACS37800 start using its new address, you need to power cycle it.
//
// This sketch expects each command to be terminated with a line-ending
// character ('\r' or '\n').  The serial monitor automatically sends a
// line-ending character when you send a message.

#include <ACS37800.h>

uint8_t lineSize;
char lineBuffer[40];

#ifdef __SAM3X8E__
// Arduino Due uses Wire1
#define WIRE Wire1
#else
#define WIRE Wire
#endif

ACS37800 acs(96, &WIRE);

// This function defines which I2C addresses this sketch is
// allowed to communicate with.
bool allowAddressCommunication(uint8_t address)
{
  // Addresses cannot be larger than 127.
  if (address >= 128) { return false; }

  // If you have devices on your bus and you want to
  // prevent this sketch from talking to them, potentially
  // causing unwanted operations, add their 7-bit addresses here
  // with a line like this:
  // if (address == 0x6B) { return false; }

  return true;
}

// This function defines which I2C addresses this sketch is
// allowed to assign to a device.
bool allowAddressAssignment(uint8_t address)
{
  // These addresses are reserved.
  if (address <= 7 || address >= 120) { return false; }

  return allowAddressCommunication(address);
}

void assignAddress()
{
  uint8_t desiredAddress = strtoul(lineBuffer + 1, NULL, 10) & 127;

  if (!allowAddressAssignment(desiredAddress))
  {
    Serial.print(F("Assignment to address "));
    Serial.print(desiredAddress);
    Serial.println(" not allowed.");
    return;
  }

  acs.writeEepromI2CAddress(desiredAddress);
  if (acs.getLastError())
  {
    Serial.print(F("Error "));
    Serial.println(acs.getLastError());
    return;
  }

  Serial.print(F("Assigned address "));
  Serial.println(desiredAddress);
  delay(50);
}

void scanForDevices()
{
  Serial.println(F("Scanning for I2C devices..."));
  for (uint8_t i = 0; i < 128; i++)
  {
    if (!allowAddressCommunication(i)) { continue; }

    WIRE.beginTransmission(i);
    uint8_t error = WIRE.endTransmission();

    switch (error)
    {
    case 0:
      Serial.print(F("Found device at address "));
      Serial.println(i);
      break;
    case 2:
      // No device at address i: we received a NACK when
      // sending the address.
      break;
    default:
      Serial.print(F("Unexpected result at address "));
      Serial.print(i);
      Serial.print(F(": "));
      Serial.println(error);
      break;
    }
  }
  Serial.println(F("Done."));
}

void targetAddress()
{
  uint8_t address = strtoul(lineBuffer + 1, NULL, 10) & 127;
  acs.setAddress(address);
  Serial.print(F("Target address is now "));
  Serial.println(address);
}

void dumpRegisters()
{
  char buffer[80];
  uint8_t regAddress = 0;
  while (regAddress <= 0x31)
  {
    char * p = buffer;
    p += sprintf(p, "%02x:", regAddress);
    for (uint8_t j = 0; j < 4; j++)
    {
      uint32_t value = acs.readReg(regAddress++);
      if (acs.getLastError())
      {
        Serial.print(F("Error "));
        Serial.print(acs.getLastError());
        Serial.println('.');
        return;
      }
      p += sprintf(p, " %08lx", (unsigned long)value);
    }
    Serial.println(buffer);
  }
  Serial.println();
}

void processSerialLine()
{
  switch (lineBuffer[0])
  {
  case 'a':
    assignAddress();
    break;

  case 's':
    scanForDevices();
    break;

  case 't':
    targetAddress();
    break;

  case 'd':
    dumpRegisters();
    break;

  default:
    Serial.println(F("Error: Unrecognized command."));
    break;
  }
}

void processSerialByte(uint8_t byteReceived)
{
  if (byteReceived == '\r' || byteReceived == '\n')
  {
    if (lineSize >= sizeof(lineBuffer))
    {
      Serial.println(F("Error: Command too long."));
    }
    else if (lineSize != 0)
    {
      lineBuffer[lineSize] = 0;
      processSerialLine();
    }
    lineSize = 0;
    return;
  }

  if (lineSize < sizeof(lineBuffer) && byteReceived != '"')
  {
    lineBuffer[lineSize++] = byteReceived;
  }
}

void setup()
{
  Serial.begin(9600);
  WIRE.begin();
}

void loop()
{
  if (Serial.available()) { processSerialByte(Serial.read()); }
}
