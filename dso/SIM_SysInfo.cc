#include "SIM_SysInfo.hh"

#include <UT/UT_DSOVersion.h>
#include <SIM/SIM_DopDescription.h>
#include <PRM/PRM_Include.h>
#include <SIM/SIM_Engine.h>

#include <sys/sysinfo.h>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <HOM/HOM_Module.h>

using namespace std;

void
initializeSIM(void *) {
    IMPLEMENT_DATAFACTORY(SIM_SysInfo);
}

SIM_SysInfo::SIM_SysInfo(const SIM_DataFactory *factory)
    : BaseClass(factory),
      SIM_OptionsUser(this) {
}

SIM_SysInfo::~SIM_SysInfo() {
}

const SIM_DopDescription *
SIM_SysInfo::getSysSimDescription() {
    static PRM_Name theBatchMode(BATCHMODEONLY,"Work Only In Batch Mode");
    static PRM_Name theShowClock(SHOWCLOCK,"Clock");
    static PRM_Name theShowMemory(SHOWMEMORY,"Memory");
    static PRM_Name theShowSwap(SHOWSWAP,"Swap");

    static PRM_Template		 theTemplates[] = {
        PRM_Template(PRM_TOGGLE, 1,&theBatchMode,PRMoneDefaults),
        PRM_Template(PRM_TOGGLE, 1,&theShowClock,PRMoneDefaults),
        PRM_Template(PRM_TOGGLE, 1,&theShowMemory,PRMoneDefaults),
        PRM_Template(PRM_TOGGLE, 1,&theShowSwap,PRMoneDefaults),
	    PRM_Template()
    };

    static SIM_DopDescription	 theDopDescription(true,
						   "hdk_sysinfo",
						   "System Info",
						   "SystemInfo",
						   classname(),
						   theTemplates);

    return &theDopDescription;
}

void SIM_SysInfo::timePerFrame() {
    auto now = std::chrono::system_clock::now();
    auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto value = now_s.time_since_epoch();
    long cur = value.count();

    long sec = cur - getClock();
    long min = sec/60;
    cout << "Time Per Frame: " << min << " min " << sec - min*60 << " sec" << endl;
    setClock(cur);
}

fpreal SIM_SysInfo::toGb(long val) {
    return static_cast<fpreal>(val)/1024/1024/1024;
}

void SIM_SysInfo::coutMemory(const long &tl,const long &fr) {
    cout << fixed << setprecision(2);
    cout << toGb(tl - fr) << " / "  << toGb(tl) << " Gb" << endl;
}

void SIM_SysInfo::memoryInfo() {
    sysinfo(&sys_info);
    if (getShowMemory()) coutMemory(sys_info.totalram,sys_info.freeram);
    if (getShowSwap()) coutMemory(sys_info.totalswap,sys_info.freeswap);
}

SIM_Solver::SIM_Result 
SIM_SysInfo::solveSingleObjectSubclass(
    SIM_Engine& engine,
    SIM_Object& object,
    SIM_ObjectArray& feedback_to_objects,
    const SIM_Time& time_step,
    bool object_is_new) {

    if (getbatchMode()) {
        if (HOM().isUIAvailable())
            return SIM_SOLVER_SUCCESS;
    }

    cout << "Frame: " << engine.getSimulationFrame(engine.getSimulationTime()) << endl;

    if (getShowClock()) timePerFrame();

    if (getShowMemory() && getShowSwap()) memoryInfo();

    cout << endl;
    return SIM_SOLVER_SUCCESS;
}