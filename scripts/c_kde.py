import ctypes
import os
from typing import List, Tuple

LIB_NAME = "hist_lib.so"

lib = ctypes.CDLL(LIB_NAME)
print(f"Succesful loading: {lib}")


# Define c struct
class KdeOutput(ctypes.Structure):
    _fields_ = [
        ("kde", ctypes.POINTER(ctypes.c_double)),
        ("bin_positions", ctypes.POINTER(ctypes.c_double)),
    ]


lib.kde_wrapper.argtypes = [
    ctypes.POINTER(ctypes.c_double),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_double,
]

lib.kde_wrapper.restype = KdeOutput

lib.free_kde_output.argtypes = [KdeOutput]
lib.free_kde_output.restype = None


def compute_kde(
    input_data: List, num_bins: int = 100, sigma: float = 1.0
) -> Tuple[List, List]:
    data_size = len(input_data)
    c_input_array = (ctypes.c_double * data_size)(*input_data)

    c_output = lib.kde_wrapper(c_input_array, data_size, num_bins, sigma)

    kde_weights = [c_output.kde[i] for i in range(num_bins)]
    bin_positions = [c_output.bin_positions[i] for i in range(num_bins)]

    # Free the memory allocated by the C library (assuming you have this function)
    lib.free_kde_output(c_output)  # Uncomment this when you have the C function

    return (kde_weights, bin_positions)


if __name__ == "__main__":
    import matplotlib.pyplot as plt
    import numpy as np

    # Small sample
    in_data = [1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0]
    num_bins = 100
    sigma = 1.0

    kde_weights, bin_positions = compute_kde(in_data)

    print(kde_weights)
    print("\n")

    print(bin_positions)

    fig, ax = plt.subplots()

    ax.plot(bin_positions, kde_weights)

    plt.savefig("simple_figure.jpg")

    # Mixture
    n = 100
    x1 = np.random.normal(-2.0, 1.0, n)
    x2 = np.random.normal(4.0, 1.0, n)

    x = np.concatenate((x1, x2), axis=-1).tolist()

    print(x)

    kde_weights, bin_positions = compute_kde(x)

    fig, ax = plt.subplots()

    ax.plot(bin_positions, kde_weights)

    plt.savefig("mixture_figure.jpg")
