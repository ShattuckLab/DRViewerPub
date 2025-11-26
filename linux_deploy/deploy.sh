#!/bin/bash
set -e
rm -rf DRViewer
install -d DRViewer/usr/bin/
DISABLE_COPYRIGHT_FILES_DEPLOYMENT=1 $HOME/Deploy/linuxdeploy-x86_64.AppImage --appdir DRViewer -e ~/GitHub/DRSuite/build-DRViewer-Desktop_Qt_6_5_1_GCC_64bit-Release/DRViewer -o appimage -d DRViewer.desktop -i DRViewer.png 
QMAKE=$HOME/Qt/6.5.1/gcc_64/bin/qmake $HOME/Deploy/linuxdeploy-plugin-qt-x86_64.AppImage --appdir  DRViewer 
