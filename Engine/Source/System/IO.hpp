#pragma once

#include <fstream>
#include <iostream>
#include <vector>


namespace Hyperion::System::IO {

	std::vector<std::byte> readBinaryFile(const std::string& filename);
}