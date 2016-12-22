#include "tape_stream.hpp"

namespace StarTape
{
//#####################################################################################################################
    StreamTapeReader::StreamTapeReader(std::istream* stream)
        : istream_{stream}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamTapeReader::seekg(uint64_t position)
    {
        istream_->seekg(position);
    }
//---------------------------------------------------------------------------------------------------------------------
    char StreamTapeReader::get()
    {
        return istream_->get();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t StreamTapeReader::read(char* string, uint64_t amount)
    {
        istream_->read(string, amount);
        return istream_->gcount();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t StreamTapeReader::tellg() const
    {
        return istream_->tellg();
    }
//---------------------------------------------------------------------------------------------------------------------
    std::istream& StreamTapeReader::getStream()
    {
        return *istream_;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool StreamTapeReader::good() const
    {
        return istream_->good();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t StreamTapeReader::getChunkCount()
    {
        istream_->seekg(0u, std::ios::end);
        auto size = istream_->tellg();
        istream_->seekg(0u);
        return size / 512u;
    }
//#####################################################################################################################
    StreamTapeWriter::StreamTapeWriter(std::ostream* stream)
        : ostream_{stream}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamTapeWriter::seekp(uint64_t position)
    {
        ostream_->seekp(position);
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamTapeWriter::put(char c)
    {
        ostream_->put(c);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t StreamTapeWriter::write(char const* string, uint64_t amount)
    {
        ostream_->write(string, amount);
        return amount;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t StreamTapeWriter::tellp() const
    {
        return ostream_->tellp();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool StreamTapeWriter::good() const
    {
        return ostream_->good();
    }
//---------------------------------------------------------------------------------------------------------------------
    void StreamTapeWriter::seekEnd()
    {
        ostream_->seekp(0, std::ios_base::end);
    }
//#####################################################################################################################
    StreamTape::StreamTape(std::iostream* stream)
        : StreamTapeReader(stream)
        , StreamTapeWriter(stream)
        , stream_{stream}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    bool StreamTape::good() const
    {
        return stream_->good();
    }
//#####################################################################################################################
}
