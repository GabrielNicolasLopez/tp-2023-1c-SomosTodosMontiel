#!/bin/bash
length=$(($#-1))
OPTIONS=${@:1:$length}
REPONAME="${!#}"
CWD=$PWD
echo -e "\n\nInstalling commons libraries...\n\n"
COMMONS="so-commons-library"
git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
cd $COMMONS
sudo make uninstall
make all
sudo make install
cd $CWD
echo -e "\n\nBuilding projects...\n\n"
make clean -C ./consola
make clean -C ./cpu
make clean -C ./kernel
make clean -C ./memoria
make clean -C ./filesystem
make -C ./consola
make -C ./cpu
make -C ./kernel
make -C ./memoria
make -C ./filesystem
echo -e "\n\nDeploy done!\n\n"
