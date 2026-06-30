#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.27)
project(QCRNG)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# See the documentation for a guide on how to enable/disable specific components
#
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, use our local headers, and link it to the Geant4 libraries
#
add_executable(QCRNG QCRNG.cc ${sources} ${headers})
target_include_directories(QCRNG PRIVATE include)
target_link_libraries(QCRNG PRIVATE ${Geant4_LIBRARIES} ROOT::RIO ROOT::Tree)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(QCRNG_SCRIPTS
  init_vis.mac
  run1.mac
  vis.mac
  )

foreach(_script ${QCRNG_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/macros/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()
