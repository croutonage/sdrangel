project(mod)

find_package(OpenCV)

add_subdirectory(modam)
add_subdirectory(modnfm)
add_subdirectory(modssb)
add_subdirectory(modwfm)

if (OpenCV_FOUND)
add_subdirectory(modatv)
endif()
