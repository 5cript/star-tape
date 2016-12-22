#include "tape_file.hpp"

namespace StarTape
{
//#####################################################################################################################
    FileTapeReader::FileTapeReader(std::string const& file)
        : file_{file, std::ios_base::binary}
        , streamTape_{&file_}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTapeReader::seekg(uint64_t position)
    {
        streamTape_.seekg(position);
    }
//---------------------------------------------------------------------------------------------------------------------
    char FileTapeReader::get()
    {
        return streamTape_.get();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTapeReader::read(char* string, uint64_t amount)
    {
        return streamTape_.read(string, amount);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTapeReader::tellg() const
    {
        return streamTape_.tellg();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool FileTapeReader::good() const
    {
        return streamTape_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTapeReader::getChunkCount()
    {
        return streamTape_.getChunkCount();
    }
//#####################################################################################################################
    FileTapeWriter::FileTapeWriter(std::string const& file)
        : file_{file, std::ios_base::binary}
        , streamTape_{&file_}
    {

    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTapeWriter::seekp(uint64_t position)
    {
        streamTape_.seekp(position);
    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTapeWriter::put(char c)
    {
        streamTape_.put(c);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTapeWriter::write(char const* string, uint64_t amount)
    {
        return streamTape_.write(string, amount);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTapeWriter::tellp() const
    {
        return streamTape_.tellp();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool FileTapeWriter::good() const
    {
        return streamTape_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTapeWriter::seekEnd()
    {
        return streamTape_.seekEnd();
    }
//#####################################################################################################################
    FileTape::FileTape(std::string const& file)
        : file_{file, std::ios_base::binary | std::ios_base::in | std::ios_base::out}
        , streamTape_{&file_}
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTape::seekp(uint64_t position)
    {
        streamTape_.seekp(position);
    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTape::put(char c)
    {
        streamTape_.put(c);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTape::write(char const* string, uint64_t amount)
    {
        return streamTape_.write(string, amount);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTape::tellp() const
    {
        return streamTape_.tellp();
    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTape::seekg(uint64_t position)
    {
        streamTape_.seekg(position);
    }
//---------------------------------------------------------------------------------------------------------------------
    char FileTape::get()
    {
        return streamTape_.get();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTape::read(char* string, uint64_t amount)
    {
        return streamTape_.read(string, amount);
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTape::tellg() const
    {
        return streamTape_.tellg();
    }
//---------------------------------------------------------------------------------------------------------------------
    bool FileTape::good() const
    {
        return streamTape_.good();
    }
//---------------------------------------------------------------------------------------------------------------------
    uint64_t FileTape::getChunkCount()
    {
        return streamTape_.getChunkCount();
    }
//---------------------------------------------------------------------------------------------------------------------
    void FileTape::seekEnd()
    {
        streamTape_.seekEnd();
    }
//#####################################################################################################################

}
