#pragma once

#include <array>

namespace StarTape
{
    namespace Constants
    {
        constexpr const char* const StarMagic = "ustar";  // null-terminated
        constexpr const char* const GnuMagic = "ustar  "; // no null

        enum class TypeFlags : char
        {
            RegularFileAlternate = '\0',
            RegularFile = '0',
            HardLink = '1',
            SymbolicLink = '2',
            CharacterSpecial = '3',
            BlockSpecial = '4',
            Directory = '5',
            FIFOSpecial = '6',
            ContiguousFile = '7',
            GlobalExtendedHeader = 'g',
            ExtendedHeader = 'x'
        };

        constexpr unsigned int MagicHeaderOffset = 257U;
        constexpr unsigned int SizeHeaderOffset = 124U;
        constexpr unsigned int FileNameHeaderOffset = 0U;
        constexpr unsigned int EntryTypeHeaderOffset = 156U;
        constexpr unsigned int PrefixHeaderOffset = 345u;
    }

    struct PosixHeader
    {
        std::array <char, 100> fileName;
        std::array <char, 8> fileMode;
        std::array <char, 8> uid;
        std::array <char, 8> gid;
        std::array <char, 12> size;
        std::array <char, 12> mTime;
        std::array <char, 8> chksum;
        char typeflag;
        std::array <char, 100> linkName;
    };

    struct StarHeader : PosixHeader
    {
        std::array <char, 6> magic;
        std::array <char, 2> version;
        std::array <char, 32> uName;
        std::array <char, 32> gName;
        std::array <char, 8> devMajor;
        std::array <char, 8> devMinor;
        std::array <char, 155> prefix;
    };

    struct Sparse
    {
        std::array <char, 12> offset;
        std::array <char, 12> numBytes;
    };

#   define TAPER_SPARSES_IN_EXTRA_HEADER  16
#   define TAPER_SPARSES_IN_OLDGNU_HEADER 4
#   define TAPER_SPARSES_IN_SPARSE_HEADER 21

    struct SparseHeader
    {
        std::array <Sparse, TAPER_SPARSES_IN_SPARSE_HEADER> sparses;
        char isExtended;
    };

    StarHeader createHeaderFromDiskNode(std::string path);

    std::string headerToString(StarHeader const& head);

    std::string concatFileName(StarHeader const& head);
}
