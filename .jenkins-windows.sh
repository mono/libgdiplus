#!/bin/bash -xe

export PATH=/usr/bin:/usr/local/bin:$PATH

wget "https://dl.hexchat.net/gtk/gtk-win32-2018-03-13.7z" -O gtk-Win32.7z
wget "https://dl.hexchat.net/gtk/gtk-x64-2018-03-13.7z" -O gtk-x64.7z
wget "https://dist.nuget.org/win-x86-commandline/v4.1.0/nuget.exe" -O nuget.exe

7z x gtk-Win32.7z -ogtk
7z x gtk-x64.7z -ogtk
chmod +x nuget.exe

./nuget.exe restore
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:PlatformToolset=v140 /p:Platform=Win32 /p:Configuration=Debug libgdiplus.sln
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:PlatformToolset=v140 /p:Platform=Win32 /p:Configuration=Release libgdiplus.sln
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:PlatformToolset=v140 /p:Platform=x64 /p:Configuration=Debug libgdiplus.sln
/cygdrive/c/Program\ Files\ \(x86\)/MSBuild/14.0/Bin/MSBuild.exe /p:PlatformToolset=v140 /p:Platform=x64 /p:Configuration=Release libgdiplus.sln

7z a -tzip libgdiplus.zip Win32 x64
