#include "AdvancedOperators.hpp"

namespace Hyperion::Cpp {

	bool pairFirstElemLess::operator() (const std::pair<uint32_t, std::function<void()>>& first, const std::pair < uint32_t, std::function<void()>>& second) {
		return first.first < second.first;
	}
}