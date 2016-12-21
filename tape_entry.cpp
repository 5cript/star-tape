#include "tape_entry.hpp"

#include "tape_utility.hpp"

namespace StarTape
{
//#####################################################################################################################
    TapeEntry::TapeEntry(TapeArchive* archive, uint64_t startChunk, uint64_t endChunk, bool readHeader)
        : archive_{archive}
        , startChunk_{startChunk}
        , endChunk_{endChunk}
    {
        if (readHeader)
            getHeader(); // called for side effects
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
            throw std::runtime_error("gnu tar format is not supported");

        reader->seekg(Constants::ChunkSize * startChunk_);

        StarHeader head;

        #define READ(NAME) read <head.NAME.size()>(reader, head.NAME)
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
        std::array <char, 100> fname;
        if (header_)
            fname = header_.get().fileName;
        else
            readHeaderEntry <Constants::FileNameHeaderOffset, 100> (fname);

        return {std::begin(fname), std::end(fname)};
    }
//---------------------------------------------------------------------------------------------------------------------
    Constants::TypeFlags TapeEntry::getEntryType() const
    {
        char type;
        if (header_)
            type = header_.get().typeflag;
        else
            readHeaderEntry <Constants::EntryTypeOffset> (type);

        return static_cast <Constants::TypeFlags> (type);
    }
//#####################################################################################################################
}
