#pragma once

#include "tape_archive.hpp"

#include "tape_io/tape_file.hpp"
#include "tape_io/tape_bzip2_stream.hpp"
#include "tape_io/tape_gzip_stream.hpp"
#include "tape_io/tape_stream.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <type_traits>

namespace StarTape
{
    template <typename ArchiveType, typename RWT, typename... List>
    class ArchiveDataBundle
    {
    private:
        std::tuple <List...> data_;
        std::shared_ptr <RWT> io_;
        ArchiveType archive_;

    public:
        ArchiveDataBundle(RWT&& io, ArchiveType&& archive, List&&... additionalData)
            : data_{std::move(additionalData)...}
            , io_{std::make_shared <RWT> (std::move(io))}
            , archive_{std::move(archive)}
        {
            archive_.open(io_.get());
        }

        template <typename I1>
        ArchiveDataBundle(ArchiveType&& archive, I1 /*dummy*/, List... additionalData)
            : data_{std::move(additionalData)...}
            , io_{std::make_shared <RWT> (&std::get <I1::value> (data_))}
            , archive_{std::move(archive)}
        {
            archive_.open(io_.get());
        }

        ArchiveType& getArchive()
        {
            return archive_;
        }

        ArchiveType const& getArchive() const
        {
            return archive_;
        }

        template <unsigned I>
        typename std::tuple_element <I, std::tuple <List...>>::type& getData() &
        {
            return std::get <I> (data_);
        }

        template <unsigned I>
        typename std::tuple_element <I, std::tuple <List...>>::type&& getData() const &&
        {
            return std::get <I> (data_);
        }

        ArchiveDataBundle(ArchiveDataBundle const&) = default;
        ArchiveDataBundle& operator=(ArchiveDataBundle const&) = default;

        ArchiveDataBundle(ArchiveDataBundle&&) = default;
        ArchiveDataBundle& operator=(ArchiveDataBundle&&) = default;
    };

    template <typename RWT, typename... List>
    using InputArchiveDataBundle = ArchiveDataBundle <InputTapeArchive, RWT, List...>;

    template <typename RWT, typename... List>
    using OutputArchiveDataBundle = ArchiveDataBundle <OutputTapeArchive, RWT, List...>;

    enum class CompressionType
    {
        None,
        Gzip,
        Bzip2
    };

    template <CompressionType>
    struct CompressionTypeToReader
    {
    };

    template <>
    struct CompressionTypeToReader <CompressionType::None>
    {
        using type = StreamTapeReader;
    };

    template <>
    struct CompressionTypeToReader <CompressionType::Gzip>
    {
        using type = GzipStreamTapeReader;
    };

    template <>
    struct CompressionTypeToReader <CompressionType::Bzip2>
    {
        using type = Bzip2StreamTapeReader;
    };

    template <CompressionType>
    struct CompressionTypeToWriter
    {
    };

    template <>
    struct CompressionTypeToWriter <CompressionType::None>
    {
        using type = StreamTapeWriter;
    };

    template <>
    struct CompressionTypeToWriter <CompressionType::Gzip>
    {
        using type = GzipStreamTapeWriter;
    };

    template <>
    struct CompressionTypeToWriter <CompressionType::Bzip2>
    {
        using type = Bzip2StreamTapeWriter;
    };

    template <typename T>
    struct ReadOrWriteTape
    {
    };

    template <>
    struct ReadOrWriteTape <InputTapeArchive>
    {
        using stream = StreamTapeReader;
        using gzip = GzipStreamTapeReader;
        using bzip2 = Bzip2StreamTapeReader;
    };

    template <>
    struct ReadOrWriteTape <OutputTapeArchive>
    {
        using stream = StreamTapeWriter;
        using gzip = GzipStreamTapeWriter;
        using bzip2 = Bzip2StreamTapeWriter;
    };

    /**
     *  Opens archive.
     **/
    template <CompressionType CT = CompressionType::None>
    InputArchiveDataBundle <typename CompressionTypeToReader<CT>::type, std::ifstream> openInputFile(std::string const& fileName)
    {
        using RWT = typename CompressionTypeToReader<CT>::type;

        std::ifstream reader{fileName, std::ios_base::binary};
        return InputArchiveDataBundle <RWT, std::ifstream> {
            {},
            std::integral_constant <unsigned, 0> {},
            std::move(reader)
        };
    }

    /**
     *  Copies an archive. Useful to decompress archives into RAM or onto disk.
     **/
    template <typename ArchiveType, typename RWT, typename StreamT>
    ArchiveDataBundle <ArchiveType, RWT, StreamT> copyArchive(InputTapeArchive const& archive, StreamT&& stream = StreamT{})
    {
        archive.dump(stream);
        return ArchiveDataBundle <ArchiveType, RWT, StreamT> {
            {},
            std::integral_constant <unsigned, 0> {},
            std::move(stream)
        };
    }

    /**
     *  Writes archive contents into stream.
     **/
    template <typename ArchiveBundleType>
    void extractArchive(ArchiveBundleType&& archive, std::ostream& stream)
    {
        archive.getArchive().dump(stream);
    }

    /**
     *  Writes archive contents into file.
     **/
    template <typename ArchiveBundleType>
    void extractArchive(ArchiveBundleType&& archive, std::string const& fileName)
    {
        std::ofstream writer(fileName, std::ios_base::binary);
        archive.getArchive().dump(writer);
    }

    /**
     *  Loads archive into RAM.
     **/
    template <typename ArchiveType>
    ArchiveDataBundle <ArchiveType, typename ReadOrWriteTape<ArchiveType>::stream, std::stringstream> loadArchive(InputTapeArchive const& archive)
    {
        return copyArchive <ArchiveType, typename ReadOrWriteTape<ArchiveType>::stream, std::stringstream> (archive);
    }

    /**
     *  Loads archive into RAM.
     **/
    template <typename ArchiveType, CompressionType CT = CompressionType::None>
    ArchiveDataBundle <ArchiveType, typename ReadOrWriteTape<ArchiveType>::stream, std::stringstream> loadArchive(std::string const& fileName)
    {
        return copyArchive <ArchiveType, typename ReadOrWriteTape<ArchiveType>::stream, std::stringstream> (openInputFile <CT> (fileName).getArchive(), std::stringstream{});
    }

    template <CompressionType CT = CompressionType::None, typename RWT, typename... List>
    void saveArchive(std::string const& fileName, InputArchiveDataBundle <RWT, List...>& oarch)
    {
        std::ofstream writer {fileName, std::ios_base::binary};
        copyArchive <OutputTapeArchive, typename CompressionTypeToWriter <CT>::type, std::ofstream> (oarch.getArchive(), std::move(writer));
    }

    template <typename ArchiveType, typename RWT, typename... List>
    ArchiveType archive(ArchiveDataBundle <ArchiveType, RWT, List...>& bundle)
    {
        return bundle.getArchive();
    }
}
