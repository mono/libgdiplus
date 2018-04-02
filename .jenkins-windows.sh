#!/bin/bash -xe

export PATH=/usr/bin:/usr/local/bin:$PATH

git clone https://github.com/Microsoft/vcpkg.git
vcpkg/bootstrap-vcpkg.bat
vcpkg/vcpkg integrate install

vcpkg/vcpkg install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango
vcpkg/vcpkg install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango --triplet x64-windows

/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:Platform=Win32 /p:Configuration=Debug libgdiplus.sln
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:Platform=Win32 /p:Configuration=Release libgdiplus.sln
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:Platform=x64 /p:Configuration=Debug libgdiplus.sln
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:Platform=x64 /p:Configuration=Release libgdiplus.sln

7z a -tzip libgdiplus.zip Win32 x64
