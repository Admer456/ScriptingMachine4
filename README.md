
# Scripting Machine IV

First of all, this is merely an experiment. ScriptingMachine started out as an attempt at learning how to use HostFxr/NetHost to host a .NET runtime in a C++ application. Given enough time, it may evolve into a C# scripting system for games.

In other words, I am rather interested in using C# as a scripting language for games. No existing one (Lua, Python, AngelScript, wren etc.) fully satisfies me, and frankly, I'm just curious, and I wanna see how C# scripting for a C++ engine can be done.

## Who's this meant for?

Okay, so, this repository will mostly be interesting to you only if you're interested in the niche topic of hosting a .NET runtime in a C++ app. E.g. you're writing a game engine which is already all in C++ and you wanna see how to add C# scripting to it, or something like that.

If your interests are anything remotely related to that, you might be interested in this.

If you're writing C# bindings for a library that's in C or C++, this has nothing to do with that, really. The technique I'm using here is irrelevant to it, and the amount of work is just massive anyway, in comparison to some simple `DllImport` or whatever.

## Notes
* This uses .NET 5 primarily because I developed the thing in VS2019, might switch to .NET 6 at some point
* Look at `src/CppHostUtilities/SharpHost.hpp` on how to initialise HostFxr and load C# DLLs. It mentions `hostfxr.h`, `nethost.h` and how to get them
* `src/CppHostUtilities/Assembly` can be vastly improved, in the way of obtaining function pointers to C# methods
* `src/CppHost/Game.cpp` showcases how to use SM4's high-level ""API"" to interact with a C# DLL
* `src/CppHost/GameCallbacks` contains the callbacks for the C# side. `GameImports`, `GameExports` and all function pointers that get exchanged are there. I am curious if there's another way of doing that
* `src/CSharpLibrary/GameExports.cs` and `GameSystem.cs` contain, respectively, methods that are exported to the host, and imported from the host
* All relevant constants, enums, POD structs etc. are duplicated across both sides. This can and probably should be automated
* Somewhere you may read that you can just get the .NET runtime headers by using NuGet. That is true, however, don't install the AppHost packages into the C# project, rather, download them standalone. But even then, you don't have to, because the headers & binaries are bundled with this repository. This means `coreclr_delegates.h`, `hostfxr.h` and `nethost.h` are here, as well as `libnethost.so`, `nethost.dll` and so on. This is because NuGet will append these to your C# project's dependencies, which means they'll need to go into `bin/net5.0/runtimes`, and if they're not there, HostFxr won't be able to load your C# assembly. The thing is, they're needless, they're just C++ headers & libraries and you already have them!
* The whole runtime process goes something like this:
	1. HostFxr is located thanks to NetHost
	2. We get a buncha function pointers from HostFxr, basically getting its API
	3. A HostFxr context is created with a given runtime config (`Library.runtimeconfig.json`) which really just says which .NET version is being used
	4. A C# assembly (`Library.dll`) is loaded in the given context, and a function pointer to its entry point is obtained (`Library.MainClass.Entry`). The name is completely arbitrary and up to you
	5. The host calls `Library.EngineInterface.ImportFunctions` to exchange APIs. It fills a struct with function pointers that the C# side will "import", and in return, the C# side gives it some delegates
	6. They call each other without ever invoking HostFxr again to look up delegates etc.
* This entire thing is rather sensitive. If you don't have a runtime config, it'll fail for example
* Also, very important, if you debug this in mixed mode (both C++ and C#), you won't be able to hit any breakpoints until an assembly is loaded. Or rather, until HostFxr is initialised
* None of this code is production-ready and won't be for a few years
* Tested only so far on Windows 10 x64, ideally I'd like to support Linux too
* Use CMake to generate/build the C++ project. As for the C# project, it's already there, I most likely won't be using CMake for it, even though CMake does have C# support. Everything should work out of the box, copying the correct files into `ScriptingMachine4/bin`
* What remains to be done is a ton of things related to automation:
	* generating glue from the C++ APIs (potential options: CppSharp or SWIG)
	* hot-reloading
* I imagine, in the future, I'll go for this kinda system with the following layers:
	* Scripts: a bunch of `.cs` files lying in some `scripts` folder
	* ScriptManager: a C# library that will use Roslyn to compile these scripts into another C# DLL, and load that
	* Host: a C++ app that will load ScriptManager and execute it, and by doing so, will also interact with Scripts
* Hot-reloading might be easier to do once this ^ kinda architecture is in place
 
## History

Now, time for a little story:

*I never really saw any articles that describe how to do this very thing. You have articles that talk about C# scripting in a C# app, and that's perfectly fine. But I was looking for C# scripting in a non-C#, non-NET app. I may somewhat recall coming across some article using Mono, but I'm not interested in Mono, and I've definitely come across a couple of articles and some basic example code which demonstrate hosting the .NET runtime using HostFxr. But of course, that's only to get one started.*

*In May 2020, I did attempt doing something with Mono and that was; in fact, ScriptingMachine the First. I gave up on it a few weeks later, for reasons I cannot really remember. It may have been a bit too much for me. I mean, I was only 18.*

*In June 2020, I attempted the same but in .NET instead of Mono. I used CLRHost and, I believe I gave up before even doing anything. This one was called ScriptingMachine_DotNETCore.*

*In August 2020, I began working on ScriptingMachine2. I discovered HostFxr and it really looked easier to use than CoreCLR. So? I started working. Some of the code from SM2 made it into SM3 and now SM4. ScriptHost and Assembly classes were originally written in SM2, and they've evolved a bit. SM2 was basically a little test app that still called a single method from a C# DLL, but the C# library it loaded would then call Roslyn to compile a C# script into another C# library, thus making this feel a little bit like C# scripting as I originally imagined.*

*Still, it was extremely lacking in many aspects. Lots of things were hardcoded, like which methods to call etc.*

*In June 2021, I attempted to rewrite SM2 and succeeded at it. I refactored things and added some new things as well. I had a tiny little 2D minigame in SDL2, and I exposed some of the host's API to the C# DLL with function pointers/delegates. It worked! Then I went to sleep again.*

And finally, nearly a year later, I came back to it, did another cleanup and now it's on GitHub. SM4 is currently not much different than SM3, but it may evolve over time.
