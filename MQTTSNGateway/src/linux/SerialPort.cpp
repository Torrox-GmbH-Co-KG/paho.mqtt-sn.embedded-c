#include "SerialPort.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

namespace MQTTSNGW
{

/*=========================================
 Class SerialPort
 =========================================*/
SerialPort::SerialPort()
{
    _tio.c_iflag = IGNBRK | IGNPAR;
    _tio.c_cflag = CS8 | CLOCAL | CRTSCTS | CREAD;
    _tio.c_cc[VINTR] = 0;
    _tio.c_cc[VTIME] = 10;   // 1 sec.
    _tio.c_cc[VMIN] = 1;
    _fd = 0;
}

SerialPort::~SerialPort()
{
    if (_fd)
    {
        ::close(_fd);
    }
}

int SerialPort::open(char* devName, unsigned int baudrate, bool parity,
        unsigned int stopbit, unsigned int flg)
{
    _fd = ::open(devName, flg);
    if (_fd < 0)
    {
        return _fd;
    }

    if (parity)
    {
        _tio.c_cflag = _tio.c_cflag | PARENB;
    }
    if (stopbit == 2)
    {
        _tio.c_cflag = _tio.c_cflag | CSTOPB;
    }

    if (cfsetspeed(&_tio, baudrate) < 0)
    {
        return errno;
    }
    return tcsetattr(_fd, TCSANOW, &_tio);
}

bool SerialPort::send(unsigned char b)
{
    if (write(_fd, &b, 1) < 0)
    {
        return false;
    }
    else
    {
        D_NWSTACK( " %02x", b);
        return true;
    }
}

bool SerialPort::recv(unsigned char* buf)
{
    struct timeval timeout;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(_fd, &rfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;    // 500ms
    if ( select(_fd + 1, &rfds, 0, 0, &timeout) > 0 )
    {
        if (read(_fd, buf, 1) > 0)
        {
            D_NWSTACK( " %02x",buf[0] );
            return true;
        }
    }
    return false;
}

void SerialPort::flush(void)
{
    tcsetattr(_fd, TCSAFLUSH, &_tio);
}

}
