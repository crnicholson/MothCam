import cv2
import numpy as np

path = "examples/mothFive.png"

# White background.
lower = np.array([0, 0, 0])
upper = np.array([179, 255, 165])

# Green background.
# lower = np.array([35, 50, 50])
# upper = np.array([85, 255, 255]) 

img = cv2.imread(path)
hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

mask_one = cv2.inRange(hsv, lower, upper)
# result_one = cv2.bitwise_and(img, img, mask=cv2.bitwise_not(mask_one))
result_one = cv2.bitwise_and(img, img, mask=mask_one)

# img = cv2.imread("examples/mothOne.png")
# hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
# lower = np.array([0, 0, 0])
# upper = np.array([179, 255, 120])
# mask_two = cv2.inRange(hsv, lower, upper)
# result_two = cv2.bitwise_and(img, img, mask=mask_two)

# diff_pixels = np.count_nonzero(cv2.absdiff(mask_one, mask_two))

# percentage = (diff_pixels / (mask_one.shape[0] * mask_one.shape[1])) * 100

# print(percentage)

cv2.imshow("Moth", result_one)

while True:
    if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
        break

# cv2.imshow("Moth", result_two)

# while True:
#     if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
#         break

cv2.destroyAllWindows()
