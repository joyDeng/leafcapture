from cgitb import grey
import cv2
from matplotlib.image import imread
import numpy as np

DATA_ROOT = "/home/dx/Research/leaf/data/"

cpath = DATA_ROOT+"calibrate_2/cali/"

images = []
i = 0
for i in range(3):
    for j in range(14):
        if i == 2 and j >= 10:
            continue
        imgpath = cpath+"/checker_{}_c{}.hdr".format(j, i)
        img = cv2.imread(imgpath, cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
        tmp = (img * 255).astype(np.int32)
        cv2.imwrite(cpath+"/checker_{}_c{}.jpg".format(j, i), tmp)

for i in range(3):

    CHECKERBOARD = (6,9)
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
 
    # Defining the world coordinates for 3D points
    objp = np.zeros((1, CHECKERBOARD[0] * CHECKERBOARD[1], 3), np.float32)
    objp[0,:,:2] = np.mgrid[0:CHECKERBOARD[0], 0:CHECKERBOARD[1]].T.reshape(-1, 2)
    prev_img_shape = None
 
# Extracting path of individual image stored in a given directory
# images = glob.glob('./images/*.jpg')
    for j in range(14):
        
        if i == 2 and j >= 10:
            continue

        if i == 1 and j >= 10:
            continue
        # Creating vector to store vectors of 3D points for each checkerboard image
        objpoints = []
        # Creating vector to store vectors of 2D points for each checkerboard image
        imgpoints = [] 

        frame = cv2.imread(cpath+"/checker_{}_c{}.jpg".format(j, i), cv2.IMREAD_UNCHANGED)
        # print(frame.shape, frame.dtype)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        cv2.imwrite(cpath+"/grey_checker+{}_c{}.jpg".format(j, i), gray)
        # Find the chess board corners
        # If desired number of corners are found in the image then ret = true
        ret, corners = cv2.findChessboardCorners(gray, CHECKERBOARD, cv2.CALIB_CB_ADAPTIVE_THRESH + cv2.CALIB_CB_FAST_CHECK + cv2.CALIB_CB_NORMALIZE_IMAGE)
        
        """
        If desired number of corner are detected,
        we refine the pixel coordinates and display 
        them on the images of checker board
        """
        if ret == True:
            objpoints.append(objp)
            # refining pixel coordinates for given 2d points.
            corners2 = cv2.cornerSubPix(gray, corners, (11,11),(-1,-1), criteria)
            
            imgpoints.append(corners2)
    
            # Draw and display the corners
            img2 = cv2.drawChessboardCorners(frame, CHECKERBOARD, corners2, ret)
            cv2.imwrite(cpath+"/checker_{}_corner_{}.jpg".format(j, i), img2)

        else:
            print("no ret", i, j)
     
    
    # cv2.waitKey(0)
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

    print("{}th Camera matrix : \n".format(i))
    print(mtx)
    print("dist : \n")
    print(dist)
    print("rvecs : \n")
    print(rvecs)
    print("tvecs : \n")
    print(tvecs)
# cv2.destroyAllWindows()
 
# h,w = img.shape[:2]
 
"""
Performing camera calibration by 
passing the value of known 3D points (objpoints)
and corresponding pixel coordinates of the 
detected corners (imgpoints)
"""

 
# for i in range(3):
#     fimg = cv2.imread(cpath+"/c{}.jpg".format(i))
#     h,  w = fimg.shape[:2]
#     newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))

#     dst = cv2.undistort(fimg, mtx, dist, None, newcameramtx)
#     # crop the image
#     x, y, w, h = roi
#     dst = dst[y:y+h, x:x+w]
#     cv2.imwrite(cpath+'/calibresult_{}.jpg'.format(i), dst)

