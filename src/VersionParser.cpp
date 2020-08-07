/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "VersionParser.hpp"
#include "TcpSerializationHelpers.hpp"

#include <sstream>

using namespace joescan;

// Returns the semantic version string. Example:
//     2.11.2-dirty-develop+1234abcd
std::string VersionParser::GetVersionString(const VersionInformation &vi)
{
  std::stringstream ss;
  ss << vi.major << "." << vi.minor << "." << vi.patch;
  if (vi.flags & VersionFlagMasks::Dirty) {
    ss << "-"
       << "dirty";
  }
  if (vi.flags & VersionFlagMasks::Develop) {
    ss << "-"
       << "develop";
  }
  ss << "+" << vi.commit;
  return ss.str();
}

bool VersionParser::AreVersionsCompatible(const VersionInformation &v1,
                                          const VersionInformation &v2)
{
  // Versions that share the same major number are (or should be)
  // compatible with each other even if the minor numbers differ
  return v1.major == v2.major;
}

void VersionParser::Serialize(std::vector<uint8_t> &message,
                              const VersionInformation &vi)
{
  SerializeIntegralToBytes(message, &vi.major);
  SerializeIntegralToBytes(message, &vi.minor);
  SerializeIntegralToBytes(message, &vi.patch);
  SerializeIntegralToBytes(message, &vi.commit);
  SerializeIntegralToBytes(message, &vi.hwid);
  SerializeIntegralToBytes(message, &vi.flags);
}

int VersionParser::Deserialize(VersionInformation &vi, uint8_t *data)
{
  uint8_t *idx_p = data;

  idx_p += ExtractFromNetworkBuffer(vi.major, idx_p);
  idx_p += ExtractFromNetworkBuffer(vi.minor, idx_p);
  idx_p += ExtractFromNetworkBuffer(vi.patch, idx_p);
  idx_p += ExtractFromNetworkBuffer(vi.commit, idx_p);
  idx_p += ExtractFromNetworkBuffer(vi.hwid, idx_p);
  idx_p += ExtractFromNetworkBuffer(vi.flags, idx_p);

  int bytes_deserialized = static_cast<int>(idx_p - data);

  return bytes_deserialized;
}
