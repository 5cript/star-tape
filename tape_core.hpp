#pragma once

namespace StarTape
{
    enum class TarFormat
    {
        Gnu,
        UStar
    };

    namespace Constants
    {
        constexpr unsigned int ChunkSize = 512U;
    }
}
