""" ========================================================================
 * ColorSignature.py
 *
 *  Created on: May 2016
 *  Author: susan
 *
 *  The ColorSignature class represents a feature that represents the overall colors in the picture.  Rather than representing them
 *  as a histogram (another choice to be considered), this represents a matrix of color values, each one of which is the average
 *  of color values in a 20x20 pixel region of the original image.  These matrices can be compared together using Euclidean distance
 *  metrics
========================================================================="""

# TODO: Switch this to build a pyramid of downscaled images?
# Not sure this is doing the right thing, not averaging colors, interpolating by those near the selected points.
import cv2
import FeatureType
import OutputLogger


class ColorSignature(FeatureType.FeatureType):
    """Holds a "color signature" matrix that contains sample colors from across the region, one per each 20x20 pixel part of the original. Could be better."""
    def __init__(self, image, logger):
        """Takes in an image and a logger and builds the color signature. Also
        initializes other instance * variables that are needed"""
        FeatureType.FeatureType.__init__(self, image, logger, 800.0, 2500.0)
        self.tileSize = 20  # TODO: this should be a function of image width and height
        (rs, cs, d) = image.shape
        self.imRows = rs
        self.imCols = cs
        self.imageArea = rs * cs
        self.rgbSignature = self._extractColorSignature(image)

    
    def evaluateSimilarity(self, otherSig):
        """Given another color signature, evaluate the similarity. This is
        done using the cv2.norm function, which computes a variation on
        Euclidean distance The return value seems to range up to 2000 or so,
        unless the signatures can't be compared.
        NOTE: Relative norm doesn't seem to be working in OPENCV python, at least I can't
        figure out how, so just using ordinary norm...
        """
        if self.rgbSignature.shape != otherSig.rgbSignature.shape:
            return -1.0
        relNorm = cv2.norm(self.rgbSignature, otherSig.rgbSignature, 
                            cv2.NORM_L2)
        #self.logger.log("Color Signature: Relative norm = " + str(relNorm))

        
        return self._normalizeSimValue(relNorm)
    
    
    def displayFeaturePics(self, windowName, startX, startY):
        """Given a window name and a starting location on the screen, this creates
        an image that represents the color signature and displays it."""
        (rows, cols, d) = self.rgbSignature.shape
        dispWidth = cols * self.tileSize
        dispHeight = rows * self.tileSize
        rgbSigImage = cv2.resize(self.rgbSignature, dsize = (dispWidth, dispHeight), interpolation= cv2.INTER_AREA)
        cv2.namedWindow(windowName)
        cv2.moveWindow(windowName, startX, startY)
        cv2.imshow(windowName, rgbSigImage)
    
    
    def _extractColorSignature(self, image):
        """Given an image, this divides the image into 20x20 pixel sections. For
        each section. It computes the average color and stores that into the
        signature matrix."""
        matHeight = self.imRows // self.tileSize
        matWidth = self.imCols // self.tileSize
        colorSig = cv2.resize(image, dsize = (matWidth, matHeight), interpolation= cv2.INTER_AREA)
        return colorSig
        