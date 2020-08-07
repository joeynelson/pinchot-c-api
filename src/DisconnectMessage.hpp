/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCAN_SERVER_DISCONNECT_MESSAGE_H
#define JSCAN_SERVER_DISCONNECT_MESSAGE_H

#include <vector>
#include "NetworkTypes.hpp"

namespace joescan {
class DisconnectMessage {
 public:
  DisconnectMessage();
  ~DisconnectMessage() = default;

  std::vector<uint8_t> Serialize() const;

 private:
#pragma pack(push, 1)
  struct DisconnectMessagePacket {
    InfoHeader header;
  };
#pragma pack(pop)

  DisconnectMessagePacket packet;
  static const uint8_t kDisconnectMessageSize = sizeof(DisconnectMessagePacket);

  static void ValidateHeader(const InfoHeader &hdr);
};
} // namespace joescan

#endif // JSCAN_SERVER_DISCONNECT_MESSAGE_H
