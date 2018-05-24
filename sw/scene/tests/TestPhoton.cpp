#include <algebra.h>
#include <gtest/gtest.h>
#include <PhotonTest.h>
#include <stdio.h>

TEST_F(PhotonTest, IsDist2Correct) 
{
	photon other;
	other.set_position(patient_->get_position());
	float dist = patient_->dist2(other);

	EXPECT_EQ(0, dist);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
