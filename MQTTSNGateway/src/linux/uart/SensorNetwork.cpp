#include "cobs.hpp"

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
            static std::string error_text = "Can't open UART: " + description_;
            throw EXCEPTION(error_text.c_str(), errno);
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

    int SensorNetwork::unicast(const std::uint8_t* buffer, std::size_t buffer_length, const SensorNetAddress*)
    {
        return broadcast( buffer, buffer_length );
    }

    int SensorNetwork::broadcast(const std::uint8_t* input, std::size_t input_length)
    {
        std::vector< std::uint8_t > buffer( input_length + cobs::encode_max_overhead( input_length ) );
        std::copy( input, input + input_length, buffer.begin() );

        const auto encoded = cobs::encode_inplace( { buffer.begin(), input_length }, buffer.size() );

        for ( const auto c: encoded )
            uart_.send( c );

        return encoded.size();
    }

    void SensorNetwork::fill_buffer()
    {
        // find start
        std::uint8_t in_byte = cobs::delimiter;
        while ( in_byte == cobs::delimiter && uart_.recv( &in_byte ) )
            ;

        if ( in_byte != cobs::delimiter )
        {
            receive_buffer_.push_back( in_byte );

            while ( uart_.recv( &in_byte ) && in_byte != cobs::delimiter )
                receive_buffer_.push_back( in_byte );
        }
    }

    int SensorNetwork::read(std::uint8_t* buffer, std::size_t buffer_length)
    {
        fill_buffer();

        const auto decoded = cobs::decode_inplace( { receive_buffer_.begin(), receive_buffer_.end() } );

        if ( decoded.size() > buffer_length )
            throw EXCEPTION("Receive Buffer too small: ", decoded.size() );

        std::copy( decoded.begin(), decoded.end(), buffer );
        receive_buffer_.clear();

        return decoded.size();
    }
}
