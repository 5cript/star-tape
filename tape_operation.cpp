#include "tape_operation.hpp"
#include "tape_archive.hpp"
#include "tape_index.hpp"
#include "tape_reader.hpp"

#include <boost/filesystem.hpp>
#include <fstream>

namespace fs = boost::filesystem;

namespace StarTape { namespace TapeOperations
{
//#####################################################################################################################
    ITapeWriter* write(ITapeWriter* lhs, ITapeReader* rhs, uint64_t chunkCount)
    {
        char buffer[Constants::ChunkSize];
        for (uint64_t i = 0ull; i != chunkCount; ++i)
        {
            rhs->read(buffer, Constants::ChunkSize);
            lhs->write(buffer, Constants::ChunkSize);
        }
        return lhs;
    }
//#####################################################################################################################
    AddFile::AddFile(std::string fileName)
        : fileName_{std::move(fileName)}
        , pathRename_{}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    AddFile::AddFile(std::string fileName, std::string pathRename)
        : fileName_{std::move(fileName)}
        , pathRename_{std::move(pathRename)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool AddFile::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        auto header = createHeaderFromDiskNode(fileName_, pathRename_);

        if (fs::is_regular_file(fileName_))
        {
            std::ifstream file{fileName_, std::ios_base::binary};
            if (!file.good())
                throw std::invalid_argument ("could not open file");
            AddStream{&file, header}.apply(baseTape, destinationTape, ctx);
        }
        else
        {
            ctx->writer->write(headerToString(header).c_str(), Constants::ChunkSize);
        }
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int AddFile::getPrecedence() const
    {
        return -10;
    }
//#####################################################################################################################
    AddDirectory::AddDirectory(std::string directory)
        : directory_{std::move(directory)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool AddDirectory::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        fs::recursive_directory_iterator iter{directory_}, end;

        AddFile{directory_}.apply(baseTape, destinationTape, ctx);

        for (;iter != end; ++iter)
        {
            AddFile{iter->path().string()}.apply(baseTape, destinationTape, ctx);
        }
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int AddDirectory::getPrecedence() const
    {
        return -10;
    }
//#####################################################################################################################
    AddString::AddString(std::string data, StarHeader header)
        : data_{std::move(data)}
        , header_{std::move(header)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    AddString::AddString(std::string const& path, std::string data)
        : data_{std::move(data)}
        , header_{createHeaderFromString(path, data_)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool AddString::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        ctx->writer->write(headerToString(header_).c_str(), Constants::ChunkSize);
        ctx->writer << data_;
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int AddString::getPrecedence() const
    {
        return -10;
    }
//#####################################################################################################################
    bool Adopt::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        if (!baseTape->getArchive()->getReader()->canSeek())
            throw std::domain_error("cannot adopt from tape reader which does not support seek");

        for (auto i = std::begin(*baseTape); i != std::end(*baseTape); ++i)
        {
            auto* reader = baseTape->getHeaderReader(i);
            write(ctx->writer, reader, i->getChunkCount());
        }
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int Adopt::getPrecedence() const
    {
        return 0;
    }
//#####################################################################################################################
    AddStream::AddStream(std::istream* stream, StarHeader header)
        : stream_{stream}
        , header_{std::move(header)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool AddStream::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        // write header
        ctx->writer->write(headerToString(header_).c_str(), Constants::ChunkSize);

        ctx->writer << *stream_;

        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int AddStream::getPrecedence() const
    {
        return -10;
    }
//#####################################################################################################################
    RemoveEntry::RemoveEntry(std::string const& fileName)
        : fileName_{std::move(fileName)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool RemoveEntry::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        auto iter = baseTape->find(fileName_);
        if (iter == std::end(*baseTape))
            throw std::invalid_argument((std::string{"could not find file in archive to remove: "} + fileName_).c_str());
        baseTape->erase(iter);
        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int RemoveEntry::getPrecedence() const
    {
        return 10;
    }
//#####################################################################################################################
    RemoveDirectoryRecursive::RemoveDirectoryRecursive(std::string directory)
        : directory_{std::move(directory)}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    bool RemoveDirectoryRecursive::apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx)
    {
        auto pathContainsFile = [](fs::path dir, fs::path file) {
            // If dir ends with "/" and isn't the root directory, then the final
            // component returned by iterators will include "." and will interfere
            // with the std::equal check below, so we strip it before proceeding.
            if (dir.filename() == ".")
                dir.remove_filename();
            // We're also not interested in the file's name.
            assert(file.has_filename());
            file.remove_filename();

            // If dir has more components than file, then file can't possibly
            // reside in dir.
            auto dir_len = std::distance(dir.begin(), dir.end());
            auto file_len = std::distance(file.begin(), file.end());
            if (dir_len > file_len)
                return false;

            // This stops checking when it reaches dir.end(), so it's OK if file
            // has more directory components afterward. They won't be checked.
            return std::equal(dir.begin(), dir.end(), file.begin());
        };

        auto p = fs::path{directory_}.parent_path();
        baseTape->filter([&pathContainsFile, &p](TapeRegion const& region) {
            return pathContainsFile(p, fs::path(region.fileName));
        });

        return true;
    }
//---------------------------------------------------------------------------------------------------------------------
    int RemoveDirectoryRecursive::getPrecedence() const
    {
        return 10;
    }
//#####################################################################################################################
}
//#####################################################################################################################
    TapeWaterfall& TapeWaterfall::operator<<(TapeModifier&& operation)
    {
        operations_.push_back(std::unique_ptr <TapeModifier> (operation.clone()));
        return *this;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TapeWaterfall::apply(OutputTapeArchive* destinationTape, TapeIndex* baseTape, bool atEnd)
    {
        std::sort(std::begin(operations_), std::end(operations_), [](std::unique_ptr <TapeModifier> const& lhs, std::unique_ptr <TapeModifier> const &rhs){
            return lhs->getPrecedence() < rhs->getPrecedence();
        });

        TapeModificationContext ctx;
        ctx.writer = destinationTape->getWriter();
        if (atEnd)
        {
            ctx.writer->seekEnd();
            auto fileSize = ctx.writer->tellp();
            ctx.lastOccupiedChunk = fileSize / Constants::ChunkSize + !!(fileSize % Constants::ChunkSize);
            auto a = ctx.lastOccupiedChunk;
            auto b = fileSize;
        }

        for (auto const& operation : operations_)
        {
            operation->apply(baseTape, destinationTape, &ctx);
        }

        // END OF FILE
        char null[1024] = {0};
        ctx.writer->write(null, 1024);

        operations_.clear();
    }
//#####################################################################################################################
}
