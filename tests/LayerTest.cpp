#include "pch.h"

#include "simulation/ObjectsInfo.h"

//#include "../game/src/player/ai/nn/Layer.h"
//#include "../game/src/objects/ObjectEnums.h"
//#include "../game/src/simulation/ObjectsInfo.h"


class LayerFixture : public ::testing::Test {
public:


protected:
	static void SetUpTestCase() {
	}

	static void TearDownTestCase() {
	}
};


TEST_F(LayerFixture, Center) {
	std::vector<float> w{1, 1, 1, 1};
	std::vector<float> bias{1, 1, 1, 1};
	//auto layer = new Layer(w, bias);
	//delete layer;
	new ObjectsInfo();
	EXPECT_EQ(1, 1);
}
