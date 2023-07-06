cmake_minimum_required(VERSION 3.1)
project(HPC2020 VERSION 1.0)
option(WITH_MPI "Use MPI" ON)
option(WITH_DOXYGEN "Use Doxygen" OFF)

if(WITH_MPI)
    find_package(MPI COMPONENTS CXX)
    if(MPI_FOUND)
        set(CMAKE_CXX_COMPILER mpic++)
    else()
        message("-- MPI not found, using default C++ compiler")
        option(WITH_MPI OFF)
    endif(MPI_FOUND)
else()
    message("-- MPI disabled, using default C++ compiler")
endif(WITH_MPI)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMPICH_IGNORE_CXX_SEEK -DMPICH_SKIP_MPICXX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMPICH_IGNORE_CXX_SEEK -DMPICH_SKIP_MPICXX")

configure_file(${PROJECT_SOURCE_DIR}/include/HPC2020Config.h.in HPC2020Config.h)
set(SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
list(APPEND EXTRA_LIBS
    Helper
    IO
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
if(WITH_DOXYGEN)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("-- Doxygen build started")
        add_custom_target( docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else(DOXYGEN_FOUND)
        message("-- Doxygen need to be installed to generate the doxygen documentation")
    endif(DOXYGEN_FOUND)
else(WITH_DOXYGEN)
    message("-- Doxygen disabled.")
endif(WITH_DOXYGEN)