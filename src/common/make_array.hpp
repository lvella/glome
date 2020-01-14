#pragma once

#include <utility>
#include <array>
#include <cstdint>

template<typename Func, std::size_t... Idx>
auto make_array_impl(Func&& creator, std::index_sequence<Idx...>)
{
	return std::array<decltype(creator(0)), sizeof...(Idx)>{
		creator(Idx)...
	};
}

template<std::size_t N, typename Func>
auto make_array(Func&& creator)
{
	return make_array_impl(
		std::forward<Func>(creator),
		std::make_index_sequence<N>{}
	);
}
