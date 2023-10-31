# from unicodedata import numeric
from fileinput import filename
from re import X
from typing import ItemsView
import yaml
import numpy as np

DATA_DIR = "/home/dx/Research/leaf/marker_mapper1.0.15/build/utils"

from pathlib import Path

light_dict = {
    '0': [40],
    '1': [34],
    '2': [41],
    '3': [43],
    '4': [53],
    '5': [24],
    '6': [44],
    '7': [51],
    '8': [52],
    '9': [42],
    '10': [50],
}

plan_dict = {
    "up" : [37, 47, 57, 36, 46, 56, 35, 45, 55],
    # "down" : [5, 15, 6, 16, 26, 7, 17, 27]
}

def get_points(filename):
    yaml_dict = yaml.safe_load(Path(DATA_DIR+filename).read_text())
    corners = yaml_dict['aruco_bc_markers']
    cid = 0
    all_dict = {}
    for corner in corners:
        id = list(corner.keys())[0].split(":")[1]
        co = np.array(corner['corners'])
        center = np.mean(co, axis=0)
        cid += 1
        
        all_dict[id] = center
    return all_dict

def get_camera_infos(filename):
    with open(DATA_DIR+filename) as f:
        t = f.readline()
        its = t.split('\n')[0].split(' ')[1:]
        infos = []
        for n in its:
            infos.append(float(n))

        return infos

from scipy.spatial.transform import Rotation as R

def camera_matrix(camera_infos):
    translation = camera_infos[:3]
    quat = camera_infos[3:]
    # print(len(quat))
    rotation = R.from_quat(quat)
    # print(rotation.as_matrix())
    matrix = np.zeros((4, 4))
    matrix[:3, :3] = rotation.as_matrix()
    matrix[:3, 3] = translation[:]
    matrix[3, 3] = 1
    return matrix


camera_0 = get_camera_infos("/C0.log")
camera_1 = get_camera_infos("/C1.log")
camera_2 = get_camera_infos("/C2.log")

def camera_up_origin_target(camerainfo):
    m = camera_matrix(camerainfo)
    origin = m[:3, 3]
    target = np.matmul(m, np.array([0, 0, 1, 0]))
    up = np.matmul(m, np.array([0, 1, 0, 0]))
    return origin, target, up

o0, t0, u0 = camera_up_origin_target(camera_0)
o1, t1, u1 = camera_up_origin_target(camera_1)
o2, t2, u2 = camera_up_origin_target(camera_2)


all_dict    = get_points("/sys_recon_2.yml")
c0_dict     = get_points("/C0.yml")
c1_dict     = get_points("/C1.yml")
c2_dict     = get_points("/C2.yml")


def FromMatrix(p1, p2, p3, p4, v1, v2, v3, v4):
    A = np.zeros((12, 12))
    b = np.zeros((12, 1))
    A[0, 0] = p1[0]
    A[1, 1] = p1[0]
    A[2, 2] = p1[0]
    A[0, 3] = p1[1]
    A[1, 4] = p1[1]
    A[2, 5] = p1[1]
    A[0, 6] = p1[2]
    A[1, 7] = p1[2]
    A[2, 8] = p1[2]
    A[0, 9] = 1.0
    A[1, 10] = 1.0
    A[2, 11] = 1.0
    

    A[3, 3] = p2[1]
    A[4, 4] = p2[1]
    A[5, 5] = p2[1]
    A[3, 6] = p2[2]
    A[4, 7] = p2[2]
    A[5, 8] = p2[2]
    A[3, 0] = p2[0]
    A[4, 1] = p2[0]
    A[5, 2] = p2[0]
    A[3, 9] = 1.0
    A[4, 10] = 1.0
    A[5, 11] = 1.0

    A[6, 6] = p3[2]
    A[7, 7] = p3[2]
    A[8, 8] = p3[2]
    A[6, 0] = p3[0]
    A[7, 1] = p3[0]
    A[8, 2] = p3[0]
    A[6, 3] = p3[1]
    A[7, 4] = p3[1]
    A[8, 5] = p3[1]
    A[6, 9] = 1.0
    A[7, 10] = 1.0
    A[8, 11] = 1.0

    A[9,  0]    = p4[0]
    A[10, 1]    = p4[0]
    A[11, 2]    = p4[0]
    A[9,  3]    = p4[1]
    A[10, 4]    = p4[1]
    A[11, 5]    = p4[1]
    A[9,  6]    = p4[2]
    A[10, 7]    = p4[2]
    A[11, 8]    = p4[2]
    A[9,  9]    = 1.0
    A[10, 10]   = 1.0
    A[11, 11]   = 1.0
    
    b[:3, 0]    = v1[:]
    b[3:6, 0]   = v2[:]
    b[6:9, 0]   = v3[:]
    b[9:12, 0]  = v4[:]

    x = np.linalg.solve(A, b)

    mT = np.zeros((4, 4))
    mT[:3, 0] = x[:3, 0]
    mT[:3, 1] = x[3:6, 0]
    mT[:3, 2] = x[6:9, 0]
    mT[:3, 3] = x[9:12, 0]
    mT[3, 3]  = 1
    return mT


def get_pv(from_dict, target_dict, idxs):
    p = []
    v = []
    for i in idxs:
        p.append(from_dict[i])
        v.append(target_dict[i])
    return p, v


p_c0, v_c0 = get_pv(c0_dict, all_dict, ['5', '6', '7', '17'])    
p_c1, v_c1 = get_pv(c1_dict, all_dict, ['35', '36', '37', '45'])
p_c2, v_c2 = get_pv(c2_dict, all_dict, ['35', '36', '37', '45'])


trans_c0 = FromMatrix(p_c0[0], p_c0[1], p_c0[2], p_c0[3], v_c0[0], v_c0[1], v_c0[2], v_c0[3])
trans_c1 = FromMatrix(p_c1[0], p_c1[1], p_c1[2], p_c1[3], v_c1[0], v_c1[1], v_c1[2], v_c1[3])
trans_c2 = FromMatrix(p_c2[0], p_c2[1], p_c2[2], p_c2[3], v_c2[0], v_c2[1], v_c2[2], v_c2[3])

def transfer_camera(origin_v, target_v, up_v, transfer):
    ori = np.ones((4))
    ori[:3] = origin_v
    print(origin_v, ori)
    print(trans_c0)

    g_o = np.matmul(transfer, ori)
    print(g_o)
    exit(0)
    g_u = np.matmul(transfer, up_v)
    g_t = np.matmul(transfer, target_v)
    return g_o, g_t, g_u

origin_0, target_0, up_0 = transfer_camera(o0, t0, u0, trans_c0)
origin_1, target_1, up_1 = transfer_camera(o1, t1, u1, trans_c1)
origin_2, target_2, up_2 = transfer_camera(o2, t2, u2, trans_c2)

print("C0 O, T, U", origin_0[:3] * 100, target_0[:3], up_0[:3])
print("C1 O, T, U", origin_1[:3] * 100, target_1[:3], up_1[:3])
print("C2 O, T, U", origin_2[:3] * 100, target_2[:3], up_2[:3])
exit(0)



light_pos_dict = {}    
for i in light_dict.items():
    corner = all_dict[f'{i[1][0]}']
    light_pos_dict[i[0]] = corner
    # print(f"light pos is {i[0]} and {i[1][0]}", corner)

for item in light_pos_dict.items():
    print(item)



for v in plan_dict.items():
    points = v[1]
    ps = [0, 0, 0]
    for p in points:
        ps += all_dict[f'{p}']
        id = f'{p}'
        print(f"point {id} and {all_dict[id]}")
    ps /= 9
    print(f"points {ps}")




