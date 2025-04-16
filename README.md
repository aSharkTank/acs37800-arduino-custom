# ACS37800 power monitoring library for Arduino

[www.pololu.com](https://www.pololu.com/)

## Summary

This is a library for the Arduino IDE that helps interface with
the ACS37800 power monitoring IC from Allegro Microsystems using I&sup2;C.

This library supports the
[ACS37800 Isolated Power Monitor Carriers][cat] from Pololu.

## Supported platforms

This library should support any Arduino-compatible board, including the
[Pololu A-Star controllers][a-star].

## Getting started

### Hardware

An [ACS37800 carrier][cat] can be purchased from Pololu's website.

### I&sup2;C connections

The GND pin of your Arduino must be connected to the GND pin of the
ACS37800 board.

You must connect logic power to the ACS37800 board.
If you have a Pololu board, connect its VCC pin to the logic power supply
of your Arduino, which should be between 3&nbsp;V and 5.5&nbsp;V.
If your Arduino has an IOREF pin, we recommend using that.
Otherwise, use either the 3V3 or 5V pin (whichever matches the logic voltage
that your Arduino's I/O pins use).

The SCL pin of your Arduino must be connected to the SCL pin of the ACS37800 board and
the SDA pin of your Arduino must be connected to the SDA pin of the ACS37800 board.

### Software

You can use the Library Manager to install this library:

1. In the Arduino IDE, open the "Sketch" menu, select "Include Library", then
   "Manage Libraries...".
2. Search for "ACS37800"
4. Click "Install" on the library published by Pololu.

If this does not work, you can manually install the library:

1. Download the [latest release archive from GitHub][releases]
   and decompress it.
2. Rename the folder "acs37800-arduino-xxxx" to "ACS37800".
3. Drag the "ACS37800" folder into the "libraries" directory inside your
   Arduino sketchbook directory. You can view your sketchbook location by
   opening the "File" menu and selecting "Preferences" in the Arduino IDE. If
   there is not already a "libraries" folder in that location, you should make
   the folder yourself.
4. After installing the library, restart the Arduino IDE.

## Examples

Several example sketches are available that show how to use the library. You can
access them from the Arduino IDE by opening the "File" menu, selecting
"Examples", and then selecting "ACS37800". If you cannot find these
examples, the library was probably installed incorrectly and you should retry
the installation instructions above.

## Documentation

For complete documentation of this library, see
[the motoron-arduino documentation][doc].
If you are already on that page, then see the documentation for the ACS37800 class.

## Version history

* 1.0.0 (2025-04-18): Original release.

[cat]: https://www.pololu.com/category/343
[releases]: https://github.com/pololu/acs37800-arduino/releases
[doc]: https://pololu.github.io/acs37800-arduino/
