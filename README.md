# star-tape

(DO NOT USE, I ONLY SUPPORT WHAT I NEED - NO AUTOMATIC TESTS)
(unless you want to submit pull request for fleshing this lib out, I'd gladly review and accept them)

star-tape is a tarball reader and writer for people with small requirements.
star-tape only supports the new UStar format and does not allow inplace operations.
The tar does not have to be located on the disk, if you implement the ITapeReader and ITapeWriter interfaces.
Two implementations already exist, for files (convenience adapter) and for streams.

## Dependencies

- boost qi (very fast number parser)
- boost filesystem
- boost system
- boost iostreams 
- bz2
- zlib
- (more from boost?)

## Example (Extract Archive to disk)

```C++
#include "star_tape.hpp"

int main()
{
  using namespace StarTape;
  extractArchive(openInputFile <CompressionType::Bzip2> ("my.tar.bz2"), "unpacked.tar");
}
```

## Example (Extract Archive to disk 2)

```C++
#include "star_tape.hpp"

int main()
{
  auto arch = loadArchive <InputTapeArchive, CompressionType::Bzip2> ("my.tar.bz2");
  saveArchive("uncompressed.tar", arch);
}
```

## Example (Save String to archive on disk)

```C++
#include "star_tape.hpp"

int main()
{
  auto bundle = createOutputFileArchive <CompressionType::None> ("new.tar");
  (TapeWaterfall{}
    << TapeOperations::AddString("bla.txt", "bla bla")
    << TapeOperations::AddString("bla2.txt", "bla bla")
  ).apply(&archive(bundle));
}
```

## Example (Multiple Archive Operations)

```C++
#include "star_tape.hpp"

int main()
{
  auto inputBundle = openInputFile("my.tar"); // no template argument = no compression
  // create archive index.
  // If you do that on compressed archives, you loose the ability to read it,
  // because compressed archives do not support a random access seek.
  // If you want to work with a compressed archive, you MUST decompress it entirely before doing work on it.
  // Either into RAM, or onto disk.
  auto index = archive(inputBundle).makeIndex();

  // open file and apply archive wrapper.
  auto outputBundle = createOutputFileArchive <CompressionType::Gzip> ("other.tar.gz");
  auto* outArchive = &archive(outputBundle);

  /*  The tape archive waterfall performs a sort on all operations before performing them.
   */
  using namespace TapeOperations;
  (TapeWaterfall{}
    << AddFile("./main.cpp") // add file from disk
    << AddDirectory("./tape_io") // add directory from disk
    << RemoveEntry("./to_tar/test.txt") // remove file entry (NOT RECURSIVE!!!)
    << Adopt() // Adopt all files and directories from input archive
  ).apply(outArchive, &index);
}
```


## Example (Load compressed archive into RAM)

```C++
#include "star_tape.hpp"

int main()
{
  auto bundle = openInputFile <CompressionType::Bzip2> ("my.tar.bz2");
  auto stringstreamTapeBundle = loadArchive <InputTapeArchive> (archive(bundle));
  archive(stringstreamTapeBundle).dump(std::cout);
}
```

## Example (Manual, indexing)

```C++
#include <star-tape/tape_archive.hpp>
#include <star-tape/tape_entry.hpp>

#include <star-tape/tape_io/tape_file.hpp>

#include <iostream>

int main()
{
  using namespace StarTape;

  InputTapeArchive archive;

  FileTapeReader myTarFile("./star.tar");
  archive.open(&ftape);

  auto index = archive.makeIndex();
  for (auto const& i : index)
  {
    std::cout << i.fileName << " (" << i.fileSize << ")\n";
  }

  //----------------------------------

  auto iter = index.findFile("test.txt");
  if (iter != std::end(index))
    index.writeFileToStream(iter, std::cout) << "\n";
}
```
