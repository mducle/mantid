#ifndef MANTIDQTCUSTOMINTERFACES_MWDIAG_H_
#define MANTIDQTCUSTOMINTERFACES_MWDIAG_H_

#include "MantidQtMantidWidgets/ui_MWDiag.h"
#include "MantidQtAPI/MantidWidget.h"
#include "WidgetDllOption.h"
#include <QSettings>
#include <QStringList>
#include <QComboBox>

namespace MantidQt
{
  namespace MantidWidgets
  {
    //-----------------------------------------------
    // Forward declarations
    //------------------------------------------------
    class MWRunFiles;
    class DiagResults;
    
    class EXPORT_OPT_MANTIDQT_MANTIDWIDGETS MWDiag : public API::MantidWidget
    {
      Q_OBJECT

    public:
      /// there has to be a default constructor but you can call it with a pointer to the thing that will take ownership of it
      MWDiag(QWidget *parent=NULL, QString prevSettingsGr="", const QComboBox * const instru = NULL);
      QString run(const QString &outWS = "", const bool saveSettings = true);
      void blockPython(const bool block = true);

    private:
      /// The form generated by Qt Designer
      Ui::MWDiag m_designWidg;
      /// check if ==NULL before use, set to point to the results dialog box which is deleted when it's closed!
      mutable DiagResults *m_dispDialog;
      /// points to the control with the user selected instrument
      const QComboBox * const m_instru;
      /// points to the RunFile object used to specify the first white beam vanadium file
      MWRunFiles *m_WBV1;
      /// points to the RunFile object used to specify a possible second white beam vanadium file
      MWRunFiles *m_WBV2;
      /// true if either of the TOF windows have been changed by the user, otherwise false
      bool m_TOFChanged;
      /// time of flight value for the start of the background window that was set through the update??? or -1 if no value was sent through
      double m_sTOFAutoVal;
      /// time of flight value for the end of the background window that was set through the update??? or -1 if no value was sent through
      double m_eTOFAutoVal;
      /// the name of the output workspace that contains many of the results from these tests
      QString m_outputWS;
      /// the run files that will be loaded to do the background test
      QStringList m_monoFiles;
      /// the values on the form the last time it was SUCCESSFULLY run accessed through this object
      QSettings m_prevSets;

      QString createDiagnosticScript() const;
      void showTestResults(const QString & testSummary) const;

      void closeDialog();
      QString openFileDialog(const bool save, const QStringList &exts);
      void loadDefaults();
      void insertFileWidgs();
      void saveDefaults();
      void setupToolTips();
      void connectSignals(const QWidget * const parentInterface);
      void setUpValidators();
      bool isInputValid() const;

      private slots:
      void browseClicked(const QString &buttonDis);
      void updateTOFs(const double &start, const double &end);
      void specifyRuns(const QStringList &runFileNames);
      void TOFUpd();
      void noDispDialog() {m_dispDialog = NULL;}
      void validateHardMaskFile();
    };
  }
}

#endif //MANTIDQTCUSTOMINTERFACES_MWDIAG_H_
