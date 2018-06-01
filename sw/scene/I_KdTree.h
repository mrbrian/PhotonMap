#pragma once

#include <vector>
#include <photon.h>

class I_KdTree
{
public:
	virtual ~I_KdTree(){};

	virtual std::vector<photon> getKNearest(const photon &p, unsigned int m_samples_per_pixel) = 0;
};
