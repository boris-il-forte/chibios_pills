#include "ch.h"
#include "hal.h"

#include "chprintf.h"

#include <math.h>

#define SERIAL_OUT_BITRATE 19200

static SerialConfig sd1Config = {SERIAL_OUT_BITRATE, 0, USART_CR2_STOP1_BITS
                                     | USART_CR2_LINEN,
                                 0};

static PWMConfig pwmcfg = {10000000, /* 100Mhz PWM clock frequency */
                           1000, /* PWM period 0.01 second */
                           NULL, /* No callback */
                           /* Only channel 4 enabled */
                           {
                             {PWM_OUTPUT_DISABLED, NULL},
                             {PWM_OUTPUT_DISABLED, NULL},
                             {PWM_OUTPUT_DISABLED, NULL},
                             {PWM_OUTPUT_ACTIVE_HIGH, NULL}},
                             0, //CR2
                             0 //DIER
                           };

static THD_WORKING_AREA(waThread1, 0);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("blinker");

  palSetLineMode(LINE_LED, PAL_MODE_OUTPUT_OPENDRAIN); // only 50MHz in GPIOv1
  palClearLine(LINE_LED);
  while (true) {
    palToggleLine(LINE_LED);
    chThdSleepMilliseconds(100);
    palToggleLine(LINE_LED);
    chThdSleepMilliseconds(100);
    palToggleLine(LINE_LED);
    chThdSleepMilliseconds(100);
    palToggleLine(LINE_LED);
    chThdSleepMilliseconds(500);
    palToggleLine(LINE_LED);
    chThdSleepMilliseconds(500);
    palToggleLine(LINE_LED);
    chThdSleepMilliseconds(500);
  }
}

static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {
  (void)arg;
  chRegSetThreadName("led_pwm");

  palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(2));
  pwmStart(&PWMD3, &pwmcfg);

  int count = 0;

  while(true) {
    float v = sin(2 * M_PI * count/1000.0);
    volatile int p = (int)(v*5000)+5000;
    pwmEnableChannel(&PWMD3, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, p));
    count++;
    chThdSleepMilliseconds(10);
  }

}

int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1,
                    NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO + 1, Thread2,
                    NULL);

  palSetLineMode(LINE_USART1_TX, PAL_MODE_ALTERNATE(7));
  palSetLineMode(LINE_USART1_RX, PAL_MODE_ALTERNATE(7));
  sdStart(&SD1, &sd1Config);

  while (true) {
    chprintf((BaseSequentialStream*)&SD1, "Hello World!\r\n");
    chThdSleepSeconds(1);
  }

  return 0;
}
