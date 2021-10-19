#include <star-tape/tape_core.hpp>
#include <star-tape/tape_header.hpp>
#include <star-tape/tape_utility.hpp>

#include <boost/filesystem.hpp>
#include <chrono>

namespace fs = boost::filesystem;

namespace StarTape
{
//#####################################################################################################################
    void preprocessPath(std::string& path)
    {
        std::replace(std::begin(path), std::end(path), '\\', '/');
        path.erase(std::unique(std::begin(path), std::end(path), [](char lhs, char rhs) {return lhs == '/' && rhs == '/';}), std::end(path));
    }
//---------------------------------------------------------------------------------------------------------------------
    template <std::size_t Size>
    void assign(std::array <char, Size>& arr, std::string const& str)
    {
        for (unsigned i = 0u; i != std::min(Size, str.length()); ++i)
        {
            arr[i] = str[i];
        }
        if (str.length() < Size)
            for (unsigned i = str.length(); i != Size; ++i)
            {
                arr[i] = '\0';
            }
    }
//---------------------------------------------------------------------------------------------------------------------
    template <std::size_t Size>
    void assignOctal(std::array <char, Size>& arr, uint64_t num)
    {
        assign <Size>(arr, makeOctal <Size-1> (num));
    }
//---------------------------------------------------------------------------------------------------------------------
    template <std::size_t Size>
    void clear(std::array <char, Size>& arr)
    {
        std::fill(std::begin(arr), std::end(arr), '\0');
    }
//#####################################################################################################################
    #define HEADER_ASSIGN(FIELD, STRING) \
	assign <StarHeaderEntrySizes::FIELD>(header.FIELD, STRING)

    #define HEADER_ASSIGN_OCTAL(FIELD, STRING) \
	assignOctal <StarHeaderEntrySizes::FIELD> (header.FIELD, STRING)

    #define HEADER_CLEAR(FIELD) \
    clear <StarHeaderEntrySizes::FIELD> (header.FIELD)
//---------------------------------------------------------------------------------------------------------------------
    void calculateChecksum(StarHeader& header)
    {
        uint32_t checksum = 0u;

        std::array <char, 7> sum;

        #define CHKSUM(FIELD) \
        for (auto const& i : header.FIELD) \
            checksum += static_cast <unsigned char> (i);

        CHKSUM(fileName)
        CHKSUM(fileMode)
        CHKSUM(uid)
        CHKSUM(gid)
        CHKSUM(size)
        CHKSUM(mTime)
        CHKSUM(linkName)
        CHKSUM(magic)
        CHKSUM(version)
        CHKSUM(uName)
        CHKSUM(gName)
        CHKSUM(devMajor)
        CHKSUM(devMinor)
        CHKSUM(prefix)

        checksum += 8u * 32u;
        checksum += static_cast <unsigned char> (header.typeflag);

        assignOctal <7> (sum, checksum);
        for (int i = 0; i != 7; ++i)
            header.chksum[i] = sum[i];
        header.chksum[7] = ' ';

        #undef CHKSUM
    }
//---------------------------------------------------------------------------------------------------------------------
    fs::path splitPath(StarHeader& header, std::string path, bool isDir = false)
    {
        preprocessPath(path);

        fs::path p{path};

        if (path.length() > 255)
            throw std::invalid_argument("path size is too large (>255)");

        // path is too long, try to split:
        if (path.length() > 100)
        {
            // 2. rule (might be false): other than that, put as much into the prefix as possible
            // 3. rule: directories end with slash, but not in the prefix, because that is implicit.
            // ?. rule (likely false): files that are in subdirs have a dir+slash in fileName, if possible.

            // first count if its actually possible to split based on amount of dirs.
            auto SlashCount = std::count_if (std::begin(path), std::end(path), [](char c) {return c == '/';});
            if (SlashCount < 2)
                throw std::invalid_argument("could not split - path is too large");

            fs::path fileNamePart = p.filename();
            fs::path prefixPart = p.parent_path();

            while (fileNamePart.string().length() + (isDir?1:0) > 100 ||
                   prefixPart.string().length() > 155)
            {
                auto previousPrefixPart = prefixPart;

                fileNamePart = prefixPart.filename() / fileNamePart;
                prefixPart = prefixPart.parent_path();

                if (prefixPart == previousPrefixPart)
                    throw std::invalid_argument("could not find a split to save the path");

            }

            std::string fn = fileNamePart.string();
            if (isDir)
                fn.push_back('/');
            HEADER_ASSIGN(fileName, fn);
            HEADER_ASSIGN(prefix, prefixPart.string());
        }
        else
        {
            std::string fn = p.string();
            if (isDir)
                fn.push_back('/');
            HEADER_ASSIGN(fileName, fn);
            HEADER_CLEAR(prefix);
        }

        return p;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::pair <StarHeader, fs::path> createHeaderCommon(std::string path, bool isDir = false)
    {
        StarHeader header;

        /////////////////////////////////////////////////////////////////////////////////////////////
        // PATH
        /////////////////////////////////////////////////////////////////////////////////////////////

        auto p = splitPath(header, path, isDir);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MODE
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_CLEAR(fileMode);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // UID
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_CLEAR(uid);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // GID
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_CLEAR(gid);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // LINK NAME
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_CLEAR(linkName);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MAGIC
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN(magic, "ustar");

        /////////////////////////////////////////////////////////////////////////////////////////////
        // VERSION
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN(version, "00");

        /////////////////////////////////////////////////////////////////////////////////////////////
        // USER NAME
        /////////////////////////////////////////////////////////////////////////////////////////////

        // I cannot be bothered with a proper solution here. try for env. variables, and clear if not available.
        char * user_name = getenv("USER");
        if (!user_name)
             user_name = getenv("USERNAME");
        if (user_name)
            HEADER_ASSIGN(uName, user_name);
        else
            HEADER_CLEAR(uName);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // GROUP NAME
        /////////////////////////////////////////////////////////////////////////////////////////////

        // not portable -> clear
        HEADER_CLEAR(gName);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // DEV MAJOR
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN_OCTAL(devMajor, 0);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // DEV MINOR
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN_OCTAL(devMinor, 0);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // FILE TYPE
        /////////////////////////////////////////////////////////////////////////////////////////////

        header.typeflag = '0';

        return {header, p};
    }
//---------------------------------------------------------------------------------------------------------------------
    StarHeader createHeaderFromString(std::string const& path, std::string const& dataString)
    {
        auto header = createHeader(path, dataString.length(), false);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MODE
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN_OCTAL(fileMode,
            fs::owner_read | fs::owner_write |
            fs::group_read | fs::group_write |
            fs::others_read | fs::others_write
        );

        /////////////////////////////////////////////////////////////////////////////////////////////
        // CHECKSUM
        /////////////////////////////////////////////////////////////////////////////////////////////

        calculateChecksum(header);

        return header;
    }
//---------------------------------------------------------------------------------------------------------------------
    StarHeader createHeader(std::string const& path, std::size_t size, bool checksum = true)
    {
        StarHeader header = createHeaderCommon(path).first;

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MODIFY TIME
        /////////////////////////////////////////////////////////////////////////////////////////////

        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        HEADER_ASSIGN_OCTAL(mTime, time);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // SIZE
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN_OCTAL(size, size);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // CHECKSUM
        /////////////////////////////////////////////////////////////////////////////////////////////

        if (checksum)
            calculateChecksum(header);

        return header;
    }
//---------------------------------------------------------------------------------------------------------------------
    StarHeader createHeaderFromDiskNode(std::string path, std::string pathOverwrite)
    {
        preprocessPath(path);

        fs::path p{path};

        if (!fs::exists(p))
            throw std::invalid_argument("file does not exist");

        auto isDir = fs::is_directory(p);
        auto isRegFile = fs::is_regular_file(p);
        auto isSymLink = fs::is_symlink(p);

        if (!isDir && !isRegFile)
            throw std::invalid_argument("disk node is neither a regular file nor a directory");

        if (pathOverwrite.empty())
            pathOverwrite = path;
        else
            preprocessPath(pathOverwrite);
        std::pair <StarHeader, fs::path> res = createHeaderCommon(pathOverwrite, isDir);
        auto header = res.first;

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MODE
        /////////////////////////////////////////////////////////////////////////////////////////////

        auto status = fs::status(p);
        HEADER_ASSIGN_OCTAL(fileMode, status.permissions());

        /////////////////////////////////////////////////////////////////////////////////////////////
        // SIZE
        /////////////////////////////////////////////////////////////////////////////////////////////

        if (isDir)
            HEADER_CLEAR(size);
        else
            HEADER_ASSIGN_OCTAL(size, fs::file_size(p));

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MODIFY TIME
        /////////////////////////////////////////////////////////////////////////////////////////////

        auto time = fs::last_write_time(p);
        HEADER_ASSIGN_OCTAL(mTime, time);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // FILE TYPE
        /////////////////////////////////////////////////////////////////////////////////////////////

        if (isRegFile)
            header.typeflag = '0';
        else if (isSymLink)
            header.typeflag = '2';
        else
            header.typeflag = '5';

        /////////////////////////////////////////////////////////////////////////////////////////////
        // CHECKSUM
        /////////////////////////////////////////////////////////////////////////////////////////////

        calculateChecksum(header);

        return header;
    }
//---------------------------------------------------------------------------------------------------------------------
    StarHeader createLinkHeader(std::string originalFile, std::string linkName, bool checksum)
    {
        preprocessPath(originalFile);
        preprocessPath(linkName);

        if (linkName.size() > 100)
            throw std::runtime_error("link is too long");

        StarHeader header = createHeaderCommon(linkName, false).first;

        /////////////////////////////////////////////////////////////////////////////////////////////
        // MODIFY TIME
        /////////////////////////////////////////////////////////////////////////////////////////////

        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        HEADER_ASSIGN_OCTAL(mTime, time);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // SIZE
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN_OCTAL(size, 0);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // LINK
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN(linkName, originalFile);

        /////////////////////////////////////////////////////////////////////////////////////////////
        // FILEMODE & TYPE
        /////////////////////////////////////////////////////////////////////////////////////////////

        HEADER_ASSIGN_OCTAL(fileMode,
            fs::owner_read | fs::owner_write | fs::owner_exe |
            fs::group_read | fs::group_write | fs::group_exe |
            fs::others_read | fs::others_write | fs::others_exe
        );
        header.typeflag = '2';

        /////////////////////////////////////////////////////////////////////////////////////////////
        // CHECKSUM
        /////////////////////////////////////////////////////////////////////////////////////////////

        if (checksum)
            calculateChecksum(header);

        return header;
    }
//---------------------------------------------------------------------------------------------------------------------
    #undef HEADER_ASSIGN
    #undef HEADER_ASSIGN_OCTAL
    #undef HEADER_CLEAR
//---------------------------------------------------------------------------------------------------------------------
    std::string headerToString(StarHeader const& head)
    {
        std::string res;
        int offset = 0;
        res.resize(Constants::ChunkSize);

        #define WRITE(FIELD) \
        for (int i = 0; i != head.FIELD.size(); ++i) \
        { \
            res[i + offset] = head.FIELD[i]; \
        } \
        offset += head.FIELD.size();

        //read <head.NAME.size()>(reader, head.NAME)

        WRITE(fileName)
        WRITE(fileMode)
        WRITE(uid)
        WRITE(gid)
        WRITE(size)
        WRITE(mTime)
        WRITE(chksum)
        res[offset] = head.typeflag; ++offset;
        WRITE(linkName)
        WRITE(magic)
        WRITE(version)
        WRITE(uName)
        WRITE(gName)
        WRITE(devMajor)
        WRITE(devMinor)
        WRITE(prefix)

        #undef WRITE

        return res;
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string concatFileName(StarHeader const& head)
    {
        if (head.prefix[0] == '\0')
            return {std::begin(head.fileName), std::end(head.fileName)};
        else
        {
            std::string fname;
            for (int i = 0; i != head.prefix.size() && head.prefix[i] != '\0'; ++i)
                fname.push_back(head.prefix[i]);
            return fname + "/" + std::string{std::begin(head.fileName), std::end(head.fileName)};
        }
    }
//#####################################################################################################################
}
