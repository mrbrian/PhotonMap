#pragma once

#include <vector>

class photon;

class I_Scene
{
public:

	virtual ~I_Scene() {};

	virtual void emit_photons(int num_photons, std::vector<photon*> *photon_map) = 0;
};
