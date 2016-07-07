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

#ifndef FBV_FRAME_HPP_INCLUDED_
#define FBV_FRAME_HPP_INCLUDED_

// ISO C headers.
#include <stdint.h>

// Local headers.
#include "FbvConstants.hpp"
#include "Debug.hpp"

class FbvFrame
{
public:
  FbvFrame()
  {
    m_frame[0] = c_fbv_frame_start;
  }

  inline void
  setPayloadLength(uint8_t length)
  {
    m_frame[1] = length + 1;
  }

  inline uint8_t
  getPayloadLength(void) const
  {
    return m_frame[1] - 1;
  }

  //! Set frame's command code.
  //! @param[in] code command code.
  inline void
  setCommandCode(uint8_t code)
  {
    m_frame[2] = code;
  }

  inline uint8_t
  getCommandCode(void) const
  {
    return m_frame[2];
  }

  inline void
  setPayloadByte(int index, uint8_t byte)
  {
    m_frame[3 + index] = byte;
  }

  inline uint8_t
  getPayloadByte(int index) const
  {
    return m_frame[3 + index];
  }

  inline const uint8_t*
  getFrame(void) const
  {
    return m_frame;
  }

  inline const int
  getFrameSize(void) const
  {
    return getPayloadLength() + 3;
  }

  void
  dump(void) const
  {
    dbg("CMD: %02X |", getCommandCode());
    for (int i = 0; i < getPayloadLength(); ++i)
    {
      dbg(" %02X", getPayloadByte(i));
    }

    dbg("\n");
  }

  void
  dumpFrame(void) const
  {
    for (int i = 0; i < getFrameSize(); ++i)
      dbg(" %02X", m_frame[i]);

    dbg("\n");
  }

private:
  //! Command frame.
  uint8_t m_frame[c_fbv_frame_max_size];
};

#endif
