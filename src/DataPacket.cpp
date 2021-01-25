/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include <bitset>
#include <map>
#include <stdexcept>

#include "DataPacket.hpp"
#include "TcpSerializationHelpers.hpp"

#ifdef __linux__
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#endif

using namespace joescan;

DataPacket::DataPacket(uint8_t *bytes, uint32_t num_bytes,
                       uint64_t received_timestamp)
{
  // TODO: Check datagram size
  (void)num_bytes;

  raw = bytes;
  raw_len = num_bytes;

  received = received_timestamp;
  scan_head = bytes[4];
  camera = (0 == bytes[5]) ? JS_CAMERA_A
                           : (1 == bytes[5]) ? JS_CAMERA_B : JS_CAMERA_MAX;
  laser = (0 == bytes[6]) ? JS_LASER_0 : JS_LASER_MAX;
  exposure_time = ntohs(*(reinterpret_cast<uint16_t *>(&bytes[2])));
  timestamp =
    hostToNetwork<uint64_t>(*(reinterpret_cast<uint64_t *>(&bytes[8])));
  part_num = ntohl(*(reinterpret_cast<uint32_t *>(&bytes[24])));
  num_parts = ntohl(*(reinterpret_cast<uint32_t *>(&bytes[28])));
  laser_on_time = ntohs(*(reinterpret_cast<uint16_t *>(&bytes[16])));

  contents =
    static_cast<DataType>(ntohs(*(reinterpret_cast<uint16_t *>(&bytes[18]))));
  std::bitset<8 * sizeof(uint16_t)> contents_bits(contents);
  num_content_types = static_cast<int>(contents_bits.count());

  payload_length = ntohs(*(reinterpret_cast<uint16_t *>(&bytes[20])));
  num_encoder_vals = bytes[22];
  // Skip over the content metadata (cols & steps) to reach the encoder vals
  const size_t encoder_offset = static_cast<size_t>(num_content_types) * 2 + 4;
  encoder_vals.resize(num_encoder_vals);
  for (size_t i = 0; i < num_encoder_vals; i++) {
    encoder_vals[i] = hostToNetwork<int64_t>(*(reinterpret_cast<int64_t *>(
      &bytes[32 + encoder_offset + i * sizeof(int64_t)])));
  }
  start_column = ntohs(*(reinterpret_cast<uint16_t *>(&bytes[32])));
  end_column = ntohs(*(reinterpret_cast<uint16_t *>(&bytes[34])));

  unsigned int offset = 36;
  unsigned int data_offset =
    (offset + num_content_types * 2) + (num_encoder_vals * 8);

  for (int i = 1; i <= contents; i <<= 1) {
    if ((contents & i) != 0) {
      FragmentLayout layout;
      DataType data_type = static_cast<DataType>(i);
      layout.step = ntohs(*(reinterpret_cast<uint16_t *>(&bytes[offset])));
      layout.offset = data_offset;

      if (i == DataType::Image) {
        // Image data arrives as blobs of sequential bytes, 4 full camera rows
        // per datagram.
        layout.num_vals = payload_length;
        layout.payload_size = payload_length;
      } else {
        // All processed data types are sent in datagrams which must fit within
        // an ethernet frame. If multiple datagrams are required for a profile,
        // the data will be distributed among the datagrams such that if we
        // lose a datagram, we lose resolution, but won't have large holes in
        // the data.
        auto num_cols = end_column - start_column + 1;
        layout.num_vals = num_cols / (num_parts * layout.step);
        // If the data doesn't divide evenly into the DataPackets, each
        // DataPacket starting from the first will have 1 additional value of
        // the type in question.
        if (((num_cols / layout.step) % num_parts) > part_num) {
          layout.num_vals++;
        }
        layout.payload_size = GetSizeFor(data_type) * layout.num_vals;
      }

      data_offset += layout.payload_size;
      offset += sizeof(uint16_t);
      fragment_layouts[data_type] = layout;
    }
  }
}

int DataPacket::GetSourceId() const
{
  return (scan_head << 16) | (camera << 8) | laser;
}

uint8_t DataPacket::GetScanHeadId() const
{
  return scan_head;
}

jsCamera DataPacket::GetCamera() const
{
  return camera;
}

jsLaser DataPacket::GetLaser() const
{
  return laser;
}

uint64_t DataPacket::GetTimeStamp() const
{
  return timestamp;
}

uint64_t DataPacket::GetReceived() const
{
  return received;
}

uint32_t DataPacket::GetPartNum() const
{
  return part_num;
}

uint32_t DataPacket::GetNumParts() const
{
  return num_parts;
}

int DataPacket::GetPayloadLength() const
{
  return payload_length;
}

uint8_t DataPacket::NumEncoderVals() const
{
  return num_encoder_vals;
}

DataType DataPacket::GetContents() const
{
  return contents;
}

int DataPacket::GetNumContentTypes() const
{
  return num_content_types;
}

std::vector<int64_t> DataPacket::GetEncoderValues() const
{
  return encoder_vals;
}

uint16_t DataPacket::GetLaserOnTime() const
{
  return laser_on_time;
}

uint16_t DataPacket::GetExposureTime() const
{
  return exposure_time;
}

FragmentLayout DataPacket::GetFragmentLayout(DataType type) const
{
  auto iter = fragment_layouts.find(type);
  if (iter != fragment_layouts.end()) {
    return iter->second;
  }

  return FragmentLayout();
}

uint8_t *DataPacket::GetRawBytes(uint32_t *byte_len) const
{
  *byte_len = raw_len;
  return raw;
}
