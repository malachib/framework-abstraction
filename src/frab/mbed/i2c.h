#pragma once

#include <mbed.h>

// TODO: decide how we want to distinguish i2c master vs
// i2c slave classes (outside of tx classes)
// framework-abstracted gpio stuff, including pin_t
#include "../gpio.h" // not ready for direct-include just yet
#include "../i2c.h"  // yes this goes in a circle - just in case we include mbed/i2c.h directly

namespace framework_abstraction {

namespace layer2 {

// TODO: most mbed i2c operations handle address as part of the call
// either we brute force our way around it, or perhaps it makes more sense
// to adapt the other i2c frab code to be more address-oriented
// TODO: make a compile-time-queryable flag denoting whether tx modes or
// regular bulk modes are preferred
class i2c_tx
{
    mbed::I2C& native;

    inline void stop()
    {
        native.stop();
    }

public:
    i2c_tx(mbed::I2C& native) : native(native) 
    {
        start();
    }

    i2c_tx(i2c_tx&& move_from) : native(move_from.native)
    {
        
    }


    inline void start()
    {
        native.start();
    }

    inline void write(uint8_t data, bool expect_ack = true)
    {
        native.write(data);
    }

    // TODO: attempt to optimize this, but so far all bulk transfer commands
    // for mbed take address as a prepended parameter
    inline void write(const uint8_t* data, size_t length, bool expect_ack = true)
    {
        while(length--)
            write(*data++);
    }


    // EXPERIMENTAL
    inline void addr(uint8_t data, bool is_write_mode = true)
    {
        write((data << 1) | (is_write_mode ? 0 : 1));
    }

    void send()
    {
        // NOOP, since writes aren't queued
    }

    void commit()
    {
        send();
        stop();
    }
};

class i2c
{
    // note mbed::I2C contains virtual functions, so we might consider moving
    // all this to layer5
    mbed::I2C native;

public:
    // FIX: decide on naming whether we want config/start/begin etc.
    void config(uint32_t clock_speed)
    {
        native.frequency(clock_speed);
    }

    i2c(pin_t sda, pin_t scl) : native(sda, scl) {}

    i2c_tx get_tx_experimental()
    {
        i2c_tx tx(native);

        return tx;
    }

    struct traits
    {
        struct experimental
        {
            static constexpr bool is_tx_preferred() { return false; }
        };
    };


    // FIX: resolve method signature mismatch with esp-idf flavor, then upgrade
    // to non-experimental status
    inline i2c_experimental::acknowledge_code_t write_experimental(uint8_t addr, const uint8_t* data, size_t length, bool repeated = false)
    {
        int nack_received = native.write(addr << 1, reinterpret_cast<const char*>(data), length, repeated);

        return nack_received ? i2c_experimental::nack : i2c_experimental::ack;
    }
};

}

namespace layer1 {

template <pin_t sda, pin_t scl>
class i2c
{
    layer2::i2c layer2;

public:
    class tx : layer2::i2c_tx
    {
        
    };
};

}


}