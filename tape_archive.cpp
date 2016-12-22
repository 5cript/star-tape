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
    TapeIndex InputTapeArchive::makeIndex()
    {
        return {this};
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t InputTapeArchive::getChunkCount() const
    {
        if (reader_)
            return reader_->getChunkCount();
        return 0;
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
