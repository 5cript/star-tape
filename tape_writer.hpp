#pragma once

#include "tape_io_base.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace StarTape
{
    /**
     *  ITapeReader interface for classes that provide reading for a tarball.
     **/
    class ITapeWriter : virtual public ITapeIoBase
    {
    public:
        /**
         *  Moves the read cursor to the given position in bytes.
         **/
        virtual void seekp(uint64_t position) = 0;

        /**
         *  Puts a single character and advances the position by one.
         **/
        virtual void put(char c) = 0;

        /**
         *  Tries to write "amount" bytes and writes them onto the string buffer.
         *  Advances the cursor position.
         *
         *  @return The amount of actual bytes written.
         **/
        virtual uint64_t write(char* string, uint64_t amount) = 0;

        /**
         *  Returns the cursor position.
         **/
        virtual uint64_t tellp() const = 0;
    };
}
