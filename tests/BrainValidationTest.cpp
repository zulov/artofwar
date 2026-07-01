#include "pch.h"

#include <vector>

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/Log.h>

#include "Game.h"
#include "player/ai/nn/Brain.h"
#include "player/ai/nn/Layer.h"
#include "utils/FileUtils.h"

// NOTE: Layer.cpp and Brain.cpp are compiled into the test binary via their
// inclusion in LayerTest.cpp (same project). Do NOT include them here too or the
// linker reports duplicate symbols (LNK2005).

namespace {
// Builds a brain in the on-disk "input holder" layout:
//   layer 0 = input holder: `inputSize` weight columns (ignored at runtime), 1 bias.
//   then one dense layer per (in -> out) pair.
// `holderWeightCount` lets tests deliberately produce a wrong declared input size.
std::vector<LayerData> makeBrainLayers(int holderInputCols, const std::vector<std::pair<int, int>>& dense) {
	std::vector<LayerData> layers;

	// Input holder: Layer maps weights as [holderWeightCount/biasCount x biasCount].
	// With biasCount == 1, getInputSize() (cols) == holderInputCols.
	LayerData holder;
	holder.weights.assign(static_cast<size_t>(holderInputCols), 0.1f);
	holder.biases.assign(1, 0.f);
	layers.push_back(std::move(holder));

	for (const auto& [in, out] : dense) {
		LayerData layer;
		layer.weights.assign(static_cast<size_t>(in) * static_cast<size_t>(out), 0.1f);
		layer.biases.assign(static_cast<size_t>(out), 0.f);
		layers.push_back(std::move(layer));
	}
	return layers;
}
}

// Provides a minimal Urho3D Log so Brain::validate()'s failure path can log safely.
class BrainValidationFixture : public ::testing::Test {
protected:
	Urho3D::SharedPtr<Urho3D::Context> context;
	Urho3D::SharedPtr<Urho3D::Log> log;

	void SetUp() override {
		Game::init();
		context = new Urho3D::Context();
		log = new Urho3D::Log(context);
		Game::setLog(log);
	}

	void TearDown() override {
		Game::dispose();
		log.Reset();
		context.Reset();
	}
};

TEST_F(BrainValidationFixture, ValidHolderBrainPassesAndReportsSizes) {
	auto layers = makeBrainLayers(26, { {26, 16}, {16, 21} });
	Brain brain("valid.csv", layers);

	EXPECT_EQ(brain.getInputSize(), 26);
	EXPECT_EQ(brain.getOutputSize(), 21);
}

TEST_F(BrainValidationFixture, SingleHiddenLayerBrainValidates) {
	auto layers = makeBrainLayers(10, { {10, 6} });
	Brain brain("attack.csv", layers);

	EXPECT_EQ(brain.getInputSize(), 10);
	EXPECT_EQ(brain.getOutputSize(), 6);
}


