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

#ifndef WRITER_H_INCLUDED_
#define WRITER_H_INCLUDED_

struct writer
{
  void (*write)(void*, uint8_t);
  void* object;
};

typedef struct writer* writer_t;

static inline void
writer_init(writer_t writer, void* object, void (*write)(void*, uint8_t))
{
  writer->write = write;
  writer->object = object;
}

static inline void
writer_write(writer_t writer, uint8_t byte)
{
  writer->write(writer->object, byte);
}

#endif
