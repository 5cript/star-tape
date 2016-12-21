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
//#####################################################################################################################
    TapeIndex::TapeIndex(TapeArchive* archive)
        : archive_{archive}
    {
        makeIndex();
    }
//---------------------------------------------------------------------------------------------------------------------
    void TapeIndex::makeIndex()
    {
        regions_.clear();

        auto chunkCount = archive_->getChunkCount();
        for (uint64_t currentChunk = 0u; currentChunk != chunkCount; ++currentChunk)
        {
            TapeEntry entry{archive_, currentChunk};
            auto fileSize = entry.getFileSize();
            auto endChunk = currentChunk + fileSize / 512u + !!(fileSize % 512u);
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
            currentChunk = endChunk;
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
    ITapeReader* TapeIndex::getContentReader(iterator entry)
    {
        auto* reader = archive_->getReader();
        if (reader == nullptr)
            throw std::runtime_error("the tar file is not opened for reading");

        reader->seekg((1 + entry->startChunk) * Constants::ChunkSize);
        return reader;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string TapeIndex::readFile(iterator entry)
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
    std::ostream& TapeIndex::writeFileToStream(iterator entry, std::ostream& stream)
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
//#####################################################################################################################
}
