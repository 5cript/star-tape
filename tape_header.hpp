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

        StarHeader() = default;
	};

	struct PosixHeaderEntrySizes
	{
		constexpr static unsigned fileName = 100;
		constexpr static unsigned fileMode = 8;
		constexpr static unsigned uid = 8;
		constexpr static unsigned gid = 8;
		constexpr static unsigned size = 12;
		constexpr static unsigned mTime = 12;
		constexpr static unsigned chksum = 8;
		constexpr static unsigned typeflag = 1;
		constexpr static unsigned linkName = 100;
	};

	struct StarHeaderEntrySizes : public PosixHeaderEntrySizes
	{
		constexpr static unsigned magic = 6;
		constexpr static unsigned version = 2;
		constexpr static unsigned uName = 32;
		constexpr static unsigned gName = 32;
		constexpr static unsigned devMajor = 8;
		constexpr static unsigned devMinor = 8;
		constexpr static unsigned prefix = 155;
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
    StarHeader createHeaderFromString(std::string const& path, std::string const& dataString);
    StarHeader createHeader(std::string const& path, std::size_t size, bool checksum);

    std::string headerToString(StarHeader const& head);

    std::string concatFileName(StarHeader const& head);
}
