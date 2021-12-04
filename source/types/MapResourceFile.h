#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

// Linux - #include <arpa/inet.h>
#include <WinSock2.h>   // Additional dependencies: ws2_32.lib - for ntohl
#include <Windows.h>

namespace RSDataParser
{
    /**
    *   The .mapresources file format is not easily described with C-style structs.
    *   Rough layout below:
    *
    *   Header
    *   0x00 - int32_t         unixTimestamp        (Little Endian)
    *   0x04 - int32_t         layerCount           (Big Endian)
    *
    *   Layers[layerCount]
    *   0x00 - int32_t         strLen               (Little Endian)
    *   0x04 - char[strLen]    layerName
    *
    *   assetTypeCount
    *   0x00 - int64_t         assetTypeCount       (Big Endian)
    *
    *   AssetTypes[assetTypeCount]
    *   0x00 - int32_t         strLen               (Little Endian)
    *   0x04 - char[strLen]    assetTypeName
    *
    *   assetCount
    *   0x00 - int32_t         assetCount           (Big Endian)
    *
    *   Assets[assetCount]
    *   0x00 - int32_t         assetTypeIndex       (Big Endian)        // index into the AssetTypes[n] above.
    *   0x04 - int32_t         strLen               (Little Endian)
    *   0x08 - char[strLen]    assetName
    *   0x?? - int64_t         unknown1                                 // always zero?
    *   0x?? - int64_t         unknown2                                 // always zero?
    *   0x?? - int64_t         unknown3             (Big Endian)        // usually 128
    *
    *   mapCount
    *   0x00 - int32_t         mapCount             (Big Endian)
    *
    *   Maps[n]
    *   0x00 - int32_t         strLen               (Little Endian)
    *   0x04 - char[strLen]    mapName
    */

    namespace fs = std::filesystem;
    
    struct Layer
    {
        uint32_t strLen;
        std::string layerName;
    };

    struct AssetType
    {
        uint32_t strLen;
        std::string assetTypeName;
    };

    struct Asset
    {
        uint32_t assetTypeIndex;
        uint32_t strLen;
        std::string assetName;
    };

    struct Map
    {
        uint32_t strLen;
        std::string mapName;
    };

    class MapResourceFile
    {
        public:
            uint32_t GetAssetCount() { return _AssetCount; }
            std::vector<Asset> GetAssets() { return _Assets; }
            std::vector<AssetType> GetAssetTypes() { return _AssetTypes; }
            MapResourceFile(fs::path& filename, int loadPriority);

        private:
            int _LoadPriority = 0;
            std::string _FilePath;
            std::vector<Layer> _Layers;
            std::vector<AssetType> _AssetTypes;
            std::vector<Asset> _Assets;
            std::vector<Map> _Maps;

            uint32_t _UnixTimestamp = 0;
            uint32_t _LayerCount = 0; 
            uint64_t _AssetTypeCount = 0;
            uint32_t _AssetCount = 0;
            uint32_t _MapCount = 0;
    };

}