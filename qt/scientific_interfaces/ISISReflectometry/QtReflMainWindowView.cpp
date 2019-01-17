// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#include "QtReflMainWindowView.h"
#include "MantidKernel/make_unique.h"
#include "QtReflBatchView.h"

#include <QMessageBox>
#include <QToolButton>

namespace MantidQt {
namespace CustomInterfaces {

DECLARE_SUBWINDOW(QtReflMainWindowView)

QtReflMainWindowView::QtReflMainWindowView(QWidget *parent)
    : UserSubWindow(parent), m_notifyee(NULL) {}

IReflBatchView *QtReflMainWindowView::newBatch() {
  auto index = m_ui.mainTabs->count();
  auto *newTab = new QtReflBatchView(this);
  m_ui.mainTabs->addTab(newTab, QString("Batch ") + QString::number(index));
  m_batchViews.emplace_back(newTab);
  return newTab;
}

void QtReflMainWindowView::removeBatch(int batchIndex) {
  m_batchViews.erase(m_batchViews.begin() + batchIndex);
  m_ui.mainTabs->removeTab(batchIndex);
}

std::vector<IReflBatchView *> QtReflMainWindowView::batches() const {
  return m_batchViews;
}

/**
Initialise the Interface
*/
void QtReflMainWindowView::initLayout() {
  m_ui.setupUi(this);
  connect(m_ui.helpButton, SIGNAL(clicked()), this, SLOT(helpPressed()));
  connect(m_ui.mainTabs, SIGNAL(tabCloseRequested(int)), this,
          SLOT(onTabCloseRequested(int)));
  connect(m_ui.newBatch, SIGNAL(triggered(bool)), this,
          SLOT(onNewBatchRequested(bool)));

  auto instruments = std::vector<std::string>(
      {{"INTER", "SURF", "CRISP", "POLREF", "OFFSPEC"}});

  auto thetaTolerance = 0.01;
  auto makeRunsTablePresenter =
      RunsTablePresenterFactory(instruments, thetaTolerance);
  auto defaultInstrumentIndex = 0;
  // TODO: Look this up properly by comparing the default instrument to the
  // values in the list;
  auto searcher = boost::shared_ptr<IReflSearcher>();
  auto messageHandler = this;

  auto makeRunsPresenter = RunsPresenterFactory(
      std::move(makeRunsTablePresenter), thetaTolerance, instruments,
      defaultInstrumentIndex, messageHandler, searcher);

  auto makeEventPresenter = EventPresenterFactory();
  auto makeSaveSettingsPresenter = SavePresenterFactory();
  auto makeExperimentPresenter = ExperimentPresenterFactory(thetaTolerance);
  auto makeInstrumentPresenter = InstrumentPresenterFactory();

  auto makeReflBatchPresenter = ReflBatchPresenterFactory(
      std::move(makeRunsPresenter), std::move(makeEventPresenter),
      std::move(makeExperimentPresenter), std::move(makeInstrumentPresenter),
      std::move(makeSaveSettingsPresenter));

  // Create the presenter
  m_presenter =
      ReflMainWindowPresenter(this, std::move(makeReflBatchPresenter));
  subscribe(&m_presenter.get());

  m_presenter.get().notifyNewBatchRequested();
  m_presenter.get().notifyNewBatchRequested();
}

void QtReflMainWindowView::onTabCloseRequested(int tabIndex) {
  m_ui.mainTabs->removeTab(tabIndex);
}

void QtReflMainWindowView::onNewBatchRequested(bool) {
  m_notifyee->notifyNewBatchRequested();
}

void QtReflMainWindowView::subscribe(ReflMainWindowSubscriber *notifyee) {
  m_notifyee = notifyee;
}

void QtReflMainWindowView::helpPressed() { m_notifyee->notifyHelpPressed(); }

/**
Runs python code
* @param pythonCode : [input] The code to run
* @return : Result of the execution
*/
std::string
QtReflMainWindowView::runPythonAlgorithm(const std::string &pythonCode) {

  QString output = runPythonCode(QString::fromStdString(pythonCode), false);
  return output.toStdString();
}

/**
Handles attempt to close main window
* @param event : [input] The close event
*/
void QtReflMainWindowView::closeEvent(QCloseEvent *event) {
  // Close only if reduction has been paused
  if (!m_presenter.get().isProcessing()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void QtReflMainWindowView::giveUserCritical(const std::string &prompt,
                                            const std::string &title) {
  QMessageBox::critical(this, QString::fromStdString(title),
                        QString::fromStdString(prompt), QMessageBox::Ok,
                        QMessageBox::Ok);
}

void QtReflMainWindowView::giveUserInfo(const std::string &prompt,
                                        const std::string &title) {
  QMessageBox::information(this, QString::fromStdString(title),
                           QString::fromStdString(prompt), QMessageBox::Ok,
                           QMessageBox::Ok);
}
} // namespace CustomInterfaces
} // namespace MantidQt
