#!/usr/bin/env python3
#coding: utf-8

import math
from pprint import pprint
import numpy as np

def XMMatrixLookAtLH(EyeFrom, LookAt, UpDirection):
	# Alt name: D3DXMatrixLookAtLH
	# [IntroD3D12] p181:
	Q = np.array(EyeFrom)
	T = np.array(LookAt)
	j = np.array(UpDirection)

	lookto = T - Q
	w = lookto / np.linalg.norm(lookto)
	jxw = np.cross(j, w)
	u = jxw / np.linalg.norm(jxw)

	v = np.cross(w, u)

	# Return the 4x4 view-matrix
	ret = np.array([
		[ u[0], v[0], w[0], 0 ],
		[ u[1], v[1], w[1], 0 ],
		[ u[2], v[2], w[2], 0 ],
		[ -np.dot(Q,u), -np.dot(Q,v), -np.dot(Q,w), 1 ],
	])
	return ret


def XMMatrixPerspectiveFovLH(FovAngleY_radian, AspectRatio, NearZ, FarZ):
	# Alt name: D3DXMatrixPerspectiveFovLH
	# [IntroD3D12] p189:
	tan = math.tan(FovAngleY_radian/2)
	r = AspectRatio
	f = FarZ
	n = NearZ
	ret = np.array([
		[ 1/(r*tan), 0, 0,       0 ],
		[ 0,     1/tan, 0,       0 ],
		[ 0,         0, f/(f-n), 1 ],
		[ 0,         0, -n*f/(f-n), 0]
	])
	return ret


if __name__=='__main__':

	# Example from [DX9Pipeline] VertexShader3.1_chj.cpp
	vm1 = XMMatrixLookAtLH([0,0,-3], [0,0,0], [0,1,0])
	print(vm1)
	"""
	[[ 1.  0.  0.  0.]
	 [ 0.  1.  0.  0.]
	 [ 0.  0.  1.  0.]
	 [-0. -0.  3.  1.]]
	"""

	vm2 = XMMatrixLookAtLH([0,3,-3], [0,1,0], [0,1,0])
	print(vm2)
	"""
	[[ 1.          0.          0.          0.        ]
	 [ 0.          0.83205029 -0.5547002   0.        ]
	 [-0.          0.5547002   0.83205029  0.        ]
	 [-0.         -0.83205029  4.16025147  1.        ]]
	 """

	vm3 = XMMatrixLookAtLH([3,3,-3], [0,1,0], [0,1,0])
	print(vm3)
	"""
	[[ 0.70710678 -0.30151134 -0.63960215  0.        ]
	 [-0.          0.90453403 -0.42640143  0.        ]
	 [ 0.70710678  0.30151134  0.63960215  0.        ]
	 [-0.         -0.90453403  5.11681719  1.        ]]
	"""

	pm1 = XMMatrixPerspectiveFovLH(math.pi/4, 400/300, 0.5, 1000)
	print(" pm1=\n" + str(pm1))
	"""
	[[ 1.81066017  0.          0.          0.        ]
	 [ 0.          2.41421356  0.          0.        ]
	 [ 0.          0.          1.00050025  1.        ]
	 [ 0.          0.         -0.50025013  0.        ]]
	"""

	exit(0)
