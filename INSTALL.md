# Installing requirements for Windows

Installation via MSYS2 has been chosen to avoid problems when dealing with the installation and posterior runtime linking of `libcurl.dll`. The installation has been tested in a Windows 10 22H2 `.iso`.

1. Install [MSYS2](https://www.msys2.org/). You will perform the rest of the build from the `mingw64` terminal, or from the `cmd` if you put `msys64/mingw64/` in your PATH variable, as well as `msys64/usr/bin/`.
2. Install `cmake`, `gcc` or `clang` and `make` or `ninja` (comes pre-installed). Updating first is recommended.
```
pacman -Syu
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-make
```
3. Add `C:\msys64\mingw64\bin` (or your MSYS2 installation path) to the PATH variable (optional, only if you want to have access to the toolkit outside the `msys64` environment).
4. Run `cmake --version` to verify the installation. If there are any issues, it is recommended to run the command with the other installed commands. 

If you find problems during the building process related to `curl` or `openssl` libraries:
```
pacman -S libcurl-devel
```

## Building this package from tarball

As of Windows 10 build 17063, `tar` is available in all systems. Go [here](./INSTALL.md#unpack-the-tarball).

## Building this package from github

If you do not have Git, install it using the installer from [Git's page](https://git-scm.com/downloads) or install the `cygwin` version using:
```
pacman -S git
```
Then go [here](./INSTALL.md#download-the-repository).


# Installing requirements for Linux
The following steps worked in a fresh `linuxmint-22.1-mate-64bit.iso`.
## Steps
- Start by updating your system if possible:
```
sudo apt-get update
```
- Then we will install git (optional):
```
sudo apt install git
```
- Now we can install `cmake`:
```
sudo apt install cmake
```
- The project uses the `libcurl` developer header and library, so you will need to install it with the following:
```
sudo apt install libcurl4-openssl-dev 
```
- As `make` and `gcc` are installed in Linux distributions, you can directly go to [unpacking the tarball](./INSTALL.md#unpack-the-tarball) or [downloading the repository](./INSTALL.md#download-the-repository).

# Installing requirements for mac OS

Sadly I had no access to a mac OS environment, so refer to the instructions in linux. As long as `cmake`, a build system and a C++ compiler are installed, and the `libcurl` developer headers and libraries are installed and in the PATH variable, the installation and runtime should work.

# Download the repository

Download the repository with
```
git clone https://github.com/tobe2098/UbuntuVersionFetcher
```

# Unpack the tarball

Unpack the tarball with 
```
tar -xf UbuntuVersionFetcher.tar.gz
tar -czvf "UbuntuVersionFetcher.tar.gz" UbuntuVersionFetcher
```


# Build the binary
Simply run (if using `make`) 
```
cmake .
```

Afterwards, 
```
ninja
```
or 
```
make
```
in the main directory (this folder).
If your `cmake` configuration has been changed, the build files may be inside `./build/`. In that case, `cd` into that folder and use your build system there. The binary will also be linked in the `./build/bin/` folder.

If you experience problems with `--build`, you may need to specify the type of build files. `cmake` detects your installed build system, but if there are any errors, try (after cleaning with `git clean -fxd` or unpacking the tarball again): 
```
cmake . -G Ninja
```
or
```
cmake . -G "Unix Makefiles"
```
## Run the binary
The binary is in the `./bin/` folder. Use the `--help` option for more information on how to use it.

