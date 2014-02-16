call "%VS100COMNTOOLS%..\..\VC\bin\vcvars32.bat"

: icu
pushd tmp\icu
msbuild /p:Platform=win32 /p:Configuration=Debug source/allinone/allinone.sln
msbuild /p:Platform=win32 /p:Configuration=Release source/allinone/allinone.sln
:msbuild /p:Platform=x64 /p:Configuration=Debug source/allinone/allinone.sln
:msbuild /p:Platform=x64 /p:Configuration=Release source/allinone/allinone.sln
popd

: b2
pushd tmp\boost_1_55_0\tools\build\v2
call bootstrap.bat
b2 install --prefix="..\..\..\..\b2"
popd
set path=%path%;%cd%\tmp\b2\bin;

: compile
pushd tmp\boost_1_55_0
:b2 --prefix=%cd%\..\..\boost_1_55_0 --without-mpi toolset=msvc-10.0 variant=debug,release threading=multi link=static address-model=32 install
b2 --prefix=%cd%\..\..\boost_1_55_0 --without-mpi toolset=msvc-10.0 variant=debug,release threading=multi link=static runtime-link=static address-model=32 install
popd

: clear
rmdir /s /q tmp