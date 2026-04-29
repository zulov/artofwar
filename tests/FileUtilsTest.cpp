#include "pch.h"

#include <chrono>
#include <filesystem>
#include <fstream>

#include "utils/FileUtils.h"

namespace {
std::filesystem::path makeTempPath() {
	const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
	return std::filesystem::temp_directory_path() / ("artofwar_brain_" + std::to_string(stamp) + ".txt");
}
}

class FileUtilsFixture : public ::testing::Test {
protected:
	std::vector<std::filesystem::path> createdPaths;

	std::filesystem::path createBrainFile(const std::string& content) {
		auto path = makeTempPath();
		std::ofstream out(path, std::ios::binary);
		out << content;
		out.close();
		createdPaths.push_back(path);
		return path;
	}

	void TearDown() override {
		for (const auto& path : createdPaths) {
			std::error_code ec;
			std::filesystem::remove(path, ec);
		}
	}
};

TEST_F(FileUtilsFixture, MissingFileReturnsFalseAndKeepsExistingLayers) {
	std::vector<LayerData> layers = { LayerData{ { 1.f }, { 2.f } } };
	auto missingPath = makeTempPath();

	EXPECT_FALSE(loadBrainFile(missingPath.string(), layers));
	ASSERT_EQ(layers.size(), 1u);
	EXPECT_EQ(layers[0].weights, std::vector<float>({ 1.f }));
	EXPECT_EQ(layers[0].biases, std::vector<float>({ 2.f }));
}

TEST_F(FileUtilsFixture, ParsesWeightsAndBiasesSeparatedByDoubleSemicolon) {
	auto path = createBrainFile("1.5;2.25;;3.5;4.75\n");
	std::vector<LayerData> layers;

	ASSERT_TRUE(loadBrainFile(path.string(), layers));
	ASSERT_EQ(layers.size(), 1u);
	EXPECT_EQ(layers[0].weights, std::vector<float>({ 1.5f, 2.25f }));
	EXPECT_EQ(layers[0].biases, std::vector<float>({ 3.5f, 4.75f }));
}

TEST_F(FileUtilsFixture, ParsesMultipleLayersAndSkipsGarbageCharacters) {
	auto path = createBrainFile("\r\n1;x;2;;3\r\n;4;5\r\n");
	std::vector<LayerData> layers;

	ASSERT_TRUE(loadBrainFile(path.string(), layers));
	ASSERT_EQ(layers.size(), 2u);
	EXPECT_EQ(layers[0].weights, std::vector<float>({ 1.f, 2.f }));
	EXPECT_EQ(layers[0].biases, std::vector<float>({ 3.f }));
	EXPECT_TRUE(layers[1].weights.empty());
	EXPECT_EQ(layers[1].biases, std::vector<float>({ 4.f, 5.f }));
}

TEST_F(FileUtilsFixture, EmptyFileLoadsSuccessfullyWithNoLayers) {
	auto path = createBrainFile("");
	std::vector<LayerData> layers = { LayerData{ { 1.f }, { 2.f } } };

	ASSERT_TRUE(loadBrainFile(path.string(), layers));
	EXPECT_TRUE(layers.empty());
}

TEST_F(FileUtilsFixture, IgnoresBlankLinesBetweenLayers) {
	auto path = createBrainFile("1;2;;3\n\n4;;5\n");
	std::vector<LayerData> layers;

	ASSERT_TRUE(loadBrainFile(path.string(), layers));
	ASSERT_EQ(layers.size(), 2u);
	EXPECT_EQ(layers[0].weights, std::vector<float>({ 1.f, 2.f }));
	EXPECT_EQ(layers[0].biases, std::vector<float>({ 3.f }));
	EXPECT_EQ(layers[1].weights, std::vector<float>({ 4.f }));
	EXPECT_EQ(layers[1].biases, std::vector<float>({ 5.f }));
}
