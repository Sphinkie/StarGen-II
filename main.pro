TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        sources/main.cpp \
        sources/lib/SG_Atmosphere.cpp \
        sources/lib/SG_File.cpp \
        sources/lib/SG_FileWriter.cpp \
        sources/lib/SG_File_TXT.cpp \
        sources/lib/SG_File_XML.cpp \
        sources/lib/SG_Gas.cpp \
        sources/lib/SG_Planet.cpp \
        sources/lib/SG_SolarSystem.cpp \
        sources/lib/SG_Star.cpp \
        sources/lib/SG_Stardust.cpp \
        sources/lib/SG_utils.cpp \
        sources/main.cpp

HEADERS += \
    sources/lib/SG_Atmosphere.h \
    sources/lib/SG_File.h \
    sources/lib/SG_FileWriter.h \
    sources/lib/SG_File_TXT.h \
    sources/lib/SG_File_XML.h \
    sources/lib/SG_Gas.h \
    sources/lib/SG_Planet.h \
    sources/lib/SG_SolarSystem.h \
    sources/lib/SG_Star.h \
    sources/lib/SG_Stardust.h \
    sources/lib/SG_const.h \
    sources/lib/SG_utils.h
