// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#include "MantidQtWidgets/LegacyQwt/ContourPreviewPlot.h"

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidGeometry/MDGeometry/IMDDimension.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"
#include "MantidGeometry/MDGeometry/MDTypes.h"
#include "MantidKernel/ReadLock.h"
#include "MantidQtWidgets/LegacyQwt/MantidColorMap.h"
#include "MantidQtWidgets/LegacyQwt/QwtRasterDataMD.h"
#include "MantidQtWidgets/LegacyQwt/SignalRange.h"

#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <qwt_color_map.h>
#include <qwt_double_rect.h>

#include <cmath>

using namespace Mantid::API;
using namespace Mantid::Geometry;
using namespace MantidQt::API;

namespace {
Mantid::Kernel::Logger g_log("ContourPreviewPlot");

MatrixWorkspace_sptr
convertToMatrixWorkspace(boost::shared_ptr<Workspace> const workspace) {
  return boost::dynamic_pointer_cast<MatrixWorkspace>(workspace);
}

template <typename T>
auto getValueFromSettings(QSettings const &settings, QString const &key,
                          T const &defaultValue) {
  return settings.value(key, defaultValue);
}

QString getColourMapFile(QSettings const &settings) {
  return getValueFromSettings(settings, "ColormapFile", "").toString();
}

int getScaleType(QSettings const &settings) {
  int scaleType = getValueFromSettings(settings, "ColorScale", -1).toInt();
  if (scaleType == -1)
    scaleType = getValueFromSettings(settings, "LogColorScale", 0).toInt();
  return scaleType;
}

double getExponent(QSettings const &settings) {
  return getValueFromSettings(settings, "PowerScaleExponent", 2.0).toDouble();
}

bool transparentZeros(QSettings const &settings) {
  return getValueFromSettings(settings, "TransparentZeros", 1).toInt();
}

} // namespace

namespace MantidQt {
namespace MantidWidgets {

ContourPreviewPlot::ContourPreviewPlot(QWidget *parent)
    : QWidget(parent), WorkspaceObserver(),
      m_mdSettings(boost::make_shared<MdSettings>()), m_workspace(),
      m_dimensions() {
  this->observePreDelete(true);
  m_spectrogram = std::make_unique<QwtPlotSpectrogram>();
  m_data = std::make_unique<QwtRasterDataMD>();
  m_normalization = NoNormalization;
  m_uiForm.setupUi(this);

  QObject::connect(m_uiForm.colorBar,
                   SIGNAL(changedColorRange(double, double, bool)), this,
                   SLOT(handleColorRangeChanged()));
  QObject::connect(m_uiForm.colorBar, SIGNAL(colorBarDoubleClicked()), this,
                   SLOT(handleLoadColorMap()));

  this->setupColourBarAndPlot();
  this->loadSettings();
  this->updateDisplay();
}

ContourPreviewPlot::~ContourPreviewPlot() {
  this->observePreDelete(false); // Disconnect notifications
  saveSettings();
  m_data.reset();
  m_spectrogram.reset();
}

/**
 * Load a color map from a file
 * @param file :: file to open; empty to ask via a dialog box.
 */
void ContourPreviewPlot::loadColorMap(QString file) {
  QString filename = colourMapFileName(filename);
  if (!filename.isEmpty()) {
    setCurrentColourMapFile(filename);
    m_uiForm.colorBar->getColorMap().loadMap(filename);
    m_spectrogram->setColorMap(m_uiForm.colorBar->getColorMap());
    m_uiForm.colorBar->updateColorMap();

    this->updateDisplay();
  }
}

QString ContourPreviewPlot::colourMapFileName(QString const &filename) {
  if (filename.isEmpty())
    return MantidColorMap::chooseColorMap(m_currentColorMapFile, this);
  return filename;
}

/**
 * Initialize objects after loading the workspace
 */
void ContourPreviewPlot::setWorkspace(MatrixWorkspace_sptr const workspace) {
  m_workspace = workspace;
  this->checkRangeLimits();
  m_data->setWorkspace(workspace);

  // Sets how the signal is normalized
  m_normalization = workspace->displayNormalization();
  m_data->setNormalization(m_normalization);

  this->setVectorDimensions();
  this->findRangeFull();

  m_uiForm.colorBar->setViewRange(m_colorRangeFull);
  m_uiForm.colorBar->updateColorMap();
  m_uiForm.plot2D->setWorkspace(workspace);
  m_spectrogram->setColorMap(m_uiForm.colorBar->getColorMap());

  this->updateDisplay();
  m_uiForm.colorBar->setScale(0);
}

/**
 * Updates the contour plot
 */
void ContourPreviewPlot::updateDisplay() {
  if (m_workspace) {
    m_data->setRange(m_uiForm.colorBar->getViewRange());

    std::vector<Mantid::coord_t> slicePoint{0, 0};
    constexpr std::size_t dimX(0);
    constexpr std::size_t dimY(1);
    IMDDimension_const_sptr X = m_dimensions[dimX];
    IMDDimension_const_sptr Y = m_dimensions[dimY];
    m_data->setSliceParams(dimX, dimY, X, Y, slicePoint);

    double const left{X->getMinimum()};
    double const top{Y->getMinimum()};
    double const width{X->getMaximum() - X->getMinimum()};
    double const height{Y->getMaximum() - Y->getMinimum()};
    QwtDoubleRect const bounds{left, top, width, height};
    m_data->setBoundingRect(bounds.normalized());

    m_spectrogram->setColorMap(m_uiForm.colorBar->getColorMap());
    m_spectrogram->setData(*m_data);
    m_spectrogram->itemChanged();
    m_uiForm.plot2D->replot();
  }
}

SafeQwtPlot *ContourPreviewPlot::getPlot2D() { return m_uiForm.plot2D; }

/**
 * Set the plot to be visible or hidden
 * @param visible :: false to hide the plot
 */
void ContourPreviewPlot::setPlotVisible(bool visible) {
  m_uiForm.plot2D->setVisible(visible);
}

/**
 * Set the colour bar to be visible or hidden
 * @param visible :: false to hide the colour bar
 */
void ContourPreviewPlot::setColourBarVisible(bool visible) {
  m_uiForm.colorBar->setVisible(visible);
}

/**
 * Slot called when the ColorBarWidget changes the range of colors
 */
void ContourPreviewPlot::handleColorRangeChanged() {
  m_spectrogram->setColorMap(m_uiForm.colorBar->getColorMap());
  this->updateDisplay();
}

/**
 * Slot called to load a colour map
 */
void ContourPreviewPlot::handleLoadColorMap() { this->loadColorMap(QString()); }

/**
 * Set whether to display 0 signal as "transparent" color.
 * @param transparent :: true if you want zeros to be transparent.
 */
void ContourPreviewPlot::setTransparentZerosSlot(bool transparent) {
  m_data->setZerosAsNan(transparent);
  this->updateDisplay();
}

/**
 * Remove the displayed data when the associated workspace is deleted
 */
void ContourPreviewPlot::preDeleteHandle(
    std::string const &workspaceName,
    boost::shared_ptr<Workspace> const workspace) {
  UNUSED_ARG(workspaceName);
  auto const deletedWorkspace = convertToMatrixWorkspace(workspace);
  if (deletedWorkspace && deletedWorkspace == m_workspace)
    this->clearPlot();
}

/**
 * Clear the plot
 */
void ContourPreviewPlot::clearPlot() { m_uiForm.plot2D->clear(); }

/**
 * Setup the ColourBar and Plot. Attach the spectrogram to the plot
 */
void ContourPreviewPlot::setupColourBarAndPlot() {
  m_uiForm.colorBar->setViewRange(1, 10);
  m_spectrogram->attach(m_uiForm.plot2D);
  m_spectrogram->setColorMap(m_uiForm.colorBar->getColorMap());
  m_uiForm.plot2D->autoRefresh();
}

/**
 * Load QSettings from .ini-type files
 */
void ContourPreviewPlot::loadSettings() {
  QSettings settings;
  settings.beginGroup("Mantid/ContourPreviewPlot");

  setCurrentColourMapFile(settings);
  if (!m_currentColorMapFile.isEmpty())
    loadColorMap(m_currentColorMapFile);

  m_uiForm.colorBar->setScale(getScaleType(settings));
  m_uiForm.colorBar->setExponent(getExponent(settings));
  this->setTransparentZerosSlot(transparentZeros(settings));

  settings.endGroup();
}

/**
 * Set the current colour map file
 * @param settings :: stores the value of the colour map file
 */
void ContourPreviewPlot::setCurrentColourMapFile(QSettings const &settings) {
  if (m_mdSettings != nullptr && m_mdSettings->getUsageGeneralMdColorMap())
    setCurrentColourMapFile(m_mdSettings->getGeneralMdColorMapFile());
  else
    setCurrentColourMapFile(getColourMapFile(settings));
}

/**
 * Set the current colour map file
 * @param file :: the filename as a QString
 */
void ContourPreviewPlot::setCurrentColourMapFile(QString const &file) {
  m_currentColorMapFile = file;
}

void ContourPreviewPlot::saveSettings() {
  QSettings settings;
  settings.beginGroup("Mantid/ContourPreviewPlot");
  settings.setValue("ColormapFile", m_currentColorMapFile);
  settings.setValue("ColorScale", m_uiForm.colorBar->getScale());
  settings.setValue("PowerScaleExponent", m_uiForm.colorBar->getExponent());
  settings.setValue("TransparentZeros", (m_data->isZerosAsNan() ? 1 : 0));
}

/**
 * Verify the limits of the stored workspace
 */
void ContourPreviewPlot::checkRangeLimits() const {
  std::ostringstream message;
  for (auto i = 0u; i < m_workspace->getNumDims(); ++i)
    checkForInfiniteLimits(dimensionRange(i), i, message);

  if (!message.str().empty()) {
    message << "Bad ranges could cause memory allocation errors. Please fix "
               "the workspace.";
    throw std::out_of_range(message.str());
  }
}

/**
 * Produces an error message if either of the limits are infinite
 */
void ContourPreviewPlot::checkForInfiniteLimits(
    DimensionRange const &range, std::size_t const &index,
    std::ostringstream &message) const {
  if (!std::isfinite(range.first) || !std::isfinite(range.second))
    message << "Dimension " << m_workspace->getDimension(index)->getName()
            << " has a bad range: (" << range.first << ", " << range.second
            << ")\n";
}

DimensionRange
ContourPreviewPlot::dimensionRange(std::size_t const &index) const {
  DimensionRange range =
      std::make_pair(dimensionMinimum(index), dimensionMaximum(index));
  if (range.second < range.first)
    range.swap(range);
  return range;
}

Mantid::coord_t
ContourPreviewPlot::dimensionMinimum(std::size_t const &index) const {
  return m_workspace->getDimension(index)->getMinimum();
}

Mantid::coord_t
ContourPreviewPlot::dimensionMaximum(std::size_t const &index) const {
  return m_workspace->getDimension(index)->getMaximum();
}

/**
 * Finds the full range of values in the workspace
 */
void ContourPreviewPlot::findRangeFull() {
  if (m_workspace) {
    auto const workspace = m_workspace;
    Mantid::Kernel::ReadLock lock(*workspace);

    m_colorRangeFull = API::SignalRange(*workspace, m_normalization).interval();
    double minimum = m_colorRangeFull.minValue();

    if (minimum <= 0 && m_uiForm.colorBar->getScale() == 1) {
      double const maximum = m_colorRangeFull.maxValue();
      minimum = pow(10., log10(maximum) - 10.);
      m_colorRangeFull = QwtDoubleInterval(minimum, maximum);
    }
  }
}

/**
 * Update m_dimensions using the loaded workspace
 */
void ContourPreviewPlot::setVectorDimensions() {
  if (m_workspace) {
    m_dimensions.clear();

    for (auto i = 0u; i < m_workspace->getNumDims(); ++i) {
      MDHistoDimension_sptr dimension(std::make_unique<MDHistoDimension>(
          m_workspace->getDimension(i).get()));
      m_dimensions.push_back(dimension);
    }
  }
}

} // namespace MantidWidgets
} // namespace MantidQt
