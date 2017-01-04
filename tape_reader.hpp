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
    class ITapeReader : virtual public ITapeIoBase
    {
    public:
        /**
         *  Moves the read cursor to the given position in bytes.
         **/
        virtual void seekg(uint64_t position) = 0;

        /**
         *  Reads a single character and advances the position by one.
         **/
        virtual char get() = 0;

        /**
         *  Tries to read "amount" bytes and writes them onto the string buffer.
         *  Advances the cursor position.
         *  Be aware, that if read does not return "amount" bytes, it'll indicate that no more can be read (eof, etc)!
         *
         *  @return The amount of actual bytes read.
         **/
        virtual uint64_t read(char* string, uint64_t amount) = 0;

        /**
         *  Returns the cursor position.
         **/
        virtual uint64_t tellg() = 0;

        /**
         *  Returns the amount of chunks contained in the file.
         **/
        virtual uint64_t getChunkCount() = 0;

        /**
         *  Compressed streams do not support seeking.
         **/
        virtual bool canSeek() const = 0;
    };
}
