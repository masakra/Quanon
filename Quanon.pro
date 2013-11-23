# Add more folders to ship with the application, here
folder_01.source = qml/Quanon
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

QT += multimedia

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = qml/Quanon

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    processor.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qml/Quanon/Button.qml \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

HEADERS += \
    processor.h
