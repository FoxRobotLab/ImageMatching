#!/usr/bin/env python
import numpy as np
import matplotlib.cm as cm
#import matplotlib.mlab as mlab
import matplotlib.pylab as pylab
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def draw3d(filename):
    imSimCRec = pylab.csv2rec(filename)
    imSimCArray = np.array(imSimCRec.tolist())
    fig1 = plt.figure()
    ax1 = fig1.gca(projection='3d')
    X = np.arange(20, 170, 1)
    Y = np.arange(20, 170, 1)
    X, Y = np.meshgrid(X, Y)
    ax1.plot_surface(X, Y, imSimCArray, cmap=cm.coolwarm)# rstride=8, cstride=8, alpha=0.3)

    ax1.set_xlabel('X')
    ax1.set_xlim(20, 170)
    ax1.set_ylabel('Y')
    ax1.set_ylim(20, 170)

def draw2d(filename):
    imSimHRec = pylab.csv2rec(filename)
    imSimHArray = np.array(imSimHRec.tolist())
    fig2 = plt.figure()
    im2 = plt.imshow(imSimHArray, interpolation="none", )


draw3d("imSimHough.csv")
draw3d("imSimColor.csv")
plt.show()

