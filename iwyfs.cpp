#include <cstdint>
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