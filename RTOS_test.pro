TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    task.cpp \
    test.cpp \
    global.cpp \
    os.cpp \
    resource.cpp \
    event.cpp

HEADERS += \
    defs.h \
    rtos_api.h \
    sys.h
