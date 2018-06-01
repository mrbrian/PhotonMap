#include <photonKdTree.h>

PhotonKdTree::PhotonKdTree(KdTree<photon,L2Norm_2,GetDim,3,float> *kd)
	: kdTree_(kd)
{
}

PhotonKdTree::~PhotonKdTree()
{
}

std::vector<photon> PhotonKdTree::getKNearest(const photon &p, unsigned int m_samples_per_pixel)
{
	return kdTree_->getKNearest(p, m_samples_per_pixel);
}
