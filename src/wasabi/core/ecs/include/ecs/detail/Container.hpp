
#pragma once

#include <typeinfo>


namespace wasabi::ecs::detail {

struct Container {
	virtual ~Container() = default;
};

} // namespace wasabi::ecs::detail
