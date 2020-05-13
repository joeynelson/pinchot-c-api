/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "VersionCompatibilityException.hpp"
#include "VersionParser.hpp"

#include <string>

using namespace joescan;

VersionCompatibilityException::VersionCompatibilityException(
  const VersionInformation& v1, const VersionInformation& v2)
  : version1(v1), version2(v2)
{
}

const char* VersionCompatibilityException::what() const throw()
{
  std::string err = "Version " + VersionParser::GetVersionString(version1) +
                    " is not compatible with " +
                    VersionParser::GetVersionString(version2);
  return err.c_str();
}
