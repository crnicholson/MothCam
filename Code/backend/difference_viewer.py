import cv2
import numpy as np
import os

cwd = os.getcwd()
image_names = []
images = []


def display_image(path):
    img = cv2.imread(f"examples/{path}")

    cv2.imshow(f"{path.replace(".png", "")}", img)

    while True:
        if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
            break

    cv2.destroyAllWindows()
    
    return img


for entry in os.listdir(f"{cwd}/examples"):
    if os.path.isfile(f"{cwd}/examples/{entry}"):
        image_names.append(entry)
        
        image = display_image(entry)
        
        if image is not None and images and image.shape != images[-1].shape:
            image = cv2.resize(image, (images[-1].shape[1], images[-1].shape[0]))
        
        images.append(image)
        
        if len(images) > 1:
            difference = cv2.absdiff(images[-1], images[-2])
    
            total_difference = np.sum(difference)
            
            max_intensity = 255
            num_pixels = images[-1].shape[0] * images[-1].shape[1] * images[-1].shape[2]  
            total_possible_difference = num_pixels * max_intensity
            
            percentage = (total_difference / total_possible_difference) * 100
            
            print(percentage)
