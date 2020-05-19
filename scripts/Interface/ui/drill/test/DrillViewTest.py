# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +

import unittest
import mock
import sys

from qtpy.QtWidgets import QApplication
from qtpy.QtCore import *
from qtpy.QtTest import *

from Interface.ui.drill.view.DrillView import DrillView


app = QApplication(sys.argv)


class DrillViewTest(unittest.TestCase):

    def setUp(self):
        patch = mock.patch('Interface.ui.drill.view.DrillView.QFileDialog')
        self.mFileDialog = patch.start()
        self.addCleanup(patch.stop)

        patch = mock.patch('Interface.ui.drill.view.DrillView.QMessageBox')
        self.mMsgBox = patch.start()
        self.addCleanup(patch.stop)

        patch = mock.patch(
                'Interface.ui.drill.view.DrillView.manageuserdirectories')
        self.mUserDir = patch.start()
        self.addCleanup(patch.stop)

        self.view = DrillView()
        self.view.table = mock.Mock()

    def test_copySelectedRows(self):
        self.assertEqual(self.view.buffer, list())
        self.view.table.getSelectedRows.return_value = [0, 1]
        self.view.table.getRowContents.return_value = ["test1", "test2"]
        self.view.cut_selected_rows()
        self.assertEqual(self.view.buffer, [["test1", "test2"],
                                            ["test1", "test2"]])

    def test_cutSelectedRows(self):
        self.assertEqual(self.view.buffer, list())
        self.view.table.getSelectedRows.return_value = [0, 1]
        self.view.table.getRowContents.return_value = ["test1", "test2"]
        self.view.cut_selected_rows()
        self.assertEqual(self.view.buffer, [["test1", "test2"],
                                            ["test1", "test2"]])

    def test_pasteRows(self):
        self.assertEqual(self.view.buffer, list())
        self.view.buffer = [["test1", "test2"],
                            ["test3", "test4"]]
        self.view.table.getLastSelectedRow.return_value = 0
        self.view.paste_rows()
        self.view.table.getLastSelectedRow.called_once()
        self.view.table.getLastRow.asset_not_called()
        self.view.table.addRow.assert_called()
        self.view.table.setRowContents.assert_called()
        calls = [mock.call(1, ["test1", "test2"]),
                 mock.call(2, ["test3", "test4"])]
        self.view.table.setRowContents.assert_has_calls(calls)
        self.assertEqual(self.view.buffer, [["test1", "test2"],
                                            ["test3", "test4"]])

        self.view.table.reset_mock()
        self.view.table.getLastSelectedRow.return_value = -1
        self.view.table.getLastRow.return_value = 0
        self.view.paste_rows()
        self.view.table.getLastSelectedRow.called_once()
        self.view.table.getLastRow.called_once()

    def test_addRowAfter(self):
        # add one row (defautlt value)
        self.view.table.getLastSelectedRow.return_value = 0
        self.view.add_row_after()
        self.view.table.getLastRow.assert_not_called()
        self.view.table.addRow.assert_called_once_with(1)

        self.view.table.reset_mock()
        self.view.table.getLastSelectedRow.return_value = -1
        self.view.table.getLastRow.return_value = 0
        self.view.add_row_after()
        self.view.table.getLastSelectedRow.assert_called_once()
        self.view.table.getLastRow.assert_called_once()
        self.view.table.addRow.assert_called_once_with(1)

        # add many rows
        self.view.table.reset_mock()
        self.view.nrows = mock.Mock()
        self.view.nrows.value.return_value = 15
        self.view.table.getLastSelectedRow.return_value = 0
        self.view.add_row_after()
        calls = [mock.call(n) for n in range(1, 16)]
        self.view.table.addRow.assert_has_calls(calls)

    def test_delSelectedRows(self):
        # no selection
        self.view.table.getSelectedRows.return_value = []
        self.view.del_selected_rows()
        self.view.table.deleteRow.assert_not_called()
        # selction
        self.view.table.getSelectedRows.return_value = [0, 1, 2]
        self.view.del_selected_rows()
        calls = [mock.call(2), mock.call(1), mock.call(0)]
        self.view.table.deleteRow.assert_has_calls(calls)

    def test_eraseSelectedCells(self):
        # no selection
        self.view.table.getSelectedCells.return_value = []
        self.view.erase_selected_cells()
        self.view.table.eraseCell.assert_not_called()
        self.view.table.getSelectedCells.return_value = [(0, 0),
                                                         (0, 1),
                                                         (2, 1)]
        # selection
        self.view.erase_selected_cells()
        calls = [mock.call(0, 0), mock.call(0, 1), mock.call(2, 1)]
        self.view.table.eraseCell.assert_has_calls(calls)

    def test_processSelectedRows(self):
        self.view.process = mock.Mock()
        # no selection
        self.view.table.getSelectedRows.return_value = []
        self.view.table.getRowsFromSelectedCells.return_value = []
        self.view.process_selected_rows()
        self.view.table.process.emit.assert_not_called()
        # rows selection
        self.view.table.getSelectedRows.return_value = [0, 2]
        self.view.process_selected_rows()
        calls = [mock.call([0, 2])]
        self.view.process.emit.assert_has_calls(calls)
        # cells selection
        self.view.process.reset_mock()
        self.view.table.getSelectedRows.return_value = []
        self.view.table.getRowsFromSelectedCells.return_value = [1, 3]
        self.view.process_selected_rows()
        calls = [mock.call([1, 3])]
        self.view.process.emit.assert_has_calls(calls)

    def test_processAllRows(self):
        self.view.process = mock.Mock()
        # empty table
        self.view.table.getAllRows.return_value = []
        self.view.process_all_rows()
        self.view.table.process.emit.assert_not_called()
        # not empty
        self.view.table.getAllRows.return_value = [0, 1, 2]
        self.view.process_all_rows()
        calls = [mock.call([0, 1, 2])]
        self.view.process.emit.assert_has_calls(calls)

    def test_loadRundex(self):
        self.view.rundex_loaded = mock.Mock()
        self.mFileDialog.getOpenFileName.return_value = ("test", "test")
        self.view.load_rundex()
        self.view.rundex_loaded.emit.assert_called_once_with("test")

    def test_saveRundex(self):
        self.view.rundex_saved = mock.Mock()
        self.mFileDialog.getSaveFileName.return_value = ("test", "test")
        self.view.save_rundex()
        self.view.rundex_saved.emit.assert_called_once_with("test")

    def test_automaticFilling(self):
        self.view.increment = mock.Mock()
        # positive increment
        self.view.table.getSelectedCells.return_value = [(0, 0),
                                                         (0, 1)]
        self.view.table.getCellContents.return_value = "1000,2000,3000"
        self.view.increment.value.return_value = 1
        self.view.automatic_filling()
        calls = [mock.call(0, 1, "1001,2001,3001")]
        self.view.table.setCellContents.assert_has_calls(calls)

        # negative increment
        self.view.table.setCellContents.reset_mock()
        self.view.increment.value.return_value = -1
        self.view.automatic_filling()
        calls = [mock.call(0, 1, "999,1999,2999")]
        self.view.table.setCellContents.assert_has_calls(calls)

        # ranges
        self.view.table.setCellContents.reset_mock()
        self.view.table.getCellContents.return_value = "1000-2000,2000:3000,3000"
        self.view.increment.value.return_value = 1
        self.view.automatic_filling()
        calls = [mock.call(0, 1, "2001-3001,3001:4001,3001")]
        self.view.table.setCellContents.assert_has_calls(calls)

    def test_keyPressEvent(self):
        self.view.copy_selected_rows = mock.Mock()
        self.view.cut_selected_rows = mock.Mock()
        self.view.paste_rows = mock.Mock()
        self.view.erase_selected_cells = mock.Mock()

        QTest.keyClick(self.view, Qt.Key_C, Qt.ControlModifier)
        self.view.copy_selected_rows.assert_called_once()
        QTest.keyClick(self.view, Qt.Key_X, Qt.ControlModifier)
        self.view.cut_selected_rows.assert_called_once()
        QTest.keyClick(self.view, Qt.Key_V, Qt.ControlModifier)
        self.view.paste_rows.assert_called_once()
        QTest.keyClick(self.view, Qt.Key_Delete, Qt.NoModifier)
        self.view.erase_selected_cells.assert_called_once()

    def test_showDirectoryManager(self):
        self.view.show_directory_manager()
        self.mUserDir.ManageUserDirectories.assert_called_once()

    def test_setAvailableInstruments(self):
        self.view.instrumentselector = mock.Mock()
        self.view.set_available_instruments("test")
        self.view.instrumentselector.setTechniques.assert_called_once_with(
                "test")

    def test_setAvailableTechniques(self):
        self.view.techniqueSelector = mock.Mock()
        self.view.set_available_techniques(["test", "test"])
        self.view.techniqueSelector.clear.assert_called_once()
        self.view.techniqueSelector.addItems.assert_called_once_with(
                ["test", "test"])

    def test_setTechnique(self):
        self.view.techniqueSelector = mock.Mock()
        self.view.set_technique("test")
        self.view.techniqueSelector.setCurrentIndex.assert_called_once_with(
                "test")

    def test_setTable(self):
        self.view.set_table(["test", "test"])
        self.view.table.setColumnCount.assert_called_once_with(2)

    def test_fillTable(self):
        # empty contents
        self.view.fill_table([])
        self.view.table.addRow.assert_called_once()
        # contents
        self.view.table.reset_mock()
        self.view.fill_table([["test", "test"],
                              ["test", "test"]])
        self.view.table.addRow.assert_not_called()
        self.view.table.setRowCount.assert_called_once()
        calls = [mock.call(0, ["test", "test"]),
                 mock.call(1, ["test", "test"])]
        self.view.table.setRowContents.assert_has_calls(calls)

    def test_setProgress(self):
        self.view.set_progress(0, 100)
        self.assertEqual(self.view.progressBar.maximum(), 100)
        self.assertEqual(self.view.progressBar.value(), 0)
        self.view.set_progress(100, 0)
        self.assertEqual(self.view.progressBar.maximum(), 0)
        self.assertEqual(self.view.progressBar.value(), 100)

    def test_setDisabled(self):
        # only visual. here we just go through the function
        self.view.set_disabled(True)
        self.view.set_disabled(False)

    def test_setRowProcessing(self):
        self.view.set_row_processing(0)
        self.view.table.setRowBackground.assert_called_once()

    def test_setRowDone(self):
        self.view.set_row_done(0)
        self.view.table.setRowBackground.assert_called_once()

    def test_setRowError(self):
        self.view.set_row_error(0)
        self.view.table.setRowBackground.assert_called_once()

    def test_processingError(self):
        self.view.processing_error([])
        self.mMsgBox.assert_called_once()


if __name__ == "__main__":
    unittest.main()