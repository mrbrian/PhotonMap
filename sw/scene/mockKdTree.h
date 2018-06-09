#include <gmock/gmock.h>
#include <I_KdTree.h>
#include <vector>

class photon;

class MockKdTree : public I_KdTree
{
public:
	MockKdTree(std::vector<photon*> *photon_map);

	MOCK_METHOD2(
		getKNearest,
		std::vector<photon>(const photon &p, unsigned int m_samples_per_pixel));

private:
	std::vector<photon*> *photons_;
};