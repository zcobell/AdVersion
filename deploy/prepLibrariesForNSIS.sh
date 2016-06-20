#!/bin/bash


QADCModulesPath=/cygdrive/c/libraries/QADCModules/bin
MetisPath=/cygdrive/c/libraries/metis/bin
LibGitPath=/cygdrive/c/libraries/libgit2/bin
netCDFPath=../../../MetOceanViewer/thirdparty/netcdf/bin_64


#...Find the deployment script
deployqt=$(which windeployqt)
if [ $? != 0 ] ; then
    echo "ERROR: Could not find windeployqt"
    exit 1
fi

#...Make a deployment directory
if [ -d deployment ] ; then
    rm -rf deployment
fi
mkdir deployment

cd deployment

#...Grab the AdVersion library
cp ../../../build-AdVersion-Desktop_Qt_5_6_0_MSVC2015_64bit-Release/libAdVersion/release/AdVersion.dll .
cp ../../../build-AdVersion-Desktop_Qt_5_6_0_MSVC2015_64bit-Release/AdVersion_cmd/release/adversion.exe .
cp ../../../build-AdVersion-Desktop_Qt_5_6_0_MSVC2015_64bit-Release/AdVersion_gui/release/AdVersion_gui.exe .


#...Grab QADCModules and all of its friends
cp $QADCModulesPath/QADCModules.dll .
cp $netCDFPath/*.dll .


#...Grab libMetis
cp $MetisPath/metis.dll .


#...Grab libGit2
cp $LibGitPath/git2.dll .


#...Run the deployment script
windeployqt --compiler-runtime -release AdVersion.dll
windeployqt --compiler-runtime -release adversion.exe
windeployqt --compiler-runtime -release AdVersion_gui.exe
