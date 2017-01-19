#pragma once

#include "tape_core.hpp"
#include "tape_header.hpp"
#include "tape_fwd.hpp"
#include "tape_writer.hpp"

#include <string>
#include <vector>
#include <memory>

namespace StarTape
{
    struct TapeModificationContext
    {
        uint64_t lastOccupiedChunk;
        ITapeWriter* writer;
    };

    struct TapeModifier
    {
        virtual ~TapeModifier() = default;
        virtual bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) = 0;
        virtual TapeModifier* clone() const = 0;
        /**
         *  High = has to be done first
         *  Low = has to be done last
         *  0 is considered neutral / dont care
         *
         *  Operations are ordered before execution using this precedence.
         **/
        virtual int getPrecedence() const { return 0; };
    };

    template <typename DerivT>
    struct TapeModifierCloneable : public TapeModifier
    {
        virtual TapeModifier* clone() const
        {
            return new DerivT(static_cast <DerivT const&> (*this));
        }
    };

    namespace TapeOperations
    {
        class AddFile : public TapeModifierCloneable <AddFile>
        {
        public:
            AddFile(std::string fileName);
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;

        private:
            std::string fileName_;
        };

        class AddDirectory : public TapeModifierCloneable <AddDirectory>
        {
        public:
            AddDirectory(std::string directory);
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;

        private:
            std::string directory_;
        };

        class AddString : public TapeModifierCloneable <AddString>
        {
        public:
            AddString(std::string data, StarHeader header);
            AddString(std::string const& path, std::string data);
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;

        private:
            std::string data_;
            StarHeader header_;
        };

        class AddStream : public TapeModifierCloneable <AddStream>
        {
        public:
            AddStream(std::istream* stream, StarHeader header);
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;

        private:
            std::istream* stream_;
            StarHeader header_;
        };

        /**
         *  Clones all from other tape (over TapeIndex)
         **/
        class Adopt : public TapeModifierCloneable <Adopt>
        {
        public:
            Adopt() = default;
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;
        };

        /**
         *  Warning! If you remove a directory, subordinate files and directories will not be removed!
         **/
        class RemoveEntry : public TapeModifierCloneable <RemoveEntry>
        {
        public:
            RemoveEntry(std::string const& fileName);
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;

        private:
            std::string fileName_;
        };

        /**
         *  Removes a directory and all subordinate entries, assuming their paths are normalized or identical
         *  in representation. (./bla/x and /home/someone/bla/x are not considered identical, the current working
         *  set directory is not taken into account).
         **/
        class RemoveDirectoryRecursive : public TapeModifierCloneable <RemoveDirectoryRecursive>
        {
        public:
            RemoveDirectoryRecursive(std::string directory);
            bool apply(TapeIndex* baseTape, OutputTapeArchive* destinationTape, TapeModificationContext* ctx) override;
            int getPrecedence() const override;

        private:
            std::string directory_;
        };
    }

    class TapeWaterfall
    {
    public:
        TapeWaterfall() = default;

        TapeWaterfall& operator<<(TapeModifier&& operation);
        void apply(OutputTapeArchive* destinationTape, TapeIndex* baseTape = nullptr, bool atEnd = false);

    private:
        std::vector <std::unique_ptr <TapeModifier>> operations_;
    };
}
