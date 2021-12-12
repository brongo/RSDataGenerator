#include "Oodle.h"

namespace RSDataParser
{
    bool OodleLibrary::Init(fs::path pathToOodleDLL)
    {
        if (fs::is_directory(pathToOodleDLL))
            pathToOodleDLL = pathToOodleDLL / "oo2core_8_win64.dll";

        if (!fs::exists(pathToOodleDLL) || (pathToOodleDLL.filename() != "oo2core_8_win64.dll"))
        {
            fprintf(stderr, "ERROR : OodleLibrary : Required file oo2core_8_win64.dll was not found. \n\n");
            return false;
        }

#ifdef _WIN32
        // Load oodle dll
        auto oodle = LoadLibraryA(pathToOodleDLL.string().c_str());
        if (oodle != NULL)
        {
            OodLZ_Decompress = (OodLZ_DecompressFunc*)GetProcAddress(oodle, "OodleLZ_Decompress");
            OodLZ_Compress = (OodLZ_CompressFunc*)GetProcAddress(oodle, "OodleLZ_Compress");
        }
#else
        // Copy oodle to current dir to prevent linoodle errors
        std::error_code ec;
        fs::copy(oodlePath, fs::current_path(), ec);
        if (ec.value() != 0)
            return false;

        // Load linoodle library
        std::string linoodlePath = basePath + "/liblinoodle.so";
        auto oodle = dlopen(linoodlePath.c_str(), RTLD_LAZY);
        OodLZ_Decompress = (OodLZ_DecompressFunc*)dlsym(oodle, "OodleLZ_Decompress");
        OodLZ_Compress = (OodLZ_CompressFunc*)dlsym, "OodleLZ_Compress");

        // Remove oodle dll
        fs::remove(fs::current_path().append("oo2core_8_win64.dll"), ec);
#endif

        if (oodle == NULL || OodLZ_Decompress == NULL || OodLZ_Compress == NULL)
            return false;

        return true;
    }

    bool OodleLibrary::DecompressAndSave(std::vector<byte> compressedData, const uint64_t decompressedSize, const fs::path& outputFile)
    {
        if (OodLZ_Decompress == NULL)
        {
            fprintf(stderr, "ERROR : OodleLibrary : Oodle has not been initialized. Run Oodle::Init() first. \n\n");
            return false;
        }

        uint8_t* output = new uint8_t[decompressedSize + SAFE_SPACE];
        uint64_t outbytes = OodLZ_Decompress(compressedData.data(), compressedData.size(), output, decompressedSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        if (outbytes == 0)
        {
            fprintf(stderr, "ERROR : OodleLibrary : Failed to decompress data with Oodle DLL. \n\n");
            return false;
        }

        FILE *f;
        fopen_s(&f, outputFile.string().c_str(), "wb");

        if (f != NULL)
        {
            fwrite(output, 1, outbytes, f);
            fclose(f);
        }

        return true;
    }

    bool OodleLibrary::CompressFile(std::string filename, std::string destFilename) {

        if (OodLZ_Compress == NULL)
        {
            fprintf(stderr, "ERROR : OodleLibrary : Oodle has not been initialized. Run Oodle::Init() first. \n\n");
            return false;
        }

        FILE* f;
        if (fopen_s(&f, filename.c_str(), "rb") != 0) {
            fprintf(stderr, "ERROR : OodleLibrary : Failed to open %s for reading! \n\n", filename.c_str());
            return 1;
        }

        fseek(f, 0, SEEK_END);
        uint64_t unpacked_size = ftell(f);
        uint8_t* input = new byte[unpacked_size];

        fseek(f, 0, SEEK_SET);
        if (fread(input, 1, unpacked_size, f) != unpacked_size) {
            fprintf(stderr, "ERROR : OodleLibrary : Failed to read file into memory.\n");
            return 1;
        }

        uint8_t* output = new byte[unpacked_size + 65536];
        if (!output) {
            fprintf(stderr, "ERROR : OodleLibrary : Couldn't allocate memory for compression.\n");
            return 1;
        }

        // 8 = Kraken, 4 = compression level
        uint64_t compressed_size = OodLZ_Compress(8, input, unpacked_size, output, 4, 0, 0, 0, 0, 0);

        if (compressed_size < 0) {
            fprintf(stderr, "ERROR : OodleLibrary : Compression failed.\n");
            return 1;
        }

        if (fopen_s(&f, destFilename.c_str(), "wb") != 0) {
            fprintf(stderr, "ERROR : OodleLibrary : Failed to open destination file for writing!\n");
            return 1;
        }

        // Write compressed data to file
        if (f != NULL) {

            fwrite(&unpacked_size, 8, 1, f);
            fwrite(output, 1, compressed_size, f);
            fclose(f);
        }
        return 0;
    }

}
