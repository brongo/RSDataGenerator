#include "DataAggregator.h"

namespace RSDataParser
{
    // Sorts .mapresources entries alphabetically, in preparation from duplicate removal with std::unique
    void DataAggregator::MapResources_SortAlphabetically(std::vector<MapResourceData>& allMapResourceData)
    {
        sort(allMapResourceData.begin(), allMapResourceData.end(), [](const MapResourceData& a, const MapResourceData& b) {
            if (a.name != b.name)
                return (a.name < b.name);
            return (a.type < b.type);
            });
        return;
    }

    // Removes .mapresources entries that match by name and type
    void DataAggregator::MapResources_RemoveDuplicatesByNameAndType(std::vector<MapResourceData>& allMapResourceData)
    {
        auto it = std::unique(allMapResourceData.begin(), allMapResourceData.end(), [](const MapResourceData& a, const MapResourceData& b) {
            if ((a.name == b.name) && (a.type == b.type))
                return 1;
            return 0;
            });

        size_t listSize = it - allMapResourceData.begin();
        allMapResourceData.resize(listSize);
        return;
    }
    
    // Sorts resource list alpabetically, in preparation for duplicate removal with std::unique
    void DataAggregator::Resources_SortAlphabetically(std::vector<ResourceData>& allResourceData)
    {
        sort(allResourceData.begin(), allResourceData.end(), [](const ResourceData& a, const ResourceData& b) {
            if (a.name != b.name)
                return (a.name < b.name);
            return (a.type < b.type);
            });
        return;
    }

    // Sorts resource list by name and load priority, in preparation for 2nd pass duplicate removal with std::unique
    void DataAggregator::Resources_SortByPriority(std::vector<ResourceData>& allResourceData)
    {
        sort(allResourceData.begin(), allResourceData.end(), [](const ResourceData& a, const ResourceData& b) {
            if (a.name != b.name)
                return (a.name < b.name);
            return (a.priority < b.priority);
            });
        return;
    }

    // Removes resources that match all 4 of these: name, type, version, and resource hash
    void DataAggregator::Resources_RemoveDuplicatesStrict(std::vector<ResourceData>& allResourceData)
    {
        auto it = std::unique(allResourceData.begin(), allResourceData.end(), [](const ResourceData& a, const ResourceData& b) {
            if ((a.name == b.name) && (a.type == b.type) && (a.version == b.version) && (a.hash == b.hash))
                return 1;
            return 0;
            });

        size_t listSize = it - allResourceData.begin();
        allResourceData.resize(listSize);
        return;
    }
    
    // Removes duplicate entries with lower loading priority
    void DataAggregator::Resources_RemoveDuplicatesByPriority(std::vector<ResourceData>& allResourceData)
    {
        auto it = std::unique(allResourceData.begin(), allResourceData.end(), [](const ResourceData& a, const ResourceData& b) {
            if ((a.name == b.name) && (a.type == b.type) && (a.priority < b.priority))
                return 1;
            return 0;
            });

        size_t listSize = it - allResourceData.begin();
        allResourceData.resize(listSize);
        return;
    }
    
    // Removes any remaining entries that match by name only
    void DataAggregator::Resources_RemoveDuplicatesByName(std::vector<ResourceData>& allResourceData)
    {
        auto it = std::unique(allResourceData.begin(), allResourceData.end(), [](const ResourceData& a, const ResourceData& b) {
            if (a.name == b.name)
                return 1;
            return 0;
            });

        size_t listSize = it - allResourceData.begin();
        allResourceData.resize(listSize);
        return;
    }
    
    // Public API function for .mapresources duplicate removal
    void DataAggregator::MapResources_RemoveDuplicates(std::vector<MapResourceData>& allMapResourceData)
    {
        printf("Sorting and removing duplicate .mapresources entries, please wait. \n");
        MapResources_SortAlphabetically(allMapResourceData);
        MapResources_RemoveDuplicatesByNameAndType(allMapResourceData);
        return;
    }

    // Public API function for .resources duplicate removal
    void DataAggregator::Resources_RemoveDuplicates(std::vector<ResourceData>& allResourceData)
    {
        // should always be used in this order
        printf("Sorting and removing duplicate .resources entries, please wait. \n");
        Resources_SortAlphabetically(allResourceData);
        Resources_RemoveDuplicatesStrict(allResourceData);
        Resources_SortByPriority(allResourceData);
        Resources_RemoveDuplicatesByPriority(allResourceData);
        Resources_RemoveDuplicatesByName(allResourceData);
        return;
    }

    // Public API function for aggregating all .resources file data into a single vector
    std::vector<ResourceData> DataAggregator::ParseResourceFiles(const std::vector<fs::path> resourceFileList)
    {
        std::vector<ResourceData> allResourceData;
        for (int i = 0; i < resourceFileList.size(); i++)
        {
            fs::path resourceFilePath = resourceFileList[i];
            printf("Reading file %s\n", resourceFilePath.string().c_str());

            // read .resources file from filesystem
            ResourceFile resourceFile(resourceFilePath, i);
            std::vector<uint64_t> pathStringIndexes = resourceFile.GetAllPathStringIndexes();
            uint32_t numFileEntries = resourceFile.GetNumFileEntries();

            // allocate vector to hold all entries from this .resources file
            std::vector<ResourceData> resourceData;
            resourceData.resize(numFileEntries);
            
            // Parse each resource file and convert to usable data
            for (uint32_t i = 0; i < numFileEntries; i++)
            {
                ResourceFileEntry& lexedEntry = resourceFile.GetResourceFileEntry(i);
                resourceData[i].priority = resourceFile.LoadPriority;
                resourceData[i].version = lexedEntry.Version;
                resourceData[i].hash = lexedEntry.StreamResourceHash;
                resourceData[i].havokFlag1 = lexedEntry.HavokFlag1;
                resourceData[i].havokFlag2 = lexedEntry.HavokFlag2;
                resourceData[i].havokFlag3 = lexedEntry.HavokFlag3;
                resourceData[i].type = resourceFile.GetResourceStringEntry(pathStringIndexes[lexedEntry.PathTuple_Index]);
                resourceData[i].name = resourceFile.GetResourceStringEntry(pathStringIndexes[lexedEntry.PathTuple_Index + 1]);
            }

            // add the entries from *this* .resources file into the main list (data from *all* .resources files)
            allResourceData.insert(std::end(allResourceData), std::begin(resourceData), std::end(resourceData));
            resourceData.clear();
        }
        return allResourceData;
    };

    // Public API function for aggregating all .mapresources file data into a single vector
    std::vector<MapResourceData> DataAggregator::ParseMapResources(const std::vector<fs::path> mapResourceList)
    {
        std::vector<MapResourceData> allMapResourceData;
        for (int i = 0; i < mapResourceList.size(); i++)
        {
            fs::path mapResourcePath = mapResourceList[i];
            printf("Reading file %s\n", mapResourcePath.string().c_str());

            // reads .mapresource file from filesystem
            MapResourceFile mapResourceFile(mapResourcePath, i);
            std::vector<Asset> assetList = mapResourceFile.GetAssets();
            std::vector<AssetType> assetTypeList = mapResourceFile.GetAssetTypes();

            // allocate vector to hold all entries from this .mapresources file
            std::vector<MapResourceData> mapResourceData;
            mapResourceData.resize(assetList.size());

            // Parse each .mapresources file and convert to usable data
            for (uint32_t i = 0; i < assetList.size(); i++)
            {
                mapResourceData[i].name = assetList[i].assetName;
                mapResourceData[i].type = assetTypeList[assetList[i].assetTypeIndex].assetTypeName;
            }

            // add the entries from *this* .mapresources file into the main list (data from *all* .mapresources files)
            allMapResourceData.insert(std::end(allMapResourceData), std::begin(mapResourceData), std::end(mapResourceData));
            mapResourceData.clear();
        }
        return allMapResourceData;
    };
}