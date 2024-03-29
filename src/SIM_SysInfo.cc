#include "SIM_SysInfo.hh"

#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Object.h>
#include <UT/UT_DSOVersion.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include <HOM/HOM_Module.h>

#include <SIM/SIM_MatrixField.h>
#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>

#include <SIM/SIM_Geometry.h>

namespace HDK_notawhale {

void initializeSIM(void *) { IMPLEMENT_DATAFACTORY(SIM_SysInfo); }

SIM_SysInfo::SIM_SysInfo(const SIM_DataFactory *factory)
    : BaseClass(factory), SIM_OptionsUser(this) {}

SIM_SysInfo::~SIM_SysInfo() {}

const SIM_DopDescription *SIM_SysInfo::getSysSimDescription() {
  static PRM_Name theBatchMode(BATCHMODEONLY, "Work Only In Batch Mode");
  static PRM_Name theShowClock(SHOWCLOCK, "Clock");

  static PRM_Name theRam("ram", "Ram");
  static PRM_Default theRamSwitcher(2, "Ram");

  static PRM_Name theShowMemory(SHOWMEMORY, "Memory");
  static PRM_Name theShowSwap(SHOWSWAP, "Swap");

  static PRM_Name theObjectMask(OBJECTMASK, "Object Mask");
  static PRM_Default theObjectMaskDefault(0, "*");

  static PRM_Name PRMMode(MODE, "Mode");
  static PRM_Name modeList[] = {PRM_Name("none", "None"), PRM_Name("field", "Field"),
                                PRM_Name("bullet", "Bullet"), PRM_Name(0)};

  static PRM_ChoiceList modeMenu(PRM_CHOICELIST_SINGLE, modeList);

  static PRM_Name theField(FIELD, "Field");
  static PRM_Default theFieldDefault(0, "density");

  static PRM_Template theTemplates[] = {
      PRM_Template(PRM_TOGGLE, 1, &theBatchMode, PRMoneDefaults),
      PRM_Template(PRM_TOGGLE, 1, &theShowClock, PRMoneDefaults),
      PRM_Template(PRM_SWITCHER, 1, &theRam, &theRamSwitcher, 0, 0, 0,
                   &PRM_SpareData::groupTypeSimple),
      PRM_Template(PRM_TOGGLE, 1, &theShowMemory, PRMoneDefaults),
      PRM_Template(PRM_TOGGLE, 1, &theShowSwap, PRMoneDefaults),

      PRM_Template(PRM_STRING, 1, &theObjectMask, &theObjectMaskDefault),
      PRM_Template(PRM_ORD, 1, &PRMMode, 0, &modeMenu),
      PRM_Template(PRM_STRING, 1, &theField, &theFieldDefault),
      PRM_Template()};

  static SIM_DopDescription theDopDescription(true, "notawhale_hdk_sysinfo", "System Info",
                                              "SystemInfo", classname(), theTemplates);

  return &theDopDescription;
}
void SIM_SysInfo::frameInfo(const SIM_Engine &eng) {
  SIM_Time t = eng.getSimulationTime();
  // eng.getSimulationFrame(t)
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "Frame: " << eng.getGlobalTime(t) / eng.getTimeStep() << "\n";
}

void SIM_SysInfo::timePerFrame(const bool &obj_new) {
  auto now = std::chrono::system_clock::now();
  auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
  auto value = now_s.time_since_epoch();
  long cur = value.count();

  if (obj_new)
    setClock(cur);

  long sec = cur - getClock();
  long min = sec / 60;
  std::cout << "Time Per Frame: " << min << " min " << sec - min * 60 << " sec"
            << "\n";
  setClock(cur);
}

fpreal SIM_SysInfo::toGb(long val) { return static_cast<fpreal>(val) / (1024 * 1024); }

void SIM_SysInfo::coutMemory(const std::string &mes, const long &tl, const long &fr) {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << mes << "\t" << toGb(tl - fr) << " / " << toGb(tl) << " Gb"
            << "\n";
}

void SIM_SysInfo::memoryInfo() {
  long mem_total, mem_ava, swap_total, swap_ava;
  std::string token;
  std::ifstream file("/proc/meminfo");
  while (file >> token) {
    if (token == "MemTotal:") {
      file >> mem_total;
    } else if (token == "MemAvailable:") {
      file >> mem_ava;
    }
    if (token == "SwapTotal:") {
      file >> swap_total;
    } else if (token == "SwapFree:") {
      file >> swap_ava;
    }
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  file.close();

  if (getShowMemory())
    coutMemory("Ram:", mem_total, mem_ava);
  if (getShowSwap())
    coutMemory("Swap:", swap_total, swap_ava);
}

template <typename T> void SIM_SysInfo::printFieldInfo(const T &field) const {
  std::cout << getField() << "\n";
  std::cout << "vs: " << field->getVoxelSize()[0] << " ";
  std::cout << "div: " << std::setprecision(0) << field->getDivisions() << " ";
  std::cout << "nv: " << field->getTotalVoxels() << "\n";
}

void SIM_SysInfo::fieldsInfo(SIM_Object &obj) const {
  SIM_ScalarField *sc_field = SIM_DATA_GET(obj, getField(), SIM_ScalarField);
  SIM_VectorField *vec_field = SIM_DATA_GET(obj, getField(), SIM_VectorField);
  SIM_MatrixField *mx_field = SIM_DATA_GET(obj, getField(), SIM_MatrixField);
  if (sc_field) {
    printFieldInfo<>(sc_field);
  } else if (vec_field) {
    printFieldInfo<>(vec_field);
  } else if (mx_field) {
    printFieldInfo<>(mx_field);
  }
}

void SIM_SysInfo::bulletInfo(SIM_Object &obj) const {
  const SIM_Geometry *geo = obj.getGeometry();
  if (geo) {
    GU_DetailHandleAutoReadLock geo_gdp(geo->getGeometry());
    const GU_Detail *gdp = geo_gdp.getGdp();
    GA_ROHandleI active_attr(gdp, GA_ATTRIB_POINT, "active");
    GA_ROHandleI blt_slp_attr(gdp, GA_ATTRIB_POINT, "bullet_sleeping");

    std::cout << "Pieces: " << gdp->getNumPoints();

    if (active_attr.isValid() && blt_slp_attr.isValid()) {
      int num_act = 0, blt_slp = 0;
      for (GA_Iterator it(gdp->getPointRange()); !it.atEnd(); it.advance()) {
        GA_Offset offset = it.getOffset();
        num_act += active_attr.get(offset);
        blt_slp += blt_slp_attr.get(offset);
      }
      std::cout << " Active: " << num_act;
      std::cout << " Sleeping: " << blt_slp;
    }
    std::cout << "\n";
  }
}

SIM_Solver::SIM_Result SIM_SysInfo::solveSingleObjectSubclass(SIM_Engine &engine,
                                                              SIM_Object &object,
                                                              SIM_ObjectArray &feedback_to_objects,
                                                              const SIM_Time &time_step,
                                                              bool object_is_new) {

  return SIM_SOLVER_SUCCESS;
}

SIM_Solver::SIM_Result SIM_SysInfo::solveObjectsSubclass(SIM_Engine &engine,
                                                         SIM_ObjectArray &objects,
                                                         SIM_ObjectArray &newobjects,
                                                         SIM_ObjectArray &feedbacktoobjects,
                                                         const SIM_Time &timestep) {

  if (getbatchMode()) {
    if (HOM().isUIAvailable())
      return SIM_SOLVER_SUCCESS;
  }

  frameInfo(engine);
  if (getShowClock())
    timePerFrame(engine.getSimulationTime() == fpreal(0));
  if (getShowMemory() && getShowSwap())
    memoryInfo();
  std::cout << "\n";

  SIM_DataFilterRootData fil(getObjectMask());
  SIM_ObjectArray obj_arr;
  auto filtered_obj = [](const SIM_DataFilterRootData &filter, const SIM_ObjectArray &objs) {
    SIM_ObjectArray temp_objs;
    objs.filter(filter, temp_objs);
    return temp_objs;
  };

  obj_arr.merge(filtered_obj(fil, objects));
  obj_arr.merge(filtered_obj(fil, newobjects));

  for (int i = 0; i < obj_arr.entries(); i++) {
    SIM_Object &object = *obj_arr(i);
    std::cout << object.getName() << "\n";
    switch (getDataMode()) {
    case 1:
      fieldsInfo(object);
      break;
    case 2:
      bulletInfo(object);
      break;
    }
  }

  std::cout << "\n";
  return SIM_SOLVER_SUCCESS;
}

} // namespace HDK_notawhale