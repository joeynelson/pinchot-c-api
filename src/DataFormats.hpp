/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCANAPI_DATA_FORMATS_H
#define JSCANAPI_DATA_FORMATS_H

#include "NetworkingTypes.hpp"
#include "joescan_pinchot.h"

#include <map>
#include <vector>

namespace joescan {
class DataFormats {
 public:
  static std::vector<uint16_t> GetStep(jsDataFormat format);
  static DataType GetDataType(jsDataFormat format);

 private:
  static std::map<jsDataFormat, std::pair<DataType, std::vector<uint16_t>>>
    formats;
};
} // namespace joescan
#endif // JSCANAPI_DATA_FORMATS_H
