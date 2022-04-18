#pragma once

#include <OBJ/OBJ_Geometry.h>

namespace HDK_notawhale {
class OBJ_FetchPackTransform : public OBJ_Geometry {
public:
  // Standard constructor and destructor.
  OBJ_FetchPackTransform(OP_Network *net, const char *name, OP_Operator *op);
  ~OBJ_FetchPackTransform() override;

  // Instantiates a new node of the type corresponding to this operator.
  static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *entry);

  // Constructs a list of the parameters for this operator.
  static OP_TemplatePair *buildTemplatePair(OP_TemplatePair *prevstuff);

  static void buildSelectPathMenu(void *data, PRM_Name *theMenu, int theMaxSize,
                                  const PRM_SpareData *, const PRM_Parm *);
  static int buildPickPathMenu(void *data, int index, fpreal t, const PRM_Template *tplate);

protected:
  // Performs the calculation of the local and the world transformation.
  OP_ERROR cookMyObj(OP_Context &context) override;

private:
  void SOPPATH(UT_String &str, fpreal t) { evalString(str, "pathToGeometry", 0, t); }
  void ATTRIBPATH(UT_String &str, fpreal t) { evalString(str, "selectPath", 0, t); }

  // Reuseble templalete to get intrinsic attribute from primitive
  template <typename Value>
  bool getIntrinsicValue(const GU_Detail *gdp, const char *attr_name, const int &prim_index,
                         Value &val) {
    const GA_LocalIntrinsic i = gdp->getPrimitiveByIndex(prim_index)->findIntrinsic(attr_name);
    if (!GAisValidLocalIntrinsic(i)) {
      UT_String errorMessage = "Can't find ";
      errorMessage += attr_name;
      errorMessage += " intrinsic attribute.";

      opError(OP_ERR_ANYTHING, errorMessage.c_str());
      return false;
    }
    if (!gdp->getPrimitiveByIndex(prim_index)->getIntrinsic(i, val)) {
      opError(OP_ERR_ANYTHING, "Can't get values.");
      return false;
    }
    return true;
  }
};

} // namespace HDK_notawhale
