/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "BroadcastConnectMessage.hpp"
#include "Enums.hpp"
#include "TcpSerializationHelpers.hpp"

using namespace joescan;

BroadcastConnectMessage::BroadcastConnectMessage()
{
  packet.header.magic = kCommandMagic;
  packet.header.size = kBroadcastConnectMessageSize;
  packet.header.type = UdpPacketType::BroadcastConnect;
}

BroadcastConnectMessage::BroadcastConnectMessage(uint32_t ip, uint16_t port,
                                                 uint8_t session_id,
                                                 uint8_t scan_head_id,
                                                 uint32_t serial_number,
                                                 ConnectionType conn_type)
  : BroadcastConnectMessage()
{
  packet.ip = ip;
  packet.port = port;
  packet.session_id = session_id;
  packet.scan_head_id = scan_head_id;
  packet.conn_type = conn_type;
  packet.serial_number = serial_number;
}

BroadcastConnectMessage BroadcastConnectMessage::Deserialize(
  std::vector<uint8_t> &data)
{
  int i = 0;
  uint8_t conn_type;
  BroadcastConnectMessage message;

  i += ExtractFromNetworkBuffer(message.packet.header.magic, &data[i]);
  i += ExtractFromNetworkBuffer(message.packet.header.size, &data[i]);
  i += ExtractFromNetworkBuffer(message.packet.header.type, &data[i]);
  ValidateHeader(message.packet.header);

  i += ExtractFromNetworkBuffer(message.packet.ip, &data[i]);
  i += ExtractFromNetworkBuffer(message.packet.port, &data[i]);
  i += ExtractFromNetworkBuffer(message.packet.session_id, &data[i]);
  i += ExtractFromNetworkBuffer(message.packet.scan_head_id, &data[i]);
  i += ExtractFromNetworkBuffer(conn_type, &data[i]);
  i += ExtractFromNetworkBuffer(message.packet.serial_number, &data[i]);
  message.packet.conn_type = ConnectionType::_from_integral(conn_type);

  if (i != kBroadcastConnectMessageSize) {
    throw std::runtime_error("Failed to deserialize the connect packet");
  }

  return message;
}

std::vector<uint8_t> BroadcastConnectMessage::Serialize() const
{
  uint8_t conn_type;
  std::vector<uint8_t> message;
  message.reserve(packet.header.size);

  ValidateHeader(packet.header);
  SerializeIntegralToBytes(message, &packet.header.magic);
  SerializeIntegralToBytes(message, &packet.header.size);
  SerializeIntegralToBytes(message, &packet.header.type);

  SerializeIntegralToBytes(message, &packet.ip);
  uint16_t port = (packet.port == 0) ? kScanServerPort : packet.port;
  SerializeIntegralToBytes(message, &port);
  SerializeIntegralToBytes(message, &packet.session_id);
  SerializeIntegralToBytes(message, &packet.scan_head_id);
  conn_type = packet.conn_type._to_integral();
  SerializeIntegralToBytes(message, &conn_type);
  SerializeIntegralToBytes(message, &packet.serial_number);

  if (message.size() != kBroadcastConnectMessageSize) {
    throw std::runtime_error("Failed to serialize the connect packet");
  }

  return message;
}

void BroadcastConnectMessage::ValidateHeader(const InfoHeader &hdr)
{
  if (hdr.magic != kCommandMagic) {
    throw std::runtime_error("Got wrong magic for connect message packet");
  }

  if (hdr.size != kBroadcastConnectMessageSize) {
    throw std::runtime_error("Got wrong size for connect message packet");
  }

  if (hdr.type != UdpPacketType::BroadcastConnect) {
    throw std::runtime_error("Got wrong type for connect message packet");
  }
}
