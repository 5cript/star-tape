#pragma once

namespace StarTape
{
    enum class TarFormat
    {
        Gnu,
        UStar,
        Other
    };

    namespace Constants
    {
        constexpr unsigned int ChunkSize = 512U;
    }
}
