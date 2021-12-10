#include "ResourceFile.h"

namespace RSDataParser
{
    // Retrieves embedded file data for a specific file entry
    std::vector<uint8_t> ResourceFile::GetEmbeddedFileData(int entryNumber)
    {
        std::vector<uint8_t> embeddedData;

        if (_FileEntries.size() < entryNumber)
        {
            fprintf(stderr, "ERROR : ResourceFile::GetEmbeddedFileData : Invalid file entry.\n");
            return embeddedData;
        }

        if (FilePath.empty())
        {
            fprintf(stderr, "ERROR : ResourceFile::GetEmbeddedFileData : No filepath specified.\n");
            return embeddedData;
        }

        FILE* f = fopen(FilePath.c_str(), "rb"); 
        if (f == NULL)
        {
            fprintf(stderr, "ERROR : ResourceFile::GetEmbeddedFileData : Failed to open %s for reading.\n", FilePath.c_str());
            return embeddedData;
        }

        if (f != NULL)
        {
            uint64_t dataStartOffset = _FileEntries[entryNumber].DataOffset;
            uint64_t dataSize = _FileEntries[entryNumber].DataSize;

            embeddedData.resize(dataSize);

            fseek(f, dataStartOffset, SEEK_SET);
            fread(&embeddedData[0], 1, dataSize, f);
            fclose(f);
        }
        return embeddedData;
    };

    ResourceFile::ResourceFile(const fs::path& filePath, const int loadPriority)
    {
        FilePath = filePath.string();
        LoadPriority = loadPriority;

        FILE* f = fopen(FilePath.c_str(), "rb");    
        if (f == NULL)
        {
            fprintf(stderr, "ERROR : ResourceFile : Failed to open %s for reading.\n", FilePath.c_str());
            return;
        }

        if (f != NULL)
        {
            // Read .resources file header
            fseek(f, 0, SEEK_SET);
            fread(&_Header, sizeof(ResourceFileHeader), 1, f);
            _FileEntries.resize(_Header.NumFileEntries);

            // Read .resources file entries
            for (int i = 0; i < _FileEntries.size(); i++)
                fread(&_FileEntries[i], sizeof(ResourceFileEntry), 1, f);
            
            // Read total # of strings in resource file
            fread(&_numStrings, sizeof(uint64_t), 1, f);

            // Allocate vectors
            _pathStringIndexes.resize(_Header.NumPathStringIndexes);
            _stringEntries.resize(_numStrings);
            _stringOffsets.resize(_numStrings + 1);
            _stringOffsets[_numStrings] = _Header.AddrDependencyEntries; // to find last entry string length
                                                                                     
            // Read string offsets into vector
            for (int i = 0; i < _numStrings; i++)
                fread(&_stringOffsets[i], 8, 1, f);

            // Read strings into vector
            for (int i = 0; i < _numStrings; i++)
            {
                int stringLength = _stringOffsets[i + 1] - _stringOffsets[i];
                char* stringBuffer = new char[stringLength];
                fread(stringBuffer, stringLength, 1, f);
                _stringEntries[i] = stringBuffer;
            }

            // Skip ahead to string indexes
            uint64_t addrPathStringIndexes = _Header.AddrDependencyIndexes + (_Header.NumDependencyIndexes * sizeof(int));
            fseek(f, addrPathStringIndexes, SEEK_SET);

            // Read string indexes into vector
            for (int i = 0; i < _Header.NumPathStringIndexes; i++)
                fread(&_pathStringIndexes[i], sizeof(uint64_t), 1, f);
        }
        fclose(f);
    }
}