#include <gtest/gtest.h>
#include <cornellBoxScene.h>
#include <PhotonSceneRenderer.h>

class TestPhotonSceneRenderer : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		scene_ = new CornellBoxScene(50, 50);

		patient_ = new PhotonSceneRenderer(*scene_, 50);
	}

	// virtual void TearDown() {}

   	CornellBoxScene* scene_;

	PhotonSceneRenderer* patient_;
};

TEST_F(TestPhotonSceneRenderer, CorrectAmountOfPhotons)
{

}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
