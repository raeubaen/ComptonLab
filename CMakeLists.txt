#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.27)
project(CrilinSim)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Find ROOT package
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})

# Setup include directories for this project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIRS}
                    ${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# Note: headers are included to show up in IDEs, but sources should be listed explicitly
set(SOURCES
    src/CrilinSim.cc
    src/CrystalHit.cc
    src/CaloGeometry.cc
    src/CrystalSD.cc
    src/ScintillatingTracker.cc
    src/Tagger.cc
    src/EventAction.cc
    src/SteppingAction.cc
    src/DetectorConstruction.cc
    src/PrimaryGeneratorAction.cc
    src/PrimaryGeneratorMessenger.cc
    src/RunAction.cc
)

set(HEADERS
    include/CrystalHit.hh
    include/CaloGeometry.hh
    include/CrystalSD.hh
    include/ScintillatingTracker.hh
    include/Tagger.hh
    include/EventAction.hh
    include/SteppingAction.hh
    include/DetectorConstruction.hh
    include/PrimaryGeneratorAction.hh
    include/PrimaryGeneratorMessenger.hh
    include/RunAction.hh
)

#----------------------------------------------------------------------------
# Add the executable and link it to the Geant4 and ROOT libraries
add_executable(CrilinSim ${SOURCES} ${HEADERS})
target_link_libraries(CrilinSim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory
set(CrilinSim_SCRIPTS
  macro/init_vis.mac
#  vis.mac
  macro/run.mac
)

foreach(_script ${CrilinSim_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
install(TARGETS CrilinSim DESTINATION bin)

#----------------------------------------------------------------------------
# Additional settings for ROOT
# Ensure ROOT libraries are correctly linked
include(${ROOT_USE_FILE})
