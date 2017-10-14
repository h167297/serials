#!/bin/sh

#distribution include from 12.04 to 18.04
distribution="trusty xenial zesty artful"  

#arg1:gpg key_id, arg2:launchpad id/ppa, arg3:app version, arg4:ubuntu distribution
if [ "$1" != "" ]; then
version=$1
else
version=$(awk -F'[()]' '{print $2}' debian/changelog | sed -n '1p')
fi

if [ "$2" != "" ]; then
./publish.sh 996EA523 h167297/hening $version $2
else
for i in $distribution; do  
./publish.sh 996EA523 h167297/hening $version $i
done  
fi
