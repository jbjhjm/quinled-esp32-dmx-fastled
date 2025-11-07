## ESP DMX Controller using fastLED

I wasn't able to find such a project anywhere, so I implemented it on my own.
I'm sharing this with you so that you have a place to start at if you want to do something similar.
Note that I'm not an expert for ESP32 and neither C++.
Please don't expect me to be able to help you if it's not working for you.

The project is supposed to be compiled using vscode/PlatformIO.

The project configuration & IO Pin assignments are set up for a _QuinLed Dig-Octa, which is based on a ESP32-WROOM-32UE_.

## Notes

use `pio run -t menuconfig` to launch Espressif's CLI based configuration tool.

use "PlatformIO: Rebuild Intellisense Index" to regenerate the c_cpp_properties.json file in the .vscode folder.
For projects using custom libraries or multiple microcontrollers, explicitly declare dependencies in lib_deps within platformio.ini.
 This ensures that the required library paths are automatically added to the IntelliSense configuration. If IntelliSense still fails to recognize symbols, manually verify the includePath and browse.path in c_cpp_properties.json to ensure they include the correct framework and library directories, such as ${workspaceRoot} and the platform-specific package paths like $HOME/.platformio/packages/framework-arduinoespressif32.