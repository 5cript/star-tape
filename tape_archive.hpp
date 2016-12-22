#pragma once

#include "tape_fwd.hpp"
#include "tape_io.hpp"
#include "tape_index.hpp"
#include "tape_operation.hpp"

namespace StarTape
{
    class TapeArchive
    {
    public:
        virtual ~TapeArchive() = default;
    };

    class InputTapeArchive : public TapeArchive
    {
    public:
        /**
         *  May return nullptr, if not opened for reading.
         **/
        ITapeReader* getReader() const;

        /**
         *  Open Tape for reading.
         **/
        void open(ITapeReader* reader);

        /**
         *  Returns the amount of chunks of the file.
         **/
        uint64_t getChunkCount() const;

        /**
         *  Make an index for the tape.
         **/
        TapeIndex makeIndex();
    private:
        ITapeReader* reader_;
    };

    class OutputTapeArchive : public TapeArchive
    {
    public:
        /**
         *  May return nullptr, if not opened for writing.
         **/
        ITapeWriter* getWriter() const;

        /**
         *  Open Tape for writing.
         **/
        void open(ITapeWriter* writer);

    private:
        ITapeWriter* writer_;
    };
}
