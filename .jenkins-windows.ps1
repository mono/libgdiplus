if (-not (Test-Path vcpkg)) {
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  .\bootstrap-vcpkg.bat
  .\vcpkg.exe integrate install
  # NOTE: update README.md too when updating these packages
  .\vcpkg.exe install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango
  .\vcpkg.exe install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango --triplet x64-windows
  cd ..
}

& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=Win32 -p:Configuration=Debug libgdiplus.sln
& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=Win32 -p:Configuration=Release libgdiplus.sln
& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=x64 -p:Configuration=Debug libgdiplus.sln
& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=x64 -p:Configuration=Release libgdiplus.sln

Compress-Archive -DestinationPath libgdiplus.zip -Path Win32,x64
