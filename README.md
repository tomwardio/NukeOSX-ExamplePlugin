# NukeOSX-ExamplePlugin
This is a really simple example XCode project for building a simple NUKE plugin.

This creates a simple Greyscale Iop, using XCode6, targetting 10.6 and using clang with libstdc++.

You can change what version of Nuke to compile against by changing the NUKE_APPLICATION_FOLDER variable from within the project settings.

A custom build script also copies the dylib into your default .nuke folder, to make it easy to update and test plugin changes.

This is released with no licensing what so ever, so feel free to clone and do whatever you want with it!
