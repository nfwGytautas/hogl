import struct
import numpy as np

version = np.uint32(0)
data = []

# Endian check
data.append(struct.pack('<L', 0x01234567))

# Version
data.append(version)

# Item count
data.append(np.uint64(1))

# Data size
data.append(np.uint64(4 + 4 + 4 + 8 + 4))

# Type
data.append(np.uint32(0))

# Name
data.append(np.uint32(4))
data.append("test".encode())

# Data
data.append(np.uint64(4))
data.append(np.uint32(50))

with open("test.hvf", "wb") as f:
	for el in data:
		f.write(el)
