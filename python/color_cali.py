import cv2
import yaml
import numpy as np
from pathlib import Path

DATA_DIR = "/home/dx/Research/leaf/marker_mapper1.0.15/build/utils"
CAM_DIR = "/home/dx/Research/leaf/aruco-3.1.12/build/utils_calibration/"
IMAGE_DIR = "/home/dx/Research/leaf/data/"

# ret, thresh = cv.threshold(imgray, 127, 255, 0)
# im2, contours, hierarchy = cv2.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

# color_grid = {
#     1: [115, 82, 68],
#     2: [194, 150, 130],
#     3: [98,  122, 157],
#     4: [87, 108, 67],
#     5: [133, 128, 177],
#     6: [103, 189, 170],

#     7: [214, 126, 44],
#     8: [80, 91, 166],
#     9: [193, 90, 99],
#     10: [94, 60, 108],
#     11: [157, 188, 64],
#     12: [224, 163, 46],

#     13: [56, 61, 150],
#     14: [70, 148, 73],
#     15: [175, 54, 60],
#     16: [231, 199, 31],
#     17: [187, 86, 149],
#     18: [8, 133, 161],

#     19: [243, 243, 242],
#     20: [200, 200, 200],
#     21: [160, 160, 160],
#     22: [122, 122, 121],
#     23: [85, 85, 85],
#     24: [52, 52, 52]
# }

color_grid={
    0: [243, 243, 242],
    1: [56, 61, 150],
    2: [214, 126, 44],
    3: [115, 82, 68],

    4: [200, 200, 200],
    5: [70, 148, 73],
    6: [80, 91, 166],
    7: [194, 150, 130],

    8: [160, 160, 160],
    9: [175, 54, 60],
    10: [193, 90, 99],
    11: [98,  122, 157],

    12: [122, 122, 121],
    13: [231, 199, 31],
    14: [94, 60, 108],
    15: [87, 108, 67],

    16: [85, 85, 85],
    17: [187, 86, 149],
    18: [157, 188, 64],
    19: [133, 128, 177],

    20: [52, 52, 52],
    21: [8, 133, 161],
    22: [224, 163, 46],
    23: [103, 189, 170],
}


image = cv2.imread(IMAGE_DIR+"/color_bi/c0_l0_rgb_bi_hdr.hdr")
origin = [1445, 312]

xx_step = 1598 - 1445 
xy_step = 330 - 312
yx_step = 1428 - 1445
yy_step = 463 - 312

XX_step = 1652 - 1445
XY_step = 338 - 312
YX_step = 1422 - 1445
YY_step = 522 - 312


def get_pixel_indices(xxstep, xystep, yxstep, yystep, XX_step, XY_step, YX_step, YY_step):
    corners = {}
    for y in range(6):
        for x in range(4):
            id = y * 4 + x
            ori = [origin[0] + XX_step * x + YX_step * y + 10, origin[1] + XY_step * x + YY_step * y + 10]
            squares = []
            squares.append(ori)
            squares.append([ori[0] + xxstep - 10,             ori[1] + xystep + 10])
            squares.append([ori[0] + xxstep + yx_step - 10,   ori[1] + xystep + yystep - 10])
            squares.append([ori[0] + yxstep + 10,             ori[1] + yystep - 10])
            corners[id] = squares

            # cv2.line(image, squares[0], squares[1], (0, 255, 0), 2)
            # cv2.line(image, squares[1], squares[2], (0, 255, 0), 2)
            # cv2.line(image, squares[2], squares[3], (0, 255, 0), 2)
            # cv2.line(image, squares[3], squares[0], (0, 255, 0), 2)

    return corners


def get_pixel_colors(corners, img):
    colors = {}
    for pair in corners.items():
        color = pair[0]
        corner = pair[1]
        topright = [max(corner[0][0], corner[3][0]), max(corner[0][1], corner[1][1])]
        botleft = [min(corner[1][0], corner[2][0]), min(corner[3][1],  corner[2][1])]

        # print(img.shape)
        subpixels = img[ topright[1] : botleft[1], topright[0] : botleft[0], :]
        colors[color] = subpixels
        # print(subpixels.shape)        
    #     cv2.line(image, topright, (botleft[0], topright[1]), (0, 255, 0), 2)
    #     cv2.line(image, (botleft[0], topright[1]), botleft, (0, 255, 0), 2)
    #     cv2.line(image, botleft, (topright[0], botleft[1]), (0, 255, 0), 2)
    #     cv2.line(image, (topright[0], botleft[1]), topright, (0, 255, 0), 2)
    
        # cv2.imwrite(IMAGE_DIR+f"/color_bi/c0_l0_{color}.hdr", subpixels)
    return colors
        
def match_color_vectors(colors, color_dict):
    ru = []
    rc = []
    for key in colors.keys():
        if key in [0, 4, 8, 12, 16, 20]:
            ru.append(colors[key].reshape(-1, 3))
            current_color = np.zeros(colors[key].shape).reshape(-1, 3)
            current_color[:, :] =  np.array(color_dict[key]) 
            rc.append(current_color)
            

    return np.array(ru).reshape(-1, 3), np.array(rc).reshape(-1, 3)


squares = get_pixel_indices(xx_step, xy_step, yx_step, yy_step, XX_step, XY_step, YX_step, YY_step)
img_colors = get_pixel_colors(squares, image)
U, C = match_color_vectors(img_colors, color_grid)

print(U.shape, C.shape)
rg = U[:, 0] / U[:, 1]
bg = U[:, 2] / U[:, 1]

RG = C[:, 0] / C[:, 1]
BG = C[:, 2] / C[:, 1]

print(rg.shape)
print(RG.shape)

xrm = np.linalg.lstsq(rg.reshape(-1, 1), RG.reshape(-1, 1), rcond=-1)
xbm = np.linalg.lstsq(bg.reshape(-1, 1), BG.reshape(-1, 1), rcond=-1)

scale_red = xrm[0]
scale_blue = xbm[0]

print("scale of red: ",     scale_red[0][0])
print("scale of blue: ",    scale_blue[0][0])

white_balanced = np.zeros(image.shape)
white_balanced[:, :, 0] = image[:, :, 0] * scale_red
white_balanced[:, :, 2] = image[:, :, 2] * scale_blue
white_balanced[:, :, 1] = image[:, :, 1]

cv2.imwrite(IMAGE_DIR+"/color_bi/white_balanced.hdr", white_balanced)



#             # convert each of the (x, y)-coordinate pairs to integers
#             topRight = (int(topRight[0]), int(topRight[1]))
#             bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
#             bottomLeft = (int(bottomLeft[0]), int(bottomLeft[1]))
#             topLeft = (int(topLeft[0]), int(topLeft[1]))

#             cv2.line(frame, topLeft, topRight, (0, 255, 0), 2)
#             cv2.line(frame, topRight, bottomRight, (0, 255, 0), 2)
#             cv2.line(frame, bottomRight, bottomLeft, (0, 255, 0), 2)
#             cv2.line(frame, bottomLeft, topLeft, (0, 255, 0), 2)

    

