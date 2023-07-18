/**
 * Author: Mike Longworth
 * https://milelo.github.io/smbee
 * License: http://unlicense.org
 * ATtiny10
 * Use 5v, 220R supply for suitable LED current. (inc 4.8v for low voltage programming; not required)
 * Code requires 12V on RA3 (Reset) to program in RSTDISBL mode (required)
 * 32bytes SRAM
 * 1024bytes Flash
 * Sleep current <200nA
 * Requires RSTDISBL to support button input:
 * http://www.engbedded.com/fusecalc/
 * Programme RSTDISBL (one-time op): avrdude -v -p attiny10 -c usbasp -U fuse:w:0xfe:m
 * Programme code: avrdude -v -p attiny10 -c usbasp -U flash:w:FILENAME.hex:i
 **/
#define F_CPU 62500UL // Define clock for macros: 62.5kHz, 16us. Keep clock slow to reduce power.

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "main.h"
#include "program.h"

// Beware variables take up stack space, keep size to absolute minimum!!
// volatile: can be changed at any time eg in an interrupt;
uint8_t ledModulatorA; // LED flashing characteristics for PWM A
uint8_t ledModulatorB; // LED flashing characteristics for PWM B

#define MODULATOR_RATE 0b00000111

#define FLASH_RATE 0b10000000 //flag

// Make volatile to support modification by ISR
volatile uint8_t status = 0; // Status bits
#define WAIT_TIMEOUT 0       // Wait-timeout 'status' bit position
#define REPEAT_PROGRAM 1     // repeat-program enable 'status' bit position

volatile uint16_t ct0_ticks = 0; // 1:1ps, tick: 4.096ms (~4.5min) @62.5kHz;
uint16_t waitTimerEnd;           // wait-timer tick-count; max 4.5min

// Define flash brightness modulation waveform
// must be put in PROGMEM (flash), don't use RAM!! See also pointers:
// PGM_VOID_P  
// Using a triangle wave to visually approximate  a rectified sine:
const PROGMEM uint8_t WAVEFORM[] = {32,  64,  96,  128, 160, 192, 224, 255, 224, 192, 160, 128, 96,  64,  32, 0};
//fade out:
//const PROGMEM uint8_t WAVEFORM[] = {255, 238, 221, 204, 187, 170, 153, 136, 119, 102, 85, 68, 51, 34, 17, 0};
//fade in:
//const PROGMEM uint8_t WAVEFORM[] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

#define SW_SETTLE_MS 10 //TE SKRBAAE010 settle time

void debounceButton(void) {
  _delay_ms(SW_SETTLE_MS);                     // press settle time
  loop_until_bit_is_set(PINB, PINB3); // wait for button release
  _delay_ms(SW_SETTLE_MS);                     // release settle time
}

void sleep(void) {
  // restart doesn't clear registers so prepare for wake
  // Sleep stops the CPU but peripheral ccts can be active. Disable as much as possible
  // to reduce sleep current. See also main() initialisation.
  PORTB = LEDS_OFF; // Disable LED's for wake
  DDRB = 0;         // Set outputs high-Z while sleeping
  debounceButton(); // Wait for release and debounce the activating button press
  SMCR = 1 << SE | SLEEP_MODE_PWR_DOWN; // Enable sleep in power-down mode (do
                                        // just before sleep)
  PCMSK = 1 << PCINT3; // Enable button pin PB3 change detect for wake-detect
  PCICR = 1 << PCIE0;  // Enable pin change interrupts: handler isn't defined so
                       // interrupt will trigger restart.
  sleep_cpu();         // Sleep well
}

/**
 * Delay until wait-time-end flag is set and pol incase button-press to invoke sleep.
 * unit: ~s/10 eg 15 = 1.5s
 **/
void wait(uint8_t time) // unit: ~s/10 eg 15 = 1.5s
{
  status &= ~(1 << WAIT_TIMEOUT); // clear wait-timeout flag
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    // Multi-byte operation so make atomic to defer possible ISR interuption.
    // Set wait-timer-end = time-now + delay-time. Ticks are incremented in ISR.
    // Overflows are supported.ı
    waitTimerEnd = ct0_ticks + time * 25; // tick = 4.096ms
  }
  // wait for timeout flag
  while (bit_is_clear(status, WAIT_TIMEOUT)) {
    // poll for button-press in the mean-time
    if (bit_is_clear(PINB, PINB3)) {
      sleep();
    }
  }
}

// Interrupt Service Routine for counter-timer0 overflow
ISR(TIM0_OVF_vect) {
  // ISR implicitly clears interrupt-flag and disabled interupts within routine.
  // called every: 256/62.5kHz = 4.096ms; (1:1 prescale)
  ++ct0_ticks; // count TC0 (8bit) overflows to give timer ticks
  // Must check for timeout with '==' on every tick to support overflows
  if (ct0_ticks == waitTimerEnd)
    status |= 1 << WAIT_TIMEOUT; // set wait-timeout flag

  // Modulate brightness: index waveform with 4 bits from ct0_ticks selected
  // according to flash rate.
  // Add waveform-index-offset if FLASH_ANTIPHASE flag is set
  if (ledModulatorA) {
    OCR0AL = WAVEFORM[((ct0_ticks >> ((ledModulatorA & MODULATOR_RATE) - 1)) +
                      (((ledModulatorA | ledModulatorB) & FLASH_ANTIPHASE) == 0 ? 0 : 0x8) & 0xF)];
  }
  if (ledModulatorB) {
    OCR0BL = WAVEFORM[(ct0_ticks >> ((ledModulatorB & MODULATOR_RATE) - 1)) &
                 0xF];
  }
}

// Missing pin-change interrupt handler will trigger restart - just what we want.
// Note, registers aren't cleared.
// ISR(PCINT0_vect) {}

// Set static brightness or flash rate for OCR0A
void setBrightnessA(uint8_t brightness) {
  if ((brightness & FLASH_RATE) == 0) {
    OCR0AL = 0xFF * brightness / 100; // Set brightness as PWM pulse width
    ledModulatorA = 0;
  } else {
    // Set ledModulator to give ISR dynamic control of OCR0A.
    ledModulatorA = brightness; // Set flash rate and phase bit
  }
}

// Set static brightness or flash rate for OCR0B
void setBrightnessB(uint8_t brightness) {
  if ((brightness & FLASH_RATE) == 0) {
    OCR0BL = 0xFF * brightness / 100; // Set brightness as PWM pulse width
    ledModulatorB = 0;
  } else {
    // Set ledModulator to give ISR dynamic control of OCR0B.
    ledModulatorB = brightness;
  }
}

void show2(uint8_t ledState, uint8_t brightness1, uint8_t brightness2) {
  uint8_t COM = 0;  // Initialise Compare Output Mode (A & B)
  if ((ledState & 0b100) == 0) {
    // non-inverting PWM - pull-up
    if ((ledState & 0b001) != 0) {
      COM = 2 << COM0A0; // antenna L (PB0)
      setBrightnessA(brightness2);
    }
    if ((ledState & 0b010) != 0) {
      COM |= 2 << COM0B0; // antenna R (PB1)
      setBrightnessB(brightness1);
    }
  } else {
    // inverting PWM - pull-down
    if ((ledState & 0b001) == 0) {
      COM = 3 << COM0A0; // sting (PB0)
      setBrightnessA(brightness2);
    }
    if ((ledState & 0b010) == 0) {
      COM |= 3 << COM0B0; // eyes (PB1)
      setBrightnessB(brightness1);
    }
  }
  DDRB = 0; // set outputs high Z
  PORTB = ledState;
  // Configure PWM: WGM0 = 0b0101, Fast PWM, 8-bit (0xFF overflow). See TCCR0B
  // setup for WGM[3:2] bits
  TCCR0A = 0b01 << WGM00 | COM;
  DDRB = 0x07; // PB3 i/p; PB2-0 o/p.
}

void show1(uint8_t ledState, uint8_t brightness) {
  show2(ledState, brightness, brightness);
}

void show(uint8_t ledState) { show2(ledState, 100, 100); }

int main(void) {
  // Enters here on power-on and wake. Register state is preserved from wake.
  cli();           // Disables all interrupts
  sleep_disable(); // Disable spurious sleep invocations as recommended
  CCP = 0xD8U;     // Unprotect I/O reg (CLKPSR) (for next 4 instruction cycles)
  CLKPSR = 0b0111; // Precede by CCP unprotect instruction!!
                   // Set clock slow for min power: 8Mhz/128 = 62.5kHz, 16us.
  PCICR = 0;       // Disable pin change interrupts
  PCMSK = 0;       // Disable pin change detects
  DIDR0 = 0x07;    // Disable digital-input on PB2-0 to reduce power consumption
  ACSR = 1 << ACD; // Analog Comparator Disable (save power)
  PUEB = 1 << PUEB3;    // Enable pull-up on PB3 button.
  PRR = ~(1 << PRTIM0); // Power reduction register: Enable only counter-timer
  TCCR0B = 0b001 << CS00 | // Clock-select: internal-clk 1:1
           0b01 << WGM02;  // 8bit fast PWM mode. Data sheet 12.9.3.
  TIMSK0 = 1 << TOIE0;     // Enable TC0 overflow Interrupt TOV0
  status = 0;              // clear timerout and repeat flags
  // Set timer for long button press detection: >3s press to enable repeat-program
  waitTimerEnd = ct0_ticks + 30 * 25; // tick = 4.096ms. SET BEFORE sei() to ensure atomic!
  sei();                              // Enable interrupts to start timer
  _delay_ms(SW_SETTLE_MS);            // press debounce
  // Wait for button release or timer-end
  while (bit_is_clear(status, WAIT_TIMEOUT) && bit_is_clear(PINB, PINB3)) {}
  if (bit_is_set(status, WAIT_TIMEOUT)) {
    status |= (1 << REPEAT_PROGRAM); // set repeat flag
    show(STING);                     // indicate repeat enabled
  }
  // wait for button release and debounce. 
  //Call here introduces redundent delay but isn't noticeable and optimizes code size.
  debounceButton();

  while (true) {
    program();
    //if REPEAT_PROGRAM is not set than sleep
    if (bit_is_clear(status, REPEAT_PROGRAM)) {
      sleep();
    }
  }
}