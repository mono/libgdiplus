$ErrorActionPreference = "Stop"

function exitonerror ($exitCode) {
   if ($exitCode -ne 0) { echo "Program failed with exit code: $exitCode"; exit $exitCode }
}

if (-not (Test-Path vcpkg)) {
  git clone https://github.com/Microsoft/vcpkg.git
  cd vcpkg
  git checkout 6984ef0c2ce27e7905f47078e79117c4e02e5ea0 # need to use this until https://github.com/Microsoft/vcpkg/issues/3631 is fixed
  git cherry-pick 1cb142d62eed89ad430a5dc5875d05508bf27c6a

  .\bootstrap-vcpkg.bat; exitonerror $LASTEXITCODE
  .\vcpkg.exe integrate install; exitonerror $LASTEXITCODE

  # NOTE: update README.md too when updating these packages
  .\vcpkg.exe install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango --triplet x86-windows; exitonerror $LASTEXITCODE
  .\vcpkg.exe install giflib libjpeg-turbo libpng cairo glib tiff libexif glib pango --triplet x64-windows; exitonerror $LASTEXITCODE

  cd ..
}

& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=Win32 -p:Configuration=Debug libgdiplus.sln; exitonerror $LASTEXITCODE
& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=Win32 -p:Configuration=Release libgdiplus.sln; exitonerror $LASTEXITCODE
& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=x64 -p:Configuration=Debug libgdiplus.sln; exitonerror $LASTEXITCODE
& "C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe" -p:PlatformToolset=v140 -p:Platform=x64 -p:Configuration=Release libgdiplus.sln; exitonerror $LASTEXITCODE

Compress-Archive -DestinationPath libgdiplus.zip -Path Win32,x64
