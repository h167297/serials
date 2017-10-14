#!/bin/sh

oridirname="${PWD##*/}"

#step1:prepare environment
cp -r ../${oridirname} ../serials_$3~$4
cd ../serials_$3~$4
sed -i '1c serials ('$3~$4') '$4'; urgency=low' debian/changelog
dirname="${PWD##*/}"

#step2:packge source filses
cd ../
tar -zcvf ${dirname}.orig.tar.gz ${dirname}/

#step3:signature and checks
cd ${dirname}/
debuild -S -rfakeroot -k$1

#step4:upload to ppa
cd ../
dput ppa:$2 ${dirname}_source.changes

#step5:delete temporary files
rm -f ${dirname}.dsc
rm -f ${dirname}.orig.tar.gz
rm -f ${dirname}.debian.tar.xz
rm -f ${dirname}_source.changes
rm -f ${dirname}_source.build
rm -f ${dirname}_source.ppa.upload

if [ "${dirname}" != "${oridirname}" ]; then
rm -rf ${dirname}
fi

