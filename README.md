## Building

The project uses C++/Qt with static linkage, in order to provide decent user experience and developer experience as well. This does
have a few drawbacks, as well, in terms of initial setup effort as Qt doesn't really play
nice for static builds with x64 Windows. I will try and explain my process as easy as possible.
I will assume you already have a recent and working Visual Studio installation with C++ available.
Credits to [this post](https://retifrav.github.io/blog/2018/02/17/build-qt-statically) for explaining
the process (you can refer to it for more in depth information).

### Building Qt

- Install a recent Python. I use 3.7 on [Miniconda](https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe).
  Install [ActivePerl](https://www.activestate.com/products/activeperl/downloads/). Make sure
  both `perl` and `python`'s executables are in your `PATH` environment variable, hence reachable
  from any command prompt by typing their name.

- Download Qt's source code from [](https://www.qt.io/offline-installers). The package should
  be named something like `qt-everywhere-5.11.2-src.zip`. I will assume the version used is
  5.11.2, but adjust accordingly in what follows.
- Create a directory in some place of your liking such as `C:\Qt` where the installation will
  be placed, and extract the zip straight in this directory. You will now have a
  `C:\Qt\qt-everywhere-5.11.2-src` directory, which I renamed to `C:\Qt\Source-5.11.2` because
  it is comfier.
- Create another directory named `C:\Qt\5.11.2-static`. This will hold our installation when
  everything is said and done.
- Download and extract [Jom](http://download.qt.io/official_releases/jom/jom.zip) to a directory
  such as `C:\Qt\jom` (so you have the executable somewhere in `C:\Qt\jom\jom.exe`).
- Now create a directory named `C:\Qt\Shadow-5.11.2`. This will contain our [shadow build](https://wiki.qt.io/Qt_shadow_builds).
  Launch a [Developer command prompt](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2017)
  and navigate to this directory.
- It is time to launch our configuration and build, and pray everything goes all right:

```
cd C:\Qt\Shadow-5.11.2
C:\Qt\Source-5.11.2\configure.bat -release -static -no-pch -optimize-size -opengl desktop -platform win32-msvc -prefix "\path\to\Qt\511-static" -skip webengine -nomake tools -nomake tests -nomake examples
C:\Qt\jom\jom.exe -j8
C:\Qt\jom\jom.exe -j8 install
```

- If everything went all right -- it should have, I fought a lot before finding this combination --
  you now have a working Qt installation in `C:\Qt\5.11.2-static` and can add the `C:\Qt\5.11.2-static\bin`
  directory to the `PATH` environment variable. This is fundamental for my build script to work.

### Using the buildscript

The buildscript is pretty trivial to use.

```
python build.py [help|-h|--help] [build] [strip] [run]

help ............ print this help
build ........... build the executable
strip ........... compress the executable with UPX
run ............. run the program (does not build)

If no parameter is specified, build is assumed.
```

The compiled program will be found in `build\release`.
