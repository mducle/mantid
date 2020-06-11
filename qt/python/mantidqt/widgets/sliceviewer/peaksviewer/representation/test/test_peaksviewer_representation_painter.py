# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.

# std imports
import unittest
from unittest.mock import MagicMock

# 3rd party
from numpy.testing import assert_allclose

# local imports
from mantidqt.widgets.sliceviewer.peaksviewer.representation.painter \
    import MplPainter, Painted


class MplPainterTest(unittest.TestCase):

    # --------------- success tests -----------------
    def test_remove_calls_remove_on_artist(self):
        painter = MplPainter(MagicMock())
        mock_artist = MagicMock()

        painter.remove(mock_artist)

        mock_artist.remove.assert_called_once()

    def test_circle_draws_circle_patch(self):
        axes = MagicMock()
        painter = MplPainter(axes)
        x, y, radius = 1, 2, 0.8

        artist = painter.circle(x, y, radius)

        axes.add_patch.assert_called_once()
        self.assertTrue(artist is not None)

    def test_cross_draws_with_only_xy(self):
        axes = MagicMock()
        painter = MplPainter(axes)
        x, y, half_width = 1, 2, 0.1

        artist = painter.cross(x, y, half_width)

        axes.add_patch.assert_called_once()
        self.assertTrue(artist is not None)
        self._verify_patch(patch=axes.add_patch.call_args[0][0], nvertices=4, alpha=None)

    def test_cross_passes_kwargs_to_mpl(self):
        axes = MagicMock()
        painter = MplPainter(axes)
        x, y, half_width = 1, 2, 0.1
        alpha = 0.8

        painter.cross(x, y, half_width, alpha=alpha)

        axes.add_patch.assert_called_once()
        self._verify_patch(patch=axes.add_patch.call_args[0][0], nvertices=4, alpha=alpha)

    def test_ellipticalshell(self):
        axes = MagicMock()
        painter = MplPainter(axes)
        x, y, outer_width, outer_height, thick = 1, 2, 0.8, 1.0, 0.2
        alpha = 1.0

        painter.elliptical_shell(x, y, outer_width, outer_height, thick, alpha=alpha)

        axes.add_patch.assert_called_once()
        self._verify_patch(patch=axes.add_patch.call_args[0][0], nvertices=100, alpha=alpha)

    def test_shell_adds_patch(self):
        axes = MagicMock()
        painter = MplPainter(axes)
        x, y, outer_radius, thick = 1, 2, 0.8, 0.2

        painter.shell(x, y, outer_radius, thick)

        axes.add_patch.assert_called_once()
        self._verify_patch(patch=axes.add_patch.call_args[0][0], nvertices=100, alpha=None)

    def test_viewlimits_returns_xy_limits_so_xy_at_center(self):
        artist, axes, bbox, inv_trans = (MagicMock(), ) * 4
        # 1:1 transformation for simplicity
        inv_trans.transform.side_effect = lambda x: x
        axes.transData.inverted.return_value = inv_trans
        artist.get_extents.return_value = bbox
        bbox.min, bbox.max = (1., 1.5), (3., 3.5)
        painter = MplPainter(axes)

        xlim, ylim = painter.viewlimits(artist)

        self.assertEqual((0.6, 3.4), xlim)
        self.assertEqual((1.1, 3.9), ylim)

    def test_painted_viewlimits_returns_limits_for_last_artist(self):
        def create_mock_artist(extents):
            artist = MagicMock()
            bbox = MagicMock()
            bbox.min, bbox.max = extents
            artist.get_extents.return_value = bbox
            return artist

        axes, inv_trans = MagicMock(), MagicMock()
        # 1:1 transformation for simplicity
        inv_trans.transform.side_effect = lambda x: x
        axes.transData.inverted.return_value = inv_trans
        artists = create_mock_artist(((1., 1.5), (3., 3.5))), create_mock_artist(
            ((1.1, 1.3), (2.9, 3.6)))
        painter = MplPainter(axes)
        painted = Painted(painter, artists)

        xlim, ylim = painted.viewlimits()

        assert_allclose((0.64, 3.36), xlim)
        assert_allclose((0.84, 4.06), ylim)

    # --------------- failure tests -----------------
    def test_construction_raises_error_if_given_non_axes_instance(self):
        self.assertRaises(ValueError, MplPainter, 1)

    # private helpers
    def _verify_patch(self, patch, nvertices, alpha):
        path = patch.get_path()
        self.assertEqual(nvertices, len(path.vertices))
        self.assertEqual(nvertices, len(path.codes))
        if alpha is not None:
            self.assertEqual(alpha, patch.get_alpha())


if __name__ == '__main__':
    unittest.main()
