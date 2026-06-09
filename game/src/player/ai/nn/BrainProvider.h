#pragma once
#include <string>
#include <vector>

class Brain;
struct LayerData;
//TODO pref load all brains from sqllite as blobs?
class BrainProvider {
public:
	~BrainProvider();

	static Brain* get(std::string name);
private:
	BrainProvider() = default;
	static std::vector<Brain*> brains;
	static std::vector<LayerData> tempLayers;
	static std::string fileBuffer;
};
