#include "InputValidator.h"

namespace RSDataParser
{
    bool InputValidator::inputIsEternalDirectory(fs::path& inputPath) const
    {
        if (!fs::is_directory(inputPath))
        {
            printf("ERROR: The <path> you entered is not a valid directory. \n");
            printf("Please provide the full path to your DOOM Eternal installation directory. \n\n");
            return 0;
        }

        fs::path pathToDOOMExecutable = inputPath / "DOOMEternalx64vk.exe";
        fs::path pathToBaseDirectory = inputPath / "base";

        if (!fs::exists(pathToDOOMExecutable) || !fs::exists(pathToBaseDirectory))
        {
            printf("ERROR: Unable to locate Doom Eternal files. \n");
            printf("Please provide the full path to your DOOM Eternal installation directory. \n");
            return 0;
        }

        return 1;
    }

    bool InputValidator::inputIsValid(fs::path& inputPath)
    {
        if (!inputIsEternalDirectory(inputPath))
            return 0;

        return 1;
    }
}