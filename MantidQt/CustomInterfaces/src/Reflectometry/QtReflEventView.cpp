#include "MantidQtCustomInterfaces/Reflectometry/QtReflEventView.h"
#include "MantidQtCustomInterfaces/Reflectometry/ReflEventPresenter.h"

namespace MantidQt {
namespace CustomInterfaces {

//----------------------------------------------------------------------------------------------
/** Constructor
* @param parent :: [input] The parent of this widget
*/
QtReflEventView::QtReflEventView(QWidget *parent) {

  UNUSED_ARG(parent);
  initLayout();

  // Add slicing option buttons to list
  m_buttonList.push_back(m_ui.uniformEvenTimeSlicesRadioButton);
  m_buttonList.push_back(m_ui.uniformTimeSlicesRadioButton);
  m_buttonList.push_back(m_ui.customTimeSlicesRadioButton);
  m_buttonList.push_back(m_ui.logValueSlicesRadioButton);

  // Whenever one of the slicing option buttons is selected, their corresponding
  // entry is enabled, otherwise they remain disabled.
  for (auto &button : m_buttonList) {
    connect(button, SIGNAL(toggled(bool)), this, SLOT(toggleSlicingOptions()));
  }

  toggleSlicingOptions(); // Run at least once

  m_presenter.reset(new ReflEventPresenter(this));
}

//----------------------------------------------------------------------------------------------
/** Destructor
*/
QtReflEventView::~QtReflEventView() {}

/**
Initialise the Interface
*/
void QtReflEventView::initLayout() { m_ui.setupUi(this); }

/** Returns the presenter managing this view
* @return :: A pointer to the presenter
*/
IReflEventPresenter *QtReflEventView::getPresenter() const {

  return m_presenter.get();
}

/** Returns the number of time slices
* @return :: The number of time slices
*/
std::string QtReflEventView::getTimeSlices() const {

  return m_ui.customTimeSlicesEdit->text().toStdString();
}

/** Returns the time slicing value(s) obtained from the selected widget
* @return :: Time slicing values
*/
std::string QtReflEventView::getTimeSlicingValues() const {

  std::string values;
  const auto checkedButton = m_ui.slicingOptionsButtonGroup->checkedButton();

  if (checkedButton == m_buttonList[0]) {
    values = m_ui.uniformEvenTimeSlicesEdit->text().toStdString();
  } else if (checkedButton == m_buttonList[1]) {
    values = m_ui.uniformTimeSlicesEdit->text().toStdString();
  } else if (checkedButton == m_buttonList[2]) {
    values = m_ui.customTimeSlicesEdit->text().toStdString();
  } else if (checkedButton == m_buttonList[3]) {
    values = m_ui.logValueComboBox->currentText().toStdString();
  }

  return values;
}

/** Enable slicing option entries for checked button and disable all others.
*/
void QtReflEventView::toggleSlicingOptions() const {

  const auto checkedButton = m_ui.slicingOptionsButtonGroup->checkedButton();

  std::vector<bool> entriesEnabled(m_buttonList.size(), false);
  for (size_t i = 0; i < m_buttonList.size(); i++) {
    if (m_buttonList[i] == checkedButton)
      entriesEnabled[i] = true;
  }

  m_ui.uniformEvenTimeSlicesEdit->setEnabled(entriesEnabled[0]);
  m_ui.uniformTimeSlicesEdit->setEnabled(entriesEnabled[1]);
  m_ui.customTimeSlicesEdit->setEnabled(entriesEnabled[2]);
  m_ui.logValueComboBox->setEnabled(entriesEnabled[3]);
}

} // namespace CustomInterfaces
} // namespace Mantid
