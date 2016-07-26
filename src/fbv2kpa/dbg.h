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

#ifndef DBG_H_INCLUDED_
#define DBG_H_INCLUDED_

#if defined(__linux__)
#  include <stdio.h>
#  define dbg(...)   fprintf(stderr, __VA_ARGS__)

#elif defined(__chibios__)
#  include <hal.h>
#  include <ch.h>
#  include <chprintf.h>
#  include <usbcfg.h>
#  define dbg(...)   chprintf((BaseSequentialStream *)&SDU1, __VA_ARGS__)

#else
#  define dbg(...)

#endif

#endif
