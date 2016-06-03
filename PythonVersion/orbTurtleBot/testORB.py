# -*- coding: utf-8 -*-
"""
Created on Mon May 09 14:02:30 2016

@author: mju
"""

import numpy as np
#import math
import os
from operator import itemgetter
import sys

import cv2


def tryToMatchFeatures(orb, img1, pointInfo, img2):
    kp2, des2 = pointInfo

    # find the keypoints and descriptors with ORB
    kp1, des1 = orb.detectAndCompute(img1,None)

    if des1 is None or des2 is None:
        return [], None, None
    elif len(des1) == 0 or len(des2) == 0:
        print("it thinks the key descriptions are empty")
        return [], None, None

    # BFMatcher with default params
    bf = cv2.BFMatcher()
    matches = bf.match(des1,des2)

    sortedMatches = sorted(matches, key = lambda x: x.distance)
    #for mat in matches:
    #    print mat.distance
    goodMatches = [mat for mat in matches if mat.distance < 300]
    print "Good match number:", len(goodMatches)
    
    matchImage = cv2.drawMatches(img1, kp1, img2, kp2, goodMatches,  
        None, matchColor = (255, 255, 0), singlePointColor=(0, 0, 255))
    cv2.imshow("Match Image", matchImage)
    cv2.waitKey(0)
    
    return goodMatches, kp1, kp2

    
def findImage(img, properties, itemsSought, j):
    
    colorImage = img
    
    #properties[i][2] holds the list of good points and the two sets of keypoints (for drawing)
    orb = cv2.ORB_create()
    for i in range (0, len(itemsSought)):
        goodPoints, targetKeypts, refKeypts = tryToMatchFeatures(orb, img, properties[i][1], properties[i][0])
        properties[i][2].append(goodPoints)
        properties[i][2].append(targetKeypts)
        properties[i][2].append(refKeypts)
    
        #properties[i][3] holds the len(goodPoints) - the number of good points of that item
        numGoodPoints = len(properties[i][2][0])
        properties[i][3] = numGoodPoints

    getcount = itemgetter(3)
    scores = map(getcount, properties)
    print(scores)
    max_index, max_value = max(enumerate(scores), key=itemgetter(1))
    
    if max_value > 90:
        print('Run ' + str(j) + ': The '+ str(itemsSought[max_index]) + ' sign was detected, with ' + str(max_value) + ' points')
    else:
        print('No sign was detected')
        
    #cleanup
    for i in range (0, len(itemsSought)):
        properties[i][2] = []
        properties[i][3] = 0
        
def initRefs(itemsSought):
    properties = [] #2D array used to store info on each item: item i is properties[i]
    
    orb = cv2.ORB_create()
    
    #properties[i][0] holds the reference image for the item
    for i in range (0, len(itemsSought)):
        properties.append([None, [], [], 0])
        
        filename = itemsSought[i] + '.jpg'
        path = os.path.join("refs", filename)
        properties[i][0] = cv2.imread(path, 0)
        if properties[i][0] is None:
            print("Reference image", itemsSought[i], "not found")

        #properties[i][1] holds the keypoints and their descriptions for the reference image
        keypoints, descriptions = orb.detectAndCompute(properties[i][0], None)
        properties[i][1].append(keypoints)
        properties[i][1].append(descriptions)
        
    return properties
    
#run this if you wanna test the feature recognition using still images
def scanImages():
    itemsSought = ['rassilon', 'flower', 'knot', 'sign', 'shield']
    properties = initRefs(itemsSought)
    
    for j in range (1, 70): #hard-coded for number of images you're checking
        filename = 'cap' + str(j) + '.jpg'
        path = os.path.join("caps", filename)
        img = cv2.imread(path, 0)
        if img is None:
            print("Target image", filename, "not found")
        cv2.imshow("Come ON", img)
        cv2.waitKey(0)
        findImage(img, properties, itemsSought, j)

scanImages()