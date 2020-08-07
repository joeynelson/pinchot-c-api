/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_ENUMS_H
#define JOESCAN_ENUMS_H

#include "enum.h"

namespace joescan {
// This enum has to agree with the `ConnectionType` enum in the C# API
BETTER_ENUM(ConnectionType, uint8_t, Normal = 0, Mappler = 1)

BETTER_ENUM(ServerConnectionStatus, uint8_t, Disconnected = 0, Connected = 1,
            Scanning = 2)

BETTER_ENUM(
  UdpPacketType, uint8_t, Invalid = 0,
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // This field is deprecated and kept for historical purposes. Do not use.
  Connect = 1,
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  StartScanning = 2, Status = 3, SetWindow = 4, GetMappleTable = 5,
  Disconnect = 6, BroadcastConnect = 7)

BETTER_ENUM(CameraExposureMode, uint8_t, Interleaved = 0, Simultaneous = 1)
} // namespace joescan

#endif
