#!/usr/bin/env python
# coding: utf-8
from __future__ import absolute_import, print_function, division
import numpy as np
import glob, os
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from .SensorPlane2ImagingSystem import SensorPlane2ImagingSystem
from .RawLighFieldSensorResponse import RawLighFieldSensorResponse
from .SimulationTruth import SimulationTruth
from .LightField import LightField
from .plot import Image as plt_Image
from .plot import RawLightFieldSensorResponse as plt_RawLightFieldSensorResponse
from .plot import LightField as plt_LightField

class Event(object):
    """
    number                      The event number in the run

    type                        A string as type indicator
                                "simulation" in case this event was simulated
                                "observation" in case this event was observed

    light_field                 The light field recorded by the plenoscope.
                                Created using the raw light field sensor  
                                response and the lixel statistics appropriate 
                                for the given sensor plane 2 imaging system 
                                orientation and position of this event.

    simulation_truth            If type == "simulation"
                                Additional 'true' information known from the 
                                simulation itself 

    raw_light_field_sensor_response     The raw light field sensor response
                                        of the plenoscope

    sensor_plane2imaging_system     The relative orientation and position of 
                                    the plenoscope's light field sensor with 
                                    respect to the plenoscope's imaging system 
                                    in the moment this event was recorded.
    """
    def __init__(self, path, lixel_statistics):
        self.__path = os.path.abspath(path)
        
        self.raw_light_field_sensor_response = RawLighFieldSensorResponse(
            os.path.join(self.__path, 'raw_light_field_sensor_response.bin'))

        self.sensor_plane2imaging_system = SensorPlane2ImagingSystem(
            os.path.join(self.__path, 'sensor_plane2imaging_system.bin'))

        try:
            self.simulation_truth = SimulationTruth(
                os.path.join(self.__path, 'simulation_truth'))
            self.type = "simulation"
        except(FileNotFoundError):
            self.type = "observation"

        self.light_field = LightField(
            self.raw_light_field_sensor_response,
            lixel_statistics,
            self.sensor_plane2imaging_system)

        self.number = int(os.path.basename(self.__path))

    def __str__(self):
        out = "Event("
        out+= "path='"+self.__path+"', "
        out+= "number "+str(self.number)+", "
        out+= "type '"+self.type
        out+="')\n"
        return out   

    def __repr__(self):
        return self.__str__()

    def plot(self):
        """
        This will open a plot showing:
        
        1   Directional intensity distribution accross the field of view
            (the classical IACT image)

        2   Positional intensity distribution on the principal aperture plane

        3   The arrival time distribution of photo equivalents accross all 
            lixels 

        4   The photo equivalent distribution accross all lixels
        """
        fig, axs = plt.subplots(2,2)
        plt.suptitle(self.simulation_truth.short_event_info())

        axs[0][0].set_title('directional image')
        plt_Image.add_pixel_image_to_ax(
            self.light_field.pixel_sum(), 
            axs[0][0])

        axs[0][1].set_title('principal aperture plane')
        plt_Image.add_paxel_image_to_ax(
            self.light_field.paxel_sum(interpolate_central_paxel=True), 
            axs[0][1])

        plt_LightField.add2ax_hist_arrival_time(
            self.light_field,
            axs[1][0])
        
        plt_RawLightFieldSensorResponse.add2ax_hist_intensity(
            self.raw_light_field_sensor_response,
            axs[1][1])
        plt_LightField.add2ax_hist_intensity(
            self.light_field,
            axs[1][1], color='green')
        raw_intensity_patch = mpatches.Patch(color='blue', label='raw')
        eff_corrected_intensity_patch = mpatches.Patch(color='green', label='efficiency corrected')
        axs[1][1].legend(handles=[raw_intensity_patch, eff_corrected_intensity_patch])
        plt.show()