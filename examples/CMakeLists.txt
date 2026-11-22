cmake_minimum_required(VERSION 3.16)
# Use the same version as the parent project.
project(QtADSExamples LANGUAGES CXX VERSION ${PROJECT_VERSION})

if (QtADS_IS_TOP_LEVEL)
	set(CMAKE_FOLDER "Examples")
endif ()

add_subdirectory(simple)
add_subdirectory(hideshow)
add_subdirectory(sidebar)
add_subdirectory(deleteonclose)
add_subdirectory(centralwidget)
add_subdirectory(autohide)
add_subdirectory(autohidedragndrop)
add_subdirectory(emptydockarea)
add_subdirectory(dockindock)
add_subdirectory(configflags)
