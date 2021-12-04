#include "MapResourceFile.h"

namespace RSDataParser
{
    MapResourceFile::MapResourceFile(fs::path& filename, int loadPriority)
    {
        _FilePath = filename.string();
        _LoadPriority = loadPriority;

        FILE* f;
        if (fopen_s(&f, _FilePath.c_str(), "rb") != 0)
        {
            printf("Error: failed to open %s for reading.\n", _FilePath.c_str());
            return;
        }

        if (f != NULL)
        {
            fseek(f, 0, SEEK_SET);
            fread(&_UnixTimestamp, 4, 1, f);

            // Read Layers
            fread(&_LayerCount, 4, 1, f);
            _LayerCount = ntohl(_LayerCount);
            _Layers.resize(_LayerCount);

            for (int i = 0; i < _LayerCount; i++)
            {
                char stringBuffer[1024] = { 0 };
                fread(&_Layers[i].strLen, 1, 4, f);
                fread(stringBuffer, _Layers[i].strLen, 1, f);
                _Layers[i].layerName = std::string(stringBuffer);
            }

            // Read AssetTypes
            fread(&_AssetTypeCount, 8, 1, f);
            _AssetTypeCount = ntohll(_AssetTypeCount);
            _AssetTypes.resize(_AssetTypeCount);

            for (int i = 0; i < _AssetTypeCount; i++)
            {
                char stringBuffer[1024] = { 0 };
                fread(&_AssetTypes[i].strLen, 1, 4, f);
                fread(stringBuffer, _AssetTypes[i].strLen, 1, f);
                _AssetTypes[i].assetTypeName = std::string(stringBuffer);
            }

            // Read Assets 
            fread(&_AssetCount, 4, 1, f);
            _AssetCount = ntohl(_AssetCount);
            _Assets.resize(_AssetCount);

            for (int i = 0; i < _AssetCount; i++)
            {
                fread(&_Assets[i].assetTypeIndex, 4, 1, f);
                _Assets[i].assetTypeIndex = ntohl(_Assets[i].assetTypeIndex);

                char stringBuffer[1024] = { 0 };
                fread(&_Assets[i].strLen, 1, 4, f);
                fread(stringBuffer, _Assets[i].strLen, 1, f);
                _Assets[i].assetName = std::string(stringBuffer);

                // Skip 24 unknown bytes
                fseek(f, 24, SEEK_CUR);
            }

            // Read Maps
            fread(&_MapCount, 4, 1, f);
            _MapCount = ntohl(_MapCount);
            _Maps.resize(_MapCount);

            for (int i = 0; i < _MapCount; i++)
            {
                char stringBuffer[1024] = { 0 };
                fread(&_Maps[i].strLen, 1, 4, f);
                fread(stringBuffer, _Maps[i].strLen, 1, f);
                _Maps[i].mapName = std::string(stringBuffer);
            }

            fclose(f);
        }
    }

}