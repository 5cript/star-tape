#pragma once

#include "../tape_reader.hpp"
#include "../tape_writer.hpp"

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
        uint64_t tellg() const override;
        bool good() const override;
        uint64_t getChunkCount() override;

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
        uint64_t write(char* string, uint64_t amount) override;
        uint64_t tellp() const override;
        bool good() const override;

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
