/**
 * Author: Mike Longworth
 * https://milelo.github.io/smbee
 * License: http://unlicense.org
**/

// PORTB values for output activation

#include <avr/io.h>

//LED state (ledState) values
#define LEDS_OFF 0
#define ANTENNA_R 2
#define ANTENNA_L 1
#define ANTENNA_RL 3 //Individually controllable
#define EYES 5       //Not individually controllable
#define STING 6
#define EYES_STING 4 //Individually controllable

//Flash rate values for dynamic brightness (brightness parameters)
#define FLASH_FAST 0b10000001
#define FLASH_MED 0b10000010
#define FLASH_SLOW 0b10000011
#define FLASH_VSLOW 0b10000100
#define FLASH_VVSLOW 0b10000111
#define FLASH_ANTIPHASE 0b01000000 // OR with flash-rate

//wait for time specified in deci-seconds unit: ~s/10 eg 15ds = 1.5s
void wait(uint8_t time);

//Show the specified ledState with 100% static brightness
void show(uint8_t ledState);

//Show (with 1 extra parameter) the specified ledState with specified 
//static-brightness 0-100% or a flashRate value.
//OR in optional FLASH_ANTIPHASE flags eg "FLASH_VSLOW | FLASH_ANTIPHASE"
void show1(uint8_t ledState, uint8_t brightness);

//Show (with 2 extra parameters). Provides individual LED control for duel-LED states.
//Show the specified ledState with specified individual static-brightness 0-100% or flashRate value.
//OR in optional FLASH_ANTIPHASE flags eg "FLASH_VSLOW | FLASH_ANTIPHASE"
void show2(uint8_t ledState, uint8_t brightness1, uint8_t brightness2);