#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "types/MapResourceFile.h"
#include "types/ResourceFile.h"
#include "types/SharedStructs.h"

namespace RSDataParser
{
    /**
    *   DataAggregator is used for bulk management of .resources and .mapresources data.
    *   
    *   It first parses all .resources and .mapresources files and gets the relevant data in two large vectors.
    *   Then it sorts and removes any unnecessary/duplicate information (for DOOM Eternal, this is a very large amount).
    * 
    *   Once all the data is collected and duplicates are removed, data is ready to be passed to DataMatcher.h.
    */

    class DataAggregator
    {
        public:   

            std::vector<ResourceData> ParseResourceFiles(const std::vector<fs::path> resourceFileList);
            std::vector<MapResourceData> ParseMapResources(const std::vector<fs::path> mapResourceList);        
            void Resources_RemoveDuplicates(std::vector<ResourceData>& allResourceData);
            void MapResources_RemoveDuplicates(std::vector<MapResourceData>& allMapResourceData);

        private:

            // sorting and duplicate removal subroutines
            void MapResources_SortAlphabetically(std::vector<MapResourceData>& allMapResourceData);
            void MapResources_RemoveDuplicatesByNameAndType(std::vector<MapResourceData>& allMapResourceData);

            void Resources_SortAlphabetically(std::vector<ResourceData> &allResourceData);
            void Resources_SortByPriority(std::vector<ResourceData>& allResourceData);
            void Resources_RemoveDuplicatesStrict(std::vector<ResourceData>& allResourceData);
            void Resources_RemoveDuplicatesByPriority(std::vector<ResourceData>& allResourceData);
            void Resources_RemoveDuplicatesByName(std::vector<ResourceData>& allResourceData);
    };
}