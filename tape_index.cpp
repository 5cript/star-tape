#include "tape_index.hpp"

#include "tape_archive.hpp"
#include "tape_entry.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/range/as_array.hpp>

#include <boost/filesystem.hpp>

namespace StarTape
{
//#####################################################################################################################
    TapeRegion::TapeRegion(std::string fileName, Constants::TypeFlags type, uint64_t fileSize, uint64_t startChunk, uint64_t endChunk)
        : fileName(std::move(fileName))
        , type(type)
        , fileSize(fileSize)
        , startChunk(startChunk)
        , endChunk(endChunk)
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t TapeRegion::getChunkCount() const
    {
        return 1 + endChunk - startChunk;
    }
//#####################################################################################################################
    TapeIndex::TapeIndex(InputTapeArchive* archive, std::ostream* dump)
        : archive_{archive}
    {
        makeIndex(dump);
    }
//---------------------------------------------------------------------------------------------------------------------
    void TapeIndex::makeIndex(std::ostream* dump)
    {
        regions_.clear();

        if (archive_->getReader() == nullptr || !archive_->getReader()->good())
            throw std::runtime_error("archive not opened");

        auto insertRegion = [this](TapeEntry& entry, uint64_t currentChunk)
        {
            auto fileSize = entry.getFileSize();
            auto endChunk = currentChunk + fileSize / Constants::ChunkSize + !!(fileSize % Constants::ChunkSize);
            regions_.emplace_back (
                [&entry](){
                    std::string fname = entry.getFileName();
                    while (!fname.empty() && fname.back() == '\0')
                        fname.pop_back();
                    return fname;
                }(),
                entry.getEntryType(),
                fileSize,
                currentChunk,
                endChunk
            );
            return endChunk;
        };

        auto* reader = archive_->getReader();
        if (reader->canSeek())
        {
            ///
            /// CanSeek Implementation
            ///
            auto chunkCount = archive_->getChunkCount();
            for (unsigned currentChunk = 0u; currentChunk != chunkCount; ++currentChunk)
            {
                TapeEntry entry{archive_, currentChunk};
                currentChunk = insertRegion(entry, currentChunk);
            }
        }
        else
        {
            ///
            /// CanNotSeek Implementation
            ///
            char chunk[Constants::ChunkSize];
            uint64_t readAmount = 0ull;
            unsigned currentChunk = 0u;

            auto readChunk = [&readAmount, &chunk, &reader, dump]()
            {
                readAmount = reader->read(chunk, Constants::ChunkSize);
                if (dump)
                    dump->write(chunk, readAmount);
            };

            do {
                readChunk();
                if (readAmount == 0)
                    break;
                if (readAmount != Constants::ChunkSize)
                    throw std::runtime_error("tar file ends on incorrect chunk size");

                // must be EOF
                if (chunk[0] == '\0')
                    return;

                TapeEntry entry{archive_, chunk, currentChunk};
                auto endChunk = insertRegion(entry, currentChunk);

                // read rest of file
                while (currentChunk != endChunk)
                {
                    readChunk();
                    if (readAmount == 0)
                        break;
                    if (readAmount != Constants::ChunkSize)
                        throw std::runtime_error("tar file ends on incorrect chunk size");

                    currentChunk++;
                }
                currentChunk++;
            } while (readAmount == Constants::ChunkSize);

            // must be EOF
            auto checkChunkEmpty = [&chunk]()
            {
                for (auto const i : chunk)
                    if (i != '\0')
                        return false;
                return true;
            };

            if (!checkChunkEmpty())
                throw std::runtime_error("end of file marker expected");
            readChunk();
            if (!checkChunkEmpty())
                throw std::runtime_error("end of file marker expected");
            readChunk();
            if (readAmount != 0)
                throw std::runtime_error("end of file marker expected");
            else
                return;
        }
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <TapeRegion>::iterator TapeIndex::begin()
    {
        return regions_.begin();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <TapeRegion>::iterator TapeIndex::end()
    {
        return regions_.end();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <TapeRegion>::const_iterator TapeIndex::begin() const
    {
        return regions_.begin();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <TapeRegion>::const_iterator TapeIndex::end() const
    {
        return regions_.end();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <TapeRegion>::const_iterator TapeIndex::cbegin() const
    {
        return regions_.cbegin();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::vector <TapeRegion>::const_iterator TapeIndex::cend() const
    {
        return regions_.cend();
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeEntry TapeIndex::getEntry(iterator const& pos)
    {
        return TapeEntry{archive_, pos->startChunk, pos->endChunk};
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeIndex::iterator TapeIndex::find(std::string const& fileName)
    {
        return std::find_if(std::begin(regions_), std::end(regions_), [&fileName](TapeRegion const& region) {
            return region.fileName == fileName;
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeIndex::iterator TapeIndex::findFile(std::string const& fileName, boost::optional <iterator> start)
    {
        if (!start)
            start = std::begin(regions_);
        return std::find_if(start.get(), std::end(regions_), [&fileName](TapeRegion const& region) {
            return boost::filesystem::path(region.fileName).filename().string() == fileName;
        });
    }
//---------------------------------------------------------------------------------------------------------------------
    ITapeReader* TapeIndex::getContentReader(iterator const& entry)
    {
        auto* reader = archive_->getReader();
        if (reader == nullptr)
            throw std::runtime_error("the tar file is not opened for reading");

        reader->seekg((1 + entry->startChunk) * Constants::ChunkSize);
        return reader;
    }
//---------------------------------------------------------------------------------------------------------------------
    ITapeReader* TapeIndex::getHeaderReader(iterator const& entry)
    {
        auto* reader = archive_->getReader();
        if (reader == nullptr)
            throw std::runtime_error("the tar file is not opened for reading");

        reader->seekg(entry->startChunk * Constants::ChunkSize);
        return reader;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string TapeIndex::readFile(iterator const& entry)
    {
        auto reader = getContentReader(entry);
        if (reader == nullptr)
            throw std::runtime_error("the tar file is not opened for reading");

        std::string result;
        uint64_t remainingRead = entry->fileSize;
        do {
            uint64_t curRead = std::min(remainingRead, 4096ull);
            result.resize(result.size() + curRead);
            auto actualRead = reader->read(&*result.begin() + result.size() - curRead, curRead);
            if (actualRead == 0ull)
                break;
            remainingRead -= curRead;
        } while (remainingRead > 0ull);

        return result;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::ostream& TapeIndex::writeFileToStream(iterator const& entry, std::ostream& stream)
    {
        auto reader = getContentReader(entry);
        if (reader == nullptr)
            throw std::runtime_error("the tar file is not opened for reading");

        std::array <char, 4096> buffer;
        uint64_t remainingRead = entry->fileSize;
        do {
            uint64_t curRead = std::min(remainingRead, 4096ull);
            auto actualRead = reader->read(buffer.data(), curRead);
            if (actualRead == 0ull)
                break;
            remainingRead -= curRead;
            stream.write(buffer.data(), curRead);
        } while (remainingRead > 0ull);

        return stream;
    }
//---------------------------------------------------------------------------------------------------------------------
    TapeIndex::iterator TapeIndex::erase(iterator const& entry)
    {
        return regions_.erase(entry);
    }
//---------------------------------------------------------------------------------------------------------------------
    void TapeIndex::filter(std::function <bool(TapeRegion const&)> predicate)
    {
        regions_.erase(std::remove_if(std::begin(regions_), std::end(regions_), predicate), std::end(regions_));
    }
//---------------------------------------------------------------------------------------------------------------------
    InputTapeArchive* TapeIndex::getArchive() const
    {
        return archive_;
    }
//#####################################################################################################################
}
