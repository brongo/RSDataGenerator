#include "RSDataGenerator.h"

using namespace RSDataParser;

namespace RSDataParser
{
    std::vector<fs::path> RSDataGenerator::GetResourceFileList(fs::path inputPath)
    {
        fs::path packageMapSpecPath = inputPath / "base" / "packagemapspec.json";
        PackageMapSpec packageMapSpec(packageMapSpecPath);

        // true = exclude meta.resources
        std::vector<fs::path> resourceFileList = packageMapSpec.GetResourceFileList(true);

        // Rewrite the resourceFileList with complete paths to each file
        for (int i = 0; i < resourceFileList.size(); i++)
        {
            fs::path pathToResourceFile = inputPath / "base" / resourceFileList[i];
            pathToResourceFile.make_preferred();
            resourceFileList[i] = pathToResourceFile;
        }
        return resourceFileList;
    }

    void RSDataGenerator::WriteRSDataFile(std::vector<MatchedPair>& matchedResourceData)
    {
        FILE* f;
        if (fopen_s(&f, "rs_data_uncompressed", "wb") != 0)
        {
            printf("Error: failed to open rs_data file for writing.\n");
            return;
        }

        if (f != NULL)
        {
            uint64_t numEntries = matchedResourceData.size();

            fwrite(&numEntries, 8, 1, f);
            for (int i = 0; i < numEntries; i++)
            {
                ResourceData& thisResource = matchedResourceData[i].resourceData;
                MapResourceData& thisMapResource = matchedResourceData[i].mapResourceData;

                int64_t filenameHash = matchedResourceData[i].filenameHash;
                int typeStrlen = thisResource.type.length();
                int mapTypeStrlen = thisMapResource.type.length();
                int mapNameStrlen = thisMapResource.name.length();

                // write to file
                fwrite(&filenameHash, 8, 1, f);
                fwrite(&thisResource.hash, 8, 1, f);
                fwrite(&thisResource.version, 1, 1, f);
                fwrite(&thisResource.havokFlag1, 1, 1, f);
                fwrite(&thisResource.havokFlag2, 1, 1, f);
                fwrite(&thisResource.havokFlag3, 1, 1, f);
                fwrite(&typeStrlen, 2, 1, f);
                fwrite(thisResource.type.c_str(), 1, typeStrlen, f);

                if (thisResource.name != thisMapResource.name || thisResource.type != thisMapResource.type)
                {
                    fwrite(&mapTypeStrlen, 2, 1, f);
                    fwrite(thisMapResource.type.c_str(), 1, mapTypeStrlen, f);
                    fwrite(&mapNameStrlen, 2, 1, f);
                    fwrite(thisMapResource.name.c_str(), 1, mapNameStrlen, f);
                }
                else
                {
                    mapTypeStrlen = 0;
                    fwrite(&mapTypeStrlen, 2, 1, f);
                }
            }
            fclose(f);
            printf("The rs_data file was generated successfully. \n");
        }
        return;
    }
}

int main(int argc, char* argv[])
{
    printf("\nRSDataGenerator v1.0 by SamPT \n\n");
    if (argc < 2)
    {
        printf("USAGE: RSDataGenerator.exe <path> \n");
        printf("<path> is the full path to your DOOM Eternal folder. \n\n");
        return 1;
    }

    fs::path inputPath = argv[1];
    DataAggregator dataAggregator; 
    InputValidator inputValidator;
    MapResourceExtractor mapResourceExtractor;
    RSDataGenerator rsDataGenerator;
    OodleLibrary Oodle;

    std::vector<fs::path> resourceFileList;
    std::vector<fs::path> mapResourceList;
    std::vector<ResourceData> allResourceData;
    std::vector<MapResourceData> allMapResourceData;
    std::vector<MapResourceHashed> allMapResourcesHashed;
    std::vector<MatchParms> allMatchParms;
    std::vector<MatchedPair> matchedResourceData;  

    // We can't find packagemapspec.json (wrong input path)
    if (!inputValidator.inputIsValid(inputPath))
        return 1;

    // Make sure Oodle is available
    if (!Oodle.Init(inputPath))
    {
        fprintf(stderr, "ERROR : Failed to initialize Oodle Library. \n\n");
        return false;
    }

    // Get resources list from packagemapspec.json
    resourceFileList = rsDataGenerator.GetResourceFileList(inputPath);
    
    // Locate and extract the embedded .mapresources files
    if (!mapResourceExtractor.ExtractAll(resourceFileList, inputPath))
    {
        fprintf(stderr, "ERROR : MapResourceExtractor : Failed to extract .mapresource files.\n");
        return 1;
    }

    // Parse all the .resource files and store relevant data within dataAggregator
    allResourceData = dataAggregator.ParseResourceFiles(resourceFileList);
    
    // Parse all the extracted .mapresources files and store data within dataAggregator
    mapResourceList = mapResourceExtractor.GetFileList();
    allMapResourceData = dataAggregator.ParseMapResources(mapResourceList);

    // Remove all duplicate .resources and .mapresources entries
    dataAggregator.Resources_RemoveDuplicates(allResourceData);
    dataAggregator.MapResources_RemoveDuplicates(allMapResourceData);

    // Hashes .mapresources name/type strings for faster comparison later
    DataMatcher dataMatcher;
    allMapResourcesHashed = dataMatcher.GetHashedMapResourceData(allMapResourceData);  

    // Converts .resources name/type strings into the format we expect to find in .mapresources
    allMatchParms.resize(allResourceData.size());
    dataMatcher.SetMapResourceNameType_NonDecls(allResourceData, allMatchParms);
    dataMatcher.SetMapResourceNameType_DeclFiles(allResourceData, allMatchParms);

    // Finds matches and returns a final set of matched data
    matchedResourceData = dataMatcher.MatchResources(allResourceData, allMapResourceData, allMapResourcesHashed, allMatchParms);

    // Finally, generate rs_data file
    rsDataGenerator.WriteRSDataFile(matchedResourceData);
    Oodle.CompressFile("rs_data_uncompressed", "rs_data");
    return 0;
}