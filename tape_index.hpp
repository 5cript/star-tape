#pragma once

#include "tape_core.hpp"
#include "tape_fwd.hpp"
#include "tape_header.hpp"
#include "tape_reader.hpp"

#include <vector>
#include <string>
#include <functional>
#include <boost/optional.hpp>

namespace StarTape
{
    struct TapeRegion
    {
        std::string fileName;
        Constants::TypeFlags type;
        uint64_t fileSize;

        uint64_t startChunk;
        uint64_t endChunk;

        uint64_t getChunkCount() const;

        TapeRegion(std::string fileName, Constants::TypeFlags type, uint64_t fileSize, uint64_t startChunk, uint64_t endChunk);
    };

    class TapeIndex
    {
    public:
        using iterator = std::vector <TapeRegion>::iterator;
        using const_iterator = std::vector <TapeRegion>::const_iterator;

    public:
        TapeIndex(InputTapeArchive* archive);

        operator std::vector <TapeRegion>&()
        {
            return regions_;
        }

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;

        TapeEntry getEntry(iterator const& pos);

        /**
         *  finds entry on tape by exact fileName match (trailing NULLs ignored).
         *  TODO: improve
         **/
        iterator find(std::string const& fileName);

        /**
         *  finds entry on tape by file name (find "text.txt" in ./bla/text.txt will match) (trailing NULLs ignored).
         **/
        iterator findFile(std::string const& fileName, boost::optional <iterator> start = boost::none);

        /**
         *  removes all entries from the index, that match the predicate.
         **/
        void filter(std::function <bool(TapeRegion const&)> predicate);

        /**
         *  Returns a tape reader for the exact entry on the tape, located on the file content start.
         **/
        ITapeReader* getContentReader(iterator const& entry);

        /**
         *  Returns a tape reader for the exact entry on the tape, located on the header.
         **/
        ITapeReader* getHeaderReader(iterator const& entry);

        /**
         *  Reads the entire file into a string.
         */
        std::string readFile(iterator const& entry);

        /**
         *  Reads the entire file into a stream.
         */
        std::ostream& writeFileToStream(iterator const& entry, std::ostream& stream);

        /**
         *  Removes the entry from the index.
         */
        iterator erase(iterator const& entry);

    private:
        void makeIndex();

    private:
        InputTapeArchive* archive_;
        std::vector <TapeRegion> regions_;
    };
}
