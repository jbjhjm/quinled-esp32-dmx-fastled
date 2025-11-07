use `pio run -t menuconfig` to launch Espressif's CLI based configuration tool.

use "PlatformIO: Rebuild Intellisense Index" to regenerate the c_cpp_properties.json file in the .vscode folder.
For projects using custom libraries or multiple microcontrollers, explicitly declare dependencies in lib_deps within platformio.ini.
 This ensures that the required library paths are automatically added to the IntelliSense configuration. If IntelliSense still fails to recognize symbols, manually verify the includePath and browse.path in c_cpp_properties.json to ensure they include the correct framework and library directories, such as ${workspaceRoot} and the platform-specific package paths like $HOME/.platformio/packages/framework-arduinoespressif32.