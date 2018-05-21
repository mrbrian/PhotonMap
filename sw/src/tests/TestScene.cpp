#include <algebra.h>
#include <gtest/gtest.h>
#include <scene.h>
#include <SceneTest.h>
#include <stdio.h>

TEST_F(SceneTest, CorrectAmountOfPhotons)
{
	vector<photon*> out_photons;
	MockLight light(Point3D(0,0,0), Color(0,0,0), 0);

	patient_->lights.push_back(&light);
	patient_->initialize_photons(1, &out_photons);

	EXPECT_EQ(1, out_photons.size());

	//setup scene

	// Light light;
	// Scene* scene(light);

	// //emit_photon
	// vector<photon*> out_photons;
	// scene->emit_photons(1, &out_photons);

	//count splats
	// should be only one

	// EXPECT_EQ(1, out_photons.size());
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
