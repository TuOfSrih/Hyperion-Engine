#pragma once

#include <string>
#include <filesystem>

namespace Hyperion {

	//TODO: Maybe make versioning work at compiletime
	class Version {
	public:
		explicit Version(int major, int minor, int patch);
		int getMajor() const;
		int getMinor() const;
		int getPatch() const;
		int getRaw() const;
	private:
		const int version;
	};

	class Configuration {

	public:
		explicit Configuration(const std::string applicationName, const Version applicationVersion);

		std::string applicationName;
		std::string engineName = "Hyperion Engine";
		Version applicationVersion;
		Version engineVersion = Version(1, 0, 0);

		std::filesystem::path shaderDir = std::filesystem::current_path() /*/ ".." / ".." / ".."*/ / "Shaders" / "default";
	};
}