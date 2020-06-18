#pragma once

struct content_info;

template <class T>
class NewBucket {
public:
	NewBucket() = default;
	~NewBucket() = default;
	NewBucket(const NewBucket&) = delete;

	std::vector<T*>& getContent() { return content; }
	void add(T* entity);
	void remove(T* entity);
private:
	std::vector<T*> content;
};

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
