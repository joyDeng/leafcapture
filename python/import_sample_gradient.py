import numpy as np

def sample_sigma_t_Tr(rnd, sigma_t):
    return -np.log(1-rnd) / sigma_t


def sample_Tr(rnd, sigma_t):
    return -np.log(1 - sigma_t * rnd) / sigma_t