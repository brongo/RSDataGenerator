#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "types/ResourceFile.h"
#include "Oodle.h"

namespace RSDataParser
{
    namespace fs = std::filesystem;

    class MapResourceExtractor
    {
        public:   
            
            bool ExtractMapResourceFile(const fs::path pathToResourceFile, const fs::path pathToOodleDLL, const std::string searchString = ".mapresources");
            bool ExtractAll(const std::vector<fs::path> resourceFileList, const fs::path pathToOodleDLL);
            std::vector<fs::path> GetFileList() { return _MapResourceList; }

        private:

            std::vector<fs::path> _MapResourceList;
            int FindMapResourceStringOffset(const std::vector<std::string>& stringEntries, const std::string searchString);
            int FindMapResourceStringIndex(const std::vector<uint64_t>& stringIndexes, const int targetOffset);
            int FindMapResourceFileEntry(const std::vector<ResourceFileEntry>& fileEntries, const int targetStringIndex);
            OodleLibrary _Oodle;
    };
}