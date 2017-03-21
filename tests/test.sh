#!/bin/bash

# As simple script to test ldglite command line functionality
# To use this script copy your ldglite bundle or executable to directory 00 - 05 accorgingly.
# Be sure to update the export paths, command string and echo descriptions as necessary.

# logging stuff
ME="$(basename "$(test -L "$0" && readlink "$0" || echo "$0")")"
CWD=`pwd`
LOG="${CWD}/$ME.log"
if [ -f "$ME.log" ]; then
  rm -f "$ME.log"
fi
exec > >(tee -a ${LOG} )
exec 2> >(tee -a ${LOG} >&2)

# PLI
#ldglite.app/Contents/MacOS/ldglite ${ARGS} -mFTestResult_Foo3.png Foo3.ldr 

# CSI
echo " "
echo "START----------------------------------------------"
echo " "
LDRAWDIR=~/Library/ldraw
LDSEARCHDIRS=
if [ ! -d ${LDRAWDIR}/parts ]; then
    echo "LDRAWDIR not found in ${LDRAWDIR}. Downloading ldraw library..."
    curl -O http://www.ldraw.org/library/updates/complete.zip
    echo "Extracting ldraw library into ${LDRAWDIR}..."
    unzip -d ${LDRAWDIR} -q complete.zip
    echo "set LDRAWDIR environment variable..."
    ../set-ldrawdir.command
fi
echo "LDRAWDIR:     ${LDRAWDIR}"
echo "LDSEARCHDIRS: ${LDSEARCHDIRS}"
export LDRAWDIR
export LDSEARCHDIRS
ARGS="-l3 -i2 -ca0.01 -cg23,-45,3031328 -J -v1240,1753 -o0,-292 -W2 -q -fh -w1"

echo " "
echo "NORMAL PART TEST-----------------------------------"
echo " "
for f in *.png; do
   [ -e "$f" ] && echo rm *.png
   break
done
echo " "
if [[ $1 -eq 2 ]]; then
   echo "Executing ldglite legacy 1.3.1 bundle build..."
   if [ -d 00/ldglite.app ]; then
       00/ldglite.app/Contents/MacOS/ldglite ${ARGS} -mFTestResult_legacy_1.3.1_Foo2.png Foo2.ldr
   else
       echo "==> ldglite legacy 1.3.1 bundle does not exist in directory ./00 "
   fi 
elif [[ $1 -eq 3 ]]; then
   echo "Executing ldglite gcc x86 SDK10.7 Qt4.7 qmake bundle build..."
   if [ -d 03/ldglite ]; then
       03/ldglite ${ARGS} -mFTestResult_gcc_x86_SDK10.7_Qt4.7_qmake_Foo2.png Foo2.ldr 
   else
       echo "==> ldglite gcc x86 SDK10.7 Qt4.7 qmake bundle does not exist in directory ./03 "
   fi 
elif [[ $1 -eq 4 ]]; then
   echo "Executing ldglite clang x86 SDK10.7 Qt4.7 qmake bundle build..."
   if [ -d 04/ldglite ]; then
       04/ldglite ${ARGS} -mFTestResult_clang_x86_SDK10.7_Qt4.7_qmake_Foo2.png Foo2.ldr 
   else
       echo "==> ldglite clang x86 SDK10.7 Qt4.7 qmake bundle does not exist in directory ./04 "
   fi 
elif [[ $1 -eq 5 ]]; then
   echo "Executing ldglite bundle on clang x86_64 SDK10.11 Qt5.7 qmake (Travis CI) build..."
   if [ ! -d 05 ]; then mkdir -p 05/bin ; else rm -rf 05 ; mkdir -p 05/bin ; fi
   if [ -d ../release/ldglite.app ] && [ -f ../release/ldglite.app/Contents/MacOS/ldglite ]; then  
       cp -rf ../release/ldglite.app 05/bin 
       05/bin/ldglite.app/Contents/MacOS/ldglite ${ARGS} -mFTestResult_clang_x86_64_SDK10.11_Qt5.7_qmake_Foo2.png Foo2.ldr 
   else
       echo "==> ldglite bundle on clang x86_64 SDK10.11 Qt5.7 bundle (Travis CI) does not exist in directory ./05 "
   fi 
elif [[ $1 -eq 6 ]]; then
   echo "Executing ldglite bundle on clang x86_64 SDK10.12 Qt5.7 qmake build..."
   if [ ! -d 05 ]; then mkdir -p 05/bin ; else rm -rf 05 ; mkdir -p 05/bin ; fi
   if [ -d ../../build-ldglite-Desktop_Qt_5_7_1_clang_64bit-Release/release/ldglite.app ] && [ -f ../../build-ldglite-Desktop_Qt_5_7_1_clang_64bit-Release/release/ldglite.app/Contents/MacOS/ldglite ]; then  
       cp -rf ../../build-ldglite-Desktop_Qt_5_7_1_clang_64bit-Release/release/ldglite.app 05/bin 
       05/bin/ldglite.app/Contents/MacOS/ldglite ${ARGS} -mFTestResult_clang_x86_64_SDK10.12_Qt5.7_qmake_Foo2.png Foo2.ldr 
   else
       echo "==> ldglite bundle on clang x86_64 SDK10.12 Qt5.7 bundle does not exist in directory ./05 "
   fi 
elif [[ $1 -eq 7 ]]; then
   echo "Executing ldglite launcher gcc x86_64 SDK10.12 build..."
   if [ ! -d 01 ]; then mkdir -p 01/bin ; else rm -rf 01 ; mkdir -p 01/bin ; fi
   if [ -f ../launcher/src/ldglite_launcher ] && [ -f ../ldglite ]; then 
      cp -rf ../launcher/src/ldglite_launcher 01/bin 
      cp -rf ../ldglite 01/bin
      cp -rf ../launcher/Frameworks 01
      01/bin/ldglite_launcher ${ARGS} -mFTestResult_launcher_gcc_x86_64_SDK10.12_Foo2.png Foo2.ldr   
   else
       echo "==> ldglite launcher gcc x86_64 SDK10.12 packages ldglite/launcher/src/ldglite_launcher and/or ldglite/ldglite does not exist"
   fi
else
   echo "Executing ldglite app gcc build..."
   if [ ! -d 01 ]; then mkdir -p 01/bin ; else rm -rf 01 ; mkdir -p 01/bin ; fi
   if [ -f ../ldglite ]; then 
      cp -rf ../ldglite 01/bin
      01/bin/ldglite ${ARGS} -mFTestResult_app_gcc_Foo2.png Foo2.ldr   
   else
       echo "==> ldglite app gcc x86_64 SDK10.9 package at ldglite/ldglite does not exist"
   fi 
fi
echo " "
echo "END------------------------------------------------"
echo " "
