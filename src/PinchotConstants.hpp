#ifndef JOESCAN_PINCHOT_CONSTANTS_HPP
#define JOESCAN_PINCHOT_CONSTANTS_HPP

namespace joescan {

#ifdef _JOESCAN_PINCHOT_NO_MAX_SCAN_RATE
static const double kPinchotConstantMaxScanRate = 10000;
#else
static const double kPinchotConstantMaxScanRate = 4000;
#endif

static const double kPinchotConstantMinScanRate = 0.2;

} // namespace joescan

#endif
