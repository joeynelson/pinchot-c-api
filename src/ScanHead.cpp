/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "httplib.hpp"
#include "json.hpp"
#include "ScanHead.hpp"
#include <iostream>

using namespace joescan;

ScanHead::ScanHead(ScanManager &manager, ScanHeadShared &shared)
  : scan_manager(manager), shared(shared), ip_address(0)
{
}

ScanManager &ScanHead::GetScanManager()
{
  return this->scan_manager;
}

ScanHeadShared &ScanHead::GetScanHeadShared()
{
  return this->shared;
}

void ScanHead::Configure(ScanHeadConfiguration &config)
{
  this->shared.SetConfig(config);
}

ScanHeadConfiguration ScanHead::GetConfiguration() const
{
  return this->shared.GetConfiguration();
}

std::string ScanHead::GetSerialNumber() const
{
  return this->shared.GetSerial();
}

uint32_t ScanHead::GetId() const
{
  return this->shared.GetId();
}

void ScanHead::SetIpAddress(uint32_t addr)
{
  ip_address = addr;

  ip_address_str = std::to_string((ip_address >> 24) & 0xFF) + "." +
                   std::to_string((ip_address >> 16) & 0xFF) + "." +
                   std::to_string((ip_address >> 8) & 0xFF) + "." +
                   std::to_string((ip_address >> 0) & 0xFF);
}

uint32_t ScanHead::GetIpAddress() const
{
  return ip_address;
}

bool ScanHead::ValidateConfig() const
{
  // TODO: Implement

  return true;
}

std::vector<std::shared_ptr<Profile>> ScanHead::GetProfiles(int max)
{
  return shared.PopProfiles(max);
}

uint32_t ScanHead::AvailableProfiles()
{
  return shared.AvailableProfiles();
}

uint32_t ScanHead::WaitUntilAvailableProfiles(uint32_t count,
                                              uint32_t timeout_us)
{
  return shared.WaitUntilAvailableProfiles(count, timeout_us);
}

StatusMessage ScanHead::GetStatusMessage() const
{
  return shared.GetStatusMessage();
}

void ScanHead::ClearStatusMessage()
{
  shared.ClearStatusMessage();
}

void ScanHead::SetDataFormat(jsDataFormat format)
{
  data_format = format;
}

jsDataFormat ScanHead::GetDataFormat() const
{
  return data_format;
}

ScanHeadTemperatures ScanHead::GetTemperatures()
{
  httplib::Client cli(ip_address_str, kRESTport);
  ScanHeadTemperatures t;

  memset(&t, 0, sizeof(ScanHeadTemperatures));

  const char *ep = "/sensors/temperature";
  httplib::Headers hdr = {{"Content-type", "application/json"}};
  std::shared_ptr<httplib::Response> res = cli.Get(ep, hdr);
  if (!res) {
    return t;
  }

  try {
    nlohmann::json json = nlohmann::json::parse(res->body);
    t.camera_temp_c[JS_CAMERA_0] = json["camera0"];
    t.camera_temp_c[JS_CAMERA_1] = json["camera1"];
    t.mainboard_temp_c = json["mainboard"];
    t.mainboard_humidity = json["mainboardHumidity"];
  } catch (std::exception &e) {
    // failed to parse
    (void)e;
  }

  return t;
}

void ScanHead::Flush()
{
  std::vector<std::shared_ptr<Profile>> profiles;
  do {
    profiles = GetProfiles(100);
  } while (profiles.size() > 0);
}
