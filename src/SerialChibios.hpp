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

#ifndef SERIAL_CHIBIOS_HPP_INCLUDED_
#define SERIAL_CHIBIOS_HPP_INCLUDED_

#include <ch.h>
#include <hal.h>

class Serial
{
public:
  Serial(BaseSequentialStream* stream):
    m_stream(stream)
  { }

  int
  read(unsigned char* b)
  {
    return chnRead(m_stream, b, 1);
  }

  int
  write(unsigned char b)
  {
    return chnWrite(m_stream, &b, 1);
  }

private:
  BaseSequenctialStream* m_stream;
};

#endif
