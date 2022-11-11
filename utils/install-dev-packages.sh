#!/bin/sh
LDGLite=`pwd`
if [ -f /etc/debian_version ]
then
	apt-get update
	apt-get install -y git lintian build-essential debhelper ccache lsb-release
	for dev_package in `grep Build-Depends $LDGLite/obs/debian/control | cut -d: -f2| sed 's/(.*)//g' | tr -d ,`
	do
		apt-get --no-install-recommends install -y $dev_package
	done
fi
