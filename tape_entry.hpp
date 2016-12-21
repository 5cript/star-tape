#pragma once

#include "tape_core.hpp"
#include "tape_fwd.hpp"
#include "tape_header.hpp"
#include "tape_io.hpp"
#include "tape_archive.hpp"

#include <cstdint>
#include <stdexcept>
#include <boost/optional.hpp>

namespace StarTape
{
    class TapeEntry
    {
    public:
        TapeEntry(TapeArchive* archive, uint64_t startChunk, uint64_t endChunk = 0 /* redundant for crawler */, bool readHeader = false);

        /**
         *  Reads the header of the file and returns it.
         *  The header is not parsed prior to calling this function, for performance reasons.
         **/
        StarHeader* getHeader();

        /**
         *  Returns the file header format.
         *  This does not read the entire header, but will use an already read header, if getHeader was called prior.
         *  This does perform any checks for ustar format. Only ustar is supported
         **/
        TarFormat getFormat() const;

        /**
         *  Returns the start chunk.
         **/
        uint64_t getStartChunk() const;

        /**
         *  Returns the last chunk.
         **/
        uint64_t getEndChunk() const;

        /**
         *  Returns the size of the file.
         *  This does not read the entire header, but will use an already read header, if getHeader was called prior.
         *  This does perform any checks for ustar format. Only ustar is supported
         **/
        uint64_t getFileSize() const;

        /**
         *  Returns the file name.
         *  This does not read the entire header, but will use an already read header, if getHeader was called prior.
         *  This does perform any checks for ustar format. Only ustar is supported
         **/
        std::string getFileName() const;

        /**
         *  Returns the type of the entry/file.
         *  This does not read the entire header, but will use an already read header, if getHeader was called prior.
         *  This does perform any checks for ustar format. Only ustar is supported
         **/
        Constants::TypeFlags getEntryType() const;

    private:
        TapeArchive* archive_;
        uint64_t startChunk_; // including header
        uint64_t endChunk_;
        boost::optional <StarHeader> header_;

    private: // internal templated functions
        template <unsigned offset, unsigned amount>
        void readHeaderEntry(std::array <char, amount>& arr) const
        {
            auto* reader = archive_->getReader();
            if (reader == nullptr)
                throw std::runtime_error("tar not opened for read access");

            reader->seekg(Constants::ChunkSize * startChunk_ + offset);
            read <amount>(reader, arr);
        }

        template <unsigned offset>
        void readHeaderEntry(char& c) const
        {
            auto* reader = archive_->getReader();
            if (reader == nullptr)
                throw std::runtime_error("tar not opened for read access");

            reader->seekg(Constants::ChunkSize * startChunk_ + offset);
            c = reader->get();
        }

        // reader will not be checked here!
        template <unsigned amount>
        void read(ITapeReader* reader, std::array <char, amount>& arr) const
        {
            auto readAmount = reader->read(arr.data(), amount);
            if (readAmount != amount)
                throw std::runtime_error("header unexpectedly incomplete");
        }

        void read(ITapeReader* reader, char& byte) const
        {
            byte = reader->get();
        }
    };
}
