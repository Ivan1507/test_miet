/*
File:Convex Hull
����� �������� ��������
Author: Razinkin Ivan
Date: 28.03.22
*/


#include "../ref.h"
#include <vector>
#include <iostream>
#include <math.h>

std::vector<vx_coordinates2d_t> FindHull(std::vector<vx_coordinates2d_t>& arr, const vx_coordinates2d_t L, const vx_coordinates2d_t R) {
	const size_t sz = arr.size();
	if (sz <= 0)
		return arr;
	double mxdist = -1e9;
	double diffy = R.y - L.y;
	double diffx = R.x - L.x;
	double znam = sqrt(diffy * diffy + diffx * diffx);
	vx_coordinates2d_t H;//the most remote point from LR

	auto beg = arr.begin();
	for (size_t i = 0; i < sz; ++i) {
		auto it = beg + i;
		double dist = (abs(diffy * (*it).x + diffx * (*it).y + R.x * L.y - L.x * R.y)) / znam;
		if (dist > mxdist) {
			mxdist = dist;
			H = *it;
		}
	}
	std::cout << H.x << " " << H.y << "\n";
	std::vector<vx_coordinates2d_t> S1;//higher LH
	std::vector<vx_coordinates2d_t> S2;//higher HR
	for (size_t i = 0; i < sz; ++i) {
		auto it = beg + i;
		if ((((*it).x - L.x) * (H.y - L.y) - (H.x - L.x) * ((*it).y - L.y)) < 0)
			S1.push_back(*it);
		else if ((((*it).x - R.x) * (H.y - R.y) - (H.x - R.x) * ((*it).y - R.y)) < 0)
			S2.push_back(*it);
	}

	std::vector<vx_coordinates2d_t> l1 = std::move(FindHull(S1, L, H));
	std::vector<vx_coordinates2d_t> l2 = std::move(FindHull(S2, H, R));
	l1.push_back(H);
	l1.insert(l1.end(), l2.begin(), l2.end());
	return l1;

}

extern "C" vx_array ref_ConvexHull(const vx_array src_array) {
	const uint32_t len = src_array->size;
	if (len <= 2) return src_array;
	vx_coordinates2d_t* beg = (vx_coordinates2d_t*)(src_array->data);
	const vx_coordinates2d_t L = *beg;
	const vx_coordinates2d_t R = *(beg + len - 1);
	std::vector<vx_coordinates2d_t> S1;// points that higher than LR
	std::vector<vx_coordinates2d_t> S2;//points that lower than LR

	for (uint32_t i = 1; i < len - 1; ++i) {
		vx_coordinates2d_t current_cord = *(beg + i);
		if (((current_cord.x - L.x) * (R.y - L.y) - (R.x - L.x) * (current_cord.y - L.y)) < 0)
			S1.push_back(current_cord);
		else S2.push_back(current_cord);
	}

	std::cout << S1.size() << " " << S2.size();
	std::vector<vx_coordinates2d_t> v1 = std::move(FindHull(S1, L, R));//Its just temporary object
	std::vector<vx_coordinates2d_t> v2 = std::move(FindHull(S2, L, R));
	v1.insert(v1.begin(), v2.begin(), v2.end());
	
	uint32_t sz1 = v1.size();
	void* p = (void*)&(*v1.begin());
	
	std::cout << v1[0].x << " " << v1[0].y << "\n";

	_vx_array res[]{
		p,
		sz1,
		VX_TYPE_COORDINATES2D
	};

	return res;
}