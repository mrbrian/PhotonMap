#pragma once

#include <vector>

class Color;
class I_KdTree;
class photon;

class I_SceneRenderer
{
public:
	virtual ~I_SceneRenderer(){};

    virtual Color *Render() = 0;
    virtual Color *Render(std::vector<photon*> *photon_map) = 0;
    virtual Color *Render(I_KdTree *kd) = 0;
	virtual int imageWidth() = 0;
	virtual int imageHeight() = 0;
	virtual void emit_photons(int num_photons, std::vector<photon*> *photon_map) = 0;
};
