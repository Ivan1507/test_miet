/*
File:Convex Hull
Simple Convex Hull implementation
Author: Razinkin Ivan
Date: 28.03.22
*/


#include "../ref.h"
#include <vector>
#include <iostream>
#include <math.h>


using vx2d_t=vx_coordinates2d_t;



template<typename Comparator = std::less<>>
bool PointPos(const vx2d_t t, const vx2d_t L, const vx2d_t H, Comparator comp = Comparator{}) {
	return comp(int((t.x - L.x) * (H.y - L.y) - (H.x - L.x) * (t.y - L.y)),0);
}

double DistoLine(const double diffx, const double diffy, const vx2d_t t, const vx2d_t L, const vx2d_t R) {
	double znam = sqrt(diffy * diffy + diffx * diffx);
	return (abs(diffy * t.x + diffx * t.y + R.x * L.y - L.x * R.y)) / znam;
}


void FindCords(const std::vector<vx2d_t>& arr, const vx2d_t L, const vx2d_t R,const vx2d_t H, std::vector<vx2d_t>& S1,//higher LH
	std::vector<vx2d_t>& S2) {
	size_t sz = arr.size();
	for (size_t i = 0; i < sz; ++i) {
		if (PointPos(arr[i], L, H))
				S1.push_back(arr[i]);
		else if (PointPos(arr[i], H, R))
			S2.push_back(arr[i]);
		
	
	}
}

std::vector<vx2d_t> FindHull(const std::vector<vx2d_t>& arr, const vx2d_t L, const vx2d_t R) {
	const size_t sz = arr.size();
	if (sz <= 0)
		return arr;
	double mxdist = -1e9;
	const double diffy = (double)R.y - L.y;
	const double diffx = (double)R.x - L.x;

	vx2d_t H;//the most remote point from LR
	for (size_t i = 0; i < sz; ++i) {
		double dist = DistoLine(diffx,diffy,arr[i], L, R);
		if (dist > mxdist) {
			mxdist = dist;
			H = arr[i];
		}
	}
	std::vector<vx2d_t> S1;//higher LH
	std::vector<vx2d_t> S2;//higher HR
	FindCords(arr,L,R,H,S1,S2);
	std::vector<vx2d_t> l1 = std::move(FindHull(S1, L, H));
	std::vector<vx2d_t> l2 = std::move(FindHull(S2, H, R));
	l1.push_back(H);
	l1.insert(l1.end(), l2.begin(), l2.end());
	return l1;

}

extern "C" vx_array ref_ConvexHull(const vx_array src_array) {
	const uint32_t len = src_array->size;
	if (len <= 2) return src_array;
	vx2d_t* beg = (vx2d_t*)(src_array->data);
	const vx2d_t L = *beg;//the most left point
	const vx2d_t R = *(beg + len - 1);//the most right point
	std::vector<vx2d_t> S1;// points that higher than LR
	std::vector<vx2d_t> S2;//points that lower than LR

	for (uint32_t i = 1; i < len - 1; ++i) {
		vx2d_t current_cord = *(beg + i);
		if (PointPos(current_cord, L, R))
			S1.push_back(current_cord);
		else S2.push_back(current_cord);
	}

	std::vector<vx2d_t> v1 = std::move(FindHull(S1, L, R));//Its temporary object
	std::vector<vx2d_t> v2 = std::move(FindHull(S2, L, R));
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

	vx2d_t* beg1 = (vx2d_t*)(res->data);
	std::cout<<"Number points: "<<v1.size()<<'\n';
	for (auto&& v : v1) {
		std::cout << "("<<v.x << ", " << v.y << ")\n";
	}

	return res;

}