// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#include "ALFView_view.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QRegExpValidator>
#include <QFileDialog>

namespace MantidQt {
namespace CustomInterfaces {

ALFView_view::ALFView_view(QWidget *parent) : QWidget(parent), m_run(nullptr) {
  QSplitter *MainLayout = new QSplitter(Qt::Vertical, this);
  QWidget *loadBar = new QWidget();

  generateLoadWidget(loadBar);

  MainLayout->addWidget(loadBar);
  //  MainLayout->addWidget(widgetSplitter);
  // this->addWidget(MainLayout);
}

void ALFView_view::generateLoadWidget(QWidget *loadBar) {
  m_run = new QLineEdit();
  m_run->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), m_run));
  connect(m_run, SIGNAL(editingFinished()), this, SLOT(runChanged()));

  m_browse = new QPushButton("Browse");
  connect(m_browse, SIGNAL(clicked()), this, SLOT(browse()));

  QHBoxLayout *loadLayout = new QHBoxLayout(loadBar);
  loadLayout->addWidget(m_run);
  loadLayout->addWidget(m_browse);
}

int ALFView_view::getRunNumber() { return m_run->text().toInt(); }

void ALFView_view::runChanged() {
  auto fsafd = 1;
	emit newRun(); 
}

void ALFView_view::browse() { 
   auto file = QFileDialog::getOpenFileName(
      this, "Open a file", "directoryToOpen",
      "File (*.nxs)");
  auto dad = file.toStdString();
   emit browsedToRun(file.toStdString());
}

} // namespace CustomInterfaces
} // namespace MantidQt