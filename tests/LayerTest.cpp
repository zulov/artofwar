#include "pch.h"

#include <array>
#include <cmath>

#include "player/ai/nn/Brain.h"
#include "player/ai/nn/Layer.h"
#include "utils/FileUtils.h"

#include "../game/src/player/ai/nn/Layer.cpp"
#include "../game/src/player/ai/nn/Brain.cpp"

namespace {
std::vector<LayerData> makeSimpleBrainLayers() {
	return {
		LayerData{ {}, { 0.f, 0.f } },
		LayerData{ { 1.f, 2.f, 3.f, 4.f }, { 0.5f, -0.25f } }
	};
}
}

class LayerFixture : public ::testing::Test {};

class BrainFixture : public ::testing::Test {};

TEST_F(LayerFixture, InputLayerStoresValuesAndDetectsExactRepeats) {
	std::vector<float> weights;
	std::vector<float> bias{ 0.f, 0.f, 0.f };
	Layer layer(weights, bias);

	const std::array<float, 3> input{ 1.f, -2.f, 3.5f };
	EXPECT_FALSE(layer.setInput(input));

	ASSERT_EQ(layer.getValues().size(), 3);
	EXPECT_FLOAT_EQ(layer.getValues()(0), 1.f);
	EXPECT_FLOAT_EQ(layer.getValues()(1), -2.f);
	EXPECT_FLOAT_EQ(layer.getValues()(2), 3.5f);

	EXPECT_TRUE(layer.setInput(input));

	const std::array<float, 3> changedInput{ 1.f, -2.f, 4.f };
	EXPECT_FALSE(layer.setInput(changedInput));
	EXPECT_FLOAT_EQ(layer.getValues()(2), 4.f);
}

TEST_F(LayerFixture, SetValuesAppliesMappedWeightsBiasAndTanh) {
	std::vector<float> weights{ 1.f, 2.f, 3.f, 4.f };
	std::vector<float> bias{ 0.5f, -0.25f };
	Layer layer(weights, bias);

	Eigen::Vector2f input;
	input << 0.1f, -0.2f;

	layer.setValues(input);

	ASSERT_EQ(layer.getValues().size(), 2);
	EXPECT_NEAR(layer.getValues()(0), std::tanh(0.2f), 1e-6f);
	EXPECT_NEAR(layer.getValues()(1), std::tanh(-0.75f), 1e-6f);
}

TEST_F(BrainFixture, DecideUsesInputHolderAndPropagatesThroughNextLayer) {
	auto layers = makeSimpleBrainLayers();
	Brain brain("unit-test", layers);

	const std::array<float, 2> input{ 0.1f, -0.2f };
	const auto result = brain.decide(input);

	EXPECT_EQ(brain.getName(), "unit-test");
	ASSERT_EQ(result.size(), 2u);
	EXPECT_NEAR(result[0], std::tanh(0.2f), 1e-6f);
	EXPECT_NEAR(result[1], std::tanh(-0.75f), 1e-6f);
}

TEST_F(BrainFixture, DecideRecomputesOutputWhenInputChanges) {
	auto layers = makeSimpleBrainLayers();
	Brain brain("unit-test", layers);

	const std::array<float, 2> firstInput{ 0.1f, -0.2f };
	const auto firstResult = brain.decide(firstInput);
	const std::vector<float> firstCopy(firstResult.begin(), firstResult.end());

	const std::array<float, 2> secondInput{ 0.2f, 0.1f };
	const auto secondResult = brain.decide(secondInput);

	ASSERT_EQ(secondResult.size(), 2u);
	EXPECT_GT(std::fabs(firstCopy[0] - secondResult[0]), 0.1f);
	EXPECT_GT(std::fabs(firstCopy[1] - secondResult[1]), 0.1f);
	EXPECT_NEAR(secondResult[0], std::tanh(0.9f), 1e-6f);
	EXPECT_NEAR(secondResult[1], std::tanh(0.75f), 1e-6f);
}
