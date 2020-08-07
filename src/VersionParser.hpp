/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_VERSION_PARSER_H
#define JOESCAN_VERSION_PARSER_H

#include "VersionInformation.hpp"

#include <string>
#include <vector>

namespace joescan {

class VersionParser {
 public:
  static std::string GetVersionString(const VersionInformation &vi);
  static bool AreVersionsCompatible(const VersionInformation &v1,
                                    const VersionInformation &v2);

  static void Serialize(std::vector<uint8_t> &message,
                        const VersionInformation &vi);
  static int Deserialize(VersionInformation &vi, uint8_t *data);
};
} // namespace joescan

#endif
