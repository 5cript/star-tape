#include "tape_bzip2_stream.hpp"
#if defined(ENABLE_BZIP2) && ENABLE_BZIP2 == 1

#include <boost/iostreams/filter/bzip2.hpp>

namespace StarTape
{
//#####################################################################################################################
    Bzip2StreamTapeReader::Bzip2StreamTapeReader(std::istream* stream)
        : reader_{}
    {
        reader_.push(boost::iostreams::bzip2_decompressor());
        reader_.push(*stream);
    }
//---------------------------------------------------------------------------------------------------------------------
    void Bzip2StreamTapeReader::seekg(uint64_t position)
    {
        throw std::domain_error("cannot seek bzip2 stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    char Bzip2StreamTapeReader::get()
    {
        return reader_.get();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t Bzip2StreamTapeReader::read(char* string, uint64_t amount)
    {
        reader_.read(string, amount);
        return reader_.gcount();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t Bzip2StreamTapeReader::tellg()
    {
        throw std::domain_error("cannot seek bzip2 stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    boost::iostreams::filtering_istream& Bzip2StreamTapeReader::getStream()
    {
        return reader_;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Bzip2StreamTapeReader::good() const
    {
        return reader_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t Bzip2StreamTapeReader::getChunkCount()
    {
        throw std::domain_error("cannot determine chunk count");
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Bzip2StreamTapeReader::canSeek() const
    {
        return false;
    }
//#####################################################################################################################
    Bzip2StreamTapeWriter::Bzip2StreamTapeWriter(std::ostream* stream)
        : writer_{}
    {
        writer_.push(boost::iostreams::bzip2_compressor(boost::iostreams::bzip2_params()));
        writer_.push(*stream);
    }
//---------------------------------------------------------------------------------------------------------------------
    void Bzip2StreamTapeWriter::seekp(uint64_t position)
    {
        throw std::domain_error("cannot seek bzip2 stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    void Bzip2StreamTapeWriter::put(char c)
    {
        writer_.put(c);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t Bzip2StreamTapeWriter::write(char const* string, uint64_t amount)
    {
        writer_.write(string, amount);
        return amount;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t Bzip2StreamTapeWriter::tellp()
    {
        throw std::domain_error("cannot seek bzip2 stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    bool Bzip2StreamTapeWriter::good() const
    {
        return writer_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    void Bzip2StreamTapeWriter::seekEnd()
    {
        throw std::domain_error("cannot seek bzip2 stream");
    }
//#####################################################################################################################
}
#endif
