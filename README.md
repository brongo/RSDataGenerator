# RSDataGenerator v1.0 by SamPT

This tool generates the "rs_data" file used by EternalModInjector. Should build easily in Windows. Needs some adjustments before it will work on Linux systems.

**Usage:** 
`RSDataGenerator.exe <path>` 
(full path to your DOOM Eternal installation directory).

The program may take a few minutes to complete. Your `rs_data` file will be placed in the same directory as `RSDataGenerator.exe` - there will also be a `rs_data_uncompressed` file in case you want to verify the contents. However this is not necessary and the file is not used by DEternal_loadMods.exe.

**Important:** 
Make sure your game is fully verified/repaired and not modified in any way prior to using this. There is no need to extract any files.

**Other:** 
 * This is untested and probably doesn't work on Gamepass versions.
 * A number of .mapresources files will be automatically extracted to `<path>/MapResources` when the program is run. They can be safely deleted afterwards.
