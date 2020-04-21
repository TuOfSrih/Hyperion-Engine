
#include "Configuration.hpp"


Hyperion::Version::Version(int major, int minor, int patch): version(((major) << 22) | ((minor) << 12) | (patch))
{
}

Hyperion::Configuration::Configuration(const std::string applicationName, const Version applicationVersion): applicationName(applicationName), applicationVersion(applicationVersion)
{
}

int Hyperion::Version::getMajor() const
{
	return version >> 22;
}

int Hyperion::Version::getMinor() const
{
	return (version >> 12) & 0x3ff;
}

int Hyperion::Version::getPatch() const
{
	return version & 0xfff;
}

int Hyperion::Version::getRaw() const
{
	return version;
}


