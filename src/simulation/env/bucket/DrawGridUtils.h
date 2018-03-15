#pragma once
#include <vector>
#include <Urho3D/Resource/Image.h>

int getIndex(const short posX, const short posZ, const short resolution) {
	return posX * resolution + posZ;
}

Urho3D::IntVector2 getCords(const int index, const short resolution) {
	return Urho3D::IntVector2(index / resolution, index % resolution);
}

void draw_grid_path(std::vector<int>* path, Urho3D::Image* image, short resolution) {
	uint32_t* data = (uint32_t*)image->GetData();
	for (auto value : *path) {
		Urho3D::IntVector2 a = getCords(value, resolution);
		int idR = getIndex(resolution - a.y_ - 1, a.x_, resolution);
		*(data + idR) -= 0x0000007F;
	}
}

void draw_grid_from(int* cameFrom, Urho3D::Image* image, short resolution) {
	uint32_t* data = (uint32_t*)image->GetData();
	for (int y = 0; y != resolution; ++y) {
		for (int x = 0; x != resolution; ++x) {
			int id = getIndex(x, y, resolution);
			if (cameFrom[id] != -1) {
				Urho3D::IntVector2 cords2 = getCords(cameFrom[id], resolution);
				int x2 = cords2.x_;
				int y2 = cords2.y_;
				int idR = getIndex(resolution - y - 1, x, resolution);
				if (x2 == x + 1) {
					*(data + idR) -= 0x00003F00;
				} else if (x2 == x - 1) {
					*(data + idR) -= 0x00007F00;
				} else if (y2 == y + 1) {
					*(data + idR) -= 0x0000BF00;
				} else if (y2 == y - 1) {
					*(data + idR) -= 0x0000FF00;
				}
			}
		}
	}
}

void draw_grid_cost(const float* costSoFar, Urho3D::Image* image, short resolution) {
	uint32_t* data = (uint32_t*)image->GetData();

	for (short y = 0; y != resolution; ++y) {
		for (short x = 0; x != resolution; ++x) {
			int id = getIndex(x, y, resolution);
			if (costSoFar[id] != -1) {
				int idR = getIndex(resolution - y - 1, x, resolution);
				*(data + idR) -= 0x0000007F;
			}
		}
	}
}
