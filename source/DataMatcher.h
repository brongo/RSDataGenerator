#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "types/SharedStructs.h"

namespace RSDataParser
{
    /**
    *   DataMatcher performs the necessary operations to match .resources entries against .mapresources.
    *   It assumes we have 2 separate vectors at this point.
    *   One vector contains all .resources data, and the other contains all .mapresources data.
    * 
    *   Re: SetMapResourceNameType_NonDecls() and SetMapResourceNameType_DeclFiles()
    *    
    *   These 2 functions calculate the "MatchParms" for each .resources entry
    *   MatchParms will contain the "needle" data we're trying to find in the .mapresources haystack.
    *   (We can't directly compare ResourcesData against MapResources due to naming differences).
    */

    class DataMatcher
    {
        public:
            int64_t CalculateHash(std::string input);
            std::string StrToLower(std::string inputString);

            // filepath parse functions
            std::string GetFilenameFromPath(std::string path);
            std::string GetParentFolderFromPath(std::string path);
            std::string DropFileExtension(std::string filename);
            std::string DropFirstDirectoryFromPath(std::string path);
            
            // data prep - run these before attempting to match
            void SetMapResourceNameType_NonDecls(std::vector<ResourceData>& allResourceData, std::vector<MatchParms>& allMatchParms);
            void SetMapResourceNameType_DeclFiles(std::vector<ResourceData>& allResourceData, std::vector<MatchParms>& allMatchParms);
            std::vector<MapResourceHashed> GetHashedMapResourceData(std::vector<MapResourceData>& allMapResourceData);

            // main matching function
            std::vector<MatchedPair> MatchResources(std::vector<ResourceData>& allResourceData, std::vector<MapResourceData>& allMapResourceData, std::vector<MapResourceHashed>& allMapResourcesHashed, std::vector<MatchParms>& allMatchParms);
    };
}