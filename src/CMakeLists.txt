# Locate Houdini's libraries and header files.
# Registers an imported library target named 'Houdini'.
list( APPEND CMAKE_PREFIX_PATH "$ENV{HFS}/toolkit/cmake" )

find_package( Houdini REQUIRED )
houdini_get_default_install_dir( install_path )
string( APPEND install_path "/dso/fs/" )

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

set ( fs_pipelinehelper FS_PipelineHelper )
add_library( ${fs_pipelinehelper} SHARED
    FS_PipelineHelper.cc
    FS_PipelineHelper.hh
)

set ( obj_fetchpacktransform OBJ_FetchPackTransform )
add_library( ${obj_fetchpacktransform} SHARED
    OBJ_FetchPackTransform.cc
    OBJ_FetchPackTransform.hh
)

# Link against the Houdini libraries, and add required include directories and compile definitions.
target_link_libraries( ${cam_frustum} Houdini )
target_link_libraries( ${sim_sysinfo} Houdini )
target_link_libraries( ${fs_pipelinehelper} Houdini )
target_link_libraries( ${obj_fetchpacktransform} Houdini )

# Configure several common target properties, such as its output directory.
houdini_configure_target( ${cam_frustum} )
houdini_configure_target( ${sim_sysinfo} )
houdini_configure_target( ${fs_pipelinehelper} ${install_path} )
houdini_configure_target( ${obj_fetchpacktransform})
houdini_configure_target( ${sop_vdb_deform_by_path} )
