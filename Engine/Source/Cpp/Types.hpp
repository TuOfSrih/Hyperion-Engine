#pragma once

#include <inttypes.h>

namespace Hyperion::Cpp {

	template<typename T>
	class Flags {
	public:
		explicit Flags() = default;
		explicit Flags(uint32_t mask) : mask(mask) {};
		explicit Flags(T flags) : mask(static_cast<uint32_t>(flags)) {};
		Flags& operator|(const T& other) const { return Flags(mask | other.mask); }
		Flags& operator&(const T& other) const { return Flags(mask & other.mask); }
		Flags& operator^(const T& other) const { return Flags(mask ^ other.mask); }
		Flags& operator|=(const T& other) const { mask |= other.mask; return *this; }
		Flags& operator&=(const T& other) const { mask &= other.mask; return *this; }
		Flags& operator^=(const T& other) const { mask ^= other.mask; return *this; }
		Flags& operator~() const { mask = ~mask; return *this; }
		explicit operator uint32_t() const { return mask; };
		operator bool() const { return mask; }

	private:
		uint32_t mask;
	};
}