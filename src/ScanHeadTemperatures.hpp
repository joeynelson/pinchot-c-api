/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef _SCAN_HEAD_TEMPERATURES_HPP
#define _SCAN_HEAD_TEMPERATURES_HPP

#include "joescan_pinchot.h"

namespace joescan {

struct ScanHeadTemperatures {
  double camera_temp_c[JS_CAMERA_MAX];
  double mainboard_temp_c;
  double mainboard_humidity;
};

} // namespace joescan

#endif
