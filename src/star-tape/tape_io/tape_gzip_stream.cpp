#include <star-tape/tape_io/tape_gzip_stream.hpp>

#if defined(ENABLE_GZIP) && ENABLE_GZIP == 1

#include <boost/iostreams/filter/gzip.hpp>

namespace StarTape
{
//#####################################################################################################################
    GzipStreamTapeReader::GzipStreamTapeReader(std::istream* stream)
        : reader_{}
    {
        reader_.push(boost::iostreams::gzip_decompressor());
        reader_.push(*stream);
    }
//---------------------------------------------------------------------------------------------------------------------
    void GzipStreamTapeReader::seekg(uint64_t position)
    {
        throw std::domain_error("cannot seek gzip stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    char GzipStreamTapeReader::get()
    {
        return reader_.get();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t GzipStreamTapeReader::read(char* string, uint64_t amount)
    {
        reader_.read(string, amount);
        return reader_.gcount();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t GzipStreamTapeReader::tellg()
    {
        throw std::domain_error("cannot seek gzip stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    boost::iostreams::filtering_istream& GzipStreamTapeReader::getStream()
    {
        return reader_;
    }
//---------------------------------------------------------------------------------------------------------------------
    bool GzipStreamTapeReader::good() const
    {
        return reader_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t GzipStreamTapeReader::getChunkCount()
    {
        throw std::domain_error("cannot determine chunk count");
    }
//---------------------------------------------------------------------------------------------------------------------
    bool GzipStreamTapeReader::canSeek() const
    {
        return false;
    }
//#####################################################################################################################
    GzipStreamTapeWriter::GzipStreamTapeWriter(std::ostream* stream)
        : writer_{}
    {
        writer_.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
        writer_.push(*stream);
    }
//---------------------------------------------------------------------------------------------------------------------
    void GzipStreamTapeWriter::seekp(uint64_t position)
    {
        throw std::domain_error("cannot seek gzip stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    void GzipStreamTapeWriter::put(char c)
    {
        writer_.put(c);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t GzipStreamTapeWriter::write(char const* string, uint64_t amount)
    {
        writer_.write(string, amount);
        return amount;
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t GzipStreamTapeWriter::tellp()
    {
        throw std::domain_error("cannot seek gzip stream");
    }
//---------------------------------------------------------------------------------------------------------------------
    bool GzipStreamTapeWriter::good() const
    {
        return writer_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    void GzipStreamTapeWriter::seekEnd()
    {
        throw std::domain_error("cannot seek gzip stream");
    }
//#####################################################################################################################
}

#endif
