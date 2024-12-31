import cv2
import numpy as np
import os

img = cv2.imread("examples/mothOne.png")
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
# np.array([15, 50, 180])
# np.array([40, 255, 255])
lower = np.array([0, 0, 0])
upper = np.array([255, 255, 160])
mask_one = cv2.inRange(hsv, lower, upper)
result_one = cv2.bitwise_and(img, img, mask=mask_one)

img = cv2.imread("examples/mothOne.png")
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
lower = np.array([0, 0, 0])
upper = np.array([255, 255, 160])
mask_two = cv2.inRange(hsv, lower, upper)
result_two = cv2.bitwise_and(img, img, mask=mask_two)

for pixel in range(len(mask_one)):
    print(pixel)

# cv2.imshow("Filtered moth", result)
# cv2.waitKey(0)
# cv2.destroyAllWindows()
