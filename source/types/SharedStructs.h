#pragma once

// Entries from .resources files
struct ResourceData
{
    std::string name;
    std::string type;
    uint64_t    hash = 0;                       // Used by game to find the file in .streamdb
    uint32_t    version = 0;                    // DOOM Eternal calls it a "version" but it's more like a file-type. 
    uint32_t    priority = 0;                   // Load priority of this resource in packagemapspec.json
    uint32_t    havokFlag1 = 0;                 // Required for Havok assets.
    uint8_t     havokFlag2 = 0;                 // Required for Havok assets.
    uint8_t     havokFlag3 = 0;                 // Required for Havok assets.
};

// Entries from .mapresources files
struct MapResourceData
{
    std::string name;
    std::string type;
};

// Hashed mapResourceData to match against MatchParms
struct MapResourceHashed
{
    int64_t nameHash = 0;
    int64_t typeHash = 0;
    int64_t typeHashLowercase = 0;
    int mapResourceIndex = 0;
};

// Parameters for matching .resources entry against .mapresources entry
struct MatchParms
{
    bool isDeclFile = false;                        
    bool isMatched = false;
    std::string mapResourceName;                // the .mapresources name we are looking for
    std::string mapResourceType;                // the .mapresources type we are looking for
    int64_t nameHash = 0;                      // hash of searchName (faster than string comparison)
    int64_t typeHash = 0;                      // hash of searchType (faster than string comparison)
    int64_t typeHashLowercase = 0;             // hash of searchType but converted to lowercase - needed for matching some mapResource types.
};

// A .resources entry matched with a .mapresources entry, for adding to rs_data.
struct MatchedPair
{
    ResourceData resourceData;
    MapResourceData mapResourceData;
    int64_t filenameHash = 0;
};