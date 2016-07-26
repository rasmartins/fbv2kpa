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

// ISO C headers.
#include <stdlib.h>

// Linux headers.
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <sys/select.h>

// FBV2KPA headers.
#include <fbv2kpa/bridge.h>

void
uart_write(void* object, uint8_t byte)
{
  int* fd = (int*)object;
  write(*fd, &byte, 1);
}

int
uart_open(const char* dev)
{
  system("setserial -v /dev/ttyUSB0 spd_cust divisor 768");

  int fd = open(dev, O_RDWR | O_NOCTTY);

  fcntl(fd, F_SETFL, 0);

  struct termios options;

  tcgetattr(fd, &options);

  options.c_cc[VMIN] = 1;
  options.c_cc[VTIME] = 5;
  options.c_cflag |= (CLOCAL | CREAD | CS8);
  options.c_iflag |= IGNBRK;
  options.c_iflag &= ~(ICRNL | IGNCR);
  options.c_oflag &= ~(OPOST | ONLCR);
  options.c_lflag &= ~(IEXTEN);

  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~PARODD;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_iflag &= ~PARMRK;
  options.c_iflag |= IGNPAR;

  options.c_cflag &= ~CSTOPB;

  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHOK | ECHOKE | ECHOCTL);

  cfsetospeed(&options, B38400);
  cfsetispeed(&options, B38400);

  tcsetattr(fd, TCSANOW, &options);

  return fd;
}

int
uart_poll(int fd, unsigned timeout_ms)
{
  fd_set m_rfd;
  FD_ZERO(&m_rfd);
  FD_SET(fd, &m_rfd);

  struct timeval tv = {0, timeout_ms * 1000};
  int rv = select(fd + 1, &m_rfd, NULL, NULL, &tv);

  if (rv == -1)
  {
    //! Workaround for when we are interrupted by a signal.
    if (errno == EINTR)
      return 0;
  }

  return rv > 0;
}

uint32_t
clock_get_ms(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000u + ts.tv_nsec / 100000000;
}

int
main(int argc, char** argv)
{
  int fd = uart_open(argv[1]);

  struct writer fbv_writer;
  writer_init(&fbv_writer, &fd, uart_write);

  struct bridge bridge;
  bridge_init(&bridge, NULL, &fbv_writer);

  uint8_t byte = 0;
  while (true)
  {
    bridge_update_time(&bridge, clock_get_ms());

    if (!uart_poll(fd, 10))
      continue;

    if (read(fd, &byte, 1) == 1)
    {
      bridge_update_fbv(&bridge, byte);
    }
  }

  return 0;
}
