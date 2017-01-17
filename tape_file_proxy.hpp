#pragma once

#include "tape_archive.hpp"

namespace StarTape
{
    template <typename DataT>
    struct FileProxy
    {
        std::string name;
        DataT data;

        FileProxy(std::string&& name, DataT&& data)
            : name{std::move(name)}
            , data{std::move(data)}
        {}
    };

    struct StringProxy : FileProxy <std::string>
    {
        StringProxy(std::string path, std::string data);

        void serialize(OutputTapeArchive* archive) const;
    };

    OutputTapeArchive& operator<<(OutputTapeArchive& archive, StringProxy const& proxy);
}
