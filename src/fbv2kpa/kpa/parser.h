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

#ifndef KPA_PARSER_H_INCLUDED_
#define KPA_PARSER_H_INCLUDED_

// ISO C 99 headers.
#include <stdint.h>
#include <stddef.h>

// Local headers.
#include "frame.h"

//! Kemper MIDI messages.
enum kpa_parser_midi_t
{
  //! MIDI Active Sense start of message.
  KPA_MIDI_AS      = 0xFE,
  //! MIDI System Exclusive start of message.
  KPA_MIDI_SYX_SOM = 0xF0,
  //! MIDI System Exclusive end of message.
  KPA_MIDI_SYX_EOM = 0xF7,
  //! MIDI Control Change start of message.
  KPA_MIDI_CC      = 0xB0,
  //! MIDI Program Change start of message.
  KPA_MIDI_PC      = 0xC0
};

//! Kemper parser states.
enum kpa_parser_state_t
{
  //! Waiting for message.
  KPA_PS_IDLE,
  //! System exclusive message header.
  KPA_PS_SYX_HDR,
  //! System exclusive message data.
  KPA_PS_SYX_DAT,
  //! Control change message.
  KPA_PS_CC,
  //! Program change message.
  KPA_PS_PC
};

//! Kemper parser structure.
struct kpa_parser
{
  //! Current parser state.
  enum kpa_parser_state_t state;
  //! Write index.
  uint8_t widx;
};

//! Kemper parser objecr.
typedef struct kpa_parser* kpa_parser_t;

//! Initialize a Kemper parser object.
//! @param[in] parser Kemper parser object.
static inline void
kpa_parser_init(kpa_parser_t parser)
{
  parser->state = KPA_PS_IDLE;
  parser->widx = 0;
}

//! Update Kemper parser state.
//! @param[in] kpa Kemper parser object.
//! @param[in] byte data byte.
static inline bool
kpa_parser_update(kpa_parser_t parser, kpa_frame_t frame, uint8_t byte)
{
  bool rv = false;

  /* dbg(" %02X", byte); */

  switch (parser->state)
  {
    case KPA_PS_IDLE:
      switch (byte)
      {
        case KPA_MIDI_AS:
          kpa_frame_set_type(frame, KPA_MT_AS);
          rv = true;
          break;

        case KPA_MIDI_SYX_SOM:
          kpa_frame_set_type(frame, KPA_MT_SYX);
          parser->widx = 0;
          frame->msg.syx.data_size = 0;
          parser->state = KPA_PS_SYX_HDR;
          break;

        case KPA_MIDI_CC:
          kpa_frame_set_type(frame, KPA_MT_CC);
          parser->widx = 0;
          parser->state = KPA_PS_CC;
          break;

        case KPA_MIDI_PC:
          kpa_frame_set_type(frame, KPA_MT_PC);
          parser->state = KPA_PS_PC;
          break;

        default:
          dbg("kpa_parser: discard: %02X\r\n", byte);
          break;
      }
      break;

    case KPA_PS_CC:
      if (parser->widx++ == 0)
      {
        frame->msg.cc.number = byte;
      }
      else
      {
        rv = true;
        frame->msg.cc.data = byte;
        parser->state = KPA_PS_IDLE;
      }
      break;

    case KPA_PS_PC:
      rv = true;
      frame->msg.pc.number = byte;
      parser->state = KPA_PS_IDLE;
      break;

      // SYX header.
    case KPA_PS_SYX_HDR:
      switch (parser->widx++)
      {
        case 0:
          if (byte != 0x00)
            parser->state = KPA_PS_IDLE;
          break;

        case 1:
          if (byte != 0x20)
            parser->state = KPA_PS_IDLE;
          break;

        case 2:
          if (byte != 0x33)
            parser->state = KPA_PS_IDLE;
          break;

          // Product type.
        case 3:
          break;

          // Device Id.
        case 4:
          break;

          // Function Code.
        case 5:
          frame->msg.syx.function = byte;
          break;

          // Instance.
        case 6:
          if (byte != 0x00)
            parser->state = KPA_PS_IDLE;
          break;

          // NRPN Address MSB.
        case 7:
          frame->msg.syx.nrpn_addr = (byte & 0x7F) << 7;
          break;
          // NRPN Address LSB.

        case 8:
          frame->msg.syx.nrpn_addr |= (byte & 0x7F);
          parser->state = KPA_PS_SYX_DAT;
          break;
      }
      break;

    case KPA_PS_SYX_DAT:
      if (byte == KPA_MIDI_SYX_EOM)
      {
        rv = true;
        parser->state = KPA_PS_IDLE;
      }
      else
      {
        frame->msg.syx.data[frame->msg.syx.data_size++] = byte;
      }
      break;
  }

  /* if (rv) */
  /*   dbg("\r\n"); */

  return rv;
}

#endif
