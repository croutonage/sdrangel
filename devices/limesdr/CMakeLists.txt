project(limesdrdevice)

set(limesdrdevice_SOURCES
    devicelimesdr.cpp
    devicelimesdrparam.cpp
    devicelimesdrshared.cpp
)

set(limesdrdevice_HEADERS
    devicelimesdr.h
    devicelimesdrparam.h
    devicelimesdrshared.h
)

include_directories(
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIMESUITE_INCLUDE_DIR}
)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_SHARED)

add_library(limesdrdevice SHARED
    ${limesdrdevice_SOURCES}
)

target_link_libraries(limesdrdevice
    ${LIMESUITE_LIBRARY}
    sdrbase
)

install(TARGETS limesdrdevice DESTINATION lib)
