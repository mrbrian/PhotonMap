#include <gtest/gtest.h>
#include <photon.h>

class PhotonTest : public ::testing::Test 
{
protected:
	virtual void SetUp() 
	{
		patient_ = new photon();
	}

	// virtual void TearDown() {}

	photon* patient_;
};