// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_MANTIDWIDGETS_CONTOURPREVIEWPLOT_H_
#define MANTID_MANTIDWIDGETS_CONTOURPREVIEWPLOT_H_

// includes for interface development
#include "DllOption.h"
#include "MantidQtWidgets/Common/MdSettings.h"
#include "ui_ContourPreviewPlot.h"
#include <QWidget>
#include <qwt_plot_spectrogram.h>
// includes for workspace handling
#include "MantidAPI/IMDWorkspace.h"
#include "MantidAPI/MatrixWorkspace_fwd.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"
#include "MantidQtWidgets/Common/WorkspaceObserver.h"

#include <QSettings>

namespace Mantid {
namespace API {
class MWDimension;
}
} // namespace Mantid

namespace MantidQt {

namespace API {
class QwtRasterDataMD;
class MdSettings;
} // namespace API

namespace MantidWidgets {
// forward declarations
class ColorBarWidget;
class SafeQwtPlot;

using MWDimension_sptr = boost::shared_ptr<Mantid::API::MWDimension>;
using MWDimension_const_sptr =
    boost::shared_ptr<const Mantid::API::MWDimension>;

using DimensionRange = std::pair<Mantid::coord_t, Mantid::coord_t>;

/** A 2D viewer for a Matrix Workspace.
 *
 * Before drawing, it acquires a ReadLock to prevent
 * an algorithm from modifying the underlying workspace while it is
 * drawing.
 *
 * If no workspace is set, no drawing occurs (silently).

  @date 2016-02-05
*/
class EXPORT_OPT_MANTIDQT_LEGACYQWT ContourPreviewPlot
    : public QWidget,
      public MantidQt::API::WorkspaceObserver {
  Q_OBJECT

public:
  ContourPreviewPlot(QWidget *parent = nullptr);
  ~ContourPreviewPlot() override;
  void loadColorMap(QString filename = QString());
  void setWorkspace(Mantid::API::MatrixWorkspace_sptr const workspace);
  void updateDisplay();
  SafeQwtPlot *getPlot2D();
  void setPlotVisible(bool visible);
  void setColourBarVisible(bool visible);

public slots:
  void handleColorRangeChanged();
  void handleLoadColorMap();
  void setTransparentZerosSlot(bool transparent);

protected:
  void preDeleteHandle(
      std::string const &workspaceName,
      boost::shared_ptr<Mantid::API::Workspace> const workspace) override;

private:
  void setupColourBarAndPlot();
  QString colourMapFileName(QString const &filename);
  void loadSettings();
  void setCurrentColourMapFile(QSettings const &settings);
  void setCurrentColourMapFile(QString const &file);
  void saveSettings();
  void checkRangeLimits() const;
  void checkForInfiniteLimits(DimensionRange const &range,
                              std::size_t const &index,
                              std::ostringstream &message) const;
  DimensionRange dimensionRange(std::size_t const &index) const;
  Mantid::coord_t dimensionMinimum(std::size_t const &index) const;
  Mantid::coord_t dimensionMaximum(std::size_t const &index) const;
  void findRangeFull();
  void setVectorDimensions();
  void clearPlot();

  Ui::ContourPreviewPlot m_uiForm;
  /// Spectrogram plot of ContourPreviewPlot
  std::unique_ptr<QwtPlotSpectrogram> m_spectrogram;
  /// Data presenter
  std::unique_ptr<API::QwtRasterDataMD> m_data;
  /// File of the last loaded color map.
  QString m_currentColorMapFile;
  /// Md Settings for color maps
  boost::shared_ptr<MantidQt::API::MdSettings> m_mdSettings;
  /// Workspace being shown
  Mantid::API::MatrixWorkspace_sptr m_workspace;
  /// The calculated range of values in the FULL data set
  QwtDoubleInterval m_colorRangeFull;
  Mantid::API::MDNormalization m_normalization;
  /// Vector of the dimensions to show.
  std::vector<Mantid::Geometry::MDHistoDimension_sptr> m_dimensions;
};

} // namespace MantidWidgets
} // namespace MantidQt

#endif /* MANTID_MANTIDWIDGETS_CONTOURPREVIEWPLOT_H_ */
