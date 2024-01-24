#include "SensorNetwork.h"

#include <sstream>

namespace MQTTSNGW {

    SensorNetAddress::SensorNetAddress() {}

    SensorNetAddress::SensorNetAddress( const std::string& uart )
        : address_( uart )
    {
    }

    bool SensorNetAddress::isMatch(const SensorNetAddress* rhs) const
    {
        return rhs && rhs->address_ == address_;
    }

    bool SensorNetAddress::setAddress(const std::string* new_address)
    {
        if ( new_address == nullptr )
            return false;

        address_ = *new_address;

        return true;
    }

    void SensorNetwork::initialize()
    {
        char param[MQTTSNGW_PARAM_MAX];
        uint32_t baudrate = 115200;

        if (theProcess->getParam("Baudrate", param) == 0)
        {
            baudrate = (uint32_t)atoi(param);
        }

        theProcess->getParam("SerialDevice", param);

        std::ostringstream buf;
        buf << "UART: " << param << " / " <<  baudrate;
        description_ = buf.str();

        errno =0;

        if ( uart_.open(param, baudrate, false, 1, O_RDWR | O_NOCTTY) < 0 )
        {
            throw EXCEPTION("Can't open UART", errno);
        }

        sender_addr_ = SensorNetAddress( param );
    }

    const char* SensorNetwork::getDescription() const
    {
        return description_.c_str();
    }

    const SensorNetAddress* SensorNetwork::getSenderAddress() const
    {
        return &sender_addr_;
    }

    int SensorNetwork::unicast(const std::uint8_t* buffer, std::size_t buffer_length, const SensorNetAddress* sendTo)
    {
        return 0;
    }

    int SensorNetwork::broadcast(const std::uint8_t* buffer, std::size_t buffer_length)
    {
        return 0;
    }

    int SensorNetwork::read(std::uint8_t* buffer, std::size_t buffer_length)
    {
        return 0;
    }
}
