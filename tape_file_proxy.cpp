#include "tape_file_proxy.hpp"
#include "tape_operation.hpp"

namespace StarTape
{
//#####################################################################################################################
    StringProxy::StringProxy(std::string path, std::string data)
        : FileProxy(std::move(path), std::move(data))
    {
    }
//---------------------------------------------------------------------------------------------------------------------
    void StringProxy::serialize(OutputTapeArchive* archive) const
    {
        using namespace TapeOperations;
        (TapeWaterfall{}
            << AddString(name, data)
        ).apply(nullptr, archive);
    }
//#####################################################################################################################
    OutputTapeArchive& operator<<(OutputTapeArchive& archive, StringProxy const& proxy)
    {
        proxy.serialize(&archive);
    }
//#####################################################################################################################
}
