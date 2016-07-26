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

#ifndef FBV_PARSER_H_INCLUDED_
#define FBV_PARSER_H_INCLUDED_

// ISO C 99 headers.
#include <stdint.h>
#include <stdbool.h>

// Local headers.
#include "frame.h"

//! Valid parser states.
enum fbv_parser_state_t
{
  //! Start of frame.
  FBV_PS_START,
  //! Frame length: command code plus parameter bytes.
  FBV_PS_LENGTH,
  //! Command code.
  FBV_PS_COMMAND,
  //! Payload.
  FBV_PS_PAYLOAD
};

//! FBV frame parser.
struct fbv_parser
{
  //! Current parser state.
  enum fbv_parser_state_t state;
  //! Payload write index.
  uint8_t frame_widx;
};

//! FBV frame parser object.
typedef struct fbv_parser* fbv_parser_t;

//! Initialize FBV parser object.
//! @param[in] parser FBV parser object.
static inline void
fbv_parser_init(fbv_parser_t parser)
{
  parser->state = FBV_PS_START;
  parser->frame_widx = 0;
}

//! Parse on byte of data.
//! @param[in] data byte.
//! @return true if a command was successfully parsed, false otherwise.
static inline bool
fbv_parser_parse(fbv_parser_t parser, fbv_frame_t frame, uint8_t byte)
{
  bool rv = false;

  switch (parser->state)
  {
    case FBV_PS_START:
      if (byte == c_fbv_frame_start)
        parser->state = FBV_PS_LENGTH;
      else
        dbg("fbv: lost sync %02X\r\n", byte);
      break;

    case FBV_PS_LENGTH:
      if (byte < 2 || byte > (c_fbv_frame_max_size - 1))
      {
        parser->state = FBV_PS_START;
      }
      else
      {
        fbv_frame_set_payload_length(frame, byte - 1);
        parser->state = FBV_PS_COMMAND;
      }
      break;

    case FBV_PS_COMMAND:
      fbv_frame_set_command_code(frame, byte);

      if (fbv_frame_get_payload_length(frame) == 0)
      {
        rv = true;
        parser->state = FBV_PS_START;
      }
      else
      {
        parser->frame_widx = 0;
        parser->state = FBV_PS_PAYLOAD;
      }
      break;

    case FBV_PS_PAYLOAD:
      fbv_frame_set_payload_byte(frame, parser->frame_widx++, byte);
      if (parser->frame_widx == fbv_frame_get_payload_length(frame))
      {
        rv = true;
        parser->state = FBV_PS_START;
      }

      break;
  }

  return rv;
}

#endif
