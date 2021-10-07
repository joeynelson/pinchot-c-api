/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "AlignmentParams.hpp"
#include <cmath>

using namespace joescan;

double AlignmentParams::rho = (std::atan(1) * 4) / 180.0; // pi / 180

AlignmentParams::AlignmentParams(double roll, double shift_x, double shift_y,
                                 bool flip_x)
{
  SetRoll(roll, flip_x);
  SetShiftX(shift_x);
  SetShiftY(shift_y);
}

double AlignmentParams::GetRoll() const
{
  return roll;
}

double AlignmentParams::GetShiftX() const
{
  return shift_x;
}

double AlignmentParams::GetShiftY() const
{
  return shift_y;
}

bool AlignmentParams::GetFlipX() const
{
  return flip_x;
}

void AlignmentParams::SetRoll(double roll, bool flip_x)
{
  this->roll = roll;
  this->flip_x = flip_x;
  yaw = (flip_x) ? 0.0 : 180.0;
  sin_roll = std::sin(roll * rho);
  cos_roll = std::cos(roll * rho);
  cos_yaw = std::cos(yaw * rho);
  sin_neg_roll = std::sin(-1.0 * roll * rho);
  cos_neg_roll = std::cos(-1.0 * roll * rho);
  cos_neg_yaw = std::cos(-1.0 * yaw * rho);

  cos_yaw_times_cos_roll = cos_yaw * cos_roll;
  cos_yaw_times_sin_roll = cos_yaw * sin_roll;
}

void AlignmentParams::SetShiftX(double shift_x)
{
  this->shift_x = shift_x;
  this->shift_x_1000 = shift_x * 1000.0;
}

void AlignmentParams::SetShiftY(double shift_y)
{
  this->shift_y = shift_y;
  this->shift_y_1000 = shift_y * 1000.0;
}
