#include "tape_entry.hpp"

#include "tape_utility.hpp"

#include <algorithm>

namespace StarTape
{
//#####################################################################################################################
    TapeEntry::TapeEntry(InputTapeArchive* archive, uint64_t startChunk, uint64_t endChunk, bool readHeader)
        : archive_{archive}
        , startChunk_{startChunk}
        , endChunk_{endChunk}
    {
        if (!archive->getReader()->canSeek())
            throw std::domain_error("cannot seek on archive reader.");

        if (readHeader)
            getHeader();
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeEntry::TapeEntry(InputTapeArchive* archive, char const* buffer, uint64_t startChunk, uint64_t endChunk)
        : archive_{archive}
        , startChunk_{startChunk}
        , endChunk_{endChunk}
    {
        auto readPart = [&buffer](unsigned size, char* out)
        {
            std::copy(buffer, buffer + size, out);
            buffer += size;
        };

        StarHeader head;

		#define READ(NAME) readPart(StarHeaderEntrySizes::NAME, &head.NAME.front())
        READ(fileName);
        READ(fileMode);
        READ(uid);
        READ(gid);
        READ(size);
        READ(mTime);
        READ(chksum);
        readPart(1, &head.typeflag);
        READ(linkName);
        READ(magic);
        READ(version);
        READ(uName);
        READ(gName);
        READ(devMajor);
        READ(devMinor);
        READ(prefix);
        #undef READ

        header_ = head;

        if (getFormat() != TarFormat::UStar)
            throw std::runtime_error("only ustar format is supported");
    }
//---------------------------------------------------------------------------------------------------------------------
    StarHeader* TapeEntry::getHeader()
    {
        if (header_)
            return &header_.get();

        auto* reader = archive_->getReader();
        if (reader == nullptr)
            throw std::runtime_error("tar not opened for read access");

        // validate header format first:
        if (getFormat() != TarFormat::UStar)
            throw std::runtime_error("only ustar format is supported");

        reader->seekg(Constants::ChunkSize * startChunk_);

        StarHeader head;

        #define READ(NAME) read <StarHeaderEntrySizes::NAME>(reader, head.NAME)
        READ(fileName);
        READ(fileMode);
        READ(uid);
        READ(gid);
        READ(size);
        READ(mTime);
        READ(chksum);
        read(reader, head.typeflag);
        READ(linkName);
        READ(magic);
        READ(version);
        READ(uName);
        READ(gName);
        READ(devMajor);
        READ(devMinor);
        READ(prefix);
        #undef READ

        header_ = head;

        return nullptr;
    }
//---------------------------------------------------------------------------------------------------------------------
    TarFormat TapeEntry::getFormat() const
    {
        std::array <char, 6> magic;
        if (header_)
            magic = header_.get().magic;
        else
            readHeaderEntry <Constants::MagicHeaderOffset, 6> (magic);

        if (magic[0] != 'u' || magic[1] != 's' || magic[2] != 't' || magic[3] != 'a' || magic[4] != 'r')
            return TarFormat::Other;
        return magic[5] == ' ' ? TarFormat::Gnu : TarFormat::UStar;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t TapeEntry::getStartChunk() const
    {
        return startChunk_;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t TapeEntry::getEndChunk() const
    {
        return endChunk_;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t TapeEntry::getFileSize() const
    {
        std::array <char, 12> size;
        if (header_)
            size = header_.get().size;
        else
            readHeaderEntry <Constants::SizeHeaderOffset, 12> (size);

        return parseOctal(std::begin(size), std::end(size));
    }
//---------------------------------------------------------------------------------------------------------------------}
    std::string TapeEntry::getFileName() const
    {
        if (header_)
            return concatFileName(header_.get());
        else
        {
            StarHeader tempHead;
			readHeaderEntry <Constants::PrefixHeaderOffset, StarHeaderEntrySizes::prefix> (tempHead.prefix);
            readHeaderEntry <Constants::FileNameHeaderOffset, StarHeaderEntrySizes::fileName> (tempHead.fileName);
            return concatFileName(tempHead);
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    Constants::TypeFlags TapeEntry::getEntryType() const
    {
        char type;
        if (header_)
            type = header_.get().typeflag;
        else
            readHeaderEntry <Constants::EntryTypeHeaderOffset> (type);

        return static_cast <Constants::TypeFlags> (type);
    }
//#####################################################################################################################
}
