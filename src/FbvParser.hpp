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

#ifndef FBV_PARSER_HPP_INCLUDED_
#define FBV_PARSER_HPP_INCLUDED_

// ISO C headers.
#include <stdint.h>
#include <cstdio>

// Local headers.
#include "FbvFrame.hpp"
#include "Debug.hpp"

class FbvParser
{
public:
  FbvParser(void):
    m_state(PS_START)
  { }

  //! Parse on byte of data.
  //! @param[in] data byte.
  //! @return true if a command was successfully parsed, false otherwise.
  bool
  parse(FbvFrame& frame, uint8_t byte)
  {
    bool rv = false;

    switch (m_state)
    {
      case PS_START:
        if (byte == c_fbv_frame_start)
          m_state = PS_LENGTH;
        break;

      case PS_LENGTH:
        if (byte < 2 || byte > (c_fbv_frame_max_size - 1))
        {
          m_state = PS_START;
        }
        else
        {
          frame.setPayloadLength(byte - 1);
          m_state = PS_COMMAND;
        }
        break;

      case PS_COMMAND:
        frame.setCommandCode(byte);

        if (frame.getPayloadLength() == 0)
        {
          rv = true;
          m_state = PS_START;
        }
        else
        {
          m_frame_widx = 0;
          m_state = PS_PARAMS;
        }
        break;

      case PS_PARAMS:
        frame.setPayloadByte(m_frame_widx++, byte);
        if (m_frame_widx == frame.getPayloadLength())
        {
          rv = true;
          m_state = PS_START;
        }

        break;
    }

    return rv;
  }

private:
  //! Valid parser states.
  enum ParserStates
  {
    //! Start of frame.
    PS_START,
    //! Frame length: command code plus parameter bytes.
    PS_LENGTH,
    //! Command code.
    PS_COMMAND,
    //! Command parameters.
    PS_PARAMS
  };

  //! Current parser state.
  ParserStates m_state;
  //! Parameter write index.
  uint8_t m_frame_widx;
};

#endif
