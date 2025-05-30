# wfys

# **WylmaFileSystemEncryption – Archive System**

A minimal, portable file/folder archiver for C++ projects, designed by **W\.y.s.** as part of the **Wylma** library.

---

## Overview

`WylmaFileSystemEncryption` is a lightweight, endian-stable archiving system for saving and restoring entire directory trees. It is built to be:

* Single-header (`wyfs.hpp`) with zero dependencies
* Suitable for both embedded and dynamic use (.dll/.so/.dylib)
* Compatible across platforms and architectures

---

## File Structure

```
your_project/
├── wyfs.hpp        # Main archive logic (header-only)
├── iwysf.cpp       # Optional: dynamic library interface
```

---

## Usage

### Save a directory into an archive

```cpp
#include "wyfs.hpp"

int main() {
	const std::string folderToSave = "assets/";
	const std::string outputArchive = "bundle.wyfs";

	int16_t result = wylma::WylmaFileSystemEncryption::save(folderToSave, outputArchive);
	if (result != 0) {
		std::cerr << "Failed to save archive.\n";
	}
}
```

---

### Load an archive into a folder

```cpp
#include "wyfs.hpp"

int main() {
	const std::string archive = "bundle.wyfs";
	const std::string extractTo = "output/";

	int16_t result = wylma::WylmaFileSystemEncryption::load(archive, extractTo);
	if (result != 0) {
		std::cerr << "Failed to load archive.\n";
	}
}
```

---

## Dynamic Library (Optional)

To expose the functions as a C ABI (for use in other languages or via plugins), compile `iwysf.cpp`:

```cpp
// iwysf.cpp

#include "wyfs.hpp"

#ifndef SAVE_FN_NAME
#define SAVE_FN_NAME WylmaFileSystemEncryptionSaveFile
#endif

#ifndef LOAD_FN_NAME
#define LOAD_FN_NAME WylmaFileSystemEncryptionLoadFile
#endif

#ifndef RETURN_TYPE
#define RETURN_TYPE int16_t
#endif

#ifndef STRING_TYPE
#define STRING_TYPE const char *
#endif

extern "C" {

	RETURN_TYPE SAVE_FN_NAME(STRING_TYPE root, STRING_TYPE output) {
		return wylma::WylmaFileSystemEncryption::save(root, output);
	}

	RETURN_TYPE LOAD_FN_NAME(STRING_TYPE archivePath, STRING_TYPE outputPath) {
		return wylma::WylmaFileSystemEncryption::load(archivePath, outputPath);
	}
}
```

---

## Build as Shared Library

**Linux/macOS:**

```bash
g++ -std=c++17 -shared -fPIC -o libwyfs.so iwysf.cpp
```

**Windows (MSVC):**

```bat
cl /LD iwysf.cpp /Fe:wyfs.dll
```

---

## Exported Functions

These functions are exported via the C ABI for external use:

```c
int16_t WylmaFileSystemEncryptionSaveFile(const char *rootDir, const char *outputArchive);
int16_t WylmaFileSystemEncryptionLoadFile(const char *archivePath, const char *outputDir);
```

---

## Notes

* Archives include relative file paths.
* Folders are automatically created during extraction.
* All binary sizes are written in **little-endian**, portable across architectures.
* Files are streamed in blocks for efficient I/O.

---

## Attribution

Developed by **W\.y.s.**
Part of the **Wylma** C++ library project
Module: `WylmaFileSystemEncryption`
