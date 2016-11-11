import unittest
import mantid
import math
from SANS2.State.StateDirector.TestDirector import TestDirector
from SANS2.Scale.ScaleHelpers import (DivideByVolumeFactory, DivideByVolumeISIS, NullDivideByVolume,
                                      MultiplyByAbsoluteScaleFactory, MultiplyByAbsoluteScaleLOQ,
                                      MultiplyByAbsoluteScaleISIS)
from SANS2.Common.SANSFunctions import create_unmanaged_algorithm
from SANS2.Common.SANSEnumerations import (SampleShape, SANSFacility)
from SANS2.State.StateBuilder.SANSStateScaleBuilder import get_scale_builder
from SANS2.State.StateBuilder.SANSStateDataBuilder import get_data_builder


class ScaleHelperTest(unittest.TestCase):
    @staticmethod
    def _get_workspace(width=1.0, height=1.0, thickness=1.0, shape=1):
        sample_name = "CreateSampleWorkspace"
        sample_options = {"WorkspaceType": "Histogram",
                          "NumBanks": 1,
                          "BankPixelWidth": 1,
                          "OutputWorkspace": "test"}
        sample_alg = create_unmanaged_algorithm(sample_name, **sample_options)
        sample_alg.execute()
        workspace = sample_alg.getProperty("OutputWorkspace").value

        sample = workspace.sample()
        sample.setGeometryFlag(shape)
        sample.setThickness(thickness)
        sample.setWidth(width)
        sample.setHeight(height)
        return workspace

    def test_that_divide_strategy_is_selected_for_isis_instrument_and_is_not_can(self):
        # Arrange
        test_director = TestDirector()
        state_isis = test_director.construct()
        divide_factory = DivideByVolumeFactory()
        # Act
        divider = divide_factory.create_divide_by_volume(state_isis, is_can=False)
        # Arrange
        self.assertTrue(isinstance(divider, DivideByVolumeISIS))

    def test_that_null_divide_strategy_is_selected_for_isis_instrument_and_is_can(self):
        # Arrange
        test_director = TestDirector()
        state_isis = test_director.construct()
        divide_factory = DivideByVolumeFactory()
        # Act
        divider = divide_factory.create_divide_by_volume(state_isis, is_can=True)
        # Arrange
        self.assertTrue(isinstance(divider, NullDivideByVolume))

    def test_that_divide_uses_settings_from_workspace(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("SANS2D00022024")
        data_state = data_builder.build()

        scale_builder = get_scale_builder(data_state)
        scale_state = scale_builder.build()

        test_director = TestDirector()
        test_director.set_states(scale_state=scale_state, data_state=data_state)
        state = test_director.construct()

        divide_factory = DivideByVolumeFactory()
        divider = divide_factory.create_divide_by_volume(state, is_can=False)

        width = 10.
        height = 5.
        thickness = 2.
        shape = 0

        workspace = ScaleHelperTest._get_workspace(width, height, thickness, shape)

        # Act
        output_workspace = divider.divide_by_volume(workspace, scale_state)

        # Assert
        expected_volume = height * math.pi * math.pow(width, 2) / 4.0
        expected_value = 0.3 / expected_volume
        data_y = output_workspace.dataY(0)
        self.assertEquals(data_y[0], expected_value)

    def test_that_divide_uses_settings_from_state_if_they_are_set(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("SANS2D00022024")
        data_state = data_builder.build()

        scale_builder = get_scale_builder(data_state)
        width = 10.
        height = 5.
        thickness = 2.
        scale_builder.set_shape(SampleShape.CylinderAxisAlong)
        scale_builder.set_thickness(thickness)
        scale_builder.set_width(width)
        scale_builder.set_height(height)
        scale_state = scale_builder.build()

        test_director = TestDirector()
        test_director.set_states(scale_state=scale_state, data_state=data_state)
        state = test_director.construct()

        divide_factory = DivideByVolumeFactory()
        divider = divide_factory.create_divide_by_volume(state, is_can=False)

        workspace = ScaleHelperTest._get_workspace()

        # Act
        output_workspace = divider.divide_by_volume(workspace, scale_state)

        # Assert
        expected_volume = thickness * math.pi * math.pow(width, 2) / 4.0
        expected_value = 0.3 / expected_volume
        data_y = output_workspace.dataY(0)
        self.assertEquals(data_y[0], expected_value)

    def test_that_correct_scale_strategy_is_selected_for_non_loq_isis_instrument(self):
        # Arrange
        test_director = TestDirector()
        state_isis = test_director.construct()
        absolute_multiply_factory = MultiplyByAbsoluteScaleFactory()
        # Act
        multiplier = absolute_multiply_factory.create_multiply_by_absolute(state_isis)
        # Arrange
        self.assertTrue(isinstance(multiplier, MultiplyByAbsoluteScaleISIS))

    def test_that_correct_scale_strategy_is_selected_for_loq(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("LOQ74044")
        data_state = data_builder.build()

        scale_builder = get_scale_builder(data_state)
        scale_state = scale_builder.build()

        test_director = TestDirector()
        test_director.set_states(scale_state=scale_state, data_state=data_state)
        state_loq = test_director.construct()

        absolute_multiply_factory = MultiplyByAbsoluteScaleFactory()
        # Act
        multiplier = absolute_multiply_factory.create_multiply_by_absolute(state_loq)

        # Assert
        self.assertTrue(isinstance(multiplier, MultiplyByAbsoluteScaleLOQ))

    def test_that_correct_scale_strategy_is_selected_for_loq(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("LOQ74044")
        data_state = data_builder.build()

        scale_builder = get_scale_builder(data_state)
        scale_builder.set_scale(2.4)
        scale_state = scale_builder.build()

        test_director = TestDirector()
        test_director.set_states(scale_state=scale_state, data_state=data_state)
        state_loq = test_director.construct()

        absolute_multiply_factory = MultiplyByAbsoluteScaleFactory()
        multiplier = absolute_multiply_factory.create_multiply_by_absolute(state_loq)

        workspace = self._get_workspace()

        # Act
        output_workspace = multiplier.multiply_by_absolute_scale(workspace, state_loq.scale)

        # Assert
        expected_value = 0.3 * 2.4 / math.pi
        data_y = output_workspace.dataY(0)
        self.assertEquals(data_y[0], expected_value)


if __name__ == '__main__':
    unittest.main()
