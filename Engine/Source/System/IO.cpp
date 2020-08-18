
#include "IO.hpp"

namespace Hyperion::System::IO {

	std::vector<std::byte> readBinaryFile(const std::string& filename) {

		std::ifstream file(filename, std::ios::binary | std::ios::ate);

		//TODO Error Handling
		if (!file) {

			std::cerr << "Could not open binary file!" << std::endl;
			throw std::runtime_error("Could not open shaderfile!");
		}

		const size_t size = (size_t)file.tellg();
		std::vector<std::byte> fileBuffer(size);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(fileBuffer.data()), size);
		file.close();

		return fileBuffer;
	}

	std::vector<std::filesystem::path> getFilesFromDirectory(const std::filesystem::path& directory, const std::filesystem::path& extension)
	{
		std::vector<std::filesystem::path> files;
		const bool checkExtension = extension != "" && extension.string().size() > 1 && extension.string()[0] == '.';

		for (const auto& file : std::filesystem::directory_iterator(directory)) {

			if (file.path().extension() == extension) {
				files.push_back(file.path());
			}
		}
		return files;
	}
}

