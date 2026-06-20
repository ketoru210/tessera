#include "core.h"

namespace tessera::core {

QString version()
{
    // TESSERA_VERSION is injected by CMake from the root project() VERSION.
    // The fallback only applies to builds that bypass the CMake definition.
#ifndef TESSERA_VERSION
#define TESSERA_VERSION "0.0.0"
#endif
    return QStringLiteral(TESSERA_VERSION);
}

} // namespace tessera::core
