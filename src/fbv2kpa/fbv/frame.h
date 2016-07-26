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

#ifndef FBV_FRAME_H_INCLUDED_
#define FBV_FRAME_H_INCLUDED_

// ISO C 99 headers.
#include <stdint.h>

// Local headers.
#include "../dbg.h"

//! Frame start byte.
#define c_fbv_frame_start    0xF0
//! Maximum frame size.
#define c_fbv_frame_max_size 64

//! Known command codes.
enum fbv_command_code_t
{
  //! Second request.
  FBV_CCODE_REQ_SEC = 0x30,
  //! Request settings (from FBV to host).
  FBV_CCODE_REQ_SET = 0x80,
  //! Button (from FBV to host).
  FBV_CCODE_BTN     = 0x81,
  //! Expression Pedal (from FBV to host).
  FBV_CCODE_EXP     = 0x82,
  //! First request.
  FBV_CCODE_REQ_FST = 0x90
};

enum fbv_command_to_dev_t
{
  FBV_SET_LED  = 0x04,
  FBV_SET_FLAT = 0x20
};

//! FBV frame structure.
struct fbv_frame
{
  //! Frame data.
  uint8_t data[c_fbv_frame_max_size];
};

//! FBV frame object.
typedef struct fbv_frame* fbv_frame_t;

//! Initialize FBV frame.
//! @param[in] frame FBV frame object.
static inline void
fbv_frame_init(fbv_frame_t frame)
{
  frame->data[0] = c_fbv_frame_start;
}

//! Set frame payload length.
//! @param[in] frame FBV frame object.
//! @param[in] length payload length.
static inline void
fbv_frame_set_payload_length(fbv_frame_t frame, uint8_t length)
{
  frame->data[1] = length + 1;
}

//! Get frame payload length.
//! @param[in] frame FBV frame object.
//! @return payload length.
static inline uint8_t
fbv_frame_get_payload_length(const fbv_frame_t frame)
{
  return frame->data[1] - 1;
}

//! Set frame's command code.
//! @param[in] code command code.
static inline void
fbv_frame_set_command_code(fbv_frame_t frame, uint8_t code)
{
  frame->data[2] = code;
}

//! Get frame command code.
//! @param[in] frame FBV frame object.
//! @return command code.
static inline uint8_t
fbv_frame_get_command_code(const fbv_frame_t frame)
{
  return frame->data[2];
}

//! Set one payload byte.
//! @param[in] frame FBV frame object.
//! @param[in] index payload byte index.
//! @param[in] byte byte.
static inline void
fbv_frame_set_payload_byte(fbv_frame_t frame, int index, uint8_t byte)
{
  frame->data[3 + index] = byte;
}

//! Get one payload byte.
//! @param[in] frame FBV frame object.
//! @param[in] index payload byte index.
//! @return payload byte.
static inline uint8_t
fbv_frame_get_payload_byte(const fbv_frame_t frame, int index)
{
  return frame->data[3 + index];
}

//! Get frame data.
//! @param[in] frame FBV frame object.
//! @return frame data.
static inline const uint8_t*
fbv_frame_get_data(const fbv_frame_t frame)
{
  return frame->data;
}

//! Get frame size.
//! @param[in] frame FBV frame object.
//! @return frame size.
static inline int
fbv_frame_get_size(const fbv_frame_t frame)
{
  return fbv_frame_get_payload_length(frame) + 3;
}

//! Dump frame payload to debug channel.
//! @param[in] frame FBV frame object.
static inline void
fbv_frame_dump_payload(const fbv_frame_t frame)
{
  dbg("CMD: %02X |", fbv_frame_get_command_code(frame));
  for (int i = 0; i < fbv_frame_get_payload_length(frame); ++i)
    dbg(" %02X", fbv_frame_get_payload_byte(frame, i));
  dbg("\n");
}

//! Dump frame contents to debug channel.
//! @param[in] frame FBV frame object.
static inline void
fbv_frame_dump(const fbv_frame_t frame)
{
  for (int i = 0; i < fbv_frame_get_size(frame); ++i)
    dbg(" %02X", frame->data[i]);
  dbg("\n");
}

#endif
