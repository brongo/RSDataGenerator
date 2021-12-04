#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "../../vendor/jsonxx/jsonxx.h"

namespace RSDataParser
{
    namespace fs = std::filesystem;

    class PackageMapSpecFile
    {
        public:
            std::string Name;
    };

    class PackageMapSpecMapFileRef
    {
        public:
            int32_t File;
            int32_t Map;

            PackageMapSpecMapFileRef()
            {
                File = -1;
                Map = -1;
            }

            PackageMapSpecMapFileRef(const int32_t file, const int32_t map)
            {
                File = file;
                Map = map;
            }
    };

    class PackageMapSpecMap
    {
        public:
            std::string Name;
    };

    class PackageMapSpec
    {
        public:
            std::vector<PackageMapSpecFile> Files;
            std::vector<PackageMapSpecMapFileRef> MapFileRefs;
            std::vector<PackageMapSpecMap> Maps;

        public:
            PackageMapSpec() {};
            PackageMapSpec(fs::path packageMapSpecPath);
            std::string Dump() const;

            bool InBaseDirectory(const std::string& filePath) const; 
            void NormalizeFilePath(std::string& filePath) const;
            size_t GetFileIndexByFileName(const std::string& filePath) const;
            size_t GetMapIndexByFileIndex(const size_t fileIndex) const;

            std::string GetRelativeFilePath(const std::string& filePath) const;
            std::vector<std::string> GetFilesByResourceName(std::string resourceFileName) const;
            std::vector<fs::path> GetResourceFileList(const bool excludeMeta = false) const;
    };
}
