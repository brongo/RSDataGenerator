#include "DataMatcher.h"

namespace RSDataParser
{
    // simple hash we'll use to reference filenames more efficiently
    int64_t DataMatcher::CalculateHash(std::string input)
    {
        uint64_t hashedValue = 3074457345618258791;
        for (int i = 0; i < input.length(); i++)
        {
            hashedValue += input[i];
            hashedValue *= 3074457345618258799;
        }
        return hashedValue;
    }

    // simple convert to lowercase function
    std::string DataMatcher::StrToLower(std::string inputString)
    {
        std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::tolower);
        return inputString;
    }

    // filepath parse functions
    std::string DataMatcher::GetFilenameFromPath(std::string path)
    {
        size_t splitPos = path.rfind("/");
        return path.substr(splitPos + 1, path.length() - splitPos);
    }
    std::string DataMatcher::GetParentFolderFromPath(std::string path)
    {
        return path.substr(0, path.find("/"));
    }
    std::string DataMatcher::DropFileExtension(std::string filename)
    {
        return filename.substr(0, filename.rfind("."));
    }
    std::string DataMatcher::DropFirstDirectoryFromPath(std::string path)
    {
        size_t splitPos = path.find("/");
        return path.substr(splitPos + 1, path.length() - splitPos);
    }

    // Converts .resources name/type into equivalent .mapresources name/type - for non .decl Files
    void DataMatcher::SetMapResourceNameType_NonDecls(std::vector<ResourceData>& allResourceData, std::vector<MatchParms>& allMatchParms)
    {
        for (int64_t i = 0; i < allResourceData.size(); i++)
        {
            ResourceData& thisEntry = allResourceData[i];

            // Set defaults
            allMatchParms[i].mapResourceName = thisEntry.name;
            allMatchParms[i].mapResourceType = thisEntry.type;

            // These are probably renderparms. Defaults will work.
            if (thisEntry.name.length() <= 4)
            {
                allMatchParms[i].nameHash = CalculateHash(allMatchParms[i].mapResourceName);
                allMatchParms[i].typeHash = CalculateHash(allMatchParms[i].mapResourceType);
                allMatchParms[i].typeHashLowercase = CalculateHash(StrToLower(allMatchParms[i].mapResourceType));
                continue;
            }              

            // Flag any .decl files and skip them
            if (thisEntry.name.substr(thisEntry.name.length() - 4, 4) == "decl")
            {
                allMatchParms[i].isDeclFile = 1;
                continue;
            }
            
            // Special handling for .resources type: "rs_emb_sfile" (geomcache)
            if (thisEntry.name.substr(thisEntry.name.length() - 5, 5) == "abc#0")
            {
                allMatchParms[i].mapResourceName = thisEntry.name.substr(10, (thisEntry.name.length() - 10) - 2);  // drop geomcache# and #0         
                allMatchParms[i].mapResourceType = "geomcache";
            }

            // Set hash values, used for comparison against .mapresources later
            allMatchParms[i].nameHash = CalculateHash(allMatchParms[i].mapResourceName);
            allMatchParms[i].typeHash = CalculateHash(allMatchParms[i].mapResourceType);
            allMatchParms[i].typeHashLowercase = CalculateHash(StrToLower(allMatchParms[i].mapResourceType));
        }
        return;
    }

    // Converts .resources name/type into equivalent .mapresources name/type - for .decl files
    void DataMatcher::SetMapResourceNameType_DeclFiles(std::vector<ResourceData>& allResourceData, std::vector<MatchParms>& allMatchParms)
    {
        for (int64_t i = 0; i < allResourceData.size(); i++)
        {
            // Not a .decl file, skip these
            if (!allMatchParms[i].isDeclFile)
                continue;

            ResourceData& thisEntry = allResourceData[i];

            // Special handling for .decl files in /maps/ path
            std::string parentFolder = GetParentFolderFromPath(thisEntry.name);
            if (parentFolder == "maps")
            {
                std::string filename = GetFilenameFromPath(thisEntry.name);
                if (filename == "mega2.decl") 
                    allMatchParms[i].mapResourceType = "material2";
                else
                    allMatchParms[i].mapResourceType = "modelAsset";
            }      

            // All other .decl files
            if (parentFolder != "maps")
            {
                // Find the mapResourceType; it is derived from the 3rd foldername in the path
                std::string tmpString = thisEntry.name;
                tmpString = DropFirstDirectoryFromPath(tmpString);                              // drops "generated/" or "maps/" from filepath
                tmpString = DropFirstDirectoryFromPath(tmpString);                              // drops "decls/" or "game/" from filepath
                allMatchParms[i].mapResourceType = GetParentFolderFromPath(tmpString);          // actual folder name we're looking for
                
                // get mapResourceName
                tmpString = DropFirstDirectoryFromPath(tmpString);                              // drops the 3rd folder, which was used for "type" above
                allMatchParms[i].mapResourceName = DropFileExtension(tmpString);                // drops the .decl extension, not used in .mapresources asset naming
            }
            
            // Set hash values, used for comparison against .mapresources later
            allMatchParms[i].nameHash = CalculateHash(allMatchParms[i].mapResourceName);
            allMatchParms[i].typeHash = CalculateHash(allMatchParms[i].mapResourceType);
            allMatchParms[i].typeHashLowercase = CalculateHash(StrToLower(allMatchParms[i].mapResourceType));
        }
        return;
    }

    // Hashes MapResourceData for faster checking against MatchParms
    std::vector<MapResourceHashed> DataMatcher::GetHashedMapResourceData(std::vector<MapResourceData>& allMapResourceData)
    {
        std::vector<MapResourceHashed> allMapResourcesHashed;
        allMapResourcesHashed.resize(allMapResourceData.size());

        for (int i = 0; i < allMapResourcesHashed.size(); i++)
        {
            allMapResourcesHashed[i].mapResourceIndex = i;
            allMapResourcesHashed[i].nameHash = CalculateHash(allMapResourceData[i].name);
            allMapResourcesHashed[i].typeHash = CalculateHash(allMapResourceData[i].type);
            allMapResourcesHashed[i].typeHashLowercase = CalculateHash(StrToLower(allMapResourceData[i].type));
        }
        return allMapResourcesHashed;
    }

    // Checks each .resources entry against .mapresources entries to find matches
    std::vector<MatchedPair> DataMatcher::MatchResources(std::vector<ResourceData>& allResourceData, std::vector<MapResourceData>& allMapResourceData, std::vector<MapResourceHashed>& allMapResourcesHashed, std::vector<MatchParms>& allMatchParms)
    {
        std::vector<MatchedPair> matchedResources;
        printf("Matching .resources entries against .mapresources. This may take several minutes. Please wait... \n");

        for (int64_t i = 0; i < allResourceData.size(); i++)
        {
            ResourceData& thisEntry = allResourceData[i];
            std::vector<int> possibleMatchIndexes;

            // find possible matches by name only
            for (int mapResourceIndex = 0; mapResourceIndex < allMapResourcesHashed.size(); mapResourceIndex++)
            {
                if (allMatchParms[i].nameHash == allMapResourcesHashed[mapResourceIndex].nameHash)
                    possibleMatchIndexes.push_back(mapResourceIndex);
            }

            // no possible matches
            if (possibleMatchIndexes.size() == 0)
            {
                // don't warn for .mapresources. these will never be found
                if (thisEntry.name.rfind(".mapresources") != -1)
                    continue;

                // otherwise, show a warning and move to next resource
                printf("WARNING: Failed to locate a match for resource: %s with type: %s \n", thisEntry.name.c_str(), thisEntry.type.c_str());
                continue;
            }
            
            // possible matches were found
            for (int matchIndex = 0; matchIndex < possibleMatchIndexes.size(); matchIndex++)
            {
                if (allMatchParms[i].typeHash == allMapResourcesHashed[possibleMatchIndexes[matchIndex]].typeHash)
                {
                    // create the matchedPair and add it to matchedResources vector
                    MatchedPair matchedPair;
                    matchedPair.mapResourceData = allMapResourceData[possibleMatchIndexes[matchIndex]];
                    matchedPair.resourceData = allResourceData[i];
                    matchedPair.filenameHash = allMatchParms[i].nameHash;
                    matchedResources.push_back(matchedPair);

                    // flag the resource as found
                    allMatchParms[i].isMatched = true;
                    break;
                }
            }

            // found it, move to next resources
            if (allMatchParms[i].isMatched)
                continue;

            // no exact matches among the possibles - try matching against lowercase type
            for (int matchIndex = 0; matchIndex < possibleMatchIndexes.size(); matchIndex++)
            {
                if (allMatchParms[i].typeHashLowercase == allMapResourcesHashed[possibleMatchIndexes[matchIndex]].typeHashLowercase)
                {
                    // create the matchedPair and add it to matchedResources vector
                    MatchedPair matchedPair;
                    matchedPair.mapResourceData = allMapResourceData[possibleMatchIndexes[matchIndex]];
                    matchedPair.resourceData = allResourceData[i];
                    matchedPair.filenameHash = allMatchParms[i].nameHash;
                    matchedResources.push_back(matchedPair);

                    // flag the resource as found
                    allMatchParms[i].isMatched = true;
                    break;
                }
                        
            }

            // found it, move to next resource
            if (allMatchParms[i].isMatched)
                continue;

            // failed to find anything
            printf("WARNING: Failed to locate a match for resource: %s with type: %s \n", thisEntry.name.c_str(), thisEntry.type.c_str());
            continue;
        }

        printf("Finished matching resource data against .mapresources. \n");
        return matchedResources;
    }
}