#pragma once
#include <vector>

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
