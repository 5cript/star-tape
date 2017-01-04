#include "tape_archive.hpp"

#include <stdexcept>

namespace StarTape
{
//#####################################################################################################################
    ITapeReader* InputTapeArchive::getReader() const
    {
        return reader_;
    }
//---------------------------------------------------------------------------------------------------------------------
    void InputTapeArchive::open(ITapeReader* reader)
    {
        reader_ = reader;

        if (!reader_->good())
            throw std::runtime_error("reader not good");
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeIndex InputTapeArchive::makeIndex(std::ostream* dump)
    {
        return {this, dump};
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t InputTapeArchive::getChunkCount() const
    {
        if (reader_)
            return reader_->getChunkCount();
        return 0;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::ostream& InputTapeArchive::dump(std::ostream& stream) const
    {
        if (reader_ == nullptr)
            throw std::runtime_error("archive not openend");
        if (!reader_->good())
            throw std::runtime_error("reader is not good");

        char buffer[4096];
        uint64_t read = 0;
        do {
            read = reader_->read(buffer, 4096);
            stream.write(buffer, read);
        } while (read == 4096);
        return stream;
    }
//#####################################################################################################################
    ITapeWriter* OutputTapeArchive::getWriter() const
    {
        return writer_;
    }
//---------------------------------------------------------------------------------------------------------------------
    void OutputTapeArchive::open(ITapeWriter* writer)
    {
        writer_ = writer;

        if (!writer_->good())
            throw std::runtime_error("writer not good");
    }
//#####################################################################################################################
}
