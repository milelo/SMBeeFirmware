
/**
 * Author: Mike Longworth
 * https://milelo.github.io/smbee
 * License: http://unlicense.org
**/

#include "main.h"

void program(void) {
  show(EYES); //Eyes on, full brightness
  wait(10); // 1s
  show1(EYES, FLASH_VSLOW);
  wait(30);
  show(ANTENNA_RL); //Both antennae on, 100% intensity.
  wait(5); // 0.5s
  show1(ANTENNA_RL, FLASH_ANTIPHASE | FLASH_SLOW);// Flash antennae in anti-phase to each other.
  wait(20);
  show(EYES);
  wait(10);
  show1(EYES, FLASH_VSLOW);
  wait(30);
  show(ANTENNA_RL);
  wait(5);
  show1(ANTENNA_RL, FLASH_SLOW);
  wait(30);
  show1(EYES, FLASH_MED);
  wait(10);
  show(ANTENNA_RL);
  wait(5);
  show1(ANTENNA_RL, FLASH_SLOW);
  wait(10);
  show2(ANTENNA_RL, FLASH_SLOW, FLASH_MED);// Flash ANTENNA_R slow & ANTENNA_L medium.
  wait(20);
  show2(ANTENNA_RL, FLASH_MED, FLASH_SLOW);
  wait(20);
  show(ANTENNA_RL);
  wait(5);
  show1(EYES, FLASH_SLOW);
  wait(15);
  show1(EYES, FLASH_MED);
  wait(15);
  show1(EYES, FLASH_FAST);
  wait(10);
  show2(EYES_STING, FLASH_SLOW, FLASH_FAST);// Eyes slow, sting fast
  wait(20);
  show1(EYES, FLASH_MED);
  wait(5);
  show2(EYES_STING, FLASH_SLOW, FLASH_FAST);
  wait(20);
  show1(EYES, FLASH_SLOW);
  wait(30);
}
