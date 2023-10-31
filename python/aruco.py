import cv2
import yaml
import numpy as np
from pathlib import Path

DATA_DIR = "/home/dx/Research/leaf/marker_mapper1.0.15/build/utils"
CAM_DIR = "/home/dx/Research/leaf/aruco-3.1.12/build/utils_calibration/"


map_bt_c0 = {
'25': 35, 
'26': 24,
'27': 22,
'15': 9,
'16': 8,
'17': 7,
'5': 2,
'6': 1,
'7': 0}

map_bt_c1 = {
'35': 14,
'45': 15,
'55': 16,
'36': 8,
'46': 10,
'56': 12,
'37': 3,
'47': 4,
'57': 5
}

map_bt_c2 = {
'57': 6,
'47': 7,
'37': 19,
'56': 3,
'46': 4,
'36': 5,
'55': 12,
'45': 1,
'35': 2
}

def point_map(cm):
    if cm == '0':
        return map_bt_c0
    elif cm == '1':
        return map_bt_c1
    elif cm == '2':
        return map_bt_c2
    return map_bt_c0

def get_points(filename):
    yaml_dict = yaml.safe_load(Path(DATA_DIR+filename).read_text())
    corners = yaml_dict['aruco_bc_markers']
    cid = 0
    all_dict = {}
    for corner in corners:
        id = list(corner.keys())[0].split(":")[1]
        co = np.array(corner['corners'])
        all_dict[id] = co
    return all_dict

def match_point(image_p, camera):
    img = []
    if camera == '0':
        img.append(image_p[2])
        img.append(image_p[3])
        img.append(image_p[0])
        img.append(image_p[1])
    elif camera == '1':
        img.append(image_p[2])
        img.append(image_p[3])
        img.append(image_p[0])
        img.append(image_p[1])
    elif camera == '2':
        img.append(image_p[0])
        img.append(image_p[1])
        img.append(image_p[2])
        img.append(image_p[3])
    return img


def load_spatial_and_image_point(camera, global_points):
    image_points = {}
    with open(DATA_DIR+f"/c{camera}_data.log") as file:
        data = file.readlines()
        for l in data:
            vs = l.split(' ')
            ps = [[int(vs[2][1:-1]), int(vs[3][:-1])], [int(vs[5][1:-1]), int(vs[6][:-1])], [int(vs[8][1:-1]), int(vs[9][:-1])], [int(vs[11][1:-1]), int(vs[12][:-1])]]
            image_points[vs[0]] = ps
    # spatial_points = get_points(f"/C{camera}.yml")
    point_dict = point_map(camera)
    # print("camera ", camera)
    
    img_points = []
    spt_points = []
    for key in point_dict.keys():
        if key in global_points.keys():
            imid = point_dict[key]

            img_point = match_point(image_points[str(imid)], camera)
            img_points.append(img_point)
            spt_points.append(global_points[key])

    return np.array(spt_points, dtype=np.float32), np.array(img_points, dtype=np.float32)
    

def load_camera_intrinsic(camera):
    yamlfile = yaml.safe_load(Path(CAM_DIR+f"/camera_c{camera}.yml").read_text())
    cm = yamlfile['camera_matrix']['data']
    distort = yamlfile['distortion_coefficients']['data']
    imgsize = [yamlfile['image_width'], yamlfile['image_height']]
    return np.array(imgsize), np.array(cm, dtype=np.float32).reshape(3, 3), np.array(distort, dtype=np.float32).reshape(5, 1)
    # print(yamlfile)
    # print(cm, distort)
    # print(yamlfile)
    # corners = yamlfile['camera_matrix']
    # cid = 0
    # all_dict = {}
    # for corner in corners:
    #     id = list(corner.keys())[0].split(":")[1]
    #     co = np.array(corner['corners'])
    #     all_dict[id] = co * 100
    # return all_dict

global_ps = get_points("/sys_recon_2.yml")

rvecs_guesses = {
    '0':  np.array([[-0.04819978],
       [-3.07243491],
       [-0.0670837 ]], dtype=np.float32),
    '1': np.array([[-0.01785693],
       [-0.00855753],
       [-1.52248574]], dtype=np.float32),
    '2': np.array([[-0.01785693],
       [-0.00855753],
       [-1.52248574]], dtype=np.float32)
}

tvecs_guesses = {
    '0':  np.array([[ 0.04053944],
       [-0.02583115],
       [ 0.27204369]], dtype=np.float32),
    '1': np.array([[-0.04762647],
       [ 0.05048181],
       [ 0.24555217]], dtype=np.float32),
    '2': np.array([[-0.04762647],
       [ 0.05048181],
       [ 0.24555217]], dtype=np.float32)
}

def get_camera_tr(camera_):
    imgsize, cm, dist = load_camera_intrinsic(camera_)
    sp, ip = load_spatial_and_image_point(camera_, global_ps)
    # cv2.calibrateCamera(sp, ip, imgsize)
    objpoints = sp.reshape(-1, 3)
    imgpoints = ip.reshape(-1, 2)
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, imgsize, cameraMatrix=cm, distCoeffs=dist, flags=cv2.CALIB_USE_INTRINSIC_GUESS)
    # suc, rvecs, tvecs = cv2.solvePnP(objpoints, imgpoints, cm, dist, rvec=rvecs_guesses[camera_], tvec=tvecs_guesses[camera_], useExtrinsicGuess=False, flags=cv2.SOLVEPNP_ITERATIVE)
    print(f" ============= Camera {camera_} ================= ")
    print("rvecs: ", rvecs)
    print("tvecs: ", tvecs)
    # print("ret: ", ret)
    print(" \n")

    rvecs = [rvecs]
    tvecs = [tvecs]
    # compute reprojection error
    # print(rvecs)
    rot_matrix, jacobian = cv2.Rodrigues(rvecs[0])
    world_to_camera = np.zeros((4, 4),dtype=np.float32)
    world_to_camera[:3, :3] = rot_matrix
    world_to_camera[:3, 3] = tvecs[0].reshape(-1)
    world_to_camera[3, 3] = 1
    camera_to_world = np.linalg.inv(world_to_camera)
    print(camera_to_world)
    # print("rotation matrix: ", rot_matrix)
    # print("jacobian: ", jacobian)

    camera_translation = -np.matmul(np.transpose(rot_matrix), tvecs[0].reshape(-1))
    print("camera translation: ", camera_translation)

    # compute reprojection error
    mean_error = 0
    for i in range(len(objpoints)):
        imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[0], tvecs[0], cm, dist)
        # print(imgpoints[i].shape, imgpoints2.shape)
        # exit(0)
        error = cv2.norm(imgpoints[i].reshape(-1, 2), imgpoints2.reshape(-1, 2), cv2.NORM_L2)/len(imgpoints2)
        mean_error += error
    print( "total error: {}".format(mean_error/len(objpoints)) )


get_camera_tr('0')
get_camera_tr('1')
get_camera_tr('2')



# mark the image

# camera = "c2"

# frame = cv2.imread(DATA_DIR+f"/{camera}/{camera}_aruco_1.png", cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
# arucoDict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
# arucoParams = cv2.aruco.DetectorParameters()

# detector = cv2.aruco.ArucoDetector(arucoDict, arucoParams)


# markerCorners, markerIds, rejectedCandidates = detector.detectMarkers(frame)

# valid_c0 = [35, 24, 22, 9, 8, 7, 2, 1, 0]
# valid_c1 = [14, 15, 16, 8, 10, 12, 3, 4, 5]
# valid_c2 = [6, 7, 19, 3, 4, 5, 12, 1, 2]



# with open(DATA_DIR+f"/{camera}_data.log", "w") as file:
#     for i in range(len(rejectedCandidates)):
#         if i in valid_c2:
#             print(rejectedCandidates[i])
#             (topLeft, topRight, bottomRight, bottomLeft) = rejectedCandidates[i][0]

#             # convert each of the (x, y)-coordinate pairs to integers
#             topRight = (int(topRight[0]), int(topRight[1]))
#             bottomRight = (int(bottomRight[0]), int(bottomRight[1]))
#             bottomLeft = (int(bottomLeft[0]), int(bottomLeft[1]))
#             topLeft = (int(topLeft[0]), int(topLeft[1]))

#             cv2.line(frame, topLeft, topRight, (0, 255, 0), 2)
#             cv2.line(frame, topRight, bottomRight, (0, 255, 0), 2)
#             cv2.line(frame, bottomRight, bottomLeft, (0, 255, 0), 2)
#             cv2.line(frame, bottomLeft, topLeft, (0, 255, 0), 2)

#             cv2.putText(frame, str(i), (topLeft[0], topLeft[1] - 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
#             cv2.putText(frame, str("br"), (bottomRight[0], bottomRight[1] - 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
#             cv2.putText(frame, str("bl"), (bottomLeft[0], bottomLeft[1] - 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
#             print("[INFO] ArUco marker ID: {}".format(i))
#             file.writelines(f"{i} topright {topRight} bottomright {bottomRight} bottomLeft {bottomLeft} topLeft {topLeft} \n")

# cv2.imwrite(DATA_DIR+f"/{camera}_with_marker.png", frame)