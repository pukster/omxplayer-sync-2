#!/bin/bash

# To understand what is going on here read the README.SYNC file        #
# 
# Delete install folder if it is still there
if [ -d omxplayer-dist ]
then
   rm -fr omxplayer-dist
fi

# Delete compressed file if it is still there
if [ -d omxplayer-dist.tar.gz ]
then
   rm -fr omxplayer-dist.tar.gz
fi

# Remeber that the Makefile has been altered to expect the RASPI       #
# variable. Technically you only have to compile it once (on account   #
# of all raspis being identical), but for completeness, you can choose #
# to compile it specifically for every machine.                        #
RASPI=raspi1
export RASPI
make && make dist

if [ $? -eq 0 ]
then
   for RASPI in raspi1 raspi2 raspi4 raspi5 raspi6
   do
      export RASPI
      
      sudo rsync --progress -a omxplayer-dist/* /mnt/CYBRAN/$RASPI/ 
   done
fi
