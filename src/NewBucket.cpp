#include "NewBucket.h"

#include "objects/resource/ResourceEntity.h"

template <class T>
void NewBucket<T>::add(T* entity) {
	content.push_back(entity);
}

template <class T>
void NewBucket<T>::remove(T* entity) {
	auto pos = std::find(content.begin(), content.end(), entity) - content.begin();

	if (pos < content.size()) {
		content.erase(content.begin() + pos);
		// std::iter_swap(content.begin() + pos, content.end()-1);
		// content.erase(content.end()-1);
	}
}

template class NewBucket<ResourceEntity>;
