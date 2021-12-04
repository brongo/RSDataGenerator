#pragma once

#include <filesystem>

namespace RSDataParser
{
	namespace fs = std::filesystem;

	class InputValidator
	{
		public:
			bool inputIsEternalDirectory(fs::path& inputPath) const;
			bool inputIsValid(fs::path& inputPath);
	};
}