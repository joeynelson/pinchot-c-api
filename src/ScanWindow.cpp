/**
 * Copyright (c) JoeScan Inc. All Rights Reserved.
 *
 * Licensed under the BSD 3 Clause License. See LICENSE.txt in the project
 * root for license information.
 */

#include "ScanWindow.hpp"
#include "Point2D.hpp"

using namespace joescan;

ScanWindow::ScanWindow(double top, double bottom, double left, double right)
  : m_top(top), m_bottom(bottom), m_left(left), m_right(right)
{
  if (top <= bottom) {
    throw std::range_error("window top must be greater than window bottom");
  }

  if (right <= left) {
    throw std::range_error("window right must be greater than window left");
  }

  // convert from inches to 1/1000 of an inch
  int32_t top1000 = static_cast<int32_t>(top * 1000.0);
  int32_t bottom1000 = static_cast<int32_t>(bottom * 1000.0);
  int32_t left1000 = static_cast<int32_t>(left * 1000.0);
  int32_t right1000 = static_cast<int32_t>(right * 1000.0);

  m_constraints.push_back(WindowConstraint(
    Point2D<int64_t>(left1000, top1000), Point2D<int64_t>(right1000, top1000)));

  m_constraints.push_back(
    WindowConstraint(Point2D<int64_t>(right1000, bottom1000),
                     Point2D<int64_t>(left1000, bottom1000)));

  m_constraints.push_back(
    WindowConstraint(Point2D<int64_t>(right1000, top1000),
                     Point2D<int64_t>(right1000, bottom1000)));

  m_constraints.push_back(
    WindowConstraint(Point2D<int64_t>(left1000, bottom1000),
                     Point2D<int64_t>(left1000, top1000)));
}

std::vector<WindowConstraint> ScanWindow::GetConstraints() const
{
  return m_constraints;
}

double ScanWindow::GetTop() const
{
  return m_top;
}

double ScanWindow::GetBottom() const
{
  return m_bottom;
}

double ScanWindow::GetLeft() const
{
  return m_left;
}

double ScanWindow::GetRight() const
{
  return m_right;
}
