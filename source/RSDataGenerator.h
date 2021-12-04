#pragma once
#define WIN32_LEAN_AND_MEAN
#define SAFE_SPACE 64

#include <string>
#include <filesystem>
#include <chrono>
#include <thread>

#include "types/MapResourceFile.h"
#include "types/PackageMapSpec.h"
#include "types/ResourceFile.h"
#include "types/SharedStructs.h"
#include "DataAggregator.h"
#include "DataMatcher.h"
#include "InputValidator.h"
#include "MapResourceExtractor.h"

namespace RSDataParser
{
	namespace fs = std::filesystem;

	class RSDataGenerator
	{
		public:
			std::vector<fs::path> GetResourceFileList(fs::path inputPath);
			void WriteRSDataFile(std::vector<MatchedPair>& matchedResourceData);
	};
}
