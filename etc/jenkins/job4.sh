#!/bin/sh

## This job must be executed on VM2 machines
## See ./README.md

echo "Job 4"
casejob=4
# change default  compilator  
change_compilator=etc/jenkins/change_compilator-`uname -s`-`uname -r`-$casejob.sh
echo try to source file  "$change_compilator"
test -f "$change_compilator" && echo  source file "$change_compilator" 
test -f "$change_compilator" && cat  "$change_compilator"
test -f "$change_compilator" && source "$change_compilator"


# configuration & build
autoreconf -i \
  && ./configure --prefix=/builds/workspace/freefem \
  && chmod +x ./etc/jenkins/blob/build.sh && sh ./etc/jenkins/blob/build.sh

if [ $? -eq 0 ]
then
  echo "Build process complete"
else
  echo "Build process failed"
  exit 1
fi

# check
chmod +x ./etc/jenkins/blob/check.sh && sh ./etc/jenkins/blob/check.sh

if [ $? -eq 0 ]
then
  echo "Check process complete"
else
  echo "Check process failed"
fi

# install
chmod +x ./etc/jenkins/blob/install.sh && sh ./etc/jenkins/blob/install.sh

if [ $? -eq 0 ]
then
  echo "Install process complete"
else
  echo "Install process failed"
fi