#include "tape_archive.hpp"

#include <stdexcept>

namespace StarTape
{
//#####################################################################################################################
    ITapeReader* TapeArchive::getReader() const
    {
        return reader_;
    }
//---------------------------------------------------------------------------------------------------------------------
    ITapeWriter* TapeArchive::getWriter() const
    {
        return writer_;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TapeArchive::open(ITapeReader* reader, ITapeWriter* writer)
    {
        reader_ = reader;
        writer_ = writer;

        if (!reader_->good())
            throw std::runtime_error("reader not good");
        if (!writer_->good())
            throw std::runtime_error("writer not good");
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeIndex TapeArchive::makeIndex()
    {
        return {this};
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t TapeArchive::getChunkCount() const
    {
        if (reader_)
            return reader_->getChunkCount();
        return 0;
    }
//#####################################################################################################################
}
