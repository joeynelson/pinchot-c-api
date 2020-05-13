/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JSCANAPI_NETWORKING_TYPES_H
#define JSCANAPI_NETWORKING_TYPES_H

#include <cstdint>
#include <string>
#include <vector>

namespace joescan {
/// The number of bits per byte.
static const int kBitsPerByte = 8;

/**
 * The maximum payload of an ethernet frame is 1500 bytes; since we want to
 * limit our datagrams to be conatined in a single ethernet frame, we split
 * all data into datagrams with a maximum of 1500 octets/bytes. Reserve 32
 * bytes for the IP & UDP headers.
 */
static const int kMaxFramePayload = 1468;

/// The port used to send commands to the server running on the scan head.
static const uint16_t kScanServerPort = 12346;
/// Identifier for Status message from scan server.
static const uint16_t kResponseMagic = 0xFACE;
/// Identifier for Data Packet message from scan server.
static const uint16_t kDataMagic = 0xFACD;
/// Identifier for Command messages from client.
static const uint16_t kCommandMagic = kResponseMagic; // Why share value?

using Datagram = std::vector<uint8_t>;

// The DataType is a bit field, it has the flag set for all data types
// present to be sent.
enum DataType : uint16_t {
  Brightness = 0x1,
  XYData = 0x2,
  Width = 0x4,
  SecondMoment = 0x8,
  Subpixel = 0x10,
  Image = 0x20,
  // others here, this is extensible
};

static int GetSizeFor(DataType data_type)
{
  switch (data_type) {
    case XYData: {
      return 2 * sizeof(uint16_t);
    }
    case Width:
    case SecondMoment:
    case Subpixel: {
      return sizeof(uint16_t);
    }
    case Brightness:
    case Image:
    default:
      return sizeof(uint8_t);
  }
}

inline DataType operator|(DataType original, DataType additionalType)
{
  return static_cast<DataType>(static_cast<uint16_t>(original) |
                               static_cast<uint16_t>(additionalType));
}

/**
 * This is the fixed size header for each datagram. The bytes have been
 * packed to word size, so that we can serialize this without resorting to
 * changing the compiler struct packing. All elements are in network byte
 * order, so all elements larger than 1 byte must be converted with
 * hton/ntoh.
 */
struct DatagramHeader {
  uint16_t magic;                // 2    2
  uint16_t exposureTime;         // 2    4
  uint8_t scanHeadId;            // 1    5
  uint8_t cameraId;              // 1    6
  uint8_t laserId;               // 1    7
  uint8_t flags;                 // 1    8
  uint64_t timestamp;            // 8    16
  uint16_t laserPulseWidth;      // 2    18
  uint16_t dataType;             // 2    20
  uint16_t payloadLength;        // 2    22
  uint8_t numEncoderVals;        // 1    23
  uint8_t DEPRECATED_DO_NOT_USE; // 1    24
  uint32_t datagramPosition;     // hack to support big buffers
  uint32_t numberOfDatagrams;    // hack to support big buffers
};

/**
 * This is the header for any packet that is _not_ a profile
 * or image data packet. This header should never change.
 */
struct InfoHeader {
  uint16_t magic;
  uint8_t size;
  uint8_t type;
};
} // namespace joescan

#endif
