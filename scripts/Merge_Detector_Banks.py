# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
# pylint: disable=invalid-name
from __future__ import (absolute_import, division, print_function)

import sys

from qtpy import QtWidgets

from MergeDetectorBanks import presenter, view


class MergeDetectorBanks(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        super(MergeDetectorBanks, self).__init__(parent)

        self.window = QtWidgets.QMainWindow()

        my_view = view.View(parent=self)
        self.presenter = presenter.Presenter(my_view)

        # set the view for the main window
        self.setCentralWidget(my_view)
        self.setWindowTitle("Merge Detector Banks")


if QtWidgets.QApplication.instance():
    app = QtWidgets.QApplication.instance()
else:
    app = QtWidgets.QApplication(sys.argv)

merge_detector_banks = MergeDetectorBanks()
merge_detector_banks.show()
app.exec_()