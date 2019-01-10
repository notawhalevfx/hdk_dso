#ifndef __SOP_Cam_Frustum_h__
#define __SOP_Cam_Frustum_h__

#include <SOP/SOP_Node.h>

namespace HDK_Sample {
class SOP_Cam_Frustum : public SOP_Node
{
public:
    static OP_Node		*myConstructor(OP_Network*, const char *,
							    OP_Operator *);
    static PRM_Template		 myTemplateList[];

protected:
	     SOP_Cam_Frustum(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_Cam_Frustum();

    virtual OP_ERROR		 cookMySop(OP_Context &context);
    void buildPoly(GU_Detail *dst, UT_Vector3RArray &pts);
    void buildFrustum(GU_Detail *dst, OP_Context &context, OBJ_Node *camera_node, fpreal scale);

private:
    void    CAMPATH(UT_String &str, fpreal t) 
                        { evalString(str, "camPath", 0, t); }
    fpreal  FARDISTFROMCAM(fpreal t) { return evalFloat("farDistFromCam",0,t); }
    fpreal  NEARDISTFROMCAM(fpreal t) { return evalFloat("nearDistFromCam",0,t); }
};
}

#endif
