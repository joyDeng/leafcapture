from unicodedata import numeric
import yaml
import numpy as np

DATA_DIR = "/home/dx/Research/leaf/marker_mapper1.0.15/build/utils"

from pathlib import Path

yaml_dict = yaml.safe_load(Path(DATA_DIR+"/16h3.yml").read_text())

light_dict = {
    '0': [31, 31],
    '1': [11, 21],
    '2': [12, 41],
    '3': [14, 23],
    '4': [33, 43],
    '5': [3, 52],
    '6': [42, 13]
}

cam_dict = {
    '0': [53, 54, 4],
    '1': [51, 22],
    '2': [44, 24, 34],
}

corners = yaml_dict['aruco_bc_markers']
cid = 0
all_dict = {}
for corner in corners:
    id = list(corner.keys())[0].split(":")[1]
    print(id)
    co = np.array(corner['corners'])
    center = np.mean(co, axis=0)
    # print(corner)
    print(co, center)
    print("\n")
    cid += 1
    
    all_dict[id] = center * 100

# exit(0)
cam_pos_dict = {}
for i in range(3):
    idxs = cam_dict['{}'.format(i)]
    point = np.zeros(all_dict['0'].shape)
    number = 0
    for p in idxs:
        number += 1
        point += all_dict['{}'.format(p)]
    cam_pos = point / number
    cam_pos_dict['{}'.format(i)] = cam_pos

for i in range(1, 3):
    cam_pos_dict['{}'.format(i)] -= cam_pos_dict['0']

light_pos_dict = {}    
# print(all_dict)
for i in range(7):
    idxs = light_dict['{}'.format(i)]
    point_1 = all_dict['{}'.format(idxs[0])]
    point_2 = all_dict['{}'.format(idxs[1])]

    light_pos = (point_1 + point_2) / 2.0
    light_pos_dict['{}'.format(i)] = light_pos - cam_pos_dict['0']

cam_pos_dict['0'] -= cam_pos_dict['0']

for i in range(7):
    print("light ", i, " ", light_pos_dict['{}'.format(i)])

for i in range(3):
    print("camera ", i, " ", cam_pos_dict['{}'.format(i)])




# with open(DATA_DIR+"/16h3.yml", 'r') as file:
#     poses = yaml.safe_load_all(file)

#     # for doc in poses:
#     #     print(doc)
#     for pos in poses:
#         print(pos)
