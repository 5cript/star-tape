#pragma once

#include "tape_reader.hpp"
#include "tape_writer.hpp"

namespace StarTape
{
    class ITape : public ITapeReader, public ITapeWriter
    {
    };
}
