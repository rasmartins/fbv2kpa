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

#ifndef CLOCK_H_INCLUDED_
#define CLOCK_H_INCLUDED_

// ISO C headers.
#include <stdint.h>

//! Retrieve the number of milliseconds since power-up.
//! @return milliseconds since power-up.
uint32_t
clock_get_ms(void);

#endif
