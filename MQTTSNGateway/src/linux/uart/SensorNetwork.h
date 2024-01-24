#ifndef SENSORNETWORK_H_
#define SENSORNETWORK_H_

#include <string>
#include <cstdint>
#include <cstddef>

#include "SerialPort.h"

namespace MQTTSNGW {

    class SensorNetAddress
    {
    public:
        SensorNetAddress();
        explicit SensorNetAddress( const std::string& uart );

        template < std::size_t S >
        const char* sprint( char ( &buffer )[ S ] ) const
        {
            const std::size_t copy_size = std::min( S, address_.size() );
            std::copy( address_.begin(), std::next( address_.begin(), copy_size ), std::begin( buffer ) );

            buffer[ S - 1 ] = 0;

            return std::begin( buffer );
        }

        bool isMatch(const SensorNetAddress* rhs) const;
        bool setAddress(const std::string* new_address);

    private:
        std::string address_;
    };

    class SensorNetwork
    {
    public:
        void initialize();
        const char* getDescription() const;
        const SensorNetAddress* getSenderAddress() const;

        int unicast(const std::uint8_t* buffer, std::size_t buffer_length, const SensorNetAddress* sendTo);
        int broadcast(const std::uint8_t* buffer, std::size_t buffer_length);
        int read(std::uint8_t* buffer, std::size_t buffer_length);

    private:
        SensorNetAddress sender_addr_;
        std::string description_;
        SerialPort  uart_;
    };
}

#endif
