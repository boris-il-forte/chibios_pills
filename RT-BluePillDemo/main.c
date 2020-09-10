#include "ch.h"
#include "hal.h"

#include "chprintf.h"

#define SERIAL_OUT_BITRATE 19200

static THD_WORKING_AREA(waThread1, 0);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("blinker");

  palSetPadMode(GPIOC, GPIOC_LED, PAL_MODE_OUTPUT_OPENDRAIN); // only 50MHz in GPIOv1
  palClearPad(GPIOC, GPIOC_LED);
  while (true) {
    palTogglePad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(100);
    palTogglePad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(100);
    palTogglePad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(100);
    palTogglePad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(500);
    palTogglePad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(500);
    palTogglePad(GPIOC, GPIOC_LED);
    chThdSleepMilliseconds(500);
  }
}

static SerialConfig sd1Config =
{ SERIAL_OUT_BITRATE, 0, USART_CR2_STOP1_BITS | USART_CR2_LINEN, 0 };

int main(void) {

  halInit();
  chSysInit();
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);
  palSetPadMode(GPIOA, 9u, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOA, 10u, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  sdStart(&SD1, &sd1Config);

  while (true) {
    chprintf((BaseSequentialStream*) &SD1, "Hello World!\r\n");
    chThdSleepSeconds(1);
  }

  return 0;
}
