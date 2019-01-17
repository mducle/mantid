// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2011 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_ISISREFLECTOMETRY_REFLEVENTPRESENTER_H
#define MANTID_ISISREFLECTOMETRY_REFLEVENTPRESENTER_H

#include "../../DllConfig.h"
#include "../../Reduction/Slicing.h"
#include "IEventPresenter.h"
#include "IEventView.h"
#include "IReflBatchPresenter.h"

namespace MantidQt {
namespace CustomInterfaces {

/** @class EventPresenter

EventPresenter is a presenter class for the widget 'Event' in the
ISIS Reflectometry Interface.
*/
class MANTIDQT_ISISREFLECTOMETRY_DLL EventPresenter
    : public IEventPresenter,
      public EventViewSubscriber {
public:
  EventPresenter(IEventView *view);

  void onReductionPaused() override;
  void onReductionResumed() override;

  void notifySliceTypeChanged(SliceType newSliceType) override;
  void notifyUniformSliceCountChanged(int sliceCount) override;
  void notifyUniformSecondsChanged(double sliceLengthInSeconds) override;
  void
  notifyCustomSliceValuesChanged(std::string pythonListOfSliceTimes) override;
  void
  notifyLogSliceBreakpointsChanged(std::string logValueBreakpoints) override;
  void notifyLogBlockNameChanged(std::string blockName) override;

  void acceptMainPresenter(IReflBatchPresenter *mainPresenter) override;

  Slicing const &slicing() const;

private:
  Slicing m_slicing;
  void setUniformSlicingByNumberOfSlicesFromView();
  void setUniformSlicingByTimeFromView();
  void setCustomSlicingFromView();
  void setLogValueSlicingFromView();
  void setSlicingFromView();
  /// The view we are managing
  IEventView *m_view;
  IReflBatchPresenter *m_mainPresenter;
  SliceType m_sliceType;
};
} // namespace CustomInterfaces
} // namespace MantidQt
#endif /* MANTID_ISISREFLECTOMETRY_REFLEVENTPRESENTER_H */
