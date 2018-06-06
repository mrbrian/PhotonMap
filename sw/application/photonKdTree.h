#pragma once

#include <I_KdTree.h>
#include <kdtree/kdtree.h>
#include <l2Norm_2.h>
#include <photon.h>
#include <vector>

class PhotonKdTree : public I_KdTree
{
public:
	PhotonKdTree(std::vector<photon*> *photon_map);
	~PhotonKdTree();

	virtual std::vector<photon> getKNearest(const photon &p, unsigned int m_samples_per_pixel);

private:
	KdTree<photon,L2Norm_2,GetDim,3,float> *kdTree_;
};
