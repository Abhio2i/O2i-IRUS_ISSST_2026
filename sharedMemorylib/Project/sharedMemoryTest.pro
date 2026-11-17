QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EntityHandler.cpp \
    main.cpp \
    sharedMemory.cpp

HEADERS += \
    EntityHandler.h \
    sharedMemory.h \
    struct.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../Desktop/Cpp Projects/sharedMemory/release/' -lSharedMemoryLib
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../Desktop/Cpp Projects/sharedMemory/debug/' -lSharedMemoryLib
# else:unix: LIBS += -L$$PWD/'../Desktop/Cpp Projects/sharedMemory/' -lSharedMemoryLib

# INCLUDEPATH += $$PWD/'../Desktop/Cpp Projects/sharedMemory'
# DEPENDPATH += $$PWD/'../Desktop/Cpp Projects/sharedMemory'
QMAKE_CXXFLAGS += -fno-sized-deallocation

