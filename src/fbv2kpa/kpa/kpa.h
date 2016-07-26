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
// along with Foobar.  If not, see <http://www.gnu.org/licenses/>.          *
//***************************************************************************

#ifndef KPA_H_INCLUDED_
#define KPA_H_INCLUDED_

// ISO C 99 headers.
#include <stdint.h>
#include <stdbool.h>

// Local headers.
#include "parser.h"
#include "note.h"

//! Kemper operation modes.
enum kpa_mode
{
  //! Unknown mode.
  KPA_MODE_UNKNOWN  = 0,
  //! Tuner mode.
  KPA_MODE_TUNER    = 1,
  //! Browser mode.
  KPA_MODE_BROWSER  = 2,
  //! Perform mode.
  KPA_MODE_PERFORM  = 3,
  //! Profiler mode.
  KPA_MODE_PROFILER = 4
};

typedef enum kpa_mode kpa_mode_t;

//! Kemper device structure.
struct kpa
{
  //! Writer object.
  writer_t writer;
  //! Current Kemper operation mode.
  kpa_mode_t mode;
  //! Kemper message parser.
  struct kpa_parser parser;
  //! Receive frame.
  struct kpa_frame rx_frame;
  //! Current tuner note.
  struct kpa_note note;
  //! Current slot name.
  char slot_name[64];
  //! Current slot number, negative numbers denote invalid/unknown slot numbers.
  int slot_number;
  //! True if device is connected false otherwise.
  bool connected;
  //! Connect event handler.
  void (*connect_event_handler)(struct kpa*);
  //! Mode change event handler.
  void (*mode_event_handler)(struct kpa*);
  //! Slot change event handler.
  void (*slot_event_handler)(struct kpa*);
  //! Note change event handler.
  void (*note_event_handler)(struct kpa*);
  //! Private data.
  void* pdata;
};

//! Kemper object.
typedef struct kpa* kpa_t;

//! Initialize Kemper structure.
//! @param[in] kpa kemper structure.
static inline void
kpa_init(kpa_t kpa, writer_t writer)
{
  kpa->writer = writer;
  kpa->mode = KPA_MODE_UNKNOWN;
  kpa->slot_name[0] = 0;
  kpa->slot_number = -1;
  kpa_parser_init(&kpa->parser);
  kpa_frame_init(&kpa->rx_frame);
  kpa_note_init(&kpa->note);

  kpa->connected = false;
  kpa->connect_event_handler = NULL;
  kpa->mode_event_handler = NULL;
  kpa->slot_event_handler = NULL;
  kpa->note_event_handler = NULL;
  kpa->pdata = NULL;
}

static inline void
kpa_set_connect_event_handler(kpa_t kpa, void (*handler)(kpa_t))
{
  kpa->connect_event_handler = handler;
}

static inline void
kpa_set_mode_event_handler(kpa_t kpa, void (*handler)(kpa_t))
{
  kpa->mode_event_handler = handler;
}

static inline void
kpa_set_slot_event_handler(kpa_t kpa, void (*handler)(kpa_t))
{
  kpa->slot_event_handler = handler;
}

static inline void
kpa_set_note_event_handler(kpa_t kpa, void (*handler)(kpa_t))
{
  kpa->note_event_handler = handler;
}

static inline void
kpa_set_pdata(kpa_t kpa, void* pdata)
{
  kpa->pdata = pdata;
}

static inline void*
kpa_get_pdata(kpa_t kpa)
{
  return kpa->pdata;
}

//! Retrieve kemper operation mode.
//! @param[in] kpa Kemper object.
//! @return Kemper operation mode.
static inline kpa_mode_t
kpa_get_mode(const kpa_t kpa)
{
  return kpa->mode;
}

static inline kpa_mode_t
kpa_get_slot_number(const kpa_t kpa)
{
  return kpa->slot_number;
}

static inline const kpa_note_t
kpa_get_note(const kpa_t kpa)
{
  return &(kpa->note);
}

static inline void
kpa_send_command(kpa_t kpa, const uint8_t* cmd, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    writer_write(kpa->writer, cmd[i]);
}

static inline void
kpa_send_syx_header(kpa_t kpa, kpa_function_codes_t function_code, uint16_t nrpn_address)
{
  // SYX.
  writer_write(kpa->writer, 0xF0);

  // Manufacturer Id.
  writer_write(kpa->writer, 0x00);
  writer_write(kpa->writer, 0x20);
  writer_write(kpa->writer, 0x33);

  // Product Id.
  writer_write(kpa->writer, 0x02);

  // Device Id.
  writer_write(kpa->writer, 0x7F);

  // Function Code.
  writer_write(kpa->writer, function_code);

  // Instance.
  writer_write(kpa->writer, 0x00);

  // NRPN Address.
  writer_write(kpa->writer, (nrpn_address >> 8) & 0x7F);
  writer_write(kpa->writer, nrpn_address & 0x7F);
}

static inline void
kpa_send_syx_footer(kpa_t kpa)
{
  writer_write(kpa->writer, 0xF7);
}

static inline void
kpa_request_single_parameter(kpa_t kpa, uint16_t nrpn_address)
{
  kpa_send_syx_header(kpa, KPA_FC_REQ_SPARM, nrpn_address);
  kpa_send_syx_footer(kpa);
}

static inline void
kpa_change_string_parameter(kpa_t kpa, uint16_t nrpn_address, const char* param)
{
  kpa_send_syx_header(kpa, KPA_FC_STRING, nrpn_address);

  while (*param != 0)
  {
    writer_write(kpa->writer, *param);
    ++param;
  }

  writer_write(kpa->writer, 0x00);

  kpa_send_syx_footer(kpa);
}

static inline void
kpa_request_current_mode(kpa_t kpa)
{
  kpa_request_single_parameter(kpa, 0x7F7E);
}

static inline void
kpa_request_current_slot(kpa_t kpa)
{
  kpa_send_syx_header(kpa, KPA_FC_REQ_SPARM, 0x2502);
  kpa_send_syx_footer(kpa);
}


static inline void
kpa_request_current_note(kpa_t kpa)
{
  kpa_request_single_parameter(kpa, 0x7D54);
}

static inline void
kpa_send_beacon(kpa_t kpa)
{
  uint8_t cmd[] =
  {
    0xF0,
    0x00, 0x20, 0x33,
    0x02,
    0x7F,
    0x7E, 0x00, 0x40, 0x01, 0x36, 0x04,
    0xF7
  };

  kpa_send_command(kpa, cmd, sizeof(cmd));
}

static inline void
kpa_send_owner(kpa_t kpa)
{
  kpa_change_string_parameter(kpa, 0x7f7f, "FBV2KPA");
}

static inline void
kpa_control_wah(kpa_t kpa, uint8_t value)
{
  uint8_t cmd[] = {0xB0, 0x01, (uint8_t)(value & 0x7F)};
  kpa_send_command(kpa, cmd, sizeof(cmd));
}

static inline void
kpa_show_tuner(kpa_t kpa, bool value)
{
  dbg("kpa: showTuner(%d)\r\n", value);
  uint8_t cmd[] = {0xB0, 31, value};
  kpa_send_command(kpa, cmd, sizeof(cmd));
}

static inline void
kpa_change_perform_slot(kpa_t kpa, unsigned slot_number)
{
  if (slot_number > 4)
    return;

  uint8_t cmd[] = {0xB0, (uint8_t)(50 + slot_number), 1};
  kpa_send_command(kpa, cmd, sizeof(cmd));
}

static inline void
kpa_handle_current_note(kpa_t kpa, const kpa_frame_syx_t syx)
{
  uint8_t value = syx->data[1];

  if (value == 0)
    return;

  kpa_note_set_from_table(&kpa->note, value % 12, (value / 12) - 1);
  if (kpa->note_event_handler != NULL)
    kpa->note_event_handler(kpa);
}

static inline void
kpa_handle_current_tuning(kpa_t kpa, const kpa_frame_syx_t syx)
{
  kpa_request_current_note(kpa);

  uint16_t value = (syx->data[0] & 0x7F) << 7;

  if (syx->data_size == 2)
    value |= syx->data[1] & 0x7F;

  kpa_note_set_tune(&kpa->note, value - (16383 / 2));
}

static inline void
kpa_handle_current_mode(kpa_t kpa, const kpa_frame_syx_t syx)
{
  if (kpa->mode == syx->data[1])
    return;

  kpa->mode = syx->data[1];

  if (!kpa->connected)
  {
    kpa->connected = true;
    if (kpa->connect_event_handler != NULL)
      kpa->connect_event_handler(kpa);
  }

  if (kpa->mode == KPA_MODE_PERFORM)
    kpa_change_perform_slot(kpa, 0);

  if (kpa->mode_event_handler != NULL)
    kpa->mode_event_handler(kpa);
}

static inline void
kpa_handle_current_slot_name(kpa_t kpa, const kpa_frame_syx_t syx)
{
  int index = 0;
  const uint8_t* ptr = syx->data;
  while (*ptr != 0)
  {
    kpa->slot_name[index++] = *ptr;
    ptr++;
  }
  kpa->slot_name[index] = '\0';
}

static inline void
kpa_handle_syx(kpa_t kpa)
{
  struct kpa_frame_syx* syx = &(kpa->rx_frame.msg.syx);

  /* dbg("SYX"); */
  /* for (int i = 0; i < syx->data_size; ++i) */
  /* { */
  /*   dbg(" %02X", syx->data[i]); */
  /* } */
  /* dbg("\r\n"); */

  /* dbg("SYX: %04X | FUN: %02X\r\n", syx->nrpn_addr, syx->function); */

  switch (syx->nrpn_addr)
  {
    case 0x3ED4:
      kpa_handle_current_note(kpa, syx);
      break;

    case 0x3E0F:
      kpa_handle_current_tuning(kpa, syx);
      break;

    case 0x3FFE:
      kpa_handle_current_mode(kpa, syx);
      break;

      // TAP event.
    case 0x3E00:
      break;

      // Current slot name.
    case 0x0001:
      kpa_handle_current_slot_name(kpa, syx);
      break;

    default:
      /* dbg("kpa: nrpn %04X: FUN: %02X DAT:", syx->nrpn_addr, syx->function); */
      /* for (size_t i = 0; i < syx->data_size; ++i) */
      /*   dbg( " %02X", syx->data[i]); */
      /* dbg("\r\n"); */
      break;
  }
}

static inline void
kpa_handle_active_sense(kpa_t kpa)
{
  kpa_send_beacon(kpa);
  kpa_send_owner(kpa);
  if (kpa->mode == KPA_MODE_UNKNOWN)
    kpa_request_current_mode(kpa);
}

static inline void
kpa_handle_change_control(kpa_t kpa)
{
  //dbg("kpa_handle_change_control()\r\n");
}

static inline void
kpa_handle_change_program(kpa_t kpa, uint8_t program_number)
{
  if (program_number == kpa->slot_number)
    return;

  kpa->slot_number = program_number;
  if (kpa->slot_event_handler != NULL)
    kpa->slot_event_handler(kpa);
}

static inline void
kpa_update_time(kpa_t kpa, uint32_t msec_now)
{
  (void)kpa;
  (void)msec_now;

  /* uint32_t now = clock_get_ms(); */
  /* uint32_t delta = now - fbv->last_message_time; */
  /* if (delta > 4000) */
  /* { */
  /*   dbg("fbv: disconnected\r\n"); */
  /*   fbv->connected = false; */
  /*   if (fbv->disconnect_handler) */
  /*     fbv->disconnect_handler(fbv); */
  /* } */
}

static inline void
kpa_update(kpa_t kpa, uint8_t byte)
{
  if (!kpa_parser_update(&kpa->parser, &kpa->rx_frame, byte))
    return;

  switch (kpa_frame_get_type(&kpa->rx_frame))
  {
    case KPA_MT_AS:
      kpa_handle_active_sense(kpa);
      break;

    case KPA_MT_CC:
      kpa_handle_change_control(kpa);
      break;

    case KPA_MT_PC:
      kpa_handle_change_program(kpa, kpa->rx_frame.msg.pc.number);
      break;

    case KPA_MT_SYX:
      kpa_handle_syx(kpa);
      break;

    default:
      break;
  }
}

#endif
