#pragma once

#include <star-tape/tape_config.hpp>
#include <star-tape/tape_reader.hpp>
#include <star-tape/tape_writer.hpp>

#if defined(ENABLE_GZIP) && ENABLE_GZIP == 1

#include <boost/iostreams/filtering_stream.hpp>

#include <iostream>

namespace StarTape
{
    class GzipStreamTapeReader : public ITapeReader
    {
    public:
        GzipStreamTapeReader(std::istream* stream);

        void seekg(uint64_t position) override;
        char get() override;
        uint64_t read(char* string, uint64_t amount) override;
        uint64_t tellg() override;
        bool good() const override;
        uint64_t getChunkCount() override;
        bool canSeek() const override;

        boost::iostreams::filtering_istream& getStream();

    private:
        boost::iostreams::filtering_istream reader_;
    };

    class GzipStreamTapeWriter : public ITapeWriter
    {
    public:
        GzipStreamTapeWriter(std::ostream* stream);

        void seekp(uint64_t position) override;
        void put(char c) override;
        uint64_t write(char const* string, uint64_t amount) override;
        uint64_t tellp() override;
        bool good() const override;
        void seekEnd() override;

    private:
        boost::iostreams::filtering_ostream writer_;
    };
}

#endif
