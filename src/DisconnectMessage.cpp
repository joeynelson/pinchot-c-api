/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "DisconnectMessage.hpp"
#include "Enums.hpp"
#include "TcpSerializationHelpers.hpp"

using namespace joescan;

DisconnectMessage::DisconnectMessage()
{
  packet.header.magic = kCommandMagic;
  packet.header.size = kDisconnectMessageSize;
  packet.header.type = UdpPacketType::Disconnect;
}

std::vector<uint8_t> DisconnectMessage::Serialize() const
{
  std::vector<uint8_t> message;
  message.reserve(packet.header.size);

  ValidateHeader(packet.header);
  SerializeIntegralToBytes(message, &packet.header.magic);
  SerializeIntegralToBytes(message, &packet.header.size);
  SerializeIntegralToBytes(message, &packet.header.type);

  if (message.size() != kDisconnectMessageSize) {
    throw std::runtime_error("Failed to serial disconnect message");
  }

  return message;
}

void DisconnectMessage::ValidateHeader(const InfoHeader &hdr)
{
  if (hdr.magic != kCommandMagic) {
    throw std::runtime_error("Got wrong magic for disconnect message packet");
  }

  if (hdr.size != kDisconnectMessageSize) {
    throw std::runtime_error("Got wrong size for disconnect message packet");
  }

  if (hdr.type != +UdpPacketType::Disconnect) {
    throw std::runtime_error("Got wrong type for disconnect message packet");
  }
}
