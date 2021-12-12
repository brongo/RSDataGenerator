#pragma once

#include <string>
#include <vector>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#define SAFE_SPACE 64

namespace RSDataParser
{
    namespace fs = std::filesystem;

    typedef int OodLZ_CompressFunc(
        int codec, uint8_t* src_buf, size_t src_len, uint8_t* dst_buf, int level,
        void* opts, size_t offs, size_t unused, void* scratch, size_t scratch_size);

    typedef int OodLZ_DecompressFunc(
        uint8_t* src_buf, int src_len, uint8_t* dst, size_t dst_size, int fuzz, int crc, int verbose,
        uint8_t* dst_base, size_t e, void* cb, void* cb_ctx, void* scratch, size_t scratch_size, int threadPhase);

    class OodleLibrary
    {
        public:
            OodLZ_CompressFunc* OodLZ_Compress = NULL; 
            OodLZ_DecompressFunc* OodLZ_Decompress = NULL;
            bool DecompressAndSave(std::vector<uint8_t> compressedData, const uint64_t decompressedSize, const fs::path& outputFile);            
            bool CompressFile(std::string filename, std::string destFilename);
            bool Init(fs::path pathToOodleDLL);
    };
}