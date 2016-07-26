#ifndef BRIDGE_H_INCLUDED_
#define BRIDGE_H_INCLUDED_

#include "fbv/fbv.h"
#include "kpa/kpa.h"
#include "writer.h"
#include "clock.h"

struct bridge
{
  struct kpa kpa;
  struct fbv fbv;
  bool pedal_state;
  uint8_t button_active;
  bool last_tap_value;
  uint32_t last_tap_time;
};

typedef struct bridge* bridge_t;

static const fbv_led_t c_slot_leds[] =
{
  FBV_LED_A,
  FBV_LED_B,
  FBV_LED_C,
  FBV_LED_D,
  FBV_LED_WAH
};

static const unsigned c_slot_leds_size = sizeof(c_slot_leds) / sizeof(fbv_led_t);

static const fbv_button_t c_slot_to_button[] =
{
  FBV_BTN_A,
  FBV_BTN_B,
  FBV_BTN_C,
  FBV_BTN_D,
  FBV_BTN_PEDAL
};

static const unsigned c_slot_to_button_size = sizeof(c_slot_to_button) / sizeof(fbv_button_t);

//! Amount of time in milliseconds holding a lit button to enter tuner mode.
static const uint32_t c_tuner_mode_hold_time = 2000;

static inline void
bridge_handle_kpa_connect(kpa_t kpa)
{
  bridge_t bridge = (bridge_t)kpa_get_pdata(kpa);
  dbg("bridge: kpa: connected\r\n");
}

static inline void
bridge_handle_kpa_mode(kpa_t kpa)
{
  bridge_t bridge = (bridge_t)kpa_get_pdata(kpa);
  kpa_mode_t mode = kpa_get_mode(kpa);

  dbg("bridge: kpa: mode changed: %d\r\n", kpa_get_mode(kpa));
  if (mode == KPA_MODE_PERFORM)
  {
    fbv_reset_ui(&bridge->fbv);
  }

}

static inline void
bridge_handle_kpa_slot(kpa_t kpa)
{
  bridge_t bridge = (bridge_t)kpa_get_pdata(kpa);
  unsigned slot_number = kpa_get_slot_number(kpa);
  uint8_t slot_led = c_slot_leds[slot_number];

  dbg("bridge: kpa: slot changed: %d\r\n", kpa_get_slot_number(kpa));

  for (unsigned i = 0; i < c_slot_leds_size; ++i)
    fbv_set_led(&bridge->fbv, c_slot_leds[i], 0);

  fbv_set_led(&bridge->fbv, slot_led, 1);
  fbv_set_display_char(&bridge->fbv, '1' + slot_number);
}

static inline void
bridge_handle_kpa_note(kpa_t kpa)
{
  bridge_t bridge = (bridge_t)kpa_get_pdata(kpa);
  const kpa_note_t note = kpa_get_note(kpa);

  char name = ' ';
  bool flat = false;
  int level = c_fbv_needle_off;

  if (kpa_note_is_valid(note))
  {
    name = kpa_note_get_name(note);
    flat = kpa_note_get_accidental(note) == KPA_NA_SHARP;

    /* // FIXME: overflow on 8-bit MCUs. */
    int cents = (kpa_note_get_tune(note) * 100) / 16383;

    if (cents < -40)
      level = -3;
    else if (cents < -25)
      level = -2;
    else if (cents < -5)
      level = -1;
    else if (cents >= -5 && cents <= 5)
      level = 0;
    else if (cents > 5)
      level = 1;
    else if (cents > 25)
      level = 2;
    else if (cents > 40)
      level = 3;
  }

  fbv_set_display_char(&bridge->fbv, name);
  fbv_set_flat(&bridge->fbv, flat);
  fbv_set_tuner_needle(&bridge->fbv, level);
}

static inline void
bridge_handle_fbv_connect(fbv_t fbv)
{
  bridge_t bridge = (bridge_t)fbv_get_pdata(fbv);
  dbg("bridge: fbv: connected\r\n");
}

static inline void
bridge_handle_fbv_disconnect(fbv_t fbv)
{
  bridge_t bridge = (bridge_t)fbv_get_pdata(fbv);
  dbg("bridge: fbv: disconnected\r\n");
}

static inline void
bridge_handle_fbv_tap(fbv_t fbv)
{
  dbg("bridge: fbv: tap %d\r\n", fbv_get_tap_value(fbv));

  bridge_t bridge = (bridge_t)fbv_get_pdata(fbv);
  bool value = fbv_get_tap_value(fbv);

  bridge->last_tap_value = value;
  bridge->last_tap_time = clock_get_ms();
}

static inline void
bridge_handle_fbv_button(fbv_t fbv)
{
  bridge_t bridge = (bridge_t)fbv_get_pdata(fbv);
  uint8_t address = fbv_get_button_address(fbv);
  bool value = fbv_get_button_value(fbv);

  dbg("bridge: fbv: button: %02X - %d\r\n", address, value);

  /* if (address == FBV_BTN_PEDAL) */
  /* { */
  /*   if (value) */
  /*   { */
  /*     bridge->pedal_state = !bridge->pedal_state; */
  /*     if (bridge->button_active == FBV_BTN_D) */
  /*       fbv_set_led(fbv, FBV_LED_VOL, bridge->pedal_state); */
  /*     else */
  /*       fbv_set_led(fbv, FBV_LED_WAH, bridge->pedal_state); */
  /*   } */
  /* } */
  /* else */
  /* { */
  /*   fbv_set_led_all(fbv, false); */

  /*   uint8_t led_addr = 0x00; */
  /*   for (unsigned i = 0; i < c_slot_leds_size; ++i) */
  /*   { */
  /*     if (c_slot_to_button[i] == address) */
  /*     { */
  /*       led_addr = c_slot_leds[i]; */
  /*       break; */
  /*     } */
  /*   } */

  /*   fbv_set_led(&bridge->fbv, led_addr, true); */
  /*   bridge->button_active = address; */
  /* } */

  if (!value)
    return;

  fbv_set_led_all(fbv, false);
  for (unsigned i = 0; i < c_slot_to_button_size; ++i)
  {
    if (c_slot_to_button[i] == address && value)
    {
      kpa_change_perform_slot(&bridge->kpa, i);
      break;
    }
  }
}

static inline void
bridge_handle_fbv_pedal(fbv_t fbv)
{
  bridge_t bridge = (bridge_t)fbv_get_pdata(fbv);
  (void)bridge;

  uint8_t value = fbv_get_pedal_value(fbv);
  dbg("bridge: fbv: pedal: %u\r\n", value);
  kpa_control_wah(&bridge->kpa, value);
}

static inline void
bridge_update_kpa(bridge_t bridge, uint8_t byte)
{
  kpa_update(&bridge->kpa, byte);
}

static inline void
bridge_update_fbv(bridge_t bridge, uint8_t byte)
{
  fbv_update(&bridge->fbv, byte);
}

static inline void
bridge_update_time(bridge_t bridge, uint32_t msec_now)
{
  fbv_update_time(&bridge->fbv, msec_now);
  kpa_update_time(&bridge->kpa, msec_now);

  /* if (bridge->last_tap_value) */
  /* { */
  /*   uint32_t delta = msec_now - bridge->last_tap_time; */
  /*   if ((kpa_get_mode(&bridge->kpa) != KPA_MODE_TUNER) && (delta > c_tuner_mode_hold_time)) */
  /*   { */
  /*     dbg("bridge: enter tuner mode\r\n"); */
  /*     kpa_show_tuner(&bridge->kpa, true); */
  /*   } */
  /*   else */
  /*   { */
  /*     dbg("bridge: leave tuner mode\r\n"); */
  /*     kpa_show_tuner(&bridge->kpa, false); */
  /*   } */
  /* } */
}

static inline void
bridge_init(bridge_t bridge, writer_t kpa_writer, writer_t fbv_writer)
{
  kpa_init(&bridge->kpa, kpa_writer);
  kpa_set_pdata(&bridge->kpa, bridge);
  kpa_set_connect_event_handler(&bridge->kpa, bridge_handle_kpa_connect);
  kpa_set_mode_event_handler(&bridge->kpa, bridge_handle_kpa_mode);
  kpa_set_slot_event_handler(&bridge->kpa, bridge_handle_kpa_slot);
  kpa_set_note_event_handler(&bridge->kpa, bridge_handle_kpa_note);

  fbv_init(&bridge->fbv, fbv_writer);
  fbv_set_pdata(&bridge->fbv, bridge);
  fbv_set_connect_event_handler(&bridge->fbv, bridge_handle_fbv_connect);
  fbv_set_disconnect_event_handler(&bridge->fbv, bridge_handle_fbv_disconnect);
  fbv_set_tap_event_handler(&bridge->fbv, bridge_handle_fbv_tap);
  fbv_set_button_event_handler(&bridge->fbv, bridge_handle_fbv_button);
  fbv_set_pedal_event_handler(&bridge->fbv, bridge_handle_fbv_pedal);

  bridge->pedal_state = false;
  bridge->button_active = 0x20;

  bridge->last_tap_time = 0;
  bridge->last_tap_value = false;
}

#endif
