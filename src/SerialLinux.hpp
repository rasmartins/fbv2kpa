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

#ifndef SERIAL_LINUX_HPP_INCLUDED_
#define SERIAL_LINUX_HPP_INCLUDED_

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

class Serial
{
public:
  Serial(const char* dev)
  {
    std::string cmd("setserial -v ");
    cmd += dev;
    cmd += " spd_cust divisor 768";

    std::system(cmd.c_str());

    //m_fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    m_fd = open(dev, O_RDWR | O_NOCTTY);

    fcntl(m_fd, F_SETFL, 0);
    tcgetattr(m_fd, &m_options);

    //m_options.c_cc[VMIN] = 0;
    m_options.c_cc[VMIN] = 1;
    //m_options.c_cc[VTIME] = 0;
    m_options.c_cc[VTIME] = 5;
    m_options.c_cflag |= (CLOCAL | CREAD | CS8);
    m_options.c_iflag |= IGNBRK;
    m_options.c_iflag &= ~(ICRNL | IGNCR);
    m_options.c_oflag &= ~(OPOST | ONLCR);
    m_options.c_lflag &= ~(IEXTEN);

    m_options.c_cflag &= ~PARENB;
    m_options.c_cflag &= ~PARODD;
    m_options.c_cflag &= ~CSTOPB;
    m_options.c_cflag &= ~CSIZE;
    m_options.c_iflag &= ~PARMRK;
    m_options.c_iflag |= IGNPAR;

    m_options.c_cflag &= ~CSTOPB;

    m_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHOK | ECHOKE | ECHOCTL);

    cfsetospeed(&m_options, B38400);
    cfsetispeed(&m_options, B38400);

    tcsetattr(m_fd, TCSANOW, &m_options);
  }

  ~Serial(void)
  {
    close(m_fd);
  }

  int
  read(unsigned char* b)
  {
    return ::read(m_fd, b, 1);
  }

  int
  write(unsigned char b)
  {
    return ::write(m_fd, &b, 1);
  }

private:
  struct termios m_options;
  int m_fd;
};

#endif
