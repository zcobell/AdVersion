
###########################################################################
#                     CMake Build File for ADVERSION
#
#    Written By: Zach Cobell
#
###########################################################################
#
# The CMake build system enable ADVERSION to be deployed and built
# in a cross platform environment. 
#
###########################################################################
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckLibraryExists) 
INCLUDE (CheckFunctionExists)

#...Set the default build type
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
        build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
        Release RelWithDebInfo MinSizeRel.")
ELSEIF(COVERAGE)
        SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
            options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
            RelWithDebInfo MinSizeRel.")
ELSE()
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build,
        options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
        RelWithDebInfo MinSizeRel.")
ENDIF()

###########################################################################
#  GENERAL OPTIONS
###########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(ADVERSION)
###########################################################################


###########################################################################
# Enable running tests
###########################################################################
IF(UNIX OR CYGWIN)
    ENABLE_TESTING()
ENDIF(UNIX OR CYGWIN)
###########################################################################


###########################################################################
# Enable Coverage
###########################################################################
#OPTION(COVERAGE "Export Code Coverage report from tests" OFF)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
IF(COVERAGE)
    IF(CMAKE_COMPILER_IS_GNUCXX) 
        INCLUDE(CodeCoverage)
        setup_target_for_coverage(adversion_coverage tests coverage)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(COVERAGE)
###########################################################################


###########################################################################
# C++ 11 Check
###########################################################################
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
IF(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE(COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF(COMPILER_SUPPORTS_CXX11)
###########################################################################


###########################################################################
#  Compiler flags 
###########################################################################
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
MARK_AS_ADVANCED( CLEAR CMAKE_CXX_FLAGS_RELEASE )
MARK_AS_ADVANCED( CLEAR CMAKE_CXX_FLAGS_DEBUG )
MARK_AS_ADVANCED( CLEAR CMAKE_C_FLAGS_RELEASE )
MARK_AS_ADVANCED( CLEAR CMAKE_C_FLAGS_DEBUG )
MARK_AS_ADVANCED( CLEAR CMAKE_CXX_COMPILER )
MARK_AS_ADVANCED( CLEAR CMAKE_C_COMPILER )
###########################################################################

###########################################################################
#  LIBRARY VERSION
###########################################################################
set(ADVERSION_VERSION_MAJOR 0)
set(ADVERSION_VERSION_MINOR 3)
set(ADVERSION_VERSION_PATCH 0)
set(ADVERSION_VERSION_STRING ${ADVERSION_VERSION_MAJOR}.${ADVERSION_VERSION_MINOR}.${ADVERSION_VERSION_PATCH})
###########################################################################

###########################################################################
# CODE VERSION (GIT)
###########################################################################
EXECUTE_PROCESS( COMMAND git describe --always --tags
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_VERSION
                    RESULT_VARIABLE GIT_RETURN_VALUE
                    OUTPUT_STRIP_TRAILING_WHITESPACE )
IF( NOT "${GIT_RETURN_VALUE}" STREQUAL "0" )
    SET(GIT_VERSION "${ADVERSION_VERSION_STRING}.cv")
ENDIF()
MESSAGE(STATUS "AdVersion Version: ${GIT_VERSION}")
###########################################################################


###########################################################################
# ABSEIL SWISS TABLES
###########################################################################
IF(NOT CYGWIN)
    OPTION(USE_GOOGLE_FLAT_MAP "Use Google's hash table to increase speed" ON)
    IF(USE_GOOGLE_FLAT_MAP)
        if(MSVC)
          # /wd4005  macro-redefinition
          # /wd4068  unknown pragma
          # /wd4244  conversion from 'type1' to 'type2'
          # /wd4267  conversion from 'size_t' to 'type2'
          # /wd4800  force value to bool 'true' or 'false' (performance warning)
          add_compile_options(/wd4005 /wd4068 /wd4244 /wd4267 /wd4800)
          add_definitions(/DNOMINMAX /DWIN32_LEAN_AND_MEAN=1 /D_CRT_SECURE_NO_WARNINGS)
        endif(MSVC)
        add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/abseil EXCLUDE_FROM_ALL)
        MESSAGE(STATUS "Using Google's Flat Map (from ABSL) instead of unordered_map for performance")
    ENDIF(USE_GOOGLE_FLAT_MAP)    
ELSE(NOT CYGWIN)
    SET(USE_GOOGLE_FLAT_MAP FALSE)
ENDIF(NOT CYGWIN)
###########################################################################

###########################################################################
# Boost
###########################################################################
FIND_PACKAGE( Boost 1.60.0 COMPONENTS filesystem REQUIRED )
###########################################################################

###########################################################################
# TESTING 
###########################################################################
OPTION(BUILD_TESTS "Build test cases" OFF)
###########################################################################

###########################################################################
#  SET THE LOCATION OF TEMPORARY STATIC LIBS
###########################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles)
###########################################################################

###########################################################################
#  NETCDF
###########################################################################
#IF(WIN32)
#    SET(NETCDF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/netcdf/include" ) 
#ELSE(WIN32)
#    IF(NOT "${NETCDFHOME}" STREQUAL "")
#        SET(NETCDF_DIR "${NETCDFHOME}")
#    ELSEIF(NOT $ENV{NETCDFHOME} STREQUAL "")
#        SET(NETCDF_DIR $ENV{NETCDFHOME})
#    ENDIF(NOT "${NETCDFHOME}" STREQUAL "")
#
#    FIND_PACKAGE(NetCDF)
#
#    SET(NETCDF_AdditionalLibs "" CACHE STRING "Additional libraries that may be required for netCDF")
#
#    IF(NOT NETCDF_FOUND)
#        MESSAGE(SEND_ERROR "Specify the netCDF path on the following screen")
#    ELSE(NOT NETCDF_FOUND)
#        
#        SET(netcdf_c_code
#    "
#        #include <netcdf.h>
#        int main(){
#            int ncid,varid;
#            int ierr = nc_def_var_deflate(ncid,varid,1,2,2);
#            return 0;
#        }
#    "
#        )
#        FILE(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/netcdf_c_check.c" "${netcdf_c_code}")
#        TRY_COMPILE(NC_DEFLATE_FOUND "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/netcdf_c_check.c" CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${NETCDF_INCLUDE_DIRS}" LINK_LIBRARIES "${NETCDF_LIBRARIES}" LINK_LIBRARIES "${NETCDF_AdditionalLibs}" OUTPUT_VARIABLE LOG1)
#            
#        IF(NC_DEFLATE_FOUND)
#            SET(NETCDF_LINKER_FLAG "${NETCDF_LIBRARIES}")
#        ELSE(NC_DEFLATE_FOUND)
#            MESSAGE(SEND_ERROR "The netCDF library is not functional.")
#        ENDIF(NC_DEFLATE_FOUND)
#    ENDIF(NOT NETCDF_FOUND)
#ENDIF(WIN32)
###########################################################################

SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

###########################################################################
# OpenSSL
###########################################################################
IF(NOT "${OPENSSLHOME}" STREQUAL "")
    SET(OPENSSL_ROOT_DIR "${OPENSSLHOME}" CACHE STRING "OpenSSL Directory")
ELSEIF(NOT $ENV{OPENSSLHOME} STREQUAL "")
    SET(OPENSSL_ROOT_DIR $ENV{OPENSSLHOME} CACHE STRING "OpenSSL Directory")
ENDIF(NOT "${OPENSSLHOME}" STREQUAL "")
###########################################################################

###########################################################################
# libgit
###########################################################################
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/thirdparty/libgit2 EXCLUDE_FROM_ALL )
###########################################################################

###########################################################################
# metis 
###########################################################################
SET( GKLIB_PATH ${CMAKE_SOURCE_DIR}/thirdparty/metis-5.1.0/GKlib CACHE STRING "GKLIB Location" )
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/thirdparty/metis-5.1.0 EXCLUDE_FROM_ALL )
###########################################################################

###########################################################################
# ADCIRCModules
###########################################################################
IF(NOT "${ADCIRCMODULESHOME}" STREQUAL "")
    SET(ADCIRCMODULES_DIR "${ADCIRCMODULESHOME}" CACHE STRING "ADCIRCModules Home Directory")
ELSEIF(NOT $ENV{ADCIRCMODULESHOME} STREQUAL "")
    SET(ADCIRCMODULES_DIR $ENV{ADCIRCMODULESHOME} CACHE STRING "ADCIRCModules Home Directory")
ENDIF(NOT "${ADCIRCMODULESHOME}" STREQUAL "")
Find_Package(ADCIRCModules)
###########################################################################

###########################################################################
#  adversion    
###########################################################################
ADD_LIBRARY( adversion SHARED ${CMAKE_SOURCE_DIR}/libraries/libadversion/adversion.cpp 
                              ${CMAKE_SOURCE_DIR}/libraries/libadversion/adversionimpl.cpp
                              ${CMAKE_SOURCE_DIR}/libraries/libadversion/partition.cpp 
                              ${CMAKE_SOURCE_DIR}/libraries/libadversion/rectangle.cpp 
                              ${CMAKE_SOURCE_DIR}/libraries/libadversion/boostio.cpp ) 
IF(WIN32)
    INSTALL(TARGETS adversion LIBRARY DESTINATION bin RUNTIME DESTINATION lib PUBLIC_HEADER DESTINATION ${HEADER_DEST})
ELSEIF(CYGWIN)
    INSTALL(TARGETS adversion LIBRARY DESTINATION lib RUNTIME DESTINATION lib PUBLIC_HEADER DESTINATION ${HEADER_DEST})
ELSE(WIN32)
    INSTALL( TARGETS adversion LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION ${HEADER_DEST})
ENDIF(WIN32)

TARGET_INCLUDE_DIRECTORIES( adversion PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/libgit2/include 
                                              ${CMAKE_SOURCE_DIR}/thirdparty/metis-5.1.0/include 
                                              ${ADCIRCMODULES_INCLUDE_DIR}
                                              ${Boost_INCLUDE_DIRS} ${NETCDF_INCLUDE_DIRS} )
TARGET_LINK_LIBRARIES( adversion git2 metis ${Boost_FILESYSTEM_LIBRARY}  ${ADCIRCMODULES_LIBRARY} )

#IF(WIN32)
#    LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/thirdparty/netcdf/libs_vc64 )
#    TARGET_LINK_LIBRARIES( adversion netcdf hdf5 hdf5_hl )
#ELSE(WIN32)
#    TARGET_LINK_LIBRARIES( adversion ${NETCDF_LIBRARIES} )
#ENDIF(WIN32)

IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 0)
    SET_TARGET_PROPERTIES(adversion PROPERTIES INSTALL_NAME_DIR "adversion")
    SET_TARGET_PROPERTIES(adversion PROPERTIES MACOSX_RPATH "adversion")
ENDIF(APPLE)

IF(USE_GOOGLE_FLAT_MAP)
    TARGET_LINK_LIBRARIES(adversion absl::base absl::container absl::hash)
    ADD_DEPENDENCIES(adversion absl::base absl::container absl::hash)
    TARGET_COMPILE_DEFINITIONS( adversion PRIVATE USE_GOOGLE_FLAT_MAP )
ENDIF(USE_GOOGLE_FLAT_MAP)
###########################################################################

###########################################################################
# Test Case
###########################################################################
ADD_EXECUTABLE( testCase ${CMAKE_SOURCE_DIR}/testing/main.cpp )
TARGET_INCLUDE_DIRECTORIES(testCase PRIVATE ${CMAKE_SOURCE_DIR}/libraries/libadversion)
TARGET_LINK_LIBRARIES(testCase adversion)
###########################################################################
