#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include "MQTTSNGWDefines.h"
#include "MQTTSNGWProcess.h"
#include <termios.h>

namespace MQTTSNGW
{
/*===========================================
  Class  SerialPort
 ============================================*/
class SerialPort{
public:
    SerialPort();
    ~SerialPort();
    int open(char* devName, unsigned int baudrate,  bool parity, unsigned int stopbit, unsigned int flg);
    bool send(unsigned char b);
    bool recv(unsigned char* b);
    void flush();

    // not implemented
    SerialPort( const SerialPort& );
    SerialPort& operator=( const SerialPort& );
private:
    int _fd;  // file descriptor
    struct termios _tio;
};

}

#endif