import cv2
import numpy as np
import os

cwd = os.getcwd()

images = []

for entry in os.listdir(f"{cwd}/examples"):
    if os.path.isfile(os.path.join(f"{cwd}/examples", entry)):
        images.append(entry)

img = cv2.imread("examples/mothOne.png")
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
lower = np.array([0, 0, 0])
upper = np.array([179, 255, 165])
mask_one = cv2.inRange(hsv, lower, upper)
result_one = cv2.bitwise_and(img, img, mask=mask_one)

img = cv2.imread("examples/mothOne.png")
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
lower = np.array([0, 0, 0])
upper = np.array([179, 255, 120])
mask_two = cv2.inRange(hsv, lower, upper)
result_two = cv2.bitwise_and(img, img, mask=mask_two)

diff_pixels = np.count_nonzero(cv2.absdiff(mask_one, mask_two))

percentage = (diff_pixels / (mask_one.shape[0] * mask_one.shape[1])) * 100

print(percentage)

cv2.imshow("165 moth", result_one)

while True:
    if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
        break

cv2.imshow("120 moth", result_two)

while True:
    if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
        break

cv2.destroyAllWindows()
