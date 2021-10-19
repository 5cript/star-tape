#pragma once

#include <star-tape/tape_fwd.hpp>
#include <star-tape/tape_index.hpp>
#include <star-tape/tape_operation.hpp>

#include <iostream>
#include <string>
#include <vector>

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
        InputTapeArchive() = default;

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
         *  Discouraged for compressed archives.
         *
         *  @param dump A stream to write the archive to. Ignored for archive readers that support seeking.
         **/
        TapeIndex makeIndex(std::ostream* dump = nullptr);

        /**
         *  Copies entire archive into stream.
         **/
        std::ostream& dump(std::ostream& stream) const;

        InputTapeArchive(InputTapeArchive const&) = default;
        InputTapeArchive(InputTapeArchive&&) = default;
        InputTapeArchive& operator=(InputTapeArchive const&) = default;
        InputTapeArchive& operator=(InputTapeArchive&&) = default;
    private:
        ITapeReader* reader_;
    };

    class OutputTapeArchive : public TapeArchive
    {
    public:
        OutputTapeArchive() = default;

        /**
         *  May return nullptr, if not opened for writing.
         **/
        ITapeWriter* getWriter() const;

        /**
         *  Open Tape for writing.
         **/
        void open(ITapeWriter* writer);

        OutputTapeArchive(OutputTapeArchive const&) = default;
        OutputTapeArchive(OutputTapeArchive&&) = default;
        OutputTapeArchive& operator=(OutputTapeArchive const&) = default;
        OutputTapeArchive& operator=(OutputTapeArchive&&) = default;

    private:
        ITapeWriter* writer_;
    };
}
