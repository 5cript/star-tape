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

## Example

```C++
#include "tape_archive.hpp"
#include "tape_entry.hpp"

#include "tape_io/tape_file.hpp"

#include <iostream>

int main()
{
  using namespace StarTape;

  TapeArchive archive;

  FileTape myTarFile("./star.tar");
  archive.open(&ftape, &ftape); // open for both reading and writing.

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