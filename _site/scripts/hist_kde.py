import ctypes
import numpy as np


lib = ctypes.CDLL("scripts/hist_kde.so")


class HistOutput(ctypes.Structure):
    _fields_ = [
        ("counts", ctypes.POINTER(ctypes.c_int)),
        ("left_bins", ctypes.POINTER(ctypes.c_double)),
        ("bin_width", ctypes.c_double),
    ]


class KdeOutput(ctypes.Structure):
    _fields_ = [
        ("kernel_density", ctypes.POINTER(ctypes.c_double)),
        ("mu_position", ctypes.POINTER(ctypes.c_double)),
    ]


# Defining io of c functions
lib.kde.argtypes = [
    ctypes.POINTER(ctypes.c_double),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_double,
]

lib.kde.restype = KdeOutput

lib.hist.argtypes = [ctypes.POINTER(ctypes.c_double), ctypes.c_int, ctypes.c_int]

lib.hist.restype = HistOutput

lib.free_kde_output.argtypes = [KdeOutput]

lib.free_hist_output.argtypes = [HistOutput]


def kde(data: np.ndarray, sigma: float, n_points: int = 100):
    n = len(data)
    res = lib.kde(np.ctypeslib.as_ctypes(data), n, n_points, sigma)

    kernel_densities = np.copy(np.ctypeslib.as_array(res.kernel_density, (n_points,)))
    mu_positions = np.copy(np.ctypeslib.as_array(res.mu_position, (n_points,)))

    lib.free_kde_output(res)

    return kernel_densities, mu_positions


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    n = 1000
    x1 = np.random.normal(0.0, 2.0, n)
    x2 = np.random.normal(3.0, 1.0, n)

    x = np.concatenate((x1, x2), axis=-1)

    kernel_dens, mu_pos = kde(x, 0.5)

    fig, ax = plt.subplots()

    ax.plot(mu_pos, kernel_dens)

    plt.savefig("kernel_dens.jpg")
