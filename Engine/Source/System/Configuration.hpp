#pragma once

#include <string>


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

		const std::string applicationName;
		const std::string engineName = "Hyperion Engine";
		const Version applicationVersion;
		const Version engineVersion = Version(1, 0, 0);
	};
}