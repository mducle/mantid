// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_CUSTOMINTERFACES_INSTRUMENT_H_
#define MANTID_CUSTOMINTERFACES_INSTRUMENT_H_

#include "../DllConfig.h"
#include "DetectorCorrections.h"
#include "MonitorCorrections.h"
#include "RangeInLambda.h"

namespace MantidQt {
namespace CustomInterfaces {

class MANTIDQT_ISISREFLECTOMETRY_DLL Instrument {
public:
  Instrument(boost::optional<RangeInLambda> wavelengthRange,
             MonitorCorrections monitorCorrections,
             DetectorCorrections detectorCorrections);

  boost::optional<RangeInLambda> const &wavelengthRange() const;
  bool integratedMonitors() const;
  size_t monitorIndex() const;
  boost::optional<RangeInLambda> monitorIntegralRange() const;
  boost::optional<RangeInLambda> monitorBackgroundRange() const;
  bool correctDetectors() const;
  DetectorCorrectionType detectorCorrectionType() const;

private:
  boost::optional<RangeInLambda> m_wavelengthRange;
  MonitorCorrections m_monitorCorrections;
  DetectorCorrections m_detectorCorrections;
};
} // namespace CustomInterfaces
} // namespace MantidQt
#endif // MANTID_CUSTOMINTERFACES_INSTRUMENT_H_
