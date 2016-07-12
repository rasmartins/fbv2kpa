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
#include "Kpa.hpp"

//static Serial uart_fbv("/dev/ttyACM0");
static Serial uart_kpa("/dev/ttyACM0");
//static Fbv fbv(uart_fbv);
static Kpa kpa(uart_kpa);

// void*
// loop(void* a)
// {
//   (void)a;
//   while (true)
//   {
//     fbv.update();
//   }

//   return NULL;
// }

void*
loop_kpa(void* a)
{
  (void)a;
  while (true)
  {
    kpa.update();
  }

  return NULL;
}

// void
// testTuner(void)
// {
//   char string[17] = "                ";
//   char order[128];

//   FILE* fd = fopen("data-right.txt", "wa");

//   for (unsigned i = 0; i < 256; ++i)
//   {
//     for (int b = 0; b < 8; ++b)
//     {
//       if (i & (1 << b))
//         string[8 + b] = '(';
//       else
//         string[8 + b] = ' ';
//     }

//     printf("%03u: '%s'\r\n", i, string);
//     fbv.setTitle(string);

//     scanf("%s", order);

//     switch (order[0])
//     {
//       case 'i':
//         break;
//       default:
//         fprintf(fd, "%s | %03u: '%s'\n", order, i, string);
//         fflush(fd);
//         break;
//     }
//   }

//   fclose(fd);
// }

int
main(void)
{
  pthread_t thread;
  if (pthread_create(&thread, NULL, loop_kpa, NULL))
  {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  usleep(2000000);

  while (true)
  {
    // for (int i = 0; i < 7; ++i)
    // {
    //   fbv.setDisplayCharacter('A' + i);
    //   fbv.setTitle(strings[i]);
    //   usleep(1000000);
    // }
    usleep(1000000);
  }

  return 0;
}
