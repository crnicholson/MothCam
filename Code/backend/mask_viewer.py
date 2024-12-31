import cv2
import numpy as np
import os

cwd = os.getcwd()
images = []
masks = []


def display_image(path):
    img = cv2.imread(f"examples/{path}")
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    lower = np.array([0, 0, 0])
    upper = np.array([179, 255, 165])
    mask = cv2.inRange(hsv, lower, upper)
    result = cv2.bitwise_and(img, img, mask=mask)

    cv2.imshow(f"{path.replace(".png", "")}", result)

    while True:
        if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
            break

    cv2.destroyAllWindows()
    
    return mask


for entry in os.listdir(f"{cwd}/examples"):
    if os.path.isfile(os.path.join(f"{cwd}/examples", entry)):
        images.append(entry)
        mask = display_image(entry)

        if masks and mask.shape != masks[-1].shape:
            mask = cv2.resize(mask, (masks[-1].shape[1], masks[-1].shape[0]))

        masks.append(mask)

        if len(masks) > 1:
            diff_pixels = np.count_nonzero(cv2.absdiff(masks[-1], masks[-2]))
            percentage = (diff_pixels / (mask.shape[0] * mask.shape[1])) * 100
            print(
                f"Difference percentage between {images[-2]} and {images[-1]}: {percentage:.2f}%"
            )
