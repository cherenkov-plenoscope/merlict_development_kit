#!/usr/bin/env python
from __future__ import absolute_import, print_function, division
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.patches import RegularPolygon
from matplotlib.patches import Circle
from matplotlib.collections import PatchCollection
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import gridspec
import mpl_toolkits.mplot3d.art3d as art3d
from tqdm import tqdm
from mpl_toolkits.axes_grid1 import make_axes_locatable

from lightfield import PlenoscopeLightFieldCalibration
from lightfield import LightField


def add_to_ax(ax, I, px, py, colormap='viridis', hexrotation=30, vmin=None, vmax=None):

    if vmin is None:
        vmin = I.min()
    if vmax is None:
        vmax = I.max()
    fov = np.abs(px).max() * 1.05
    Area = fov * fov
    bin_radius = 1.15 * np.sqrt((Area/I.shape[0]))

    nfov = fov + bin_radius
    ax.set_xlim([-nfov, nfov])
    ax.set_ylim([-nfov, nfov])
    ax.set_aspect("equal")
    
    orientation=np.deg2rad(hexrotation)

    patches = []
    for d in range(I.shape[0]):
        patches.append(
            RegularPolygon(
                (px[d], py[d]),
                numVertices=6,
                radius=bin_radius,
                orientation=orientation
            )
        )

    p = PatchCollection(patches, cmap=colormap, alpha=1, edgecolor='none')

    p.set_array(I)
    p.set_clim([vmin, vmax])

    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.05)
    plt.colorbar(p, cax=cax)


    ax.add_collection(p)
    ax.set_aspect('equal')

    return p 

class FigureSize():

    def __init__(self, cols, rows, dpi):
        self.cols = cols
        self.rows = rows
        self.dpi = dpi
        self.hight = self.rows/self.dpi
        self.width = self.cols/self.dpi

def get_n_highest(I, n):
    flat_idxs = np.argsort(I.flatten())[-n:]
    flat_mask = np.zeros(shape=I.shape[0]*I.shape[1], dtype=bool)
    flat_mask[flat_idxs] = True
    return flat_mask.reshape(I.shape)

def save_principal_aperture_arrival_stack(lf, steps=6, n_channels=137, outprefix='aperture3D'):

    plt.rcParams.update({'font.size': 12})
    plt.rc('text', usetex=True)
    plt.rc('font', family='serif')

    fsz = FigureSize(1920, 1080, dpi=240)
    fig = plt.figure(figsize=(fsz.width, fsz.hight))
    ax = fig.gca(projection='3d')

    above_threshold = get_n_highest(lf.I, n_channels)

    min_t = lf.t[above_threshold].min()
    dur_t = lf.t[above_threshold].max() - min_t
    max_I = lf.I[above_threshold].max()

    for pax in range(lf.n_paxel):
        for pix in range(lf.n_pixel):
            if above_threshold[pix, pax]:
                d = (lf.t[pix, pax]-min_t)

                xpix = lf.pixel_pos['x'][pix]
                ypix = lf.pixel_pos['y'][pix]

                incident = np.array([
                    xpix,
                    ypix,
                    np.sqrt(1. -xpix*xpix - ypix*ypix)
                ])

                x = np.array([
                    lf.paxel_pos['x'][pax], 
                    lf.paxel_pos['x'][pax] + d*incident[0]
                ])
                y = np.array([
                    lf.paxel_pos['y'][pax], 
                    lf.paxel_pos['y'][pax] + d*incident[1]
                ])
                z = np.array([
                    0., 
                    d*incident[2]
                ])
                #ax.plot(x, y, z, 'b')
                I = lf.I[pix, pax]/max_I
                ax.scatter(x[1], y[1], z[1], lw=0, s=35., alpha=I**2)

    p = Circle((0, 0), 25, edgecolor='k', facecolor='none', lw=1.)
    ax.add_patch(p)
    art3d.pathpatch_2d_to_3d(p, z=0, zdir="z")

    ax.set_xlim(-25, 25)
    ax.set_ylim(-25, 25)
    ax.set_zlim(0, dur_t)
    ax.set_xlabel('X/m')
    ax.set_ylabel('Y/m')
    ax.set_zlabel('t/s')

    ndigits = int(np.ceil(np.log10(steps)))
    azimuths = np.linspace(0., 360., steps, endpoint=False)
    for i, azimuth in enumerate(azimuths):
        ax.view_init(elev=5., azim=azimuth)
        plt.subplots_adjust(left=0.01, right=0.99, top=0.99, bottom=0.01)
        plt.savefig(outprefix+'_'+str(i).zfill(ndigits)+".png", dpi=fsz.dpi)

    plt.close()

def plot_sum_event(path, thresh=0):
    plt.ion()
    plf.load_epoch_160310(path)
    lf = LightField(plfc, plf)

    I = lf.I
    I[I<thresh]=0
    fig, (ax_dir, ax_pap) = plt.subplots(1,2)
    add_to_ax(ax_dir, np.sum(I, axis=1), lf.pixel_pos['x'], lf.pixel_pos['y'])
    add_to_ax(ax_pap, np.sum(I, axis=0), lf.paxel_pos['x'], lf.paxel_pos['y'])

def refocus(lf, wanted_object_distance):
    alpha = get_alpha(wanted_object_distance)
    refocused_image = np.zeros(lf.n_pixel)
    focal_length = 75.0

    dx = np.tan(lf.pixel_pos['x'])*focal_length
    dy = np.tan(lf.pixel_pos['y'])*focal_length

    px = lf.paxel_pos['x']
    py = lf.paxel_pos['y']

    dx_tick = px[:, np.newaxis] + (dx[np.newaxis, :] - px[:, np.newaxis])/alpha  # (127, 8400)
    dy_tick = py[:, np.newaxis] + (dy[np.newaxis, :] - py[:, np.newaxis])/alpha

    dx_des = np.arctan(dx_tick/focal_length)
    dy_des = np.arctan(dy_tick/focal_length)

    # (127*8400, 2)
    desired_x_and_y = np.zeros((np.prod(dx_des.shape), 2))
    desired_x_and_y[:, 0] = dx_des.flatten()
    desired_x_and_y[:, 1] = dy_des.flatten()

    nearest_pixel_distances, nearest_pixel_indices = lf.pixel_pos_tree.query(desired_x_and_y)

    summanden = lf.I[nearest_pixel_indices, np.arange(lf.n_paxel).repeat(lf.n_pixel)]
    summanden = summanden.reshape(lf.n_paxel, lf.n_pixel)
    refocused_image = summanden.sum(axis=0)

    return refocused_image

def get_alpha(wanted_object_distance):
    focal_length = 75.0
    initial_object_distance = 10e3
    initial_image_distance = 1/(1/focal_length - 1/initial_object_distance)
    alpha = 1/initial_image_distance * 1/((1/focal_length) - (1/wanted_object_distance))
    return alpha

def save_sum_projections(lf, outpath, thresh=0):
    I = lf.I
    I[I<thresh]=0

    plt.rcParams.update({'font.size': 20})
    plt.rc('text', usetex=True)
    plt.rc('font', family='serif')

    fig, (ax_dir, ax_pap) = plt.subplots(1, 2, figsize=(16,9))

    ax_dir.set_xlabel('x/deg')
    ax_dir.set_ylabel('y/deg')   
    ax_dir.spines['right'].set_visible(False)
    ax_dir.spines['top'].set_visible(False)
    add_to_ax(ax_dir, np.sum(I, axis=1), np.rad2deg(lf.pixel_pos['x']), np.rad2deg(lf.pixel_pos['y']))

    gp = lf.paxel_eff>0.4
    ax_pap.set_xlabel('x/m')
    ax_pap.set_ylabel('y/m')   
    ax_pap.spines['right'].set_visible(False)
    ax_pap.spines['top'].set_visible(False)
    add_to_ax(ax_pap, np.sum(I[:,gp], axis=0), lf.paxel_pos['x'][gp], lf.paxel_pos['y'][gp], hexrotation=0.0)

    plt.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.05)
    plt.savefig(outpath+".png", dpi=120)
    plt.close()

def save_refocus_stack(lf, obj_dist_min, obj_dist_max, steps, outprefix='refocus', use_absolute_scale=True):

    plt.rcParams.update({'font.size': 12})
    plt.rc('text', usetex=True)
    plt.rc('font', family='serif')

    object_distances = np.logspace(
        np.log10(obj_dist_min),
        np.log10(obj_dist_max),
        steps
    )

    images = [refocus(lf, object_distance) for object_distance in object_distances]
    if use_absolute_scale:
        vmin=np.array(images).min()
        vmax=np.array(images).max()
    else:
        vmin, vmax = None, None

    for i in range(len(images)):
        image = images[i]
        object_distance = object_distances[i]

        fig = plt.figure(figsize=(7, 6)) 
        gs = gridspec.GridSpec(1, 2, width_ratios=[1, 6]) 
        ax0 = plt.subplot(gs[0])
        ax0.set_xlim([0, 1])
        ax0.set_ylim([0, obj_dist_max/1e3])
        ax0.yaxis.tick_left()
        ax0.set_ylabel('object distance/km')
        ax0.spines['right'].set_visible(False)
        ax0.spines['top'].set_visible(False)
        ax0.spines['bottom'].set_visible(False)
        ax0.xaxis.set_visible(False)

        ax1 = plt.subplot(gs[1])
        ax1.set_xlabel('x/deg')
        ax1.set_ylabel('y/deg')
        ax1.spines['right'].set_visible(False)
        ax1.spines['top'].set_visible(False)
        ax1.set_aspect('equal')  
        ax0.plot([0, .5], [object_distance/1e3, object_distance/1e3], linewidth=5.0)
        ax0.text(0.0, -1.0, format(object_distance/1e3, '.2f')+r'\,km')

        add_to_ax(
            ax1, 
            image,
            np.rad2deg(lf.pixel_pos['x']),
            np.rad2deg(lf.pixel_pos['y']),
            vmin=vmin,
            vmax=vmax
        )
        ndigits = int(np.ceil(np.log10(steps)))
        plt.savefig(outprefix+'_'+str(i).zfill(ndigits)+".png", dpi=180)

        plt.close()

class Path():
    def __init__(self, fullpath):
        self.full = fullpath
        self.path = os.path.split(fullpath)[0]
        self.name = os.path.split(fullpath)[1]
        self.name_wo_ext = os.path.splitext(self.name)[0]
        self.ext = os.path.splitext(fullpath)[1]

import subprocess
import os
from glob import glob

def save_sum(evt_path):
    evt_path = Path(evt_path)
    plf.load_epoch_160310(evt_path.full)
    lf = LightField(plfc, plf)   
    save_sum_projections(lf, evt_path.path+'/'+evt_path.name_wo_ext+'_sum_projection')

def save_refocus_gif(evt_path, steps=10, use_absolute_scale=True):
    evt_path = Path(evt_path)
    lf = LightField(evt_path.full, plfc)

    work_dir = evt_path.path+'/'+evt_path.name_wo_ext+'_refocus_temp'
    mkdir_ret = subprocess.call(['mkdir', work_dir])
    if mkdir_ret != 0:
        return

    save_refocus_stack(
        lf,
        0.75e3,
        15e3,
        steps,
        outprefix=work_dir+'/'+'refocus',
        use_absolute_scale=use_absolute_scale)
    subprocess.call(
        ['convert', 
        work_dir+'/'+'refocus_*.png', 
        '-set' ,'delay', '10', 
        '-reverse',
        work_dir+'/'+'refocus_*.png',
        '-set' ,'delay', '10', 
        '-loop', '0',
        evt_path.path+'/'+evt_path.name_wo_ext+'_refocus.gif'
        ])
    subprocess.call(['rm', '-r', '-f', work_dir])


def save_aperture_photons_gif(evt_path, steps=72):

    T=20 #seconds
    spf = T/steps
    s100pf = int(spf*100.0)

    evt_path = Path(evt_path)
    lf = LightField(evt_path.full, plfc)

    work_dir = evt_path.path+'/'+evt_path.name_wo_ext+'_aperture3D_temp'
    mkdir_ret = subprocess.call(['mkdir', work_dir])
    if mkdir_ret != 0:
        return

    save_principal_aperture_arrival_stack(lf, steps=steps, n_channels=512, outprefix=work_dir+'/'+'aperture3D')
    subprocess.call(
        ['convert', 
        work_dir+'/'+'aperture3D_*.png', 
        '-set' ,'delay', str(s100pf),
        '-loop', '0',
        evt_path.path+'/'+evt_path.name_wo_ext+'_aperture3D.gif'
        ])
    subprocess.call(['rm', '-r', '-f', work_dir])


plfc = PlenoscopeLightFieldCalibration('sub_pixel_statistics.txt')
lf = LightField('1.txt', plfc)
save_refocus_gif("./1.txt")
