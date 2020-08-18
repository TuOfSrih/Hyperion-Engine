#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>


namespace Hyperion::System::IO {

	std::vector<std::byte> readBinaryFile(const std::string& filename);


	std::vector<std::filesystem::path> getFilesFromDirectory(const std::filesystem::path& directory, const std::filesystem::path& extension = "");
}