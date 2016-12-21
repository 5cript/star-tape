#pragma once

#include "tape_fwd.hpp"
#include "tape_io.hpp"
#include "tape_index.hpp"

namespace StarTape
{
    class TapeArchive
    {
    public:
        /**
         *  May return nullptr, if not opened for writing.
         **/
        ITapeWriter* getWriter() const;

        /**
         *  May return nullptr, if not opened for reading.
         **/
        ITapeReader* getReader() const;

        /**
         *  Returns the amount of chunks of the file.
         **/
        uint64_t getChunkCount() const;

        /**
         *  Make an index for the tape.
         **/
        TapeIndex makeIndex();

        void open(ITapeReader* reader, ITapeWriter* writer);

    private:
        ITapeReader* reader_;
        ITapeWriter* writer_;
    };
}
