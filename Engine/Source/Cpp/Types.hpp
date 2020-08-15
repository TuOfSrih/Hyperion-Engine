#pragma once

#include <inttypes.h>

namespace Hyperion::Cpp {

	template<typename T>
	class Flags {
	public:
		Flags() = default;
		Flags(uint32_t mask) : mask(mask) {};
		Flags(T flags) : mask(static_cast<uint32_t>(flags)) {};
		Flags operator|(const T& other) const { return Flags(mask | static_cast<uint32_t>(other)); }
		Flags operator&(const T& other) const { return Flags(mask & static_cast<uint32_t>(other)); }
		Flags operator^(const T& other) const { return Flags(mask ^ static_cast<uint32_t>(other)); }
		Flags operator|(const Flags<T>& other) const { return Flags(mask | other.mask); }
		Flags operator&(const Flags<T>& other) const { return Flags(mask & other.mask); }
		Flags operator^(const Flags<T>& other) const { return Flags(mask ^ other.mask); }
		Flags& operator|=(const T& other) const { mask |= static_cast<uint32_t>(other); return *this; }
		Flags& operator&=(const T& other) const { mask &= static_cast<uint32_t>(other); return *this; }
		Flags& operator^=(const T& other) const { mask ^= static_cast<uint32_t>(other); return *this; }
		Flags& operator|=(const Flags<T>& other) const { mask |= other.mask; return *this; }
		Flags& operator&=(const Flags<T>& other) const { mask &= other.mask; return *this; }
		Flags& operator^=(const Flags<T>& other) const { mask ^= other.mask; return *this; }
		Flags& operator~() const { mask = ~mask; return *this; }
		explicit operator uint32_t() const { return mask; }
		operator bool() const { return mask; }

	private:
		uint32_t mask;
	};
}