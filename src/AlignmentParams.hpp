/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#ifndef JOESCAN_ALIGNMENT_PARAMS_H
#define JOESCAN_ALIGNMENT_PARAMS_H

#include "Point2D.hpp"

namespace joescan {

class AlignmentParams {
 public:
  /**
   * Initialize the AlinmentParams object for use in coordinate conversion.
   *
   * @param roll The rotation to be applied along the Z axis.
   * @param shift_x The shift to be applied specified in inches.
   * @param shift_y The shift to be applied specified in inches.
   * @param flip_x Set to `true` to rotate the coordinate system by 180
   * degrees about the Y axis, placing postive X at former negative X.
   */
  AlignmentParams(double roll = 0.0, double shift_x = 0.0, double shift_y = 0.0,
                  bool flip_x = false);

  AlignmentParams(const AlignmentParams &other) = default;

  /**
   * Obtain the applied rotation for use in coordinate conversion.
   *
   * @return The current rotation.
   */
  double GetRoll() const;

  /**
   * Obtain the applied X direction shift for use in coordinate conversion.
   *
   * @return The current X shift expressed in inches.
   */
  double GetShiftX() const;

  /**
   * Obtain the applied Y direction shift for use in coordinate conversion.
   *
   * @return The current Y shift expressed in inches.
   */
  double GetShiftY() const;

  /**
   * Obtain the flip configuration of X axis.
   *
   * @return Boolean `true` if X axis is flipped, `false` otherwise.
   */
  bool GetFlipX() const;

  /**
   * For XY profile data, this will rotate the points around the mill
   * coordinate system origin.
   *
   * @param roll The rotation to be applied.
   */
  void SetRoll(double roll);

  /**
   * For XY profile data, apply the specified shift to X when converting
   * from camera coordinates to mill coordinates.
   *
   * @param shift_x The shift to be applied specified in inches.
   */
  void SetShiftX(double shift_x);

  /**
   * For XY profile data, apply the specified shift to Y when converting
   * from camera coordinates to mill coordinates.
   *
   * @param roll shift_y The shift to be applied specified in inches.
   */
  void SetShiftY(double shift_y);

  /**
   * Convert XY profile data from camera coordinates to mill coordinates.
   *
   * @param p Geometry X and Y value held in Point2D object.
   * @return Converted geometry point.
   */
  Point2D<int32_t> CameraToMill(Point2D<int32_t> p) const;

  /**
   * Convert XY profile data from camera coordinates to mill coordinates.
   *
   * @param x Geometry X value expressed in 1/1000 inches.
   * @param y Geometry Y value expressed in 1/1000 inches.
   * @return Converted geometry point.
   */
  inline Point2D<int32_t> CameraToMill(int32_t x, int32_t y) const;

  /**
   * Convert XY profile data from mill coordinates to camera coordinates.
   *
   * @param p Geometry X and Y value held in Point2D object.
   * @return Converted geometry point.
   */
  inline Point2D<int32_t> MillToCamera(Point2D<int32_t> p) const;

  /**
   * Convert XY profile data from mill coordinates to camera coordinates.
   *
   * @param x Geometry X value expressed in 1/1000 inches.
   * @param y Geometry Y value expressed in 1/1000 inches.
   * @return Converted geometry point.
   */
  inline Point2D<int32_t> MillToCamera(int32_t x, int32_t y) const;

 private:
  static double rho;
  double roll;
  double yaw;
  double sin_roll;
  double cos_roll;
  double cos_yaw;
  double sin_neg_roll;
  double cos_neg_roll;
  double cos_neg_yaw;
  double shift_x;
  double shift_y;
  double shift_x_1000;
  double shift_y_1000;
  bool flip_x;
};

/*
 * Note: We inline these functions for performance benefits.
 */
inline Point2D<int32_t> AlignmentParams::CameraToMill(Point2D<int32_t> p) const
{
  return CameraToMill(p.x, p.y);
}

inline Point2D<int32_t> AlignmentParams::CameraToMill(int32_t x,
                                                      int32_t y) const
{
  // static cast int32_t fields to doubles before doing our math
  double xd = static_cast<double>(x);
  double yd = static_cast<double>(y);

  // now calculate the mill values for both X and Y
  double xm = (xd * cos_yaw * cos_roll) - (yd * sin_roll) + shift_x_1000;
  double ym = (xd * cos_yaw * sin_roll) + (yd * cos_roll) + shift_y_1000;

  // now convert back to int32_t values
  int32_t xi = static_cast<int32_t>(xm);
  int32_t yi = static_cast<int32_t>(ym);

  return Point2D<int32_t>(xi, yi);
}

inline Point2D<int32_t> AlignmentParams::MillToCamera(Point2D<int32_t> p) const
{
  return MillToCamera(p.x, p.y);
}

inline Point2D<int32_t> AlignmentParams::MillToCamera(int32_t x,
                                                      int32_t y) const
{
  // static cast int32_t fields to doubles before doing our math
  double xd = static_cast<double>(x);
  double yd = static_cast<double>(y);

  // now calculate the camera values for both X and Y
  double xc = ((xd - shift_x_1000) * cos_neg_yaw * cos_neg_roll) -
              ((yd - shift_y_1000) * cos_neg_yaw * sin_neg_roll);
  double yc =
    ((xd - shift_x_1000) * sin_neg_roll) + ((yd - shift_y_1000) * cos_neg_roll);

  // now convert back to int32_t values
  int32_t xi = static_cast<int32_t>(xc);
  int32_t yi = static_cast<int32_t>(yc);

  return Point2D<int32_t>(xi, yi);
}

} // namespace joescan

#endif // JOESCAN_ALIGNMENT_PARAMS_H
