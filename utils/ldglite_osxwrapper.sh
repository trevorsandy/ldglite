#!/bin/bash

# This wrapper will use the OSX "open" command and the LDGLITEARGS env var
# to run ldglite with the app bundle settings and avoid the popup error msgs.
# I tend to run ldglite on the command line.  This makes that work on OSX.
# 
# Set this to executable and copy it to somewhere like /usr/local/bin.
# 
#   chmod a+x ldglite_osxwrapper.sh
#   sudo cp ldglite_osxwrapper.sh /usr/local/bin/ldglite


args=""
filename=""

# Separate the filename from the args starting with a plus or minus.
while [[ $# > 0 ]]
do
  arg="$1"
  case $arg in 
    -*|+*) args="$args $arg" ;;
    *)  filename="$arg" ;;
  esac
  shift # past argument or value
done

# If filename is found, then open it with app bundle as if it were clicked on.
# Otherwise pass it with args and let ldglite use search paths to find it.
if [ -f "${filename}" ] ; then
  LDGLITEARGS="$LDGLITEARGS $args" open -a ldglite "${filename}"
else
  LDGLITEARGS="$LDGLITEARGS $args ${filename}" open -a ldglite
fi
