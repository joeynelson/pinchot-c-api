/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCANAPI_VERSION_COMPATIBILITY_EXCEPTION_H
#define JSCANAPI_VERSION_COMPATIBILITY_EXCEPTION_H

#include <exception>

#include "VersionInformation.hpp"

namespace joescan {

/**
 * @brief This exception is to be thrown when there is a detection of
 * version incompatibility with the server and client. To detect a version
 * mismatch, use `VersionParser::AreVersionsCompatibile`
 */
class VersionCompatibilityException : std::exception {
 private:
  VersionInformation version1;
  VersionInformation version2;

 public:
  VersionCompatibilityException(const VersionInformation& v1,
                                const VersionInformation& v2);
  const char* what() const throw();
};
} // namespace joescan

#endif
