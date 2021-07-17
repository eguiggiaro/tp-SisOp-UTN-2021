#!/bin/bash

CWD=$PWD
RULE=""

echo -e "\n\nInstalling commons libraries...\n\n"

COMMONS="so-commons-library"
NIVELGUI="so-nivel-gui-library"

cd $CWD
rm -rf $COMMONS
git clone "https://github.com/sisoputnfrba/${COMMONS}.git" $COMMONS
cd $COMMONS
sudo make uninstall
make all
sudo make install

cd $CWD
rm -rf $NIVELGUI
git clone "https://github.com/sisoputnfrba/${NIVELGUI}.git" $NIVELGUI
cd $NIVELGUI
sudo make uninstall
make all
sudo make install


cd $CWD
DEPENDENCIES=(shared)
PROJECTS=(miram store discordiador)

echo -e "\n\nBuilding dependencies...\n\n"

for i in "${DEPENDENCIES[@]}"
do
  echo -e "\n\nBuilding ${i}\n\n"
  cd $i
  make install
  cd $CWD 
done

echo -e "\n\nBuilding projects...\n\n"

for i in "${PROJECTS[@]}"
do
  echo -e "\n\nBuilding ${i}\n\n"
  cd $i
  make $RULE
  cd $CWD
done

echo -e "\n\nDeploy done!\n\n"
