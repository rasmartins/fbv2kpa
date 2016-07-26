//***************************************************************************
// This file is part of fbv2kpa.                                            *
//***************************************************************************
// fbv2kpa is free software: you can redistribute it and/or modify          *
// it under the terms of the GNU General Public License as published by     *
// the Free Software Foundation, either version 3 of the License, or        *
// (at your option) any later version.                                      *
//                                                                          *
// fbv2kpa is distributed in the hope that it will be useful,               *
// but WITHOUT ANY WARRANTY; without even the implied warranty of           *
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
// GNU General Public License for more details.                             *
//                                                                          *
// You should have received a copy of the GNU General Public License        *
// along with fbv2kpa. If not, see <http://www.gnu.org/licenses/>.          *
//***************************************************************************

#ifndef FBV_H_INCLUDED_
#define FBV_H_INCLUDED_

// ISO C 99 headers.
#include <stdint.h>

// Local headers.
#include "parser.h"
#include "../writer.h"
#include "../clock.h"

//! FBV LED enumeration.
enum fbv_led
{
  //! LED A.
  FBV_LED_A     = 0x20,
  //! LED B.
  FBV_LED_B     = 0x30,
  //! LED C.
  FBV_LED_C     = 0x40,
  //! LED D.
  FBV_LED_D     = 0x50,
  //! LED Volume.
  FBV_LED_VOL   = 0x03,
  //! LED Wah-Wah.
  FBV_LED_WAH   = 0x13
};

//! FBV LED type.
typedef enum fbv_led fbv_led_t;

//! FBV button enumeration.
enum fbv_button
{
  //! Tap tempo button.
  FBV_BTN_TAP   = 0x61,
  //! Button A.
  FBV_BTN_A     = 0x20,
  //! Button B.
  FBV_BTN_B     = 0x30,
  //! Button C.
  FBV_BTN_C     = 0x40,
  //! Button D.
  FBV_BTN_D     = 0x50,
  //! Pedal button.
  FBV_BTN_PEDAL = 0x43
};

//! FBV button type.
typedef enum fbv_button fbv_button_t;

//! FBV device structure.
struct fbv
{
  //! Writer object.
  writer_t writer;
  //! True if connected to device, false otherwise.
  bool connected;
  //! Command parser.
  struct fbv_parser parser;
  //! Receive frame buffer.
  struct fbv_frame rx_frame;
  //! Transmit frame buffer.
  struct fbv_frame tx_frame;
  //! Current tap value.
  bool tap_value;
  //! Current button address.
  uint8_t button_address;
  //! Current button value.
  bool button_value;
  //! Current pedal value.
  uint8_t pedal_value;
  //! Character on display.
  char display_char;
  //! Time of last received message.
  uint32_t last_message_time;
  //! Private data.
  void* pdata;
  //! Connect event handler.
  void (*connect_handler)(struct fbv*);
  //! Disconnect event handler.
  void (*disconnect_handler)(struct fbv*);
  //! Tap event handler callback.
  void (*tap_handler)(struct fbv*);
  //! Button event handler callback.
  void (*button_handler)(struct fbv*);
  //! Pedal event handler callback.
  void (*pedal_handler)(struct fbv*);
};

//! FBV device object.
typedef struct fbv* fbv_t;

//! FBV invalid needle value (turns off needle).
static const int c_fbv_needle_off = 4;
//! FBV disconnect timeout in millisecond.
static const uint32_t c_fbv_timeout = 4000;

//! Initialize FBV device object.
//! @param[in] fbv FBV device object.
static inline void
fbv_init(fbv_t fbv, writer_t writer)
{
  fbv->tap_value = false;
  fbv->button_address = 0,
  fbv->button_value = false;
  fbv->pedal_value = 255;
  fbv->display_char = 0;
  fbv->last_message_time = 0;
  fbv->writer = writer;
  fbv->connected = false;
  fbv->connect_handler = NULL;
  fbv->disconnect_handler = NULL;
  fbv->tap_handler = NULL;
  fbv->button_handler = NULL;
  fbv->pedal_handler = NULL;
  fbv->pdata = NULL;
  fbv_parser_init(&fbv->parser);
  fbv_frame_init(&fbv->rx_frame);
  fbv_frame_init(&fbv->tx_frame);
}

static inline void
fbv_set_pdata(fbv_t fbv, void* pdata)
{
  fbv->pdata = pdata;
}

static inline void*
fbv_get_pdata(fbv_t fbv)
{
  return fbv->pdata;
}

static inline bool
fbv_get_tap_value(const fbv_t fbv)
{
  return fbv->tap_value;
}

static inline uint8_t
fbv_get_button_address(const fbv_t fbv)
{
  return fbv->button_address;
}

static inline bool
fbv_get_button_value(const fbv_t fbv)
{
  return fbv->button_value;
}

static inline uint8_t
fbv_get_pedal_value(const fbv_t fbv)
{
  return fbv->pedal_value;
}

//! Set handler function for connect events.
//! @param[in] fbv FBV device object.
static inline void
fbv_set_connect_event_handler(fbv_t fbv, void (*handler)(fbv_t))
{
  fbv->connect_handler = handler;
}

//! Set handler function for disconnect events.
//! @param[in] fbv FBV device object.
static inline void
fbv_set_disconnect_event_handler(fbv_t fbv, void (*handler)(fbv_t))
{
  fbv->disconnect_handler = handler;
}

//! Set handler function for tap events.
//! @param[in] fbv FBV device object.
static inline void
fbv_set_tap_event_handler(fbv_t fbv, void (*handler)(fbv_t))
{
  fbv->tap_handler = handler;
}

//! Set handler function for button events.
//! @param[in] fbv FBV device object.
static inline void
fbv_set_button_event_handler(fbv_t fbv, void (*handler)(fbv_t))
{
  fbv->button_handler = handler;
}

//! Set handler function for pedal events.
//! @param[in] fbv FBV device object.
static inline void
fbv_set_pedal_event_handler(fbv_t fbv, void (*handler)(fbv_t))
{
  fbv->pedal_handler = handler;
}


//! Send buffered transmit frame to device.
//! @param[in] fbv FBV device object.
static inline void
fbv_send_frame(fbv_t fbv)
{
  int frame_size = fbv_frame_get_size(&fbv->tx_frame);
  const uint8_t* frame_data = fbv_frame_get_data(&fbv->tx_frame);

  //dbg("fbv: write:");
  for (int i = 0; i < frame_size; ++i)
  {
    //dbg(" %02X", frame_data[i]);
    writer_write(fbv->writer, frame_data[i]);
  }
  //dbg("\r\n");
}

//! Send heartbeat frame to device.
//! @param[in] fbv FBV device object.
static inline void
fbv_send_heartbeat(fbv_t fbv)
{
  fbv_frame_set_command_code(&fbv->tx_frame, 0x01);
  fbv_frame_set_payload_length(&fbv->tx_frame, 1);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 0, 0x00);
  fbv_send_frame(fbv);
}

static inline void
fbv_set_display_char(fbv_t fbv, char character)
{
  if (fbv->display_char == character)
    return;

  fbv_frame_set_command_code(&fbv->tx_frame, 0x08);
  fbv_frame_set_payload_length(&fbv->tx_frame, 4);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 0, 0x20);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 1, 0x20);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 2, 0x20);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 3, 0x20);
  fbv_send_frame(fbv);

  fbv_frame_set_command_code(&fbv->tx_frame, 0x0C);
  fbv_frame_set_payload_length(&fbv->tx_frame, 1);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 0, character);
  fbv_send_frame(fbv);

  fbv->display_char = character;
}

//! Set LED state.
//! @param[in] fbv FBV device object.
//! @param[in] led LED address.
//! @param[in] state true to turn on, false to turn off.
static inline void
fbv_set_led(fbv_t fbv, uint8_t led, bool state)
{
  dbg("fbv: set led %02X to %d\r\n", led, state);

  fbv_frame_set_command_code(&fbv->tx_frame, FBV_SET_LED);
  fbv_frame_set_payload_length(&fbv->tx_frame, 2);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 0, led);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 1, state);
  fbv_send_frame(fbv);
}

//! Set LED state of all LEDs.
//! @param[in] fbv FBV device object.
//! @param[in] state true to turn on, false to turn off.
static inline void
fbv_set_led_all(fbv_t fbv, bool state)
{
  fbv_set_led(fbv, FBV_LED_A, state);
  fbv_set_led(fbv, FBV_LED_B, state);
  fbv_set_led(fbv, FBV_LED_C, state);
  fbv_set_led(fbv, FBV_LED_D, state);
  fbv_set_led(fbv, FBV_LED_WAH, state);
  fbv_set_led(fbv, FBV_LED_VOL, state);
}

//! Turn on/off the flat indicator.
//! @param[in] fbv FBV device object.
//! @param[in] value true to turn on, false to turn off.
static inline void
fbv_set_flat(fbv_t fbv, bool value)
{
  fbv_frame_set_command_code(&fbv->tx_frame, FBV_SET_FLAT);
  fbv_frame_set_payload_length(&fbv->tx_frame, 1);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 0, value);
  fbv_send_frame(fbv);
}

static inline void
fbv_set_title(fbv_t fbv, const char* title)
{
  fbv_frame_set_command_code(&fbv->tx_frame, 0x10);
  fbv_frame_set_payload_byte(&fbv->tx_frame, 0, 0x00);

  int index = 0;
  while (title[index] != '\0')
  {
    fbv_frame_set_payload_byte(&fbv->tx_frame, 2 + index, title[index]);
    ++index;
  }

  fbv_frame_set_payload_byte(&fbv->tx_frame, 1, index + 1);
  fbv_frame_set_payload_length(&fbv->tx_frame, index + 3);
  fbv_send_frame(fbv);
}

//! Retrieve the string associated with a given tune level.
//! @param[in] level tune between -3 and 3
//! @return tuner needle string.
static inline const char*
fbv_get_tuner_needle(int level)
{
  static const char* strings[] =
  {
    "  ))))          ",
    "   )))          ",
    "    ))          ",
    "       **       ",
    "          ((    ",
    "          (((   ",
    "          ((((  "
  };

  if (level < -3 || level > 3)
    return "                ";

  return strings[level + 3];
}

//! Set tuner needle.
//! @param[in] fbv FBV device object.
//! @param[in] level tuner level.
static inline void
fbv_set_tuner_needle(fbv_t fbv, int level)
{
  fbv_set_title(fbv, fbv_get_tuner_needle(level));
}

static inline void
fbv_set_tuner_needle_off(fbv_t fbv)
{
  fbv_set_title(fbv, fbv_get_tuner_needle(4));
}

static inline void
fbv_reset_ui(fbv_t fbv)
{
  dbg("fbv: reset ui\r\n");
  fbv_set_flat(fbv, false);
  fbv_set_display_char(fbv, ' ');
  fbv_set_tuner_needle_off(fbv);
  fbv_set_led_all(fbv, false);
  fbv_set_led(fbv, FBV_LED_A, true);
}

//! Test if FBV device is connected.
//! @param[in] fbv FBV device object.
//! @return true if device is connected and alive, false otherwise.
static inline bool
fbv_is_connected(const fbv_t fbv)
{
  return fbv->connected;
}

//! Handle connection.
//! @param[in] fbv FBV device object.
static inline void
fbv_handle_connection(fbv_t fbv)
{
  fbv->connected = true;
  fbv_reset_ui(fbv);
  if (fbv->connect_handler != NULL)
    fbv->connect_handler(fbv);
}

//! Handle button event.
//! @param[in] fbv FBV device object.
static inline void
fbv_handle_button_event(fbv_t fbv)
{
  uint8_t addr = fbv_frame_get_payload_byte(&fbv->rx_frame, 0);
  bool value = fbv_frame_get_payload_byte(&fbv->rx_frame, 1) != 0;

  if (addr == FBV_BTN_TAP)
  {
    fbv->tap_value = value;
    if (fbv->tap_handler != NULL)
      fbv->tap_handler(fbv);
  }
  else
  {
    fbv->button_address = addr;
    fbv->button_value = value;

    if (fbv->button_handler != NULL)
      fbv->button_handler(fbv);
  }
}

//! Handle pedal event.
//! @param[in] fbv FBV device object.
static inline void
fbv_handle_pedal_event(fbv_t fbv)
{
  uint8_t value = fbv_frame_get_payload_byte(&fbv->rx_frame, 1) & 0x7F;
  if (fbv->pedal_value == value)
    return;

  fbv->pedal_value = value;
  if (fbv->pedal_handler != NULL)
    fbv->pedal_handler(fbv);
}

static inline void
fbv_update_time(fbv_t fbv, uint32_t msec_now)
{
  if (!fbv_is_connected(fbv))
    return;

  uint32_t delta = msec_now - fbv->last_message_time;
  if (delta > c_fbv_timeout)
  {
    fbv->connected = false;
    if (fbv->disconnect_handler)
      fbv->disconnect_handler(fbv);
  }
}

//! Update FBV device object state with new data byte.
//! @param[in] fbv FBV device object.
//! @param[in] byte data byte.
static inline void
fbv_update(fbv_t fbv, uint8_t byte)
{
  if (!fbv_parser_parse(&fbv->parser, &fbv->rx_frame, byte))
    return;

  fbv->last_message_time = clock_get_ms();
  uint8_t cmd_code = fbv_frame_get_command_code(&fbv->rx_frame);

  switch (cmd_code)
  {
    case FBV_CCODE_REQ_FST:
      break;

    case FBV_CCODE_REQ_SEC:
      fbv_send_heartbeat(fbv);
      if (!fbv_is_connected(fbv))
        fbv_handle_connection(fbv);
      break;

    case FBV_CCODE_BTN:
      fbv_handle_button_event(fbv);
      break;

    case FBV_CCODE_EXP:
      fbv_handle_pedal_event(fbv);
      break;

    case FBV_CCODE_REQ_SET:
      break;

    default:
      dbg("fbv: unknown command: %02X\r\n", cmd_code);
      break;
  }
}

#endif
