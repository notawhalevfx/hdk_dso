# hdk_dso
Some My Houdini HDK DSO Plugins. 

This code can be not perfect. I write it for self-education and my production purpose. If somebody has any ideas, fixes, etc contact me or push your code.

Welcome and feel free to contribute!

## Compilation
```bash
cd <houdini_path>
. houdini_setup
cd <folder_with_src>
hcustom ./SOP_Cam_Frustum.cc
...
hcustom ./SOP_SysInfo.cc
```

## SOP Frustum Camera
Show Frustum camera

### Parameters

Name | Description
--- | ---
Path to Camera | Path to camera
Mode | Draw as **Polygons** or **Points**
Consolidate Points | Consolidate points
Near | Nearest distance from the camera.
Far |Farest distance from the camera.

## SIM Sys Info
Print information in a terminal about memory usage and time spent per time step. Useful to debug simulation on a render farm.
At this moment support only **Linux**-based OS
### Parameters

Name | Description
--- | ---
Work Only In Batch Mode | Print information only when UI unavailable
Clock | Print spended time needed to solve step
Memory | Print memory usage information
Swap | Print swap usage information