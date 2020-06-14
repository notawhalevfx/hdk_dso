# Specify the minimum required version of CMake to build the project.
cmake_minimum_required( VERSION 3.6 )
project( My_HDK_Project )
# Locate Houdini's libraries and header files.
# Registers an imported library target named 'Houdini'.
find_package( Houdini REQUIRED )
# Add a library with two source files.
set( cam_frustum SOP_Cam_Frustum )
add_library( ${cam_frustum} SHARED
    SOP_Cam_Frustum.cc
    SOP_Cam_Frustum.hh
)

set( sim_sysinfo SIM_SysInfo )
add_library( ${sim_sysinfo} SHARED
    SIM_SysInfo.cc
    SIM_SysInfo.hh
)

# Link against the Houdini libraries, and add required include directories and compile definitions.
target_link_libraries( ${cam_frustum} Houdini )
target_link_libraries( ${sim_sysinfo} Houdini )

# Configure several common target properties, such as its output directory.
houdini_configure_target( ${cam_frustum} )
houdini_configure_target( ${sim_sysinfo} )