# hdk_dso
My Houdini HDK DSO Plugins. 

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
or use just use Cmake

## SOP Frustum Camera (SOP)
Show Frustum camera

### Parameters

Name | Description
--- | ---
Path to Camera | Path to camera
Mode | Draw as **Polygons** or **Points**
Consolidate Points | Consolidate points
Near | Nearest distance from the camera.
Far |Farest distance from the camera.

## SIM Sys Info (DOP)
Print memory usage and time spent per time step into a terminal. Useful to debug simulation on a render farm.
At this moment support only **Linux**-based OS
### Parameters

Name | Description
--- | ---
Work Only In Batch Mode | Print information only when UI unavailable
Clock | Print time needed to solve step
Memory | Print memory usage information
Swap | Print swap usage information
Objects Mask | Mask for objects
Mode | None - don't print additional information<br>Field - print information about field<br>Bullet - print inforamtion about pack bullet object
Field | Name of field to print information

## FS_PipelineHelper
An example of, how can be used abstract paths to read/write files inside Houdini. After compilation move FS_PipileineHelper to .../dso/fs/ if it didn't happen automatically.

## Fetch Pack Transform (OBJ)
Fetch transformation from Alembic, USD, Pack and etc geometry on Object level.

### Parameters
Name | Description
--- | ---
Path To Geometry | Path to Geometry to get transformation.
Select Path | Primitive path attribute to get transforms.