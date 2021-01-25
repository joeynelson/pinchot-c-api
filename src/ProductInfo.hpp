#ifndef JOESCAN_PRODUCT_INFO_H
#define JOESCAN_PRODUCT_INFO_H

#include "joescan_pinchot.h"

namespace joescan {

#define JS50WX_CAMERA_BRIGHTNESS_BIT_DEPTH (8)
#define JS50WX_NUM_CAMERAS (JS_CAMERA_B + 1)
#define JS50WX_NUM_LASERS (JS_LASER_0 + 1)
#define JS50WX_NUM_ENCODERS (JS_ENCODER_2 + 1)
#define JS50WX_MAX_CAMERA_HEIGHT (1088)
#define JS50WX_MAX_CAMERA_WIDTH (1456)

#define JS50SC_CAMERA_BRIGHTNESS_BIT_DEPTH (8)
#define JS50SC_NUM_CAMERAS (JS_CAMERA_A + 1)
#define JS50SC_NUM_LASERS (JS_LASER_0 + 1)
#define JS50SC_NUM_ENCODERS (JS_ENCODER_2 + 1)
#define JS50SC_MAX_CAMERA_HEIGHT (1088)
#define JS50SC_MAX_CAMERA_WIDTH (1456)

static jsError GetProductCapabilities(jsScanHeadType t,
                                      jsScanHeadCapabilities *c)
{
  jsError r = JS_ERROR_NONE;

  switch (t) {
    case (JS_SCAN_HEAD_JS50WX):
      // Remove this in the future
      static_assert(
        (JS50WX_MAX_CAMERA_HEIGHT == JS_CAMERA_IMAGE_DATA_MAX_HEIGHT) &&
          (JS50WX_MAX_CAMERA_WIDTH == JS_CAMERA_IMAGE_DATA_MAX_WIDTH),
        "camera max dimensions define mismatch");

      c->camera_brightness_bit_depth = JS50WX_CAMERA_BRIGHTNESS_BIT_DEPTH;
      c->max_camera_image_height = JS50WX_MAX_CAMERA_HEIGHT;
      c->max_camera_image_width = JS50WX_MAX_CAMERA_WIDTH;
      c->max_scan_rate = kPinchotConstantMaxScanRate;
      c->num_cameras = JS50WX_NUM_CAMERAS;
      c->num_encoders = JS50WX_NUM_ENCODERS;
      c->num_lasers = JS50WX_NUM_LASERS;
      break;

    case (JS_SCAN_HEAD_JS50SC):
      // Remove this in the future
      static_assert(
        (JS50SC_MAX_CAMERA_HEIGHT == JS_CAMERA_IMAGE_DATA_MAX_HEIGHT) &&
          (JS50SC_MAX_CAMERA_WIDTH == JS_CAMERA_IMAGE_DATA_MAX_WIDTH),
        "camera max dimensions define mismatch");

      c->camera_brightness_bit_depth = JS50SC_CAMERA_BRIGHTNESS_BIT_DEPTH;
      c->max_camera_image_height = JS50SC_MAX_CAMERA_HEIGHT;
      c->max_camera_image_width = JS50SC_MAX_CAMERA_WIDTH;
      c->max_scan_rate = kPinchotConstantMaxScanRate;
      c->num_cameras = JS50SC_NUM_CAMERAS;
      c->num_encoders = JS50SC_NUM_ENCODERS;
      c->num_lasers = JS50SC_NUM_LASERS;
      break;

    default:
      r = JS_ERROR_INVALID_ARGUMENT;
  }

  return r;
}

} // namespace joescan

#endif
