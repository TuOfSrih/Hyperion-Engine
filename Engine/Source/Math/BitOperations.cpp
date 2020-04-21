
#include "BitOperations.hpp"

namespace Hyperion::Math {

	unsigned int hammingDistance(uint32_t a, uint32_t b)
	{
		unsigned int dist = 0;

		while (a || b) {

			dist += (a % 2) ^ (b % 2);
			a = a >> 1;
			b = b >> 1;
		}

		return dist;
	}
}


