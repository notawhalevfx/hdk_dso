#ifndef __SIM_SYSINFO_HH__
#define __SIM_SYSINFO_HH__

#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_SingleSolver.h>

#include <string>

#define BATCHMODEONLY "batchMode"
#define SHOWMEMORY "printMemory"
#define SHOWSWAP "printSwap"
#define SHOWCLOCK "printClock"
#define CLOCK "clock"

#define OBJECTMASK "objectmask"
#define MODE "mode"
#define FIELD "field"

namespace HDK_notawhale {

class SIM_SysInfo : public SIM_SingleSolver, public SIM_OptionsUser {
public:
  GETSET_DATA_FUNCS_B(BATCHMODEONLY, batchMode);
  GETSET_DATA_FUNCS_B(SHOWMEMORY, ShowMemory);
  GETSET_DATA_FUNCS_B(SHOWSWAP, ShowSwap);
  GETSET_DATA_FUNCS_B(SHOWCLOCK, ShowClock);
  GETSET_DATA_FUNCS_I(CLOCK, Clock);
  GETSET_DATA_FUNCS_S(OBJECTMASK, ObjectMask);
  GETSET_DATA_FUNCS_I(MODE, DataMode);
  GETSET_DATA_FUNCS_S(FIELD, Field);

protected:
  explicit SIM_SysInfo(const SIM_DataFactory *factory);
  virtual ~SIM_SysInfo();
  // This implements your own solver step
  SIM_Result solveObjectsSubclass(SIM_Engine &engine, SIM_ObjectArray &objects,
                                  SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects,
                                  const SIM_Time &timestep);
  SIM_Result solveSingleObjectSubclass(SIM_Engine &engine, SIM_Object &object,
                                       SIM_ObjectArray &feedback_to_objects,
                                       const SIM_Time &time_step, bool object_is_new);

private:
  static const SIM_DopDescription *getSysSimDescription();

  void frameInfo(const SIM_Engine &engine);

  void timePerFrame(const bool &obj_new);

  fpreal toGb(long val);
  void coutMemory(const std::string &mes, const long &tl, const long &fr);

  void memoryInfo();

  void fieldsInfo(SIM_Object &obj) const;
  template <typename T> void printFieldInfo(const T &field) const;

  void bulletInfo(SIM_Object &obj) const;

  DECLARE_STANDARD_GETCASTTOTYPE();
  DECLARE_DATAFACTORY(SIM_SysInfo, SIM_SingleSolver, "System Info", getSysSimDescription());
};

} // namespace HDK_notawhale

#endif
