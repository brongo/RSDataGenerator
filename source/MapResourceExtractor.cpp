#include "MapResourceExtractor.h"

namespace RSDataParser
{
    // Subroutines for locating .mapresources file entry in the .resources file
    int MapResourceExtractor::FindMapResourceStringOffset(const std::vector<std::string>& stringEntries, const std::string searchString)
    {
        // .mapresources is usually the last *string* in .resources file
        if (stringEntries[stringEntries.size() - 1].rfind(searchString) != -1)
            return stringEntries.size() - 1;

        // if not, search all 
        auto it = std::find_if(stringEntries.begin(), stringEntries.end(), [searchString](const std::string& str) {
            return str.find(searchString) != std::string::npos; 
        });

        // found, returns index of .mapresources entry
        if (it != stringEntries.end())
            return (it - stringEntries.begin());

        // not found
        return -1;
    };
    int MapResourceExtractor::FindMapResourceStringIndex(const std::vector<uint64_t>& stringIndexes, const int targetOffset)
    {
        // .mapresources is usually the last *string index* in .resources file
        if (stringIndexes[(stringIndexes.size() - 1)] == targetOffset)
            return stringIndexes.size() - 1;

        // if not, search all
        auto it = std::find(stringIndexes.begin(), stringIndexes.end(), targetOffset);
        if (it != stringIndexes.end())
            return (it - stringIndexes.begin());

        // not found
        return -1;
    }
    int MapResourceExtractor::FindMapResourceFileEntry(const std::vector<ResourceFileEntry>& fileEntries, const int targetStringIndex)
    {
        // .mapresources is usually the last *file entry* in .resources file
        if ((fileEntries[fileEntries.size() - 1].PathTuple_Index + 1) == targetStringIndex)
            return fileEntries.size() - 1;

        // if not, search all
        for (int i = 0; i < fileEntries.size(); i++)
        {
            int stringIndex = fileEntries[i].PathTuple_Index + 1;
            if (stringIndex == targetStringIndex)
                return i;
        }

        // not found
        return -1;
    }

    // Extract .mapresources data from .resources file
    bool MapResourceExtractor::ExtractMapResourceFile(const fs::path pathToResourceFile, const fs::path pathToOodleDLL, const std::string searchString)
    {
        // Step 0: Load the .resources file and initialize Oodle library.
        ResourceFile resourceFile(pathToResourceFile);
        if (!_Oodle.Init(pathToOodleDLL))
        {
            fprintf(stderr, "ERROR : MapResourceExtractor : Failed to initialize Oodle Library. \n\n");
            return false;
        }

        // Step 1: Find the string containing .mapresources filename
        std::vector<std::string> stringEntries = resourceFile.GetAllStringEntries();
        int targetOffset = FindMapResourceStringOffset(stringEntries, searchString);

        if (targetOffset == -1)
        {
            fprintf(stderr, "ERROR : MapResourceExtractor : Failed to locate .mapresources string offset. \n\n");
            return false;
        }
   
        // Step 2: Find the PathStringIndex that references the string we found in Step 1.
        std::vector<uint64_t> stringIndexes = resourceFile.GetAllPathStringIndexes();
        int targetStringIndex = FindMapResourceStringIndex(stringIndexes, targetOffset);

        if (targetStringIndex == -1)
        {
            fprintf(stderr, "ERROR : MapResourceExtractor : Failed to locate .mapresources string index. \n\n");
            return false;
        }

        // Step 3: Find the ResourceFileEntry that references the PathStringIndex we found in Step 2.
        std::vector<ResourceFileEntry>& fileEntries = resourceFile.GetAllFileEntries();
        int targetFileEntry = FindMapResourceFileEntry(fileEntries, targetStringIndex);

        if (targetFileEntry == -1)
        {
            fprintf(stderr, "ERROR : MapResourceExtractor : Failed to locate .mapresources file entry. \n\n");
            return false;
        }

        // Step 4: Get the .mapresources data offset and size from the ResourceFileEntry. Read data into a byte vector.
        uint64_t decompressedSize = fileEntries[targetFileEntry].DataSizeUncompressed;
        std::vector<byte> embeddedData = resourceFile.GetEmbeddedFileData(targetFileEntry);

        if (embeddedData.empty())
            return false;

        // Step 5: Create output directories
        fs::path resourceName = pathToResourceFile.filename().replace_extension();  
        fs::path mapDir = pathToOodleDLL / "MapResources";
        if (!fs::exists(mapDir))
            fs::create_directory(mapDir);

        fs::path outputDir = mapDir / resourceName;
        if (!fs::exists(outputDir))
            fs::create_directory(outputDir);

        // Step 6: Pass the data to Oodle for decompression & save to local filesystem.
        fs::path mapResourceName = fs::path(stringEntries[targetOffset]).filename();
        fs::path outputFile = outputDir / mapResourceName;

        if (!_Oodle.DecompressAndSave(embeddedData, decompressedSize, outputFile))
        {
            fprintf(stderr, "ERROR : MapResourceExtractor : Failed to extract %s. \n\n", outputFile.string().c_str());
            return false;
        }

        // Success - add this file to _MapResourceList for later reference
        printf("Extracted file: %s\n", outputFile.string().c_str());
        _MapResourceList.push_back(outputFile);
        return true;
    }

    bool MapResourceExtractor::ExtractAll(const std::vector<fs::path> resourceFileList, const fs::path pathToOodleDLL)
    {
        for (int i = 0; i < resourceFileList.size(); i++)
        {
            bool success = false;

            if (resourceFileList[i].filename().string().find("gameresources") != std::string::npos)
            {
                bool foundCommon = ExtractMapResourceFile(resourceFileList[i], pathToOodleDLL, "common.mapresources");
                bool foundInit = ExtractMapResourceFile(resourceFileList[i], pathToOodleDLL, "init.mapresources");
                if (foundCommon && foundInit)
                    success = true;
            }
            else
            {
                success = ExtractMapResourceFile(resourceFileList[i], pathToOodleDLL);
            }

            if (!success)
            {
                std::string filename = resourceFileList[i].string();
                fprintf(stderr, "ERROR : MapResourceExtractor : Failed to extract %s. \n\n", filename.c_str());
                return false;
            }
        }
        return true;
    }
}