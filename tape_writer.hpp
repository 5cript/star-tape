#pragma once

#include "tape_io_base.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

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
         *  seeks to the end of the file.
         **/
        virtual void seekEnd() = 0;

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
        virtual uint64_t write(char const* string, uint64_t amount) = 0;

        /**
         *  Returns the cursor position.
         **/
        virtual uint64_t tellp() = 0;
    };

    ITapeWriter* operator<<(ITapeWriter* lhs, std::istream& stream);
    ITapeWriter* operator<<(ITapeWriter* lhs, std::string const& string);
}
