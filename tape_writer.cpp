#include "tape_writer.hpp"

#include <array>

namespace StarTape
{
//#####################################################################################################################
    void fill(ITapeWriter* lhs, uint64_t written)
    {
        auto trail = 512 - (written%512);
        while (trail--)
            lhs->put('\0');
    }
//#####################################################################################################################
    ITapeWriter* operator<<(ITapeWriter* lhs, std::istream& stream)
    {
        char buffer[4096] = {0};
        int read = 0;
        uint64_t totalWritten = 0u;
        do {
            stream.read(buffer, 4096);
            read = stream.gcount();
            if (read == 0 || read == -1)
                return lhs;
            lhs->write(buffer, read);
            totalWritten += read;
        } while (read == 4096);

        fill(lhs, totalWritten);

        return lhs;
    }
//---------------------------------------------------------------------------------------------------------------------
    ITapeWriter* operator<<(ITapeWriter* lhs, std::string const& string)
    {
        lhs->write(string.c_str(), string.length());

        fill(lhs, string.length());
    }
//#####################################################################################################################
}
