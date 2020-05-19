// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "DllOption.h"
#include "MantidQtWidgets/Common/CoordinateConversion.h"

namespace MantidQt {
namespace MantidWidgets {

/**
 * Creates an object containing information about converting display coordinates
 * to data coordinates within a workspace for the slice viewer
 **/

enum state { X = 0, Y = 1 };

class EXPORT_OPT_MANTIDQT_COMMON SliceViewerCoordConversion
    : public CoordinateConversion {
public:
  SliceViewerCoordConversion();
  /**
   * Converts the x and y display coordinates to the repective x and y data
   * coordinates
   **/
  std::tuple<double, double>
  toDataCoord(const double xDisplayCoord,
              const double yDisplayCoord) const override;

  void changeDimensions(const int xAxisState, const int yAxisState);

private:
  state m_xAxis;
  state m_yAxis;
};
} // namespace MantidWidgets
} // namespace MantidQt
