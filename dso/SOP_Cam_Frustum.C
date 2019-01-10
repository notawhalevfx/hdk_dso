#include "SOP_Cam_Frustum.h"

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>

#include <OBJ/OBJ_Node.h>
#include <SOP/SOP_Error.h>
#include <OP/OP_Director.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
//#include <limits.h>

//#include <ostream>

using namespace HDK_Sample;
using namespace std;

void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "hdk_cam_frustum",                 // Internal name
        "Camera Frustum",                     // UI name
        SOP_Cam_Frustum::myConstructor,    // How to build the SOP
        SOP_Cam_Frustum::myTemplateList,   // My parameters
        0,                          // Min # of sources
        0,                          // Max # of sources
        0,
        OP_FLAG_GENERATOR));      
}

static PRM_Name     PRMcameraPath("camPath", "Path to Camera");
static PRM_Name     PRMfarDistFromCam("farDistFromCam", "Far");
static PRM_Name     PRMnearDistFromCam("nearDistFromCam", "Near");
//				     ^^^^^^^^    ^^^^^^^^^^^^^^^
//				     internal    descriptive version

static PRM_Default camPathDefault(0,"/obj/cam1");
static PRM_Default farDistFromCamDef(10);
static PRM_Default nearDistFromCamDef(0.1);
/*
static PRM_Default	fiveDefault(5);		// Default to 5 divisions
static PRM_Default	radiiDefaults[] = {
			   PRM_Default(1),		// Outside radius
			   PRM_Default(0.3)		// Inside radius
			};*/

PRM_Template
SOP_Cam_Frustum::myTemplateList[] = {
    PRM_Template(PRM_STRING,    PRM_TYPE_DYNAMIC_PATH,1, &PRMcameraPath, &camPathDefault, 0, 0, 0, &PRM_SpareData::objCameraPath),
    PRM_Template(PRM_FLT,    1, &PRMnearDistFromCam, &nearDistFromCamDef, 0),
    PRM_Template(PRM_FLT,    1, &PRMfarDistFromCam, &farDistFromCamDef, 0),
    PRM_Template(),
};

OP_Node *
SOP_Cam_Frustum::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Cam_Frustum(net, name, op);
}

SOP_Cam_Frustum::SOP_Cam_Frustum(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op)
{
}

SOP_Cam_Frustum::~SOP_Cam_Frustum() {}

void 
SOP_Cam_Frustum::buildPoly(GU_Detail *dst, UT_Vector3RArray &pts) {
    GEO_PrimPoly *poly = GEO_PrimPoly::build(dst, pts.size(), GU_POLY_CLOSED);
    
    for (int i = 0; i < pts.size(); i++)
    {
        GA_Offset ptoff = poly->getPointOffset(i);
        dst->setPos3(ptoff,pts[i]);
    }
}

void 
SOP_Cam_Frustum::buildFrustum(GU_Detail *dst, OP_Context &context, OBJ_Node *camera_node,fpreal scale) {

    fpreal now = context.getTime();

    const fpreal resx = camera_node->evalFloat("res",0, now);
    const fpreal resy = camera_node->evalFloat("res",1, now);
    const fpreal focal = camera_node->evalFloat("focal",0, now);
    const fpreal aperture = camera_node->evalFloat("aperture",0, now);
    const fpreal aspect = camera_node->evalFloat("aspect",0, now);

    //UT_Vector3 up_right = UT_Vector3(aperture*0.5/focal,(resy*aperture)/(resx*aspect)*0.5/focal,-1);
    UT_Vector3 up_right = UT_Vector3(aperture*0.5,(resy*aperture)/(resx*aspect)*0.5,-focal);
    // scale
    up_right *=scale/focal;

    UT_Vector3 down_right = up_right * UT_Vector3(1,-1,1);
    UT_Vector3 up_left = up_right * UT_Vector3(-1,1,1);
    UT_Vector3 down_left = up_right * UT_Vector3(-1,-1,1);
    //(ch("../../cam1/resy")*ch("../../cam1/aperture")) / (ch("../../cam1/resx")*ch("../../cam1/aspect")) * 0.5
    UT_Vector3RArray pos_up = (UT_Vector3RArray) {
                        up_right,
                        up_left,
                        down_left,
                        down_right
                        
    };

    //build poly
    //buildPoly(dst,std::move(pos_up));
    buildPoly(dst,pos_up);
}

OP_ERROR
SOP_Cam_Frustum::cookMySop(OP_Context &context)
{
    fpreal now = context.getTime();

    UT_String path;
    CAMPATH(path,now);

    if (error() >= UT_ERROR_ABORT)
    {
        return error();
    }

    if (path.length()==0) {
        addError(SOP_MESSAGE,"Path to camera is empty");
        return error();
    }

    OBJ_Node *   camera_node;
    camera_node = OPgetDirector()->findOBJNode(path);
    if (!camera_node) {
        addError(SOP_MESSAGE,"Camera can't be found");
        return error();
    }

    gdp->clearAndDestroy();

    // build farest poly
    buildFrustum(gdp,context,camera_node,FARDISTFROMCAM(now));

    // build nearest poly
    buildFrustum(gdp,context,camera_node,NEARDISTFROMCAM(now));

    //reverse polygon
    GA_Primitive *prim = gdp->getPrimitive(GA_Offset(1));
    prim->reverse();


    // get parent node transform
    UT_Matrix4 loc_tranform;
    OBJ_Node *  creator = CAST_OBJNODE(getCreator());
    creator->getLocalToWorldTransform(context, loc_tranform);
    loc_tranform.invert();
    gdp->transform(loc_tranform);
    //Make node dependet from other node
    addExtraInput(creator, OP_INTEREST_DATA); 

    //Get Camera transformation
    UT_Matrix4 world_tranform;
    camera_node->getLocalToWorldTransform(context,world_tranform);
    //Make node dependet from other node
    addExtraInput(camera_node, OP_INTEREST_DATA); 

    gdp->transform(world_tranform);

    /*unlockInput(0);*/
    return error();
}
