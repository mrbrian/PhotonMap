#include <gtest/gtest.h>
#include <scene.h>

class SceneTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		patient_ = new Scene(10);
	}

	// virtual void TearDown() {}

    Camera cam_;
    ImagePlane *imgPlane_;
    std::vector<SceneObject*> objects_;
    std::vector<Light*> lights_;

	Scene* patient_;
};
