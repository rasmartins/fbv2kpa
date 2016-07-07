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
