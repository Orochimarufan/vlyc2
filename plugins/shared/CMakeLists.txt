
set(HEADERS
../include/plugin.h
../include/siteplugin.h
../include/video.h
)

set(SOURCES
video.cpp
)

add_definitions(-DLIBVLYC2PLUGIN_LIBRARY)

add_library(vlyc2plugin SHARED ${HEADERS} ${SOURCES})
qt5_use_modules(vlyc2plugin Core)
