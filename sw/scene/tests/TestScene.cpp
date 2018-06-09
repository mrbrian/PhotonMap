#include <algebra.h>
#include <gtest/gtest.h>
#include <Light.h>
#include <scene.h>
#include <stdio.h>

class TestScene : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		patient_ = new Scene(10);
	}

	// virtual void TearDown() {}

    Camera cam_;
    ImagePlane *imgPlane_;
    std::vector<I_SceneObject*> objects_;
    std::vector<Light*> lights_;

	Scene* patient_;
};

// TEST_F(TestScene, CorrectAmountOfPhotons)
// {
// 	vector<photon*> out_photons;
// 	Light light(Point3D(0,0,0), Color(0,0,0), 0);

// 	patient_->lights.push_back(&light);
// 	patient_->initialize_photons(1, &out_photons);

// 	EXPECT_EQ(1, out_photons.size());

	//setup scene

	// Light light;
	// Scene* scene(light);

	// //emit_photon
	// vector<photon*> out_photons;
	// scene->emit_photons(1, &out_photons);

	//count splats
	// should be only one

	// EXPECT_EQ(1, out_photons.size());
// }


// TEST_F(TestScene, WillDeletePhotons)
// {
// 	vector<photon*> out_photons;
// 	Light light(Point3D(0,0,0), Color(0,0,0), 0);
// 	Scene* scene(light);

// 	vector<photon*> out_photons;
// 	scene->emit_photons(1, &out_photons);

// 	EXPECT_EQ(1, out_photons.size());
// }

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
