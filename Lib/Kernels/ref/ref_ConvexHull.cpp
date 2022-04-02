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

template<typename Comparator = std::less<int>>
bool point_pos(const vx_coordinates2d_t t, const vx_coordinates2d_t L, const vx_coordinates2d_t H, Comparator comp = Comparator{}) {
	return comp(int((t.x - L.x) * (H.y - L.y) - (H.x - L.x) * (t.y - L.y)),0);
}

double dist_to_line(const double diffx, const double diffy, const vx_coordinates2d_t t, const vx_coordinates2d_t L, const vx_coordinates2d_t R) {
	double znam = sqrt(diffy * diffy + diffx * diffx);
	return (abs(diffy * t.x + diffx * t.y + R.x * L.y - L.x * R.y)) / znam;
}


void FindCords(const std::vector<vx_coordinates2d_t>& arr, const vx_coordinates2d_t L, const vx_coordinates2d_t R,const vx_coordinates2d_t H, std::vector<vx_coordinates2d_t>& S1,//higher LH
	std::vector<vx_coordinates2d_t>& S2) {
	size_t sz = arr.size();
	for (size_t i = 0; i < sz; ++i) {
		vx_coordinates2d_t it = arr[i];
	
		if (int((it.x - L.x) * (H.y - L.y) - (H.x - L.x) * (it.y - L.y)) > 0)
				S1.push_back(it);
			else if (int((it.x - R.x) * (H.y - R.y) - (H.x - R.x) * (it.y - R.y)) > 0)
				S2.push_back(it);
	
	}
}

std::vector<vx_coordinates2d_t> FindHull(const std::vector<vx_coordinates2d_t>& arr, const vx_coordinates2d_t L, const vx_coordinates2d_t R,bool bot) {
	const size_t sz = arr.size();
	//std::cout <<"sz is "<< sz << "\n";
	if (sz <= 0)
		return arr;
	double mxdist = -1e9;
	const double diffy = (double)R.y - L.y;
	const double diffx = (double)R.x - L.x;

	vx_coordinates2d_t H;//the most remote point from LR
	for (size_t i = 0; i < sz; ++i) {
		vx_coordinates2d_t it = arr[i];
		double dist = dist_to_line(diffx,diffy,it,L,R);
		if (dist > mxdist) {
			mxdist = dist;
			H = it;
		}
	}
	//std::cout << H.x << ", " << H.y << "\n";
	std::vector<vx_coordinates2d_t> S1;//higher LH
	std::vector<vx_coordinates2d_t> S2;//higher HR
	for (size_t i = 0; i < sz; ++i) {
		vx_coordinates2d_t it = arr[i];
		if (bot) {
			if (int((it.x - L.x) * (H.y - L.y) - (H.x - L.x) * (it.y - L.y)) > 0)
				S1.push_back(it);
			else if (int((it.x - R.x) * (H.y - R.y) - (H.x - R.x) * (it.y - R.y)) > 0)
				S2.push_back(it);
		}
		else {
			if (int((it.x - L.x) * (H.y - L.y) - (H.x - L.x) * (it.y - L.y)) < 0)
				S1.push_back(it);
			else if (int((it.x - R.x) * (H.y - R.y) - (H.x - R.x) * (it.y - R.y)) < 0)
				S2.push_back(it);
		}
	}
	std::vector<vx_coordinates2d_t> l1 = std::move(FindHull(S1, L, H,bot));
	std::vector<vx_coordinates2d_t> l2 = std::move(FindHull(S2, H, R,bot));
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
		if (point_pos(current_cord, L, R))
			S1.push_back(current_cord);
		else S2.push_back(current_cord);
	}
	std::cout << S1.size() << " " << S2.size() << '\n';
	
	std::vector<vx_coordinates2d_t> v1 = std::move(FindHull(S1, L, R,false));//Its temporary object
	std::vector<vx_coordinates2d_t> v2 = std::move(FindHull(S2, L, R,true));
	v1.insert(v1.begin(), v2.begin(), v2.end());
	v1.push_back(L);
	v1.push_back(R);

	uint32_t sz1 = v1.size();
	void* p = (void*)&(*v1.begin());

	_vx_array res[]{
		p,
		sz1,
		VX_TYPE_COORDINATES2D
	};
	vx_coordinates2d_t* beg1 = (vx_coordinates2d_t*)(res->data);
	for (auto&& v : v1) {
		std::cout << "("<<v.x << ", " << v.y << ")\n";
	}

	return res;

}