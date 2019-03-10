#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
//#include <SIM/SIM_Utils.h>

#include <string>

#define BATCHMODEONLY   "batchMode"
#define SHOWMEMORY      "printMemory"
#define SHOWSWAP        "printSwap"
#define SHOWCLOCK       "printClock"
#define CLOCK           "clock"

#define MODE           "mode"
#define FIELD           "field"

using namespace std;

class SIM_SysInfo : public SIM_SingleSolver, 
                          public SIM_OptionsUser {
public:
    GETSET_DATA_FUNCS_B(BATCHMODEONLY, batchMode);
    GETSET_DATA_FUNCS_B(SHOWMEMORY, ShowMemory);
    GETSET_DATA_FUNCS_B(SHOWSWAP, ShowSwap);
    GETSET_DATA_FUNCS_B(SHOWCLOCK, ShowClock);
    GETSET_DATA_FUNCS_I(CLOCK, Clock);
    GETSET_DATA_FUNCS_S(FIELD, Field);
    GETSET_DATA_FUNCS_I(MODE, DataMode);
protected:
    explicit	SIM_SysInfo(const SIM_DataFactory *factory);
    virtual		~SIM_SysInfo();
    // This implements your own solver step
    SIM_Result solveSingleObjectSubclass(
        SIM_Engine& engine, SIM_Object& object,
        SIM_ObjectArray& feedback_to_objects,
        const SIM_Time& time_step,
        bool object_is_new
    );
private:
    static const SIM_DopDescription* getSysSimDescription();

    void timePerFrame();

    fpreal toGb(long val);
    void coutMemory(const string &mes,const long &tl,const long &fr);

    void memoryInfo();

    void fieldsInfo(SIM_Object &obj);
    template<typename T> void printFieldInfo(const T &field);

    void bulletInfo(SIM_Object &obj);

    DECLARE_STANDARD_GETCASTTOTYPE();
    DECLARE_DATAFACTORY(SIM_SysInfo,
                        SIM_SingleSolver,
                        "System Info",
                        getSysSimDescription());
};