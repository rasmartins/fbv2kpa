#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "usbcfg.h"

// FBV2KPA headers.
#include <fbv2kpa/bridge.h>

// UART.
static SerialConfig uartConfig =
{
 speed : 31250,
 cr1 : 0,
 cr2 : USART_CR2_STOP1_BITS,
 cr3 : 0
};

uint32_t
clock_get_ms(void)
{
  return ST2MS(chVTGetSystemTimeX());
}

static void
uart_init(void)
{
  dbg("uart: initializing\r\n");

  // KPA.
  palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(7));
  sdStart(&SD1, &uartConfig);

  // FBV.
  palSetPadMode(GPIOC, 6, PAL_MODE_ALTERNATE(8));
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(8));
  sdStart(&SD6, &uartConfig);
}

static void
uart_write(void* object, uint8_t byte)
{
  SerialDriver* sd = (SerialDriver*)object;
  sdPut(sd, byte);
}

bool
readByte(SerialDriver* dev, uint8_t* byte)
{
  int r = chnGetTimeout(dev, TIME_IMMEDIATE);
  if (r < 0 || r > 255)
    return false;

  *byte = r;
  return true;
}

static THD_WORKING_AREA(uart_thread_wa, 4096);
static THD_FUNCTION(uart_thread, arg)
{
  (void)arg;
  chRegSetThreadName("UART thread");

  // KPA.
  event_listener_t kpa_event_listener;
  chEvtRegisterMaskWithFlags((event_source_t*)chnGetEventSource(&SD1), &kpa_event_listener, EVENT_MASK(1), CHN_INPUT_AVAILABLE);
  struct writer kpa_writer;
  writer_init(&kpa_writer, &SD1, uart_write);

  // FBV.
  event_listener_t fbv_event_listener;
  chEvtRegisterMaskWithFlags((event_source_t*)chnGetEventSource(&SD6), &fbv_event_listener, EVENT_MASK(2), CHN_INPUT_AVAILABLE);
  struct writer fbv_writer;
  writer_init(&fbv_writer, &SD6, uart_write);

  // Bridge-
  struct bridge bridge;
  bridge_init(&bridge, &kpa_writer, &fbv_writer);

  uint8_t byte = 0;
  while (true)
  {
    eventflags_t evt = chEvtWaitAnyTimeout(EVENT_MASK(1) | EVENT_MASK(2), MS2ST(1));

    if (evt & EVENT_MASK(1))
    {
      chEvtGetAndClearFlags(&kpa_event_listener);
      while (readByte(&SD1, &byte))
        bridge_update_kpa(&bridge, byte);
    }

    if (evt & EVENT_MASK(2))
    {
      chEvtGetAndClearFlags(&fbv_event_listener);
      while (readByte(&SD6, &byte))
        bridge_update_fbv(&bridge, byte);
    }

    bridge_update_time(&bridge, clock_get_ms());
  }
}

int
main(void)
{
  halInit();
  chSysInit();
  static const evhandler_t evhndl[] = { };

  // CDC.
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  chThdSleepMilliseconds(4000);

  uart_init();
  chThdCreateStatic(uart_thread_wa, sizeof(uart_thread_wa), NORMALPRIO, uart_thread, NULL);

  while (true)
  {
    palTogglePad(GPIOC, GPIOC_LED);
    chEvtDispatch(evhndl, chEvtWaitOneTimeout(ALL_EVENTS, MS2ST(100)));
  }
}
