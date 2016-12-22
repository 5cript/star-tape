#pragma once

namespace StarTape
{
    class ITapeIoBase
    {
    public:
        virtual bool good() const = 0;
        virtual ~ITapeIoBase() = default;
    };
}
