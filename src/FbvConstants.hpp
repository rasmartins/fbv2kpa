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

#ifndef FBV_CONSTANTS_HPP_INCLUDED_
#define FBV_CONSTANTS_HPP_INCLUDED_

//! Frame start byte.
static const uint8_t c_fbv_frame_start = 0xF0;
//! Maximum frame size.
static const int c_fbv_frame_max_size = 64;

enum CommandCode
{
  //! First request.
  FBV_CCODE_REQ_FST = 0x90,
  //! Second request.
  FBV_CCODE_REQ_SEC = 0x30,
  //! Button.
  FBV_CCODE_BTN = 0x81,
  //! Expression Pedal.
  FBV_CCODE_EXP = 0x82,
};

enum FbvCommandHostToDev
{
  FBV_SET_LED  = 0x04,
  FBV_SET_FLAT = 0x20
};

#endif
