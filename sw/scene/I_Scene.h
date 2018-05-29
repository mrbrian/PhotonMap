#pragma once

#include <vector>
#include <kdtree/kdtree.h>
#include <l2Norm_2.h>
#include <scene.h>

class Color;
class photon;

class I_Scene
{
public:

	virtual ~I_Scene(){};

    virtual Color *Render() = 0;
    virtual Color *Render(std::vector<photon*> *photon_map) = 0;
    virtual Color *Render(KdTree<photon,L2Norm_2,GetDim,3,float> *kd) = 0;
	virtual int imageWidth() = 0;
	virtual int imageHeight() = 0;
	virtual void emit_photons(int num_photons, std::vector<photon*> *photon_map) = 0;
};
