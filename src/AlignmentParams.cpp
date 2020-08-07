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
  this->roll = roll;
  this->flip_x = flip_x;
  yaw = (flip_x) ? 180.0 : 0.0;
  sin_roll = std::sin(roll * rho);
  cos_roll = std::cos(roll * rho);
  cos_yaw = std::cos(yaw * rho);
  sin_neg_roll = std::sin(-1.0 * roll * rho);
  cos_neg_roll = std::cos(-1.0 * roll * rho);
  cos_neg_yaw = std::cos(-1.0 * yaw * rho);
  this->shift_x = shift_x;
  this->shift_y = shift_y;
  this->shift_x_1000 = shift_x * 1000.0;
  this->shift_y_1000 = shift_y * 1000.0;
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

void AlignmentParams::SetRoll(double roll)
{
  this->roll = roll;
  sin_roll = std::sin(roll * rho);
  cos_roll = std::cos(roll * rho);
  sin_neg_roll = std::sin(-1.0 * roll * rho);
  cos_neg_roll = std::cos(-1.0 * roll * rho);
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
