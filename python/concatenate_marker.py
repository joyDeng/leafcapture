import cv2
import numpy as np

path = "/home/dx/Research/leaf/aruco-3.1.12/build/utils/ARUCO_MIP_16h3/"

image = []

for i in range(7):
    rows = []
    for j in range(10):
        img = cv2.imread(path+"aruco_mip_16h3_{:05d}.png".format(i * 10 + j), cv2.IMREAD_UNCHANGED)
        rows.append(img)
    row_img = np.concatenate(rows, axis=0)
    image.append(row_img)
final_image = np.concatenate(image, axis=1)

cv2.imwrite(path+"in_use.png", final_image)