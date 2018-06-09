#include <cornellBoxScene.h>
#include <gtest/gtest.h>
#include <PhotonMap.h>
#include <PhotonSceneRenderer.h>

class TestPhotonSceneRenderer : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		scene_ = new CornellBoxScene(50, 50);
		photonMap_ = new PhotonMap(*scene_, 50);

		patient_ = new PhotonSceneRenderer(*scene_, *photonMap_);
	}

	// virtual void TearDown() {}

   	CornellBoxScene* scene_;
	PhotonMap *photonMap_;

	PhotonSceneRenderer* patient_;
};

TEST_F(TestPhotonSceneRenderer, CorrectAmountOfPhotons)
{
	// setupRendererWithPhotonMap();
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
