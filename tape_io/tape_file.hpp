#pragma once

#include "../tape_reader.hpp"
#include "tape_stream.hpp"

#include <fstream>

namespace StarTape
{
    /**
     *  For further documentation, read up on ITapeReader.
     */
    class FileTapeReader : public ITapeReader
    {
    public:
        FileTapeReader(std::string const& file);

        void seekg(uint64_t position) override;
        char get() override;
        uint64_t read(char* string, uint64_t amount) override;
        uint64_t tellg() override;
        bool good() const override;
        uint64_t getChunkCount() override;
        bool canSeek() const override;

    private:
        std::ifstream file_;
        StreamTapeReader streamTape_;
    };

    class FileTapeWriter : public ITapeWriter
    {
    public:
        FileTapeWriter(std::string const& file);

        void seekp(uint64_t position) override;
        void put(char c) override;
        uint64_t write(char const* string, uint64_t amount) override;
        uint64_t tellp() override;
        bool good() const override;
        void seekEnd() override;

    private:
        std::ofstream file_;
        StreamTapeWriter streamTape_;
    };

    class FileTape : public ITapeReader, public ITapeWriter
    {
    public:
        FileTape(std::string const& file);

        void seekp(uint64_t position) override;
        void put(char c) override;
        uint64_t write(char const* string, uint64_t amount) override;
        uint64_t tellp() override;
        void seekg(uint64_t position) override;
        char get() override;
        uint64_t read(char* string, uint64_t amount) override;
        uint64_t tellg() override;
        bool good() const override;
        uint64_t getChunkCount() override;
        void seekEnd() override;

    private:
        std::fstream file_;
        StreamTape streamTape_;
    };
}
