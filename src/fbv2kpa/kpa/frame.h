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

#ifndef KPA_MESSAGE_H_INCLUDED_
#define KPA_MESSAGE_H_INCLUDED_

//! Kemper MIDI frame type.
enum kpa_frame_type
{
  //! Unknown frame type.
  KPA_MT_UNK,
  //! Active Sense frame type.
  KPA_MT_AS,
  //! System Exclusive frame type.
  KPA_MT_SYX,
  //! Control Change frame type.
  KPA_MT_CC,
  //! Program Change frame type.
  KPA_MT_PC
};

typedef enum kpa_frame_type kpa_frame_type_t;

//! Kemper System Exclusive function codes.
enum kpa_function_codes
{
  //! Single Parameter Change.
  KPA_FC_SPARM     = 0x01,
  //! Multi Parameter Change.
  KPA_FC_MPARM     = 0x02,
  //! String Parameter.
  KPA_FC_STRING    = 0x03,
  //! Request Single Parameter Value.
  KPA_FC_REQ_SPARM = 0x41,
  //! Request Multiple Parameter Values.
  KPA_FC_REQ_MPARM = 0x42
};

typedef enum kpa_function_codes kpa_function_codes_t;

//! Kemper MIDI Control Change frame.
struct kpa_frame_cc
{
  //! Controller number.
  uint8_t number;
  //! Data.
  uint8_t data;
};

//! Kemper MIDI Program Change frame.
struct kpa_frame_pc
{
  //! Program number.
  uint8_t number;
};

//! Kemper System Exclusive frame structure.
struct kpa_frame_syx
{
  //! Function Code.
  uint8_t function;
  //! NRPN Address.
  uint16_t nrpn_addr;
  //! Data.
  uint8_t data[64];
  //! Data size.
  uint8_t data_size;
};

//! Kemper System exclusive frame object.
typedef struct kpa_frame_syx* kpa_frame_syx_t;

//! Kemper frame structure.
struct kpa_frame
{
  //! Frame type.
  kpa_frame_type_t type;

  union
  {
    struct kpa_frame_cc cc;
    struct kpa_frame_pc pc;
    struct kpa_frame_syx syx;
  } msg;
};

//! Kemper frame object.
typedef struct kpa_frame* kpa_frame_t;

static inline void
kpa_frame_init(kpa_frame_t frame)
{
  frame->type = KPA_MT_UNK;
  frame->msg.syx.function = 0;
  frame->msg.syx.nrpn_addr = 0;
}

static inline void
kpa_frame_set_type(kpa_frame_t frame,  kpa_frame_type_t type)
{
  frame->type = type;
}

static inline kpa_frame_type_t
kpa_frame_get_type(const kpa_frame_t frame)
{
  return frame->type;
}

#endif
