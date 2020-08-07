/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_SUBPIXEL_H
#define JOESCAN_SUBPIXEL_H

#include <cstdint>

namespace joescan {
/*
 * This function is used to convert a raw subpixel value into a double. A
 * raw subpixel value is a 16 bit fractional value where the first 5 bits are
 * the fractional component and the remaining bits are the integer component.
 */
double subpixelConvert(int32_t subpixel)
{
  return subpixel / 32.0;
}
} // namespace joescan

#endif
