QT += core gui
QT += widgets
QT += openglwidgets

CONFIG += c++23
CONFIG += sdk_no_version_check
unix:QMAKE_CXXFLAGS += -Wall -Werror -Wpedantic -Wextra
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#INCLUDEPATH += include
INCLUDEPATH += $$PWD/src/
INCLUDEPATH += $$PWD/3rdParty/

unix:LIBS += -lz -lm
win32 {
PLATFORMTOOLSET=v142
LIBS += -lopengl32 -lglu32
win32-msvc2019:PLATFORMTOOLSET=v142

INCLUDEPATH += $$PWD/3rdParty/zlib-1.2.13

DEFINES += "NOMINMAX"
DEFINES += "_AFXDLL"
DEFINES += "ZLIB_WINAPI"
DEFINES += "_USE_MATH_DEFINES"
LIBDIR=$$PWD/lib/
DEFINES += "QT_X86_64"
LIBS += -L$${LIBDIR}x64/$${PLATFORMTOOLSET}
CONFIG(release, debug|release): LIBS += -lzlib_x64_MD
else:CONFIG(debug, debug|release): LIBS += -lzlib_x64_MDd
QMAKE_LFLAGS_RELEASE += /LTCG
}

macx {
ICON = src/images/DRViewer.icns
licenses.files = LICENSE.txt COPYRIGHT.txt ThirdPartyLicenses.txt
licenses.path = Contents/Resources
QMAKE_BUNDLE_DATA += licenses
}

SOURCES += \
  src/axisinfo.cpp \
  src/checkglerror.cpp \
  src/colorbar.cpp \
  src/drsuitesettings.cpp \
  src/drwindow_filehandling.cpp \
  src/drwindow_menus.cpp \
  src/drwindow_utility.cpp \
  src/dsimage.cpp \
  src/dsshaderprogram.cpp \
  src/main.cpp \
  src/drwindow.cpp \
  src/marchingsquares.cpp \
  src/plotwidget.cpp \
  src/relaxationtoolboxform.cpp \
  src/spectralimageinfo.cpp \
  src/spectralimagewidget.cpp \
  src/spectroscopicshaderobject.cpp \
  src/spectrumglwidget.cpp \
  src/spectrumglwidget_gestures.cpp \
  src/spectrumviewcontroller.cpp \
  src/verticaltextlabel.cpp \
  src/vol3dtexture.cpp \
  src/colormap.cpp \
  src/volnbase.cpp

HEADERS += \
  src/axisinfo.h \
  src/checkglerror.h \
  src/colorops.h \
  src/colormap.h \
  src/drsuitesettings.h \
  src/drwindow.h \
  src/dsimage.h \
  src/dsshaderprogram.h \
  src/imagestate.h \
  src/marchingsquares.h \
  src/plotwidget.h \
  src/relaxationtoolboxform.h \
  src/rgb8.h \
  src/spectralimageinfo.h \
  src/spectroscopicshaderobject.h \
  src/spectrumglwidget.h \
  src/spectrumviewcontroller.h \
  src/silttypes.h \
  src/slicet.h \
  src/strutil.h \
  src/uvpoint.h \
  src/verticaltextlabel.h \
  src/vol3dtexture.h \
  src/volnbase.h \
  src/volnd.h \
  src/volnd_t.h \
  src/zstream.h

FORMS += \
   src/drwindow.ui \
   src/relaxationtoolboxform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DEFINES += PROGNAME='\\"$${TARGET}\\"'

DISTFILES += \
  images/spectrum256px.png \
  src/spectrum.frag \
  src/spectrum.vert

RESOURCES += \
  src/shaders.qrc
