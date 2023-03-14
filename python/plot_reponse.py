import cv2
import matplotlib.pyplot as plt
import numpy as np

response = cv2.imread("../build/green4_response.hdr")

print(response.shape)

# plt.style.use('_mpl-gallery')

# make data
x = np.linspace(0, 1, 256)
# y = 4 + 2 * np.sin(2 * x)

# plot
fig, ax = plt.subplots()

ax.plot(x, np.array(response[:, 0, 0]), color="r", linewidth=2.0)
ax.plot(x, np.array(response[:, 0, 1]), color="g", linewidth=2.0)
ax.plot(x, np.array(response[:, 0, 2]), color="b", linewidth=2.0)

# ax.set(xlim=(0, 8), xticks=np.arange(1, 8),
#        ylim=(0, 8), yticks=np.arange(1, 8))

plt.show()
