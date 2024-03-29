#pragma once

#include <star-tape/tape_reader.hpp>
#include <star-tape/tape_writer.hpp>

#include <iostream>

namespace StarTape
{
    class StreamTapeReader : public ITapeReader
    {
    public:
        StreamTapeReader(std::istream* stream);

        void seekg(uint64_t position) override;
        char get() override;
        uint64_t read(char* string, uint64_t amount) override;
        uint64_t tellg() override;
        bool good() const override;
        uint64_t getChunkCount() override;
        bool canSeek() const override;

        std::istream& getStream();

    private:
        std::istream* istream_;
    };

    class StreamTapeWriter : public ITapeWriter
    {
    public:
        StreamTapeWriter(std::ostream* stream);

        void seekp(uint64_t position) override;
        void put(char c) override;
        uint64_t write(char const* string, uint64_t amount) override;
        uint64_t tellp() override;
        bool good() const override;
        void seekEnd() override;

    private:
        std::ostream* ostream_;
    };

    class StreamTape : public StreamTapeReader, public StreamTapeWriter
    {
    public:
        StreamTape(std::iostream* stream);
        bool good() const override;

    private:
        std::iostream* stream_;
    };
}
