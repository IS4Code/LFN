Long Function Names v1.0
==========

_LFN_ adds SA-MP support for AMX programs having public names longer than 31 characters. Native functions, public functions, and public variables can have unlimited length (i.e. up to 65535 characters) with this plugin.

Compiler support is needed to allow functions with long names. Check out my [fork](//github.com/IllidanS4/pawn-compiler) for increased maximum function length.

## Installation
Download the latest [release](//github.com/IllidanS4/LFN/releases/latest) for your platform to the "plugins" directory and add "LFN" (or "LFN.so" on Linux) to the `plugins` line in server.cfg.

## Building
Use Visual Studio to build the project on Windows, or `make` on Linux.

## Example
```pawn
#include <a_samp>

forward VeryLongPublicFunctionNameExceeding31Characters();
public VeryLongPublicFunctionNameExceeding31Characters()
{
    print("hello!");
}

public OnFilterScriptInit()
{
    CallLocalFunction("VeryLongPublicFunctionNameExceeding31Characters", "");
}
```