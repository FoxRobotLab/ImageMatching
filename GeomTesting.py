"""
4.9 3.2
4.5 3.3
4.9 1.7
4.5 3.6
3.5 4.4
4.7 2.8
4.9 4.5

4.2 3.1
"""

import numpy as np
import cv2


def perpDistance(line1, line2):
     (px, py) = np.mean( line1.reshape( (2, 2) ), axis = 0 )
     print "middle point", px, py
     [x1, y1, x2, y2] = line2
     if x1 == x2:  # special case for vertical lines
	  return (abs(px - x1), int(px), int(py))
     slope = float((y2 - y1 )) / (x2 - x1)
     print "slope of line", line2, "is", slope
     A = slope
     B = -1.0
     C = y1 - (slope * x1)
     print "coeffs", A, B, C
     print "numerator", abs(A*px + B*py + C)
     pDist = abs(A * px + B * py + C) / np.sqrt(A**2 + B**2)
     
     return (pDist, int(px), int(py))
 

def testPD():

     for pt1 in [[100, 100], [100, 250], [200, 250], [300, 100]]:
	  for pt2 in [[200, 350], [250, 50], [350, 250]]:
	       line1 = np.array( pt1 + pt2, dtype=np.int32)
	       
	       for pt1B in [[100, 100], [100, 250], [200, 250], [300, 100]]:
		    for pt2B in [[200, 350], [250, 50], [350, 250]]:
			 line2 = np.array( pt1B + pt2B, dtype=np.int32)
			 (pDist1, px1, py1) = perpDistance(line1, line2)
			 (pDist2, px2, py2) = perpDistance(line2, line1)
			 print line1, line2, pDist1, pDist2
			 newPic = np.zeros( (400, 400, 3), dtype=np.uint8)
			 cv2.line(newPic, (line1[0], line1[1]), (line1[2], line1[3]), (255, 255, 0))
			 cv2.line(newPic, (line2[0], line2[1]), (line2[2], line2[3]), (255, 0, 255))
			 cv2.circle(newPic, (px1, py1), 3, (0, 255, 0))
			 cv2.circle(newPic, (px2, py2), 3, (0, 0, 255))
			 cv2.imshow("Test", newPic)
			 x = cv2.waitKey(0)
			 if chr(x & 0xFF) == 'q':
			      return
	  
     cv2.destroyAllWindows()
     
     

     


cluster1 = np.array( [[  63., 50., 282., 58., 177.90793372],
                      [  63., 51., 127., 54., 177.31622484]] )
cluster2 = np.array( [[ 154.,  61., 319.,  23.,  12.96925346],
                      [ 155.,  63., 176.,  59.,  10.78429787]] )
cluster3 = np.array( [[ 6., 88., 177., 88., 0.],
                      [ 6., 87., 133., 87., 0.],
                      [ 61., 89., 175., 89., 0.],
                      [ 61., 90., 173., 90., 0.]] )
cluster4 = np.array( [[ 8., 47., 8., 239., 90.],
                      [ 6., 47., 6., 239., 90.],
                      [ 7., 47., 7., 239., 90.]] )
cluster5 = np.array( [[ 166., 172., 243., 183., 171.86989765],
                      [   6., 147., 144., 169., 170.94211187],
                      [ 185., 172., 258., 186., 169.14358665],
                      [ 166., 173., 195., 177., 172.1466867 ],
                      [ 148., 167., 281., 192., 169.35433666],
                      [  95., 155., 133., 161., 171.02737339],
                      [ 200., 172., 228., 177., 169.87532834],
                      [  97., 154., 138., 161., 170.31121344],
                      [  89., 156., 123., 161., 171.63411388],
                      [  97., 163., 131., 168., 171.63411388],
                      [ 116., 155., 158., 163., 169.21570213],
                      [ 120., 165., 154., 169., 173.29016319],
                      [  32., 151., 104., 161., 172.0928373 ],
                      [ 214., 180., 299., 192., 171.96428929],
                      [  84., 158., 104., 160., 174.28940686],
                      [ 190., 172., 244., 180., 171.57303098]] )
cluster6 = np.array( [[   6.,  84., 128.,  86., 179.06080905],
                      [  53.,  83., 133.,  84., 179.28384005],
                      [  57.,  74., 110.,  80., 173.54118362],
                      [  59.,  79., 178.,  85., 177.11358146],
                      [  58.,  78.,  96.,  79., 178.49256424],
                      [  80.,  76., 110.,  79., 174.28940686],
                      [  61.,  82., 132.,  83., 179.19307054]] )
cluster7 = np.array( [[  54.,  57., 163.,  50.,   3.67449952]] )


clusters = [cluster1, cluster2, cluster3, cluster4, cluster5, cluster6, cluster7]

def rotateAxes(line, angle, toNew = True):
     """Takes in a line (which has two (x, y) endpoints and potentially
     an angle, but the angle is ignored. It also takes in the angle to rotate the axes by.
     It has an optional argument, toNew, that is true when we are converting a
     line from the original axes to the new rotated ones, and it is false when
     we are converting a line in the rotated axes back to the original ones.
     It computes and returns the new line."""
     theta = np.radians(angle)
     rotToNew = np.array([[np.cos(theta), -np.sin(theta)], 
                          [np.sin(theta),  np.cos(theta)]])
     rotToOld = rotToNew.T
     if toNew:
	  rotMatr = rotToNew
     else:
	  rotMatr = rotToOld
     oldPt1 = line[0:2].reshape( (-1, 1) )
     newPt1 = np.matmul(rotMatr, oldPt1)
     oldPt2 = line[2:4].reshape( (-1, 1) )
     newPt2 = np.matmul(rotMatr, oldPt2)
     newLine = np.concatenate( (newPt1.ravel(), newPt2.ravel()) )
     return newLine

def mergeCluster(clust):
     """Takes in a cluster, a matrix where rows contain lines given as
     two (x, y) endpoints plus the angle of the line, and it computes
     a single representative line to take the place of the cluster.
     Returns that line."""
     print clust
     if len(clust) == 1:
	  print("One Line, nothing to do")
	  return clust[0]
     avgAng = np.mean(clust[:,4])
     print "Average Angle =", avgAng
     def doRot(line):
	  return rotateAxes(line, avgAng)
     linesNewAxis = np.apply_along_axis(doRot, 1, clust)
     xValues = linesNewAxis[:, [0, 2]]
     yValues = linesNewAxis[:, [1, 3]]
     minX = xValues.min()
     maxX = xValues.max()
     avgY = yValues.mean()
     newLine = np.array( [minX, avgY, maxX, avgY] )
     newOrig = rotateAxes(newLine, avgAng, toNew = False)
     print newOrig
     newOrig = newOrig.astype(np.int32)
     blank = np.zeros( (400, 400, 3), dtype=np.uint8)
     for i in range(len(clust)):
	  line = clust[i].astype(np.int32)
	  cv2.line(blank, (line[0], line[1]), (line[2], line[3]), (0, 255, 255))
	  cv2.imshow("FindAvg", blank)
     cv2.waitKey(0)
     cv2.line(blank, (newOrig[0], newOrig[1]), (newOrig[2], newOrig[3]), (255, 255, 0))
     cv2.imshow("FindAvg", blank)
     cv2.waitKey(0)
     return np.append(newOrig, avgAng)
				     
				     
				     
for cluster in clusters:
     newLine = mergeCluster(cluster)
     print newLine
     