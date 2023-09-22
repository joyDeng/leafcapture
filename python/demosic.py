from cmath import isnan
from configparser import Interpolation
from ctypes import alignment, resize
from fileinput import filename
from matplotlib.axis import Axis
import numpy as np
import os
import cv2
os.environ["OPENCV_IO_ENABLE_OPENEXR"]="1"

import argparse
from scipy import signal

# from scipy import optimize

parser = argparse.ArgumentParser()
parser.add_argument('--name', type=str, default='petal_')
sc = parser.parse_args()

DATA_ROOT = "/home/dx/Research/leaf/data/"
file_dir = DATA_ROOT + sc.name + "/"

import os

wb_dir = file_dir+"/{}_demo/".format(sc.name)

if not os.path.exists(wb_dir):
    os.makedirs(wb_dir)


class RawImage:
    def __init__(self, folder, numcam = 3, numlight = 11):
        self.monos = {}
        # self.names = []
        self.demos = {}
        self.num_camera = numcam
        self.num_light = numlight
        
        self.folder = folder
        for filename in os.listdir(folder):
            if filename[-12:] == "mono_hdr.hdr":
                self.monos[filename] = cv2.imread(folder + "/" + filename, cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)

            if filename.find("rgb") != -1:
                self.demos[filename] = cv2.imread(folder + "/" + filename, cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
    

    def bw2color(self, method="bi"):
        if method == "bi":
            demo_fuc = demosiac_blur3x3
        elif method == "ahd":
            demo_fuc = demosiac_AHD
        self.demos = {}
        for pairs in self.monos.items():
            fname = pairs[0].replace("mono", "rgb_{}".format(method))
            self.demos[fname] = demo_fuc(pairs[1])
            cv2.imwrite(self.folder + "/" + fname, self.demos[fname])


    def arrangeAll(self):
        imgs = []

        for i in range(self.num_camera):
            il = []
            for j in range(self.num_light):
                il.append([])    
            imgs.append(il)
        
        for pairs in self.demos.items():
            info = pairs[0].split("_")
            print(info)
            cid = int(info[0][1:])
            lid = int(info[1][1:])
            imgs[cid][lid] = pairs[1]
        
        img1 = []
        for row in imgs:
            row_img = np.concatenate(row, axis=1)
            img1.append(row_img)
        img2 = np.concatenate(img1, axis=0)

        w = img2.shape[0] // 3
        h = img2.shape[1] // 3
        print(w, h)

        resized = cv2.resize(img2, (h, w), interpolation=cv2.INTER_LINEAR)
        cv2.imwrite(self.folder+"/"+"overview.hdr", resized)
        cv2.imwrite(self.folder+"/"+"full.hdr",img2)

            



def computef(img):
    h0 = np.array([-0.25, 0.0, 0.5, 0.0, -0.25], dtype=np.float32)
    h1 = np.array([0.0, 0.5, 0.0, 0.5, 0.0], dtype=np.float32)
    G = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    BR = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)

    HG = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    HBR = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    GN = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    delta_BRG = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    delta_GR = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    delta_GB = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)

    for i in range(img.shape[0]):
        if i % 2 == 0:
            G[i, 0::2, 0] += img[i, 0::2, 1]
            BR[i, 1::2, 0] += img[i, 1::2, 1]
            HG[i, :, 0] = np.convolve(h1, G[i, :, 0], "same")
            HBR[i, :, 0] = np.convolve(h0, BR[i, :, 0], "same")
            GN[i, :, 0]  = G[i, :, 0]  + HG[i, :, 0] + HBR[i, :, 0]
            delta_BRG[i, 1::2, 0] += (BR[i, 1::2, 0] - GN[i, 1::2, 0])
        else:
            G[i, 1::2, 0] += img[i, 1::2, 1]
            BR[i, ::2, 0] += img[i, ::2, 1]
            HG[i, :, 0] = np.convolve(h1, G[i, :, 0], "same")
            HBR[i, :, 0] = np.convolve(h0, BR[i, :, 0], "same")
            GN[i, :, 0]  = G[i, :, 0] + HG[i, :, 0] + HBR[i, :, 0] 
            delta_BRG[i, ::2, 0] = BR[i, ::2, 0] - GN[i, ::2, 0]
            # print("green", G[i, :, 0])
            # print("hg", HG[i, :, 0])
            # print(GN[i, :, 0])
            # exit(0)

    # bilinear interpolation
    for i in range(img.shape[0]):
        if i % 2 == 0:
            interpo = np.zeros(img.shape[1])
            interpo = np.convolve(h1, delta_BRG[i, :, 0], "same")
            delta_GR[i, ::2, 0] = interpo[::2]
            delta_GR[i, 1::2, 0] = delta_BRG[i, 1::2, 0]
            # print("delta GR", delta_GR[i, :, 0])
        if i % 2 == 1:
            interpo = np.zeros(img.shape[1])
            interpo = np.convolve(h1, delta_BRG[i, :, 0], "same")
            delta_GB[i,1::2, 0] = interpo[1::2]
            delta_GB[i, 0::2, 0] = delta_BRG[i, 0::2, 0]
        
    for i in range(img.shape[1]):
            interpo_gr = np.zeros(img.shape[0])
            interpo_gr = np.convolve(h1, delta_GR[:, i, 0], "same")
            interpo_gb = np.zeros(img.shape[0])
            interpo_gb = np.convolve(h1, delta_GB[:, i, 0], "same")
            delta_GR[1::2, i, 0] = interpo_gr[1::2]
            delta_GB[0::2, i, 0] = interpo_gb[0::2]

    cv2.imwrite(wb_dir+"/delta-gr.exr", delta_GR)
    cv2.imwrite(wb_dir+"/delta-gb.exr", delta_GB)

    R = GN + delta_GR
    B = GN + delta_GB
    cv2.imwrite(wb_dir+"/green_n.exr", np.concatenate([GN, GN, GN], axis=2))
    return np.concatenate([R, GN, B], axis=2)

def non_linear_map_in_lab(vec):
    kappa = 903.3
    epl = 0.008856

    ret = np.zeros(vec.shape, dtype=np.float32)
    ret[vec>epl] = np.power(vec[vec>epl], 1.0 / 3.0)
    ret[vec<=epl] = (vec[vec<=epl] * kappa + 16.0) / 116.0
    return ret

def LRGB2CIELAB(img):
    img_pos = img.copy()
    img_pos[img_pos<0.0] = 0.0

    img_pos_srgb = np.power(img_pos, 1.0 / 2.2)
   

    M = np.array([[0.4124564,  0.3575761,  0.1804375],
        [0.2126729,  0.7151522,  0.0721750],
        [0.0193339,  0.1191920,  0.9503041]])

    img_xyz = np.einsum("li,jki", M, img_pos_srgb)
    

    max_c = np.max(img_xyz)

    # normalize XYZ
    img_xyz_n = np.array(img_xyz / max_c, dtype=np.float32)
    
    d65_white = [0.950456, 1, 1.088754 ]
    xyz_r = img_xyz_n / d65_white
    # f_xyz = np.zeros(xyz_r.shape, dtype=np.float32)

    fx = non_linear_map_in_lab(xyz_r[:, :, 0])
    fy = non_linear_map_in_lab(xyz_r[:, :, 1])
    fz = non_linear_map_in_lab(xyz_r[:, :, 2])

    L = (116 *  fy - 16.0).reshape(xyz_r.shape[0], xyz_r.shape[1], 1)
    a = (500 * (fx - fy)).reshape(xyz_r.shape[0], xyz_r.shape[1], 1)
    b = (200 * (fy - fz)).reshape(xyz_r.shape[0], xyz_r.shape[1], 1)

    cie_lab = np.concatenate([L, a, b], axis=2)
    # print(L.shape, cie_lab.shape)
    return cie_lab

def compute_epl(f, dir, space):
    
    if space == "L":
        if dir == "h":
            fp1 = np.roll(f, 1, axis=1)
            fm1 = np.roll(f, -1, axis=1)
        else:
            fp1 = np.roll(f, 1, axis=0)
            fm1 = np.roll(f, -1, axis=0)
        dis_p1 = np.abs(f[:, :, 0] - fp1[:, :, 0])
        dis_m1 = np.abs(f[:, :, 0] - fm1[:, :, 0])
        dis_L = np.maximum(dis_p1, dis_m1)
        # print("L e shape", dis_L.shape)
        return dis_L
    else:
        if dir == "h":
            fp1 = np.roll(f, 1, axis=1)
            fm1 = np.roll(f, -1, axis=1)
        else:
            fp1 = np.roll(f, 1, axis=0)
            fm1 = np.roll(f, -1, axis=0)
        dis_p1 = np.sqrt(np.power(f[:, :, 1] - fp1[:, :, 1], 2.0) + np.power(f[:, :, 2] - fp1[:, :, 2], 2.0))
        dis_p2 = np.sqrt(np.power(f[:, :, 1] - fm1[:, :, 1], 2.0) + np.power(f[:, :, 2] - fm1[:, :, 2], 2.0))
        dis_CL = np.maximum(dis_p1, dis_p2)
        # print("ad e shape", dis_CL.shape)
        return dis_CL

def compute_H(f_lab, e_L, e_C, sigma):
    # HL
    H_f = np.zeros(e_L.shape, dtype=np.float32)
    B_s = np.zeros(e_L.shape, dtype=np.float32)
    for i in range(-sigma, sigma):
        for j in range(-sigma, sigma):
            if i * i + j * j <= sigma * sigma:
                f_m = np.roll(f_lab, i, axis=0)
                # print(f_m.shape)
                f_m = np.roll(f_m, j, axis=1)
                dis_L = np.abs(f_m[:, :, 0] - f_lab[:, :, 0])
                dis_ab = np.sqrt(np.power(f_m[:, :, 1] - f_lab[:, :, 1], 2.0), np.power(f_m[:, :, 2] - f_lab[:, :, 2], 2.0))
                H_f[(dis_L <= e_L) & (dis_ab <= e_C)] += 1.0
                B_s += 1.0
    return H_f / B_s

def compute_m(f, ksize):
    delta_RG = f[:, :, 0] - f[:, :, 1]
    delta_BG = f[:, :, 2] - f[:, :, 1]
    delta_GR = f[:, :, 1] - f[:, :, 0]
    delta_GB = f[:, :, 1] - f[:, :, 2]
    print(delta_RG.shape)

    med_rg = signal.medfilt2d(delta_RG, (ksize, ksize))
    med_bg = signal.medfilt2d(delta_BG, (ksize, ksize))
    med_gr = signal.medfilt2d(delta_GR, (ksize, ksize))
    med_gb = signal.medfilt2d(delta_GB, (ksize, ksize))
    print(med_gb.shape)

    R = med_rg + f[:, :, 1]
    G = 0.5 * (med_gr + med_gb + f[:, :, 0] + f[:, :, 2])
    B = med_bg + f[:, :, 1]

    # print(R.shape, G.shape, B.shape)
    color = np.concatenate([R.reshape(f.shape[0], f.shape[1], 1), G.reshape(f.shape[0], f.shape[1], 1), B.reshape(f.shape[0], f.shape[1], 1)], axis=2)
    return color

def demosiac_blur3x3(img):
    G = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.float32)
    print(G.shape)
    R = np.zeros(G.shape,  dtype=np.float32)
    B = np.zeros(G.shape,  dtype=np.float32)
    R[::2, 1::2, 0] = img[::2, 1::2, 0]
    B[1::2, ::2, 0] = img[1::2, ::2, 0]
    G[::2, ::2, 0] = img[::2, ::2, 0]
    G[1::2, 1::2, 0] = img[1::2, 1::2, 0]
    h1 = [0.5, 0.0, 0.5]
    for i in range(img.shape[0]):
        if i % 2 == 0:
            interpo = np.zeros(img.shape[1])
            interpo = np.convolve(h1, R[i, :, 0], "same")
            R[i, ::2, 0] = interpo[::2]
            # R[i, 1::2, 0] = R[i, 1::2, 0]
            # print("delta GR", delta_GR[i, :, 0])
        if i % 2 == 1:
            interpo = np.zeros(img.shape[1])
            interpo = np.convolve(h1, B[i, :, 0], "same")
            B[i, 1::2, 0] = interpo[1::2]
            # delta_GB[i, ::2, 0] = R[i, ::2, 0]
        
    for i in range(img.shape[1]):
            interpo_gr = np.zeros(img.shape[0])
            interpo_gr = np.convolve(h1, R[:, i, 0], "same")
            interpo_gb = np.zeros(img.shape[0])
            interpo_gb = np.convolve(h1, B[:, i, 0], "same")
            R[1::2, i, 0] = interpo_gr[1::2]
            B[::2, i, 0] = interpo_gb[::2]

    M = [[0.0, 0.25, 0.0], 
        [0.25, 0.0, 0.25],
        [0.0, 0.25, 0.0]
    ]

    # print(G.shape)

    G_inter = signal.convolve2d(G.reshape(G.shape[0], G.shape[1]), M, boundary='symm', mode='same')
    
    G[1::2, ::2, 0] = G_inter[1::2, ::2]
    G[::2, 1::2, 0] = G_inter[::2, 1::2]

    rgb = np.concatenate([B, G, R], axis=2)
    # print(rgb.shape)

    # cv2.imwrite(wb_dir+"blur.exr", rgb)

    return rgb

    
def demosiac_AHD(img):
    cv2.imwrite(wb_dir+"normalized.exr", img)
    img_t = img.transpose(1, 0, 2)
    # print(img_t.shape)
    # print(img.shape)
    fh = computef(img)
    fv = computef(img_t).transpose(1, 0, 2)
    temp = np.zeros(fv.shape)
    temp[:, :, 2] = fv[:, :, 0] 
    fv[:, :, 0] = fv[:, :, 2]
    fv[:, :, 2] = temp[:, :, 2]
    
    f = np.zeros(fh.shape, dtype=np.float32)

    fh_lab = LRGB2CIELAB(fh)
    fv_lab = LRGB2CIELAB(fv)

    eL_h = compute_epl(fh_lab, "h", "L")
    eC_h = compute_epl(fh_lab, "h", "ab")

    eL_v = compute_epl(fv_lab, "v","L")
    eC_v = compute_epl(fv_lab, "v","ab")

    eL = np.minimum(eL_v, eL_h)
    eC = np.minimum(eC_v, eC_h)

    Hf_h = compute_H(fh_lab, eL, eC, 3)
    Hf_v = compute_H(fv_lab, eL, eC, 3)

    cv2.imwrite(wb_dir+"Hf_h.exr", Hf_h)
    cv2.imwrite(wb_dir+"Hf_v.exr", Hf_v)

    cv2.imwrite(wb_dir+"f_h.exr", fh)
    cv2.imwrite(wb_dir+"f_v.exr", fv)

    
    A = np.ones([3, 3],dtype=np.float32) / 9.0
    # print(A)
    # print(Hf_h.shape)
    cov_Hf_h = signal.convolve2d(Hf_h, A, boundary='symm', mode='same')
    cov_Hf_v = signal.convolve2d(Hf_v, A, boundary='symm', mode='same')
    # print(cov_Hf_h.shape)
    f[cov_Hf_h > cov_Hf_v] = fh[cov_Hf_h > cov_Hf_v]
    f[cov_Hf_h < cov_Hf_v] = fv[cov_Hf_h < cov_Hf_v]
    f[cov_Hf_h == cov_Hf_v] = (0.5 * fv[cov_Hf_v == cov_Hf_h] + 0.5 * fh[cov_Hf_v == cov_Hf_h])

    # print("size of f", f.shape)
    # iterate 3 times
    # fiter = f.copy()
    fbgr = np.zeros(f.shape, dtype=np.float32)
    fbgr[:, :, 0] = f[:, :, 2]
    fbgr[:, :, 1] = f[:, :, 1]
    fbgr[:, :, 2] = f[:, :, 0]
    fiter = fbgr.copy()
    for i in range(3):
        fiter = compute_m(fiter, 3)
    # print(fiter.shape)

    
    # cv2.imwrite(wb_dir+"f3.exr", fbgr)
    # cv2.imwrite(wb_dir+"f3_i3.exr", fiter)
    # cv2.imwrite(wb_dir+"img_v_xyz.exr", fv_xyz_n)
    # exit(0)
    return fiter

# name = "/example.hdr"
# img = cv2.imread(file_dir + name, cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)

# demosiac_blur3x3(img, name)
# demosiac_AHD(img, name)      
raws = RawImage(file_dir)
raws.bw2color()
raws.arrangeAll()

# for cid in range(1,2):
#     for fid in range(4,5):
#         # print(file_dir+"/c{}_l{}_mono_hdr.hdr")
#         name = "/demo_c{}_l{}_mono_hdr.hdr".format(cid, fid)
        
#         img = cv2.imread(file_dir+"/c{}_l{}_mono_hdr.hdr".format(cid, fid), cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
#         # print(img)
#         # exit(0)
#         demosiac_blur3x3(img, name)
#         # demo_img = demosiac_AHD(img, name)
#         exit(0)