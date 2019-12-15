
#include "IO.hpp"

namespace Hyperion::System::IO {

	std::vector<std::byte> readBinaryFile(const std::string& filename) {

		std::ifstream file(filename, std::ios::binary | std::ios::ate);

		//TODO Error Handling
		if (!file) {

			std::cerr << "Could not open shader!" << std::endl;
			throw std::runtime_error("Could not open shaderfile!");
		}

		const size_t size = (size_t)file.tellg();
		std::vector<std::byte> fileBuffer(size);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(fileBuffer.data()), size);
		file.close();

		return fileBuffer;
	}
}

