#pragma once

#include <mutex>
#include <vector>

template<class T>
class ObjectAdder
{
public:
	void add(T&& new_obj)
	{
		std::lock_guard l(mutex);
		new_elems.push_back(new_obj);
	}

protected:
	std::mutex mutex;
	std::vector<T> new_elems;
};
