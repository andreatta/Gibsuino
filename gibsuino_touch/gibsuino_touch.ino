/*******************************************************************\
 * Gibsuino Touchpad
 *
 * Example for the capacitive touchpad directly printed on the PCB.
 * This Example is written by Cyril Andreatta based on the work of
 * Aaron Weiss, and the MPR121 Keypad Example by Jim Lindblom.
 *
 * license: OSHW 1.0, http://freedomdefined.org/OSHW
 * 
 * Notes: The Wiring library is not used for I2C, a default Atmel I2C
 * lib is used. Be sure to keep the *.h files with the project.
 *******************************************************************/

// include the atmel I2C libs
#include "mpr121.h"
#include "i2c.h"

#define DIGITS 			11
// interrupt for capacitive touchpad controller
// Pin 18, IRQ 5 on Gibsuino
#define TOUCH_IRQ 		5
#define TOUCH_PIN 		18

char touchButtons[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9','*','0','#'};
uint16_t touchstatus = 0;

int rgb[] = {10,9,8};
enum colors {RED, GREEN, BLUE};

void setup()
{
	// make sure the interrupt pin is an input and pulled-up
	pinMode(TOUCH_PIN, INPUT_PULLUP);

	// set LEDs as Outputs
	for (int i = 62; i < 70; i++) {
		pinMode(i, OUTPUT);
	}

	for (int i = 0; i < 3; i++) {
		pinMode(rgb[i], OUTPUT);
		digitalWrite(rgb[i], HIGH);
	} 

	// output on PD1 (20) = SDA
	DDRD |= 0b00000010;
	// set pull-ups on I2C bus
	PORTD |= 0b00000011; 

	// initalize I2C bus
	i2cInit();

	// initialize mpr121 capacitive touchpad controller
	delay(100);
	mpr121QuickConfig();

	// Create an interrupt to trigger when a button is touched.
	// IRQ pin (5) goes low, function touch_irq is executed
	attachInterrupt(TOUCH_IRQ, touch_irq, LOW);

	// configure serial out
	Serial.begin(115200);

	//digitalWrite(rgb[GREEN], LOW);
	//digitalWrite(rgb[BLUE],LOW);
	//digitalWrite(rgb[RED], LOW);
}

void loop()
{
	static int led = 62;  
	digitalWrite(led, LOW);

	if (led++ >= 70) 
		led = 62;

	digitalWrite(led, HIGH);
	delay(100);

	if (touchstatus) {
		Serial.println(touchButtons[getPressedNumber(touchstatus)]);
		touchstatus = 0;
	}
}

int getPressedNumber(uint16_t touch)
{
	for (int i = 0; i <= DIGITS; i++) {
		if ((touch >> i) & 0x01)
			return i;
	}

	return 0;
}

void touch_irq()
{
	touchstatus = mpr121Read(0x01) << 8;
	touchstatus |= mpr121Read(0x00);
}
