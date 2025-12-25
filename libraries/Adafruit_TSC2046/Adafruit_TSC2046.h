/*!
 * @file Adafruit_TSC2046.h
 *
 * @mainpage TI TSC2046 Resistive Touchscreen Library.
 *
 * @section intro_sec Introduction
 *
 * This is a library for the TI TSC2046 resistive touchscreen.
 *
 * @see Adafruit_TSC2046
 * @see Adafruit_TSC2046::begin
 * @see Adafruit_TSC2046::getPoint
 * @see TSPoint
 *
 * @section author Author
 *
 * Written by Qyriad <qyriad@qyriad.me>, 2023.
 *
 * @section license License
 *
 * MIT license. All text above must be included in any redistribution.
 *
 * @section usage Usage
 *
 * @subsection connecting Connecting the breakout
 *
 * Adafruit's TSC2046 breakout board will have 11 pins. At least 6 of those
 * pins must be connected to use the touchscreen: Vin, GND, SCK, MISO, MOSI,
 * and CS. Vin should be connected to `+5V` for 5V boards like the Arduino Uno,
 * and `+3V3` for 3.3V boards like the MKR Zero. You can find the voltage of
 * your board in the "Tech Specs" section of the documentation for that board
 * under "I/O Voltage".
 *
 * The other 5 pins should be connected as follows:
 *
 * | TSC2046 Breakout Pin | Arduino Pin |
 * | -------------------- | ----------- |
 * | GND                  | GND         |
 * | SCK                  | SCK         |
 * | MISO                 | CIPO        |
 * | MOSI                 | COPI        |
 * | CS                   | SS          |
 *
 * Check the pinout for your board to see which pin numbers these correspond
 * to. As an example, on the Arduino Uno, SCK, MISO, MOSI, and CS would be
 * connected to D13, D12, D11, and D10 respectively.
 *
 * @subsubsection irq_pin The IRQ Pin
 * The IRQ pin can be used to trigger an interrupt service routine whenever
 * the touchscreen detects a touch. To do this, connect the IRQ pin to a
 * digital pin on your Arduino board (see
 * [Adafruit's documentation][attachInterrupt] on interrupts for what pins are
 * usable for interrupts for each board), and "attach" the interrupt to an
 * interrupt service routine. Again see Adafruit's documentation on
 * [attachInterrupt] for more information, and see
 * Adafruit_TSC2046::enableInterrupts for information on the TSC2046's
 * interrupts specifically.
 *
 * @subsubsection vbat_pin The Vbat Pin
 * The Vbat pin can be used to measure some other voltage external to the
 * touchscreen, though as the name suggests it is intended to measure a
 * battery. To do this, connect the positive terminal of your battery to the
 * Vbat pin, and the negative terminal to a common ground (GND), and call
 * Adafruit_TSC2046::readBatteryVoltage to get its voltage. You can measure
 * voltages (inclusively) between 0V and 6V this way.
 *
 * @subsubsection aux_pin The AUX Pin
 * The AUX pin, like the Vbat pin, can be used to measure some other voltage
 * external to the touchscreen. Unlike Vbat, however, the maximum voltage
 * this pin can measure is determined by the reference voltage, which by
 * default is 2.5V. To measure higher voltages with this pin, take a look
 * at the next paragraph which talks about VRef.
 *
 * @subsubsection vref_pin The VRef pin
 * The VRef pin can be used to override the "reference voltage" that the
 * TSC2046 uses to measure other voltages. The TSC2046 has an internal
 * reference voltage of 2.5V that by default we use when measuring temperature,
 * Vbat, and AUX voltages. Connecting a 5V supply to the VRef pin instead will
 * allow us to use that as the reference voltage, which will increase the
 * *accuracy* of voltage reads for temperature, Vbat, and AUX; and will also
 * increase the *range* of voltage reads for AUX.
 *
 * However, and this is important, if you connect something to the VRef pin,
 * you must connect VRef to **the same thing you connected Vin to.**
 *
 * If you *do* override the reference voltage, pass the voltage you're giving
 * it as Adafruit_TSC2046::begin's @p vRef parameter.
 *
 * [attachInterrupt]:
 * https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 *
 * @subsection code Using the library
 *
 * Once you have your board connected up, interacting with it will be done
 * with the Adafruit_TSC2046 class. First create an instance of this class
 * (likely as a global variable), then call Adafruit_TSC2046::begin (usually
 * in `setup()`. With that, you can call Adafruit_TSC2046::isTouched to
 * check if the touchscreen is being touched, and then call
 * Adafruit_TSC2046::getPoint to get the coordinates of the current touch.
 * Adafruit_TSC2046::getPoint returns a @ref TSPoint object, which contains
 * the direct X and Y coordinates as `point.x` and `point.y`. You may also
 * get the X and Y coordinates as percentages with `point.xPercent()` and
 * `point.yPercent()`. The pressure is in `point.z`, and its value *decreases*
 * as physical pressure *increases*.
 *
 * @note Adafruit_TSC2046::begin has one particularly notable required
 * parameter, @p xResistance — the value you pass here is something you must
 * measure with a multimeter. Set your multimeter to measure resistance,
 * place one probe on the pin-hole labeled "X-" on your TSC2046 breakout board,
 * and place the other probe on the pin-hole labled "X+". Your multimeter
 * should show you a number in ohms (Ω), the unit for resistance. Pass that
 * number as the @p xResistance argument. If your multimeter gives you a value
 * in kilohms (kΩ), divide that number by 1000 to get ohms and pass that
 * value for the @p xResistance argument. If you do not have a multimeter or
 * otherwise don't have a measurement, `400` (400Ω) is a reasonable value to
 * use here, though note that the pressure measurements returned by
 * Adafruit_TSC2046::getPoint may not be accurate in that case.
 *
 * Below is a simple example; a more complete example can be found in the
 * `examples/` directory.
 *
 * @code{.cpp}
 *
 * Adafruit_TSC2046 touchscreen;
 *
 * void setup() {
 *   touchscreen.begin(400); // 400 ohms measured between X- and X+.
 * }
 *
 * void loop() {
 *   if (touchscreen.isTouched()) {
 *     TSPoint point = touchscreen.getPoint();
 *     Serial.print("X, Y, Z: ");
 *     Serial.print(point.xPercent());
 *     Serial.print(", ");
 *     Serial.print(point.yPercent());
 *     Serial.print(", ");
 *     Serial.println(point.z);
 *   }
 * }
 *
 * @endcode
 *
 * @see Adafruit_TSC2046
 * @see Adafruit_TSC2046::begin
 * @see Adafruit_TSC2046::getPoint
 * @see TSPoint
 */

#ifndef ADA_TSC2046_H
#define ADA_TSC2046_H

#include "Arduino.h"
#include "Print.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_SPIDevice.h>
#include <stdint.h>

/*!
 * @brief The type returned by Adafruit_TSC2046::getPoint.
 *
 * See the individual fields and methods for more information.
 */
class TSPoint {
public:
  /*! @brief Create a new TSPoint with these exact values.
   * You usually don't need to call this constructor in user code.
   * Call Adafruit_TSC2046::getPoint instead.
   *
   * @param x The value to initialize the X coordinate to.
   * @param y The value to initalize the Y coordinate to.
   * @param z The value to initalize the Z coordinate/pressure to.
   */
  TSPoint(int16_t x, int16_t y, float z);

  /*! @brief The full scale raw X coordinate from the touchscreen.
   * For the X-coordinate as a percentage, see TSPoint::xPercent.
   * If the touchscreen is not being touched, this value is meaningless.
   */
  int16_t x;

  /*! @brief The full scale raw Y coordinate from the touchscreen.
   * For the Y-coordinate as a percentage, see TSPoint::yPercent.
   * If the touchscreen is not being touched, this value is meaningless.
   */
  int16_t y;

  /*! @brief The resistance measurement that corresponds to the pressure
   * currently exerted on the touchscreen. The *higher* the pressure, the
   * *lower* this resistance value will be. Unlike the X and Y coordinates,
   * this value is not in an arbitrary unit of a full scale, but is a physical
   * measurement, in Ohms (Ω).
   */
  float z;

  /*! @brief The X-coordinate as a percentage. Note that physical touchscreens
   * vary, and the range of yours may not perfectly extend from 0% to 100%.
   * Like TSPoint::x, this value is meaningless if the touchscreen is not
   * being touched.
   *
   * @returns The X-coordinate as a percentage of the maximum coordinate.
   */
  float xPercent();

  /*! @brief The Y-coordinate as a percentage. Note that physical touchscreens
   * vary, and the range of yours may not perfectly extend from 0% to 100%.
   * Like TSPoint::y, this value is meaningless if the touchscreen is not
   * being touched.
   *
   * @returns the Y-coordinate as a percentage of the maximum coordinate.
   */
  float yPercent();
};

/*! @brief Class for interfacing with a TSC2046 touchscreen controller.
 *
 * Notable methods: Adafruit_TSC2046::begin, Adafruit_TSC2046::isTouched,
 * and Adafruit_TSC2046::getPoint.
 *
 * @note @parblock The way power works with the TSC2046 is a bit unintuitive.
 * When interrupts are *enabled* (which is the default, and only changed with
 * Adafruit_TSC2046::enableInterrupts), the TSC2046 is put into an
 * "auto power-down" mode, in which the TSC2046 automatically powers down
 * at the end of a read, and automatically powers back up at the beginning of
 * a read. According to the datasheet there's no delay for that and the
 * power-up is instant. Because of that, this library leaves the TSC2046 in
 * that mode by default.
 *
 * In that mode, however, TSC2046 interrupts are enabled, meaning the IRQ
 * pin goes LOW when the touchscreen is touched. *If* you have the IRQ pin
 * connected to a pin on your Arduino that has interrupts enabled *and* you
 * have enabled interrupts on that pin, *and* you *don't* want interrupts
 * from this chip, you should call @link Adafruit_TSC2046::enableInterrupts
 * `enableInterrupts(false)`@endlink.
 * This will prevent the TSC2046 from powering down between reads, which uses
 * more power, but will prevent the IRQ pin from going LOW if the touchscreen
 * is touched and potentially causing an unwanted hardware interrupt.
 * @endparblock
 */
class Adafruit_TSC2046 {
public:
  ~Adafruit_TSC2046();

  /*!
   * @brief Initialize this TSC2046 using SPI. You must call this method before
   * calling Adafruit_TSC2046::getPoint.
   *
   * @param xResistance The resistance in Ohms between X- and X+ on the TSC2046
   * breakout. With a multimeter set to measure resistance, place one probe on
   * the pin-hole labled "X-" on your TSC2046 breakout board, and place the
   * other probe on the pin-hole labled "X+". Your multimeter should show you a
   * number in ohms (Ω), the unit for resistance. Pass that number as this
   * parameter. If your multimeter gives you a value in kilohms (kΩ), divide
   * that number by 1000 to get Ohms and pass that value. \n
   * If you do not have a multimeter or otherwise don't have a measurement,
   * `400` (400Ω) is a reasonable value to use here.
   *
   * @param spiChipSelect The pin number on your board that you have connected
   * to the SPI CS (Chip Select) pin on the TSC2046. Defaults to the `SS` pin
   * if not provided, which is also the default chip select pin of the
   * [default SPI interface][SPI].
   *
   * @param the_spi The SPI interface to use when communicating to this
   * touchscreen. Defaults to [SPI], the default SPI interface on Arduino
   * boards. This is often connected to pins labeled `SCK`, `MOSI`, and `MISO`
   * on the physical board. For example, on Arduino Uno the MISO of the default
   * `SPI` interface is pin 12.
   *
   * @param spiFrequency The clock frequency for the SPI peripheral. Defaults
   * to 2 MHz if not specified. Must not be higher than 2 MHz, per the TSC2046
   * datasheet.
   *
   * [SPI]: https://docs.arduino.cc/learn/communications/spi
   */
  void begin(int spiChipSelect, SPIClass *the_spi = &SPI,
             uint32_t xResistance = 400,
             uint32_t spiFrequency = 2L * 1000L * 1000L);

  /*!
   * @brief Indicates the voltage connected to the TSC2046's "VRef" pin, if any.
   * Use `-1` if nothing is connected to the VRef pin. See the documentation
   * for Adafruit_TSC2046::begin's @p vRef parameter for more information.
   *
   * @param vRef The voltage in volts of the supply connected to the TSC2046's
   * VRef pin, if any. `-1` (the default if you don't call this function)
   * indicates that nothing is connected to the TSC2046's VRef pin. Connecting
   * VRef to a voltage higher than 2.5V increases the accuracy of
   * **non**-touchscreen reads (temperature, battery voltage, and auxiliary
   * voltage), and also directly determines the maximum voltage value that can
   * be measured by Adafruit_TSC2046::readAuxiliaryVoltage. It has no effect
   * on touchscreen coordinate reads. \n
   * The TSC2046's VRef pin should either be connected to the same supply as
   * the TSC2046's Vin pin, or not connected at all (Vin should be connected
   * to a 5V or 3.3V supply from the Arduino). If you do not connect the VRef
   * pin, either don't call this function at all, or pass `-1` to this argument.
   */
  void setVRef(float vRef);

  /*!
   * @brief Sets the threshold for Adafruit_TSC2046::isTouched.
   *
   * @param rTouchThreshold
   * @parblock The resistance value (technically in Ohms) to use
   * as the threshold for Adafruit_TSC2046::isTouched. Any pressure readings
   * that are higher than the value provided here are considered "not touching"
   * (remember that the pressure readings get LOWER as the physical pressure
   * increases, see TSPoint::z). Also note that regardless of the threshold
   * value, resistances of 0 and nonfinite numbers (like infinity) are always
   * considered not touching.
   *
   * If not set, the default value is `100000` (100kΩ).
   *
   * @endparblock
   */
  void setTouchedThreshold(float rTouchThreshold);

  /*!
   * @brief Gets the coordinates of the the current touch on the touchscreen.
   * Use Adafruit_TSC2046::isTouched to determine if the touchscreen is being
   * touched in the first place.
   *
   * @returns The X, Y, and Z (pressure) coordinates as a @ref TSPoint object.
   *
   * @see ::TSPoint.
   */
  TSPoint getPoint();

  /*! @brief Determines if the touchscreen is currently being touched.
   * The X and Y coordinates returned by Adafruit_TSC2046::getPoint are
   * meaningless if this is false.
   *
   * You can also change the threshold used to determine if the touchscreen
   * is being touched with Adafruit_TSC2046::setTouchedThreshold.
   *
   * @returns True if the touchscreen is being touched, false if it is not.
   */
  bool isTouched();

  /*! @brief Enables or disables interrupts that fire when the touchscreen
   * is touched. When an interrupt fires, the `IRQ` pin on the TSC2046
   * is pulled LOW. That pin can be connected to an interrupt-enabled Arduino
   * pin to run code when the TSC2046 detects a touch. See [here] for
   * information on using Arduino interrupts.
   *
   * @note On the TSC2046 side, interrupts are **enabled** by default, because
   * disabling interrupts on this chip requires more power. If you do not want
   * interrupts from the TSC2046 to run, it's recommended to instead either
   * simply not connect the IRQ pin, or [mask] the interrupt on the Arduino
   * processor. See the note on power in the
   * [class documentation](@ref Adafruit_TSC2046) for more information.
   *
   * @param enable True to enable interrupts, false to disable them.
   *
   * [here]:
   * https://reference.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
   * [mask]:
   * https://www.arduino.cc/reference/en/language/functions/external-interrupts/detachinterrupt/
   */
  void enableInterrupts(bool enable);

  /*! @brief Reads the temperature measurement in degrees Celsius.
   *
   * @returns The temperature in degrees Celsius (°C).
   *
   * @see Adafruit_TSC2046::readTemperatureF.
   */
  float readTemperatureC();

  /*! @brief Reads the temperature measurement in degreese Fahrenheit.
   *
   * @returns The temperature in degrees Fahrenheit (°F).
   *
   * @see Adafruit_TSC2046::readTemperatureC.
   */
  float readTemperatureF();

  /*! @brief Reads the voltage on the "VBat" pin, in volts.
   *
   * The TSC2046 allows you to connect the positive voltage terminal of
   * a battery to the "VBat" pin, and then monitor its voltage. The battery
   * voltage can be (inclusively) between 0V and 6V, regardless of the voltage
   * supply provided to Vin/Vcc.
   *
   * @returns The voltage of the connected battery, in volts.
   */
  float readBatteryVoltage();

  /*! @brief Reads the voltage on the "AUX" pin, in volts.
   *
   * The TSC2046 allows you to measure the voltage of whatever you connect to
   * the AUX pin, however the voltage cannot be higher than the voltage
   * reference. See the documentation for the @p vRef parameter of
   * Adafruit_TSC2046::begin for more information, but in summary if you don't
   * connect anything to the "VRef" pin on your TSC2046, the maximum auxiliary
   * voltage you can read is 2.5V. If you want to be able to read higher
   * voltages, connect the same pin you connected to "Vin" on the TSC2046 to
   * the VRef pin on the TSC2046, and then pass the voltage of those pins
   * to the `vRef` paramter of Adafruit_TSC2046::begin, or to
   * Adafruit_TSC2046::setVRef.
   *
   * Alternatively, if you want to measure voltages higher than the reference
   * voltage, see Adafruit_TSC2046::readBatteryVoltage, which can read
   * up to 6V.
   *
   * @returns The voltage on the AUX pin, in volts.
   */
  float readAuxiliaryVoltage();

  /*! @brief Gets the effective reference voltage, which is 2.5V if no external
   * reference voltage value was provided in Adafruit_TSC2046::begin or
   * Adafruit_TSC2046::setVRef, or the value of the @p vRef argument of those
   * functions otherwise.
   *
   * You probably don't need to call this function unless you're doing math
   * directly on reads from this class.
   *
   * @returns The effective reference voltage in volts.
   *
   * @see Adafruit_TSC2046::setVRef.
   */
  float effectiveVRef();

private:
  SPIClass *_spi;
  Adafruit_SPIDevice *_spiDev;
  int _spiCS;
  int64_t _spiFrequency;
  uint32_t _xResistance;

  // NOTE(Qyriad): In my testing, the absolute most delicate touch where the X
  // and Y coordinate values were not completely nonsensical had R_TOUCH at
  // about 5.7kΩ (and even then the X and Y coordinates were still fairly
  // off), and every complete false positive was above 100kΩ. To be on the
  // safe side we'll use 100kΩ as the default threshold.
  float _touchedThreshold = 100000.f;

  bool _interruptsEnabled = true;
  float _vRef;

  float readTemperatureK();

  // Performs a 12-bit differential reference mode read,
  // used for coordinate reads.
  uint16_t readCoord(uint8_t channelSelect);

  // Performs a 12-bit single-ended reference mode read,
  // used for non-coordinate reads.
  uint16_t readExtra(uint8_t channelSelect);

  static uint16_t parse12BitValue(uint8_t spiUpperByte, uint8_t spiLowerByte);
};

// NOTE: Bitfield structs like these are not generally portable as the union'd
// word can be different on machines of different endianness. Normally we would
// detect a big endian platform with #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__,
// and #error in that case. However, here, our struct is only 1 byte wide, so
// byte order shouldn't come into play.
/*! @private */
union Command {
  // NOTE: The order these are defined in is the opposite as they appear in the
  // datasheet, because bitfield structs are least-significant-field first.
  struct {
    /*!
     * PD0: This bit is technically for enabling (HIGH) or disabling (LOW) the
     * ADC, but when PD1 and PD0 are both 0, then it leaves the ADC off
     * *between* conversions, but powers it on *during* conversions. According
     * to the datasheet the ADC is able to power up instantly and there are no
     * delays incured by leaving the ADC powered off between conversions.
     * Leaving the ADC on is intended for certain strategies that use external
     * capacitors to filter out touchscreen noise. This doesn't apply to us, but
     * there is one more consideration, which is that the PENIRQ' output used to
     * trigger interrupts is disabled if this bit is HIGH (1).
     */
    uint8_t enableOrIdleADC : 1;

    /*! PD1: Enable (HIGH) or disable (LOW) the internal VREF. */
    uint8_t enableInternalVRef : 1;

    /*!
     * SER/DFR': use the internal or external VREF (HIGH), or use the voltage
     * across the touchscreen drivers as the ADC reference voltage (LOW).
     * The latter is more accurate, but is only available for touchscreen
     * coordinate reads, and not available for temperature, VBAT, or the other
     * extras.
     */
    uint8_t singleEndedRef : 1;

    /*! ADC conversion mode: LOW for 12-bit mode, and HIGH for 8-bit mode. */
    uint8_t use8BitConv : 1;

    /*!
     * A2:A0: the channel select/"address" bits, which control the multiplexer
     * output. This will be one of the `ADDR` values near the top of the file.
     */
    uint8_t addr : 3;

    /*! START bit, always 1. */
    uint8_t start : 1;
  };
  uint8_t word;
};

#endif
