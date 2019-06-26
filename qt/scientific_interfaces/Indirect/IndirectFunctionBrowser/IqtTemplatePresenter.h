// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef INDIRECT_IQTTEMPLATEPRESENTER_H_
#define INDIRECT_IQTTEMPLATEPRESENTER_H_

#include "DllConfig.h"
#include "IqtFunctionModel.h"
#include "ParameterEstimation.h"

#include <QMap>
#include <QWidget>

class QtProperty;

namespace MantidQt {
namespace MantidWidgets {
class EditLocalParameterDialog;
}
namespace CustomInterfaces {
namespace IDA {

class IqtTemplateBrowser;

/**
 * Class FunctionTemplateBrowser implements QtPropertyBrowser to display
 * and set properties that can be used to generate a fit function.
 *
 */
class MANTIDQT_INDIRECT_DLL IqtTemplatePresenter : public QObject {
  Q_OBJECT
public:
  IqtTemplatePresenter(IqtTemplateBrowser *view);
  void setNumberOfExponentials(int);
  void setStretchExponential(bool);
  void setBackground(const QString &name);
  void setNumberOfDatasets(int);
  int getNumberOfDatasets() const;
  void setFunction(const QString &funStr);
  IFunction_sptr getGlobalFunction() const;
  IFunction_sptr getFunction() const;
  QStringList getGlobalParameters() const;
  QStringList getLocalParameters() const;
  void setGlobalParameters(const QStringList &globals);
  void setGlobal(const QString &parName, bool on);
  void updateMultiDatasetParameters(const IFunction &fun);
  void updateMultiDatasetParameters(const ITableWorkspace &paramTable);
  void updateParameters(const IFunction &fun);
  void setCurrentDataset(int i);
  void setDatasetNames(const QStringList &names);
  void setViewParameterDescriptions();
  void setErrorsEnabled(bool enabled);
  void tieIntensities(bool on);
  bool canTieIntensities() const;
  void
  updateParameterEstimationData(DataForParameterEstimationCollection &&data);

signals:
  void functionStructureChanged();

private slots:
  void editLocalParameter(const QString &parName);
  void editLocalParameterFinish(int result);
  void viewChangedParameterValue(const QString &parName, double value);

private:
  void updateViewParameters();
  QStringList getDatasetNames() const;
  double getLocalParameterValue(const QString &parName, int i) const;
  bool isLocalParameterFixed(const QString &parName, int i) const;
  QString getLocalParameterTie(const QString &parName, int i) const;
  QString getLocalParameterConstraint(const QString &parName, int i) const;
  void setLocalParameterValue(const QString &parName, int i, double value);
  void setLocalParameterFixed(const QString &parName, int i, bool fixed);
  void setLocalParameterTie(const QString &parName, int i, const QString &tie);
  void updateViewParameterNames();
  void updateView();
  IqtTemplateBrowser *m_view;
  IqtFunctionModel m_model;
  EditLocalParameterDialog *m_editLocalParameterDialog;
};

} // namespace IDA
} // namespace CustomInterfaces
} // namespace MantidQt

#endif /*INDIRECT_IQTTEMPLATEPRESENTER_H_*/
