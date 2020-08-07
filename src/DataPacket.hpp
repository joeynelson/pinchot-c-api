/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_DATA_PACKET_H
#define JOESCAN_DATA_PACKET_H

#include <map>
#include <memory>
#include <vector>

#include "NetworkTypes.hpp"
#include "joescan_pinchot.h"

namespace joescan {
struct FragmentLayout {
  unsigned int step = 0;
  unsigned int num_vals = 0;
  unsigned int offset = 0;
  unsigned int payload_size = 0;
};

class DataPacket {
 public:
  DataPacket() = default;
  DataPacket(uint8_t *bytes, uint32_t num_bytes, uint64_t received_timestamp);
  DataPacket(const DataPacket &other) = default;

  int GetSourceId() const;
  uint8_t GetScanHeadId() const;
  jsCamera GetCamera() const;
  jsLaser GetLaser() const;
  uint64_t GetTimeStamp() const;
  uint64_t GetReceived() const;

  /**
   * Profile data can be transmitted over the wire through multiple UDP
   * packets if the payload is large enough. This function returns the
   * packet's sequential number within the total number of packets to be sent.
   *
   * @return The packet's number within all packets needed for the Profile.
   */
  uint32_t GetPartNum() const;

  /**
   * Profile data can be transmitted over the wire through multiple UDP
   * packets if the payload is large enough. This function returns the
   * total number of packets that comprise a given Profile's data.
   *
   * @return The total number of packets required for the Profile.
   */
  uint32_t GetNumParts() const;
  int GetPayloadLength() const;
  uint8_t NumEncoderVals() const;
  DataType GetContents() const;
  int GetNumContentTypes() const;
  // inline these two functions for small optimization, gprof indicates they
  // are called frequently so inlining them saves some call overhead
  inline uint16_t GetStartColumn() const;
  inline uint16_t GetEndColumn() const;

  std::vector<int64_t> GetEncoderValues() const;
  uint16_t GetLaserOnTime() const;
  uint16_t GetExposureTime() const;

  FragmentLayout GetFragmentLayout(DataType type) const;
  uint8_t *GetRawBytes(uint32_t *byte_len) const;

 private:
  std::map<DataType, FragmentLayout> fragment_layouts;
  uint8_t *raw;
  uint32_t raw_len;

  uint8_t scan_head;
  jsCamera camera;
  jsLaser laser;
  uint64_t timestamp;
  uint64_t received;
  uint32_t part_num;
  uint32_t num_parts;
  int payload_length;
  uint8_t num_encoder_vals;
  DataType contents;
  int num_content_types;
  uint16_t start_column;
  uint16_t end_column;

  std::vector<int64_t> encoder_vals;
  uint16_t laser_on_time;
  uint16_t exposure_time;
};

inline uint16_t DataPacket::GetStartColumn() const
{
  return start_column;
}

inline uint16_t DataPacket::GetEndColumn() const
{
  return end_column;
}
} // namespace joescan

#endif // JOESCAN_DATA_PACKET_H
