# - Find ADCIRCMODULES
# Find the native ADCIRCModules includes and library
#
#  ADCIRCMODULES_INCLUDE_DIR  - user modifiable choice of where netcdf headers are
#  ADCIRCMODULES_LIBRARY      - user modifiable choice of where netcdf libraries are
#
#  find_package (ADCIRCModules REQUIRED)
#  target_link_libraries (uses_everthing ${ADCIRCMODULES_LIBRARIES})
#  target_link_libraries (only_uses_f90 ${ADCIRCMODULES_F90_LIBRARIES})

#search starting from user editable cache var
if (ADCIRCMODULES_INCLUDE_DIR AND ADCIRCMODULES_LIBRARY)
  # Already in cache, be silent
  set (ADCIRCMODULES_FIND_QUIETLY TRUE)
endif ()

set(USE_DEFAULT_PATHS "NO_DEFAULT_PATH")
if(ADCIRCMODULES_USE_DEFAULT_PATHS)
  set(USE_DEFAULT_PATHS "")
endif()

find_path (ADCIRCMODULES_INCLUDE_DIR adcircmodules.h
  HINTS "${ADCIRCMODULES_DIR}/include")
mark_as_advanced (ADCIRCMODULES_INCLUDE_DIR)
set (ADCIRCMODULES_C_INCLUDE_DIRS ${ADCIRCMODULES_INCLUDE_DIR})

find_library (ADCIRCMODULES_LIBRARY NAMES adcircmodules
  HINTS "${ADCIRCMODULES_DIR}/lib")
mark_as_advanced (ADCIRCMODULES_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set ADCIRCMODULES_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (ADCIRCModules
    DEFAULT_MSG ADCIRCMODULES_LIBRARY ADCIRCMODULES_INCLUDE_DIR )
