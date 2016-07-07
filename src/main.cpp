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

#include <cstdlib>
#include "Serial.hpp"
#include "FbvParser.hpp"

#include "Fbv.hpp"

static Serial uart_fbv("/dev/ttyUSB0");
static Fbv fbv(uart_fbv);

void*
loop(void* a)
{
  (void)a;
  while (true)
  {
    fbv.update();
  }

  return NULL;
}

int
main(void)
{
  pthread_t thread;
  if (pthread_create(&thread, NULL, loop, NULL))
  {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  usleep(2000000);

  while (true)
  {
    // fbv.setTitle("I              I");
    // usleep(100000);
    // fbv.setTitle("I        ( ( ( I");
    // usleep(100000);
    // fbv.setTitle("I        ( (   I");
    // usleep(100000);
    // fbv.setTitle("I        (     I");
    // usleep(100000);
    fbv.setDisplayCharacter('A');
    //fbv.setTitle("I      **(     I");
    // usleep(100000);
    //fbv.setTitle("I      **      I");
    // fbv.setTitle("I     )**      I");
    // usleep(100000);
    fbv.setTitle("I   ) )        I");
    // usleep(100000);
    // fbv.setTitle("I ) ) )        I");
    // usleep(100000);
    usleep(100000);
  }

  return 0;
}
