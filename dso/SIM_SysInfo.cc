#include "SIM_SysInfo.hh"

#include <UT/UT_DSOVersion.h>
#include <SIM/SIM_DopDescription.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <SIM/SIM_Engine.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>

#include <HOM/HOM_Module.h>

#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>
#include <SIM/SIM_MatrixField.h>

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

    static PRM_Name theRam("ram","Ram");
    static PRM_Default theRamSwitcher(2, "Ram");

    static PRM_Name theShowMemory(SHOWMEMORY,"Memory");
    static PRM_Name theShowSwap(SHOWSWAP,"Swap");

    static PRM_Name theField(FIELD,"Field");
    static PRM_Default theFieldDefault(0,"density");

    static PRM_Template		 theTemplates[] = {
        PRM_Template(PRM_TOGGLE, 1,&theBatchMode,PRMoneDefaults),
        PRM_Template(PRM_TOGGLE, 1,&theShowClock,PRMoneDefaults),
        PRM_Template(PRM_SWITCHER, 1,&theRam,&theRamSwitcher, 0, 0, 0,
                     &PRM_SpareData::groupTypeSimple),
        PRM_Template(PRM_TOGGLE, 1,&theShowMemory,PRMoneDefaults),
        PRM_Template(PRM_TOGGLE, 1,&theShowSwap,PRMoneDefaults),

        PRM_Template(PRM_STRING, 1,&theField,&theFieldDefault),
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
    return static_cast<fpreal>(val)/(1024*1024);
}

void SIM_SysInfo::coutMemory(const string &mes,const long &tl,const long &fr) {
    cout << fixed << setprecision(2);
    cout << mes << "\t" << toGb(tl - fr) << " / "  << toGb(tl) << " Gb" << endl;
}

void SIM_SysInfo::memoryInfo() {
    long mem_total, mem_ava, swap_total, swap_ava;
    string token;
    std::ifstream file("/proc/meminfo");
    while(file >> token) {
        if(token == "MemTotal:") {
            file >> mem_total;
        } else if (token == "MemAvailable:") {
            file >> mem_ava;
        }
        if(token == "SwapTotal:") {
            file >> swap_total;
        } else if (token == "SwapFree:") {
            file >> swap_ava;
        }
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    file.close();

    if (getShowMemory()) coutMemory("Ram:",mem_total,mem_ava);
    if (getShowSwap()) coutMemory("Swap:",swap_total,swap_ava);
}

template<typename T>
void SIM_SysInfo::printFieldInfo(const T &field)
{
        cout << endl;
        cout << getField() << endl;
        cout << "vs: " << field->getVoxelSize()[0] << " "; 
        cout << "div: " << setprecision(0) << field->getDivisions() << " ";
        cout << "nv: " << field->getTotalVoxels() << endl;
}

void SIM_SysInfo::fieldsInfo(SIM_Object &obj) {
    SIM_ScalarField *sc_field = SIM_DATA_GET(obj, 
                         getField(), 
                         SIM_ScalarField);
    SIM_VectorField *vec_field = SIM_DATA_GET(obj, 
                            getField(), 
                            SIM_VectorField);
    SIM_MatrixField *mx_field = SIM_DATA_GET(obj, 
                            getField(), 
                            SIM_MatrixField);
    if (sc_field) {
        printFieldInfo<>(sc_field);
    } else if (vec_field) {
         printFieldInfo<>(vec_field);
    } else if (mx_field) {
         printFieldInfo<>(mx_field);
    }
    // if (field) {
    //     cout << endl;
    //     cout << field->getField() << endl;
    //     cout << "vs: " << field->getVoxelSize()[0] << " "; 
    //     cout << "div: " << setprecision(0) << field->getDivisions() << " ";
    //     cout << "nv: " << field->getTotalVoxels() << endl;
    // }
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

    fieldsInfo(object);

    cout << endl;
    return SIM_SOLVER_SUCCESS;
}