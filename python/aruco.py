import cv2
import yaml
import numpy as np
from pathlib import Path

DATA_DIR = "/home/dx/Research/leaf/marker_mapper1.0.15/build/utils"
CAM_DIR = "/home/dx/Research/leaf/aruco-3.1.12/build/utils_calibration/"


map_bt_c0 = {
'25': 26, 
'26': 25,
'27': 22,
'15': 9,
'16': 8,
'17': 34,
'5': 3,
'6': 2
}

map_bt_c1 = {
'45': 16,
'55': 17,
'46': 12,
'56': 13,
'47': 5,
'57': 6,
}

map_bt_c2 = {
'57': 19,
'47': 21,
'37': 20,
'56': 11,
'46': 12,
'36': 13,
'55': 4,
'45': 3,
'35': 5
}

type_1 = {'57', '47', '56', '46', '35'}

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

def match_point(image_p, camera, id):
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
    elif camera == '2' and (id in type_1):
        img.append(image_p[0])
        img.append(image_p[1])
        img.append(image_p[2])
        img.append(image_p[3])
    elif camera == '2' and (id not in type_1):
        img.append(image_p[1])
        img.append(image_p[2])
        img.append(image_p[3])
        img.append(image_p[0])
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

            img_point = match_point(image_points[str(imid)], camera, key)
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

global_ps = get_points("/sys_3.yml")

# print("================== camera 0 =======================")
# print("=========================15")
# print(global_ps['15'])
# print("=========================25")
# print(global_ps['25'])
# print("=========================26")
# print(global_ps['26'])
# print("=========================27")
# print(global_ps['27'])
# print("================== camera 1 =======================")
# print("=========================57")
# print(global_ps['57'])
# print("=========================47")
# print(global_ps['47'])
# print("=========================46")
# print(global_ps['46'])
# print("=========================45")
# print(global_ps['45'])
# print("=========================")

# exit(0)

def camera_approx(camera_, gpoint):
    if camera_ == '0':
        return gpoint['54'][0]
    elif camera_ == '1':
        return gpoint['4'][0]
    elif camera_ == '2':
        return gpoint['14'][0]
    



def get_camera_tr(camera_):
    imgsize, cm, dist = load_camera_intrinsic(camera_)
    sp, ip = load_spatial_and_image_point(camera_, global_ps)
    # cv2.calibrateCamera(sp, ip, imgsize)
    objpoints = [sp.reshape(-1, 3)]
    imgpoints = [ip.reshape(-1, 2)]
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, imgsize, cameraMatrix=cm, distCoeffs=dist, flags=cv2.CALIB_USE_INTRINSIC_GUESS)
    # suc, rvecs, tvecs = cv2.solvePnP(objpoints, imgpoints, cm, dist, rvec=rvecs_guesses[camera_], tvec=tvecs_guesses[camera_], useExtrinsicGuess=False, flags=cv2.SOLVEPNP_ITERATIVE)
    print(f" ============= Camera {camera_} ================= ")
    print("rvecs: ", rvecs)
    print("tvecs: ", tvecs)
    # print("ret: ", ret)
    print(" \n")

    # rvecs = [rvecs]
    # tvecs = [tvecs]
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
    
    print("camera approx", camera_approx(camera_, global_ps))

    print(" camera origin :", camera_translation)
    print(" up vector : ", np.matmul(camera_to_world, np.array([0, 1, 0, 0], dtype=np.float32))[:3])
    up = np.matmul(camera_to_world, np.array([0, 1, 0, 0], dtype=np.float32))[:3]
    print(" target vector : ", np.matmul(camera_to_world, np.array([0, 0, 1, 0], dtype=np.float32))[:3])
    print(" target : ", camera_translation + np.matmul(camera_to_world, np.array([0, 0, 1, 0], dtype=np.float32))[:3])
    target = camera_translation + np.matmul(camera_to_world, np.array([0, 0, 1, 0], dtype=np.float32))[:3]

    print(mtx)
    print("fx, fy: ", mtx[0][0], mtx[1][1])
    print("cx, cy: ", mtx[0][2], mtx[1][2])
    
    # f = (mtx[0][0] + mtx[1][1]) / 2.0
    # H = 2592
    # AFOV = 2.0 * np.arctan2(H, (2 * f)) / (2 * np.pi) * 360
    # print(" half angular field of view: ", AFOV)

    # compute reprojection error
    mean_error = 0
    for i in range(len(objpoints)):
        imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[0], tvecs[0], mtx, dist)
        # print(imgpoints[i].shape, imgpoints2.shape)
        # exit(0)
        error = cv2.norm(imgpoints[i].reshape(-1, 2), imgpoints2.reshape(-1, 2), cv2.NORM_L2)/len(imgpoints2)
        mean_error += error
    print( "total error: {}".format(mean_error/len(objpoints)) )

    camera_dict = {}
    camera_dict['name'] = camera_
    camera_dict['resolution'] = [2592, 1944]
    camera_dict['origin'] = camera_translation.tolist()
    camera_dict['up'] = up.tolist()
    camera_dict['target'] = target.tolist()
    camera_dict['camera_to_world'] = camera_to_world.tolist()
    camera_dict['world_to_camera'] = world_to_camera.tolist()
    camera_dict['intrinsic_matrix'] = mtx.tolist()
    camera_dict['rvecs'] = rvecs[0].tolist()
    camera_dict['tvecs'] = tvecs[0].tolist()
    camera_dict['dist'] = dist.tolist()

    return camera_dict

system_cameras = {
    'c0': get_camera_tr('0'),
    'c1': get_camera_tr('1'),
    'c2': get_camera_tr('2')
}

import json

with open(DATA_DIR+'/sys_camera.json', 'w') as camerafile:
    json.dump(system_cameras, camerafile)



# mark the image

# camera = "c0"

# frame = cv2.imread(DATA_DIR+f"/{camera}/{camera}_aruco_2.png", cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
# arucoDict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
# arucoParams = cv2.aruco.DetectorParameters()

# detector = cv2.aruco.ArucoDetector(arucoDict, arucoParams)


# markerCorners, markerIds, rejectedCandidates = detector.detectMarkers(frame)

# valid_c0 = [26, 25, 22, 9, 8, 34, 3, 2]
# valid_c1 = [16, 17, 12, 13, 5, 6]
# valid_c2 = [19, 21, 20, 11, 12, 13, 4, 3, 5]



# with open(DATA_DIR+f"/{camera}_data.log", "w") as file:
#     for i in range(len(rejectedCandidates)):
#         if i in valid_c0:
#             # print(rejectedCandidates[i])
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