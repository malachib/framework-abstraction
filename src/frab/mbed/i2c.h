#pragma once

#include <mbed.h>

// framework-abstracted gpio stuff, including pin_t
//#include "gpio.h" // not ready for direct-include just yet

namespace framework_abstraction {

namespace layer1 {

}

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
    i2c_tx(mbed::I2C& native) : native(native) {}

    i2c_tx(i2c_tx&& move_from) : native(move_from.native)
    {
        
    }


    inline void start()
    {
        native.start();
    }

    inline void write(uint8_t data)
    {
        native.write(data);
    }

    // TODO: attempt to optimize this, but so far all bulk transfer commands
    // for mbed take address as a prepended parameter
    inline void write(const uint8_t* data, size_t length)
    {
        while(length--)
            write(*data++);
    }


    // EXPERIMENTAL
    inline void addr(uint8_t data, bool is_write_mode = true)
    {
        write((data << 1) | (is_write_mode ? 0 : 1));
    }
};

class i2c
{
    // note mbed::I2C contains virtual functions, so we might consider moving
    // all this to layer5
    mbed::I2C native;

    // FIX: do this instead by pulling in gpio.h, when we can
    typedef PinName pin_t;

public:
    i2c(pin_t sda, pin_t scl) : native(sda, scl) {}

    i2c_tx get_tx_experimental()
    {
        i2c_tx tx(native);

        return tx;
    }
};

}

}