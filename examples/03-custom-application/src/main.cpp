/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include <iostream>
#include "joescan_pinchot.h"

int main(int argc, char *argv[])
{
  /**
   * Add your own code here. Alternatively, delete this file and place your
   * existing source code within this directory in order to leverage the open
   * source code for the Pinchot C API.
   */
  {
    // Display the API version to console output for visual confirmation as to
    // the version being used for this example.
    uint32_t major, minor, patch;
    jsGetAPISemanticVersion(&major, &minor, &patch);
    std::cout << "Joescan API version " << major << "." << minor << "." << patch
              << std::endl;
  }

  return 0;
}
