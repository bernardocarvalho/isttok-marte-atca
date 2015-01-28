#!/bin/sh

#if [ -z "$1" ]; then
#	echo "Please specify the location of the configuration file"
#	exit
#else
#	echo "Going to start MARTe with the configuration specified in: " $1
#fi

CODE_DIRECTORY=$JETCONTROLPATH
#cd $CODE_DIRECTORY/MARTe/IOGAMS/ATCAadc/module/
#./mknod.sh
#cd -
LD_LIBRARY_PATH=.:./linux:$CODE_DIRECTORY/BaseLib2/linux
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/MarteSupportLib/linux
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/IOGAMS/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/IOGAMS/ATCAadc/linux/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/IOGAMS/FakeATCAAdc/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/GAMs/WebStatisticGAM/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/GAMs/DataCollectionGAM/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/HTTPMARTeInterface/CFGUploader/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/HTTPMARTeInterface/SignalHandler/linux/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/HTTPMARTeInterface/MATLABHandler/linux/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/ExtMARTeInterface/MDSPlusHandler/linux/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/GAMs/NoiseGAM/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/BaseLib2/GAMs/LoadData2/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/IOGAMS/LinuxTimer/linux/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CODE_DIRECTORY/MARTe/IOGAMS/StreamingDriver/linux/

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo $LD_LIBRARY_PATH


#taskset 2 $CODE_DIRECTORY/MARTe/linux/MARTe.ex $1
taskset 2 ./linux/driver_test.ex 
#$1

#cgdb --args $CODE_DIRECTORY/MARTe/linux/MARTe.ex $1