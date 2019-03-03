#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
//#include <SIM/SIM_Utils.h>

#include <sys/sysinfo.h>

#define MEMORY	        "memory"
#define SWAP    	    "swap"


class SIM_SysInfo : public SIM_SingleSolver, 
                          public SIM_OptionsUser {
public:
    GETSET_DATA_FUNCS_B(MEMORY, ShowMemory);
    GETSET_DATA_FUNCS_B(SWAP, ShowSwap);
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

    struct sysinfo sys_info;
    fpreal toGb(long val);
    void coutMemory(const long &tl,const long &fr);
    void memoryInfo();

    DECLARE_STANDARD_GETCASTTOTYPE();
    DECLARE_DATAFACTORY(SIM_SysInfo,
                        SIM_SingleSolver,
                        "System Info",
                        getSysSimDescription());
};