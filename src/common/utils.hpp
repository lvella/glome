#pragma once

#include <vector>
#include <memory>
#include <algorithm>

class NonCopyable {
public:
	NonCopyable() = default;

	NonCopyable(NonCopyable&&) = default;
	NonCopyable& operator=(NonCopyable&&) = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

class NonMovable {
public:
	NonMovable() = default;

	NonMovable(NonMovable&&) = delete;
	NonMovable& operator=(NonMovable&&) = delete;

	NonMovable(const NonMovable&) = default;
	NonMovable& operator=(const NonMovable&) = default;
};

class MandatorySingleton {
private:
	class CreationToken: public NonCopyable, public NonMovable {
		CreationToken() = default;
		friend MandatorySingleton;
	};

public:
	template<class T>
	static T& get_instance()
	{
		static T singleton{CreationToken{}};
		return singleton;
	}

protected:
	MandatorySingleton(const CreationToken&) {}
};

template<typename T, typename F>
void remove_if(std::vector<T>& v, F&& func)
{
	auto new_end = std::remove_if(v.begin(), v.end(), std::move(func));
	v.erase(new_end, v.end());
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
