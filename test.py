

import numpy as np
import cv2
from matplotlib import pyplot as plt
import matplotlib.cm as cm

img = cv2.imread("TestImages/water_coins.jpg")
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

kernel = np.ones((3, 3), np.uint8)
opening = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel, iterations = 2)

sure_bg = cv2.dilate(opening, kernel, iterations = 3)

dist_trans = cv2.distanceTransform(opening, cv2.DIST_L2, 5)
ret, sure_fg = cv2.threshold(dist_trans, 0.7 * dist_trans.max(), 255, 0)

sure_fg = np.uint8(sure_fg)
unknown = cv2.subtract(sure_bg, sure_fg)

ret, markersB = cv2.connectedComponents(sure_fg)
markersB = markersB + 1
markersB[unknown == 255] = 0

cv2.imshow("Original", img)
cv2.moveWindow("Original", 0, 0)
cv2.imshow("Gray", gray)
cv2.moveWindow("Gray", 400, 0)
cv2.imshow("Opening", opening)
cv2.moveWindow("Opening", 800, 0)

cv2.imshow("Sure BG", sure_bg)
cv2.moveWindow("Sure BG", 0, 300)

dTrans = dist_trans.astype(np.uint8)
dTrans = (dTrans * 255) / dTrans.max()

cv2.imshow("D Trans", dTrans)
cv2.moveWindow("D Trans", 1200, 0)
cv2.imshow("Sure FG", sure_fg)
cv2.moveWindow("Sure FG", 400, 300)
cv2.imshow("Unknown", unknown)
cv2.moveWindow("Unknown", 800, 300)

plt.imshow(markersB, cmap=cm.hot)

plt.show()


cv2.waitKey(0)

markersA = cv2.watershed(img, markersB)
img[markersA == -1] = [255, 0, 0]


plt.imshow(markersA, cmap=cm.hot)

cv2.waitKey(0)
cv2.destroyAllWindows()
