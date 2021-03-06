project(sdrdaemonfec)

find_package(LibNANOMSG)

if (HAS_SSSE3)
    message(STATUS "SDRdaemonFEC: use SSSE3 SIMD" )
elseif (HAS_NEON)
    message(STATUS "SDRdaemonFEC: use Neon SIMD" )
else()
    message(STATUS "SDRdaemonFEC: Unsupported architecture")
    return()
endif()

set(sdrdaemonfec_SOURCES
    sdrdaemonfecbuffer.cpp
    sdrdaemonfecgui.cpp
    sdrdaemonfecinput.cpp
    sdrdaemonfecplugin.cpp
    sdrdaemonfecudphandler.cpp
)

set(sdrdaemonfec_HEADERS
    sdrdaemonfecbuffer.h
    sdrdaemonfecgui.h
    sdrdaemonfecinput.h
    sdrdaemonfecplugin.h
    sdrdaemonfecudphandler.h
)

set(sdrdaemonfec_FORMS
    sdrdaemonfecgui.ui
)

#include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

#qt4_wrap_cpp(sdrdaemonfec_HEADERS_MOC ${sdrdaemonfec_HEADERS})
qt5_wrap_ui(sdrdaemonfec_FORMS_HEADERS ${sdrdaemonfec_FORMS})

add_library(inputsdrdaemonfec SHARED
    ${sdrdaemonfec_SOURCES}
    ${sdrdaemonfec_HEADERS_MOC}
    ${sdrdaemonfec_FORMS_HEADERS}
)

if (BUILD_DEBIAN)
target_include_directories(inputsdrdaemonfec PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIBCM256CCSRC}
    ${LIBNANOMSG_INCLUDE_DIR}
)
else (BUILD_DEBIAN)
target_include_directories(inputsdrdaemonfec PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CM256CC_INCLUDE_DIR}
    ${LIBNANOMSG_INCLUDE_DIR}
)
endif (BUILD_DEBIAN)

if (BUILD_DEBIAN)
target_link_libraries(inputsdrdaemonfec
    ${QT_LIBRARIES}
    cm256cc
    ${LIBNANOMSG_LIBRARIES}
    sdrbase
)
else (BUILD_DEBIAN)
target_link_libraries(inputsdrdaemonfec
    ${QT_LIBRARIES}
    ${CM256CC_LIBRARIES}
    ${LIBNANOMSG_LIBRARIES}
    sdrbase
)
endif (BUILD_DEBIAN)

qt5_use_modules(inputsdrdaemonfec Core Widgets)

install(TARGETS inputsdrdaemonfec DESTINATION lib/plugins/samplesource)
