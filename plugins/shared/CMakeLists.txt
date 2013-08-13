
set(HEADERS
../include/plugin.h
../include/foreignplugin.h
../include/siteplugin.h
../include/video.h
../include/videoimpl.h
../include/vlyc2pointer.h
)

set(SOURCES
video.cpp
vlyc2pointer.cpp
)

add_definitions(-DLIBVLYC2_LIBRARY)

add_library(libvlyc2 SHARED ${HEADERS} ${SOURCES})
qt5_use_modules(libvlyc2 Core Network)
set_target_properties(libvlyc2 PROPERTIES OUTPUT_NAME vlyc2)
