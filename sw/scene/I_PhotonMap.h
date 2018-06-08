#pragma once 

class photon;

class I_PhotonMap
{
public:
	virtual const photon &getPhoton(int i) const = 0;
	virtual int count() const = 0;
};