#include <cornellBoxScene.h>
#include <gtest/gtest.h>
#include <PhotonMap.h>
#include <PhotonSceneRenderer.h>

struct PosCoordPair
{
	PosCoordPair(Point3D p, Point2D c)
	{
		position = p;
		coord = c;
	}
	Point3D position;
	Point2D coord;
};

class TestPhotonSceneRenderer : public ::testing::TestWithParam<const PosCoordPair*>
{
protected:
	virtual void SetUp()
	{
		scene_ = new CornellBoxScene(50, 50);
		photonMap_ = new PhotonMap(*scene_, 50);

		patient_ = new PhotonSceneRenderer(*scene_, *photonMap_);
	}

	virtual void TearDown()
	{
		delete patient_;

		delete photonMap_;
		delete scene_;
	}

   	CornellBoxScene* scene_;
	PhotonMap *photonMap_;

	PhotonSceneRenderer* patient_;
};

TEST_P(TestPhotonSceneRenderer, CorrectImageCoords)
{
	const PosCoordPair *pair = GetParam();
	Point3D position = pair->position;
	Point2D expectedCoord = pair->coord;
	Point2D coord = patient_->calc_image_coords(position);
	double EPSILON = 1e-6;

	EXPECT_NEAR(coord[0], expectedCoord[0], EPSILON);
	EXPECT_NEAR(coord[1], expectedCoord[1], EPSILON);

	delete pair;
}

INSTANTIATE_TEST_CASE_P(CorrectImageCoords_Origin,
                        TestPhotonSceneRenderer,
                        ::testing::Values(
                        	new PosCoordPair(
                        		Point3D(0, 0, -5),
                        	 	Point2D(25, 25))));


INSTANTIATE_TEST_CASE_P(CorrectImageCoords_Up,
                        TestPhotonSceneRenderer,
                        ::testing::Values(
                        	new PosCoordPair(
                        		Point3D(0, 1, -5),
                        	 	Point2D(25, 15))));


INSTANTIATE_TEST_CASE_P(CorrectImageCoords_Down,
                        TestPhotonSceneRenderer,
                        ::testing::Values(
                        	new PosCoordPair(
                        		Point3D(0, -1, -5),
                        	 	Point2D(25, 35))));

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
