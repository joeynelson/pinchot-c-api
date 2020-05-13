/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCANAPI_VERSION_INFORMATION_H
#define JSCANAPI_VERSION_INFORMATION_H

#include <cstdint>

namespace joescan {

enum HardwareId : uint16_t {
  Invalid = 0,
  XU3 = 1,
  TE0820 = 2,
};

enum VersionFlagMasks : uint16_t {
  Dirty = 1 << 0,
  Develop = 1 << 1,
};

/**
 * This struct should _never_ change so version
 * mismatches can be handled cleanly
 */
struct VersionInformation {
  uint32_t major = 0;
  uint32_t minor = 0;
  uint32_t patch = 0;
  uint32_t commit = 0;
  uint16_t hwid = 0;
  uint16_t flags = 0;
};
} // namespace joescan

#endif
