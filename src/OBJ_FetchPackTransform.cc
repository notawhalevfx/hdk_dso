#include <OP/OP_OperatorTable.h>
#include <UT/UT_DSOVersion.h>

#include <GU/GU_Detail.h>
#include <GU/GU_DetailHandle.h>
#include <OP/OP_Director.h>
#include <OP/OP_Error.h>
#include <PRM/PRM_SpareData.h>
#include <SOP/SOP_Node.h>
#include <UT/UT_StringStream.h>

#include "OBJ_FetchPackTransform.hh"

#define PATH_ATTR "path"

// using namespace HDK_notawhale;

namespace HDK_notawhale {

// Constructor for new object class
OBJ_FetchPackTransform::OBJ_FetchPackTransform(OP_Network *net, const char *name, OP_Operator *op)
    : OBJ_Geometry(net, name, op) {
  // initialize local member data here.
}

// virtual destructor for new object class
OBJ_FetchPackTransform::~OBJ_FetchPackTransform() {}

void OBJ_FetchPackTransform::buildSelectPathMenu(void *data, PRM_Name *theMenu, int theMaxSize,
                                                 const PRM_SpareData *, const PRM_Parm *) {
  OBJ_FetchPackTransform *me = (OBJ_FetchPackTransform *)data;
  UT_String path;
  me->SOPPATH(path, 0.0F);
  SOP_Node *sop_node = OPgetDirector()->getCwd()->findSOPNode(path);

  // Check if path exist
  if (sop_node == nullptr) {
    theMenu[0].setToken(0);
    return;
  }

  const GU_Detail *gdp = sop_node->getLastGeo();
  // Check if we have a valid detail handle.
  const GA_Attribute *attrib = gdp->findStringTuple(GA_ATTRIB_PRIMITIVE, PATH_ATTR);
  if (!attrib) {
    theMenu[0].setToken(0);
    return;
  }
  const GA_AIFSharedStringTuple *stuple = attrib->getAIFSharedStringTuple();
  if (!stuple) {
    theMenu[0].setToken(0);
    return;
  }
  UT_StringArray strings;
  UT_IntArray indices;
  stuple->extractStrings(attrib, strings, indices);

  for (int i = 0; i < strings.size() && i < theMaxSize; i++) {
    theMenu[i].setToken(strings[i].c_str());
    theMenu[i].setLabel(strings[i].c_str());
  }
  theMenu[strings.size()].setToken(0); // Need a null terminater
}

int OBJ_FetchPackTransform::buildPickPathMenu(void *data, int index, fpreal t,
                                              const PRM_Template *tplate) {

  OBJ_FetchPackTransform *me = (OBJ_FetchPackTransform *)data;
  UT_String path;
  me->SOPPATH(path, t);
  SOP_Node *sop_node = OPgetDirector()->getCwd()->findSOPNode(path);

  // Check if path exist
  if (sop_node == nullptr) {
    return 1;
  }

  const GU_Detail *gdp = sop_node->getLastGeo();
  // Check if we have a valid detail handle.
  const GA_Attribute *attrib = gdp->findStringTuple(GA_ATTRIB_PRIMITIVE, PATH_ATTR);
  if (!attrib) {
    return 1;
  }
  const GA_AIFSharedStringTuple *stuple = attrib->getAIFSharedStringTuple();
  if (!stuple) {
    return 1;
  }

  UT_StringArray strings;
  UT_IntArray indices;
  stuple->extractStrings(attrib, strings, indices);

  UT_WorkBuffer cmd;
  cmd.strcpy("treechooser");

  UT_String selectedValue;
  me->ATTRIBPATH(selectedValue, t);
  if (selectedValue.isstring()) {
    cmd.strcat(" -s ");
    cmd.protectedStrcat(selectedValue);
  }
  for (const auto &str : strings) {
    cmd.strcat(" ");
    cmd.protectedStrcat(str.c_str());
  }

  CMD_Manager *mgr = CMDgetManager();
  UT_OStringStream os;
  mgr->execute(cmd.buffer(), 0, &os);
  UT_String result(os.str().buffer());
  result.trimBoundingSpace();
  me->setChRefString(result, CH_STRING_LITERAL, "selectPath", 0, t);

  return 0;
}

static PRM_Name pathToGeometryName("pathToGeometry", "Path To Geometry");
static PRM_Name selectPathName("selectPath", "Select Path");
static PRM_Default selectShapeDefault(0, "");
static PRM_Name pickPathName("pickPath", "");
static PRM_SpareData theTreeButtonSpareData(PRM_SpareArgs()
                                            << PRM_SpareToken(PRM_SpareData::getButtonIconToken(),
                                                              "BUTTONS_tree"));

static PRM_ChoiceList choicePathMenu(PRM_CHOICELIST_REPLACE,
                                     &OBJ_FetchPackTransform::buildSelectPathMenu);

// this is the template list that defines the new parameters that are
// used by the worldalign object. In this particular case the list is empty
// because we have no new parameters.
static PRM_Template templatelist[] = {
    // blank terminating Template.
    PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &pathToGeometryName),
    PRM_Template(PRM_STRING, PRM_TYPE_JOIN_PAIR, 1, &selectPathName, &selectShapeDefault,
                 &choicePathMenu, NULL, PRM_Callback()),
    PRM_Template(PRM_CALLBACK, PRM_TYPE_NO_LABEL, 1, &pickPathName, 0, 0, 0,
                 &OBJ_FetchPackTransform::buildPickPathMenu, &theTreeButtonSpareData),
    PRM_Template()};

// this function copies the original parameter and then adds an invisible flag
static void copyParmWithInvisible(PRM_Template &src, PRM_Template &dest) {
  PRM_Name *new_name;

  new_name = new PRM_Name(src.getToken(), src.getLabel(), src.getExpressionFlag());
  new_name->harden();
  dest.initialize((PRM_Type)(src.getType() | PRM_TYPE_INVISIBLE), src.getTypeExtended(),
                  src.exportLevel(), src.getVectorSize(), new_name, src.getFactoryDefaults(),
                  src.getChoiceListPtr(), src.getRangePtr(), src.getCallback(), src.getSparePtr(),
                  src.getParmGroup(), (const char *)src.getHelpText(), src.getConditionalBasePtr());
}

// this function returns the OP_TemplatePair that combines the parameters
// of this object with those of its ancestors in the (object class hierarchy)
OP_TemplatePair *OBJ_FetchPackTransform::buildTemplatePair(OP_TemplatePair *prevstuff) {
  OP_TemplatePair *align, *geo;

  // The parm templates here are not created as a static list because
  // if that static list was built before the OBJbaseTemplate static list
  // (which it references) then that list would be corrupt. Thus we have
  // to force our static list to be created after OBJbaseTemplate.
  static PRM_Template *theTemplate = 0;

  if (!theTemplate) {
    PRM_Template *obj_template;
    int i;
    int size;
    UT_String parm_name;

    obj_template = OBJ_Geometry::getTemplateList(OBJ_PARMS_PLAIN);
    size = PRM_Template::countTemplates(obj_template);
    theTemplate = new PRM_Template[size + 1]; // add +1 for sentinel
    for (i = 0; i < size; i++) {
      theTemplate[i] = obj_template[i];
      theTemplate[i].getToken(parm_name);

      copyParmWithInvisible(obj_template[i], theTemplate[i]);
    }
  }

  // Here, we have to "inherit" template pairs from geometry and beyond. To
  // do this, we first need to instantiate our template list, then add the
  // base class templates.
  align = new OP_TemplatePair(templatelist, prevstuff);
  geo = new OP_TemplatePair(theTemplate, align);

  return geo;
}

// the myConstructor method is used to create new objects of the correct
// type from the OperatorTable. This method is passed to the OP_Operator
// constructor and then is invoked whenever a new node needs to be created.
OP_Node *OBJ_FetchPackTransform::myConstructor(OP_Network *net, const char *name, OP_Operator *op) {
  return new OBJ_FetchPackTransform(net, name, op);
}

OP_ERROR OBJ_FetchPackTransform::cookMyObj(OP_Context &context) {
  OP_ERROR errorstatus;
  UT_DMatrix4 packedXform;
  UT_Vector3R pivot;

  const fpreal now = context.getTime();

  // OBJ_Geometry::cookMyObj computes the local and global transform, and
  // dirties the inverse of the global transform matrix. These are stored
  // in myXform, myWorldXform, and myIWorldXform, respectively.
  errorstatus = OBJ_Geometry::cookMyObj(context);

  // Get path
  UT_String path;
  SOPPATH(path, now);
  SOP_Node *sop_node = OPgetDirector()->getCwd()->findSOPNode(path);

  // Check if path exist
  if (sop_node == nullptr) {
    addError(OBJ_Error::OBJ_ERR_INVALID_PATH);
    return errorstatus;
  }

  // Make node dependet from other node
  addExtraInput(sop_node, OP_INTEREST_DATA);

  GU_DetailHandle gd_handle = sop_node->getCookedGeoHandle(context);
  // Check if we have a valid detail handle.
  if (!gd_handle.isNull()) {
    // Finally, get at the actual GU_Detail.
    const GU_Detail *gdp = gd_handle.gdp();
    const GA_Attribute *attrib = gdp->findStringTuple(GA_ATTRIB_PRIMITIVE, PATH_ATTR);
    if (!attrib) {
      addError(OBJ_ERR_NO_ATTRIBUTE, PATH_ATTR);
      return errorstatus;
    }

    GA_RWHandleS attrib_h(gdp, GA_ATTRIB_PRIMITIVE, PATH_ATTR);
    if (!attrib_h.isValid()) {
      addError(OBJ_ERR_NO_ATTRIBUTE, PATH_ATTR);
      return errorstatus;
    }

    UT_String attrib_path;
    ATTRIBPATH(attrib_path, now);
    int index = -1;
    for (GA_Iterator it(gdp->getPrimitiveRange()); !it.atEnd(); ++it) {
      GA_Offset offset = *it;
      const char *string_value = attrib_h.get(offset);
      if (attrib_path.compare(string_value) == 0) {
        index = attrib_h.getIndex(offset);
        break;
      }
    }

    if (index == -1) {
      opError(OP_ERR_ANYTHING, "Can't find selected shape in geometry.");
      return errorstatus;
    }

    if (!getIntrinsicValue(gdp, "packedfulltransform", index, packedXform))
      return errorstatus;

    if (!getIntrinsicValue(gdp, "pivot", index, pivot))
      return errorstatus;
  }

  // we need to move transform to centroid
  packedXform.pretranslate(pivot);

  // set result
  setWorldXform(packedXform);
  setLocalXform(packedXform);

  return errorstatus;
}

} // namespace HDK_notawhale

// this function installs the new object in houdini's object table.
// It is automatically called by Houdini when this dynamic library is loaded.
void newObjectOperator(OP_OperatorTable *table) {
  OP_Operator *op = new OP_Operator(
      "hdk_fetch_pack_transform",                                  // operator unique name
      "Fetch Pack Transform",                                      // operator label
      HDK_notawhale::OBJ_FetchPackTransform::myConstructor,        // node instance constructor
      HDK_notawhale::OBJ_FetchPackTransform::buildTemplatePair(0), // parameters
      HDK_notawhale::OBJ_FetchPackTransform::theChildTableName,
      0,  // minimum inputs
      0,  // maximum inputs
      0); // variables

  op->setOpTabSubMenuPath("notawhale");
  op->setIconName("OBJ_fetch");
  table->addOperator(op);
}
