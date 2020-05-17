#pragma once

#include <vector>
#include <memory>

template<typename T, typename F>
void remove_if(std::vector<T>& v, F&& func)
{
	auto curr = v.begin();
	auto last_empty = v.begin();
	auto end = v.end();
	while(curr != end) {
		if(!func(*curr)) {
			if(curr != last_empty) {
				*last_empty = std::move(*curr);
			}
			++last_empty;
		}

		++curr;
	}

	if(last_empty != curr) {
		v.resize(last_empty - v.begin());
	}
}

template<typename T, typename F>
void clean_and_for_each_valid(std::vector<std::weak_ptr<T>>& v, F&& func)
{
	remove_if(v, [&] (std::weak_ptr<T>& wptr) {
		std::shared_ptr<T> ptr = wptr.lock();
		if(!ptr) {
			return true;
		}

		func(std::move(ptr));
		return false;
	});
}
