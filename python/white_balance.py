import cv2 
import numpy as np
import argparse

from scipy import optimize

parser = argparse.ArgumentParser()
parser.add_argument('--name', type=str, default='petal_')
sc = parser.parse_args()

DATA_ROOT = "/home/dx/Research/leaf/data/"
file_dir = DATA_ROOT + sc.name + "/"

import os

wb_dir = file_dir+"/{}_wb/".format(sc.name)

if not os.path.exists(wb_dir):
    os.makedirs(wb_dir)
# print(file_dir)
# exit(0)

# rs = []
# bs = []

# for i in range(1, 7):
#     print(i)
#     img = cv2.imread(file_dir+"{}_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
#     # print(img[775, 1213, :])
#     # print(img[1213, 775, :])
#     r = img[1259, 769, 1] / img[1259, 769, 0]
#     b = img[1259, 769, 1] / img[1259, 769, 2]
#     rs.append(r)
#     bs.append(b)

#     img[:, :, 0] *= img[1259, 769, 1] / img[1259, 769, 0]
#     img[:, :, 2] *= img[1259, 769, 1] / img[1259, 769, 2]
#     cv2.imwrite(file_dir+"{}_hdrDebevec_wb.hdr".format(i), img)

# print(rs)
# print(bs)
range_dict = {
    '0': [[1065, 1058, 1139, 1184], [1190, 1045, 1259, 1161], [1302, 1027, 1366, 1141], [1408, 1012, 1468, 1122], [1508, 996, 1561, 1105], [1605, 981, 1652, 1084]],
    '1': [[506, 1151, 596, 1279], [674, 1172, 771, 1308], [849, 1198, 949, 1336], [1024, 1225, 1122, 1359], [1198, 1253, 1299, 1387], [1375, 1276, 1470, 1402]],
    '2': [[2042, 502, 2182, 661], [1788, 466, 1876, 603], [1532, 430, 1638, 562], [1281, 396, 1413, 533], [1057, 374, 1185, 493], [800, 356, 925, 456]]
}

light_dict = {
    '0': [1, 4, 5],
    '1': [1, 3, 4, 5, 6],
    '2': [0, 2]
}

up_down = {
    '0': "up",
    '1': "up",
    '2': "down"
}

colors = np.array([[243, 243, 242], [200, 200, 200], [160, 160, 160], [122, 122, 121], [85, 85, 85], [52, 52, 52]], dtype=np.float)
ref_colors_regularized = colors / colors[:, 1].reshape(-1, 1)
# # print(colors_regularized)
# # exit(0)

light_dict_correction = {}

def white_balance_status(camera_id):
    for l in light_dict[camera_id]:
        image_name = "{}/color_cali_{}/c{}_l{}_hdrDebevec.hdr".format(DATA_ROOT, up_down[camera_id], camera_id, l)
        # print(image_name)
        image = cv2.imread(image_name, cv2.IMREAD_UNCHANGED)
        colors_regularized = []
        for i in range(6):
            color_box = range_dict[camera_id][i]
            b_color = image[ color_box[1] : color_box[3], color_box[0] : color_box[2]:]
            b_sum_0 = np.sum(b_color, axis=0)
            b_sum = np.sum(b_sum_0, axis=0)
            # print(b_sum)
            b_regular = b_sum / b_sum[1].reshape(-1, 1)
            colors_regularized.append(b_regular)
            # check 
            # cv2.imwrite("{}/color_cali_{}/c{}_l{}_b{}.hdr".format(DATA_ROOT, up_down[camera_id], camera_id, l, i), b_color)
        # print()
        colors_regularized = np.array(colors_regularized).reshape(-1, 3)
        # exit(0)
        RA = np.vstack([np.array(colors_regularized)[:, 0], np.zeros(6)]).T
        Rm, Rc = np.linalg.lstsq(RA, ref_colors_regularized[:, 0], rcond=None)[0]

        BA = np.vstack([np.array(colors_regularized)[:, 2], np.zeros(6)]).T
        Bm, Bc = np.linalg.lstsq(BA, ref_colors_regularized[:, 2], rcond=None)[0]

        # print(Rm * np.array(colors_regularized)[:, 0], ref_colors_regularized[:, 0])
        # print(Bm * np.array(colors_regularized)[:, 2], ref_colors_regularized[:, 2])
        

        print("camera_id", camera_id, "light", l, "Blue m", Rm, "Red m", Bm)
        


# # white_balance_status('0')
# # white_balance_status('1')
# # white_balance_status('2')
# # exit(0)

# #camera_id-light_id, blue, red
# corrections = {
#   "0-1": [1.566057737762876, 0.7409084503252773],
#   "0-4": [2.29092692484407, 0.4103836027894829],
#   "0-5": [2.9517756681150993, 0.620944678756729],
#   "1-1": [1.1463052094844977, 0.8274224840183456],
#   "1-3": [1.1945563685582845, 0.8360864770461229],
#   "1-4": [1.1242073872197746, 0.8352239946818203],
#   "1-5": [1.2114026710428565, 0.8337177967717393],
#   "1-6": [1.166873463057552, 0.8388988311201658],
#   "2-0": [1.1151537213519074, 0.7953494175506945],
#   "2-2": [1.1956255344699427, 0.809433074295525],
# }



# def white_balance():
#     rs = [1.2736843, 1.2949641, 1.2949641, 1.3222222, 1.2736843, 1.3095238, 1.3120568]
#     bs = [0.85211265, 0.8450704, 0.8450704, 0.8686131, 0.85211265, 0.84615386, 0.8604651]

#     for i in range(0,7):
#         for j in range(0, 3):
#             correction_id = "{}-{}".format(j, i)
#             img = cv2.imread(file_dir+"c{}_l{}_hdrDebevec.hdr".format(j, i), cv2.IMREAD_UNCHANGED)
#             # print(img[775, 1213, :])
#             # print(img[1213, 775, :])
            
#             if correction_id in corrections.keys():
#                 current_corrections = corrections[correction_id]
#                 img[:, :, 0] *= current_corrections[0]
#                 img[:, :, 2] *= current_corrections[1]
#                 print(current_corrections)
#             else:
#                 img[:, :, 0] *= rs[i]
#                 img[:, :, 2] *= bs[i]

#             # img /= 200.0
#             cv2.imwrite(wb_dir+"c{}_l{}.hdr".format(j, i), img)


# white_balance()
# exit(0)



def select_calibrate_view(name):
    fil_dir = DATA_ROOT + "/{}/".format(name)

    ret_path = fil_dir+"/cali/"
    if not os.path.exists(ret_path):
        os.makedirs(ret_path)
        

    for i in range(10):
        print(fil_dir+"/checker_{}_c0_l6_hdrDebevec.hdr".format(i))
        img0 = cv2.imread(fil_dir+"/checker_{}_c0_l6_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        if i == 7:
            img1 = cv2.imread(fil_dir+"/checker_{}_c1_l2_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        else:
            img1 = cv2.imread(fil_dir+"/checker_{}_c1_l6_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        if i == 9:
            img2 = cv2.imread(fil_dir+"/checker_{}_c2_l3_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        elif i == 3:
            img2 = cv2.imread(fil_dir+"/checker_{}_c2_l6_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        else:
            img2 = cv2.imread(fil_dir+"/checker_{}_c2_l4_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)

        img0 = img0 / 100.0
        if i==7:
            img1 = img1 / 10.0
        else:
            img1 = img1 / 100.0
        if i==3:
            img2 = img2 / 1.0
        else:
            img2 = img2 / 100.0

        cv2.imwrite(ret_path+"/checker_{}_c0.hdr".format(i), img0)
        cv2.imwrite(ret_path+"/checker_{}_c1.hdr".format(i), img1)
        cv2.imwrite(ret_path+"/checker_{}_c2.hdr".format(i), img2)

    for i in range(10, 14):
        
        if i == 11:
            img0 = cv2.imread(fil_dir+"/checker_{}_c0_l4_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)    
        else:
            img0 = cv2.imread(fil_dir+"/checker_{}_c0_l6_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        
        # if i == 10:
        img1 = cv2.imread(fil_dir+"/checker_{}_c1_l3_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)
        # else:
            # img1 = cv2.imread(fil_dir+"/checker_{}_c1_l3_hdrDebevec.hdr".format(i), cv2.IMREAD_UNCHANGED)

        if i == 11:
            img0 = img0 / 10
        else:
            img0 = img0 / 10
        

        img1 = img1 / 100

        cv2.imwrite(ret_path+"/checker_{}_c0.hdr".format(i), img0)
        cv2.imwrite(ret_path+"/checker_{}_c1.hdr".format(i), img1)
select_calibrate_view("calibrate_2")


# for i in range(4,5):
#     for j in range(0, 3):
#         img = cv2.imread(file_dir+"c{}_l{}_hdrDebevec.hdr".format(j, i), cv2.IMREAD_UNCHANGED)
#         # print(img[775, 1213, :])
#         # print(img[1213, 775, :])

#         img /= 5.0
#         cv2.imwrite(file_dir+"c{}_l{}_hdrDebevec.hdr".format(j, i), img)


