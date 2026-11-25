#ifndef Berry_H_
#define Berry_H_

#include "HARDWARE_TOP.hpp"
#include <map>
#include <memory>
#include <functional>

#include <vector>
#include <numeric>
using namespace std;

#define Berry_Head_Len  8
#define W_File_Finish   0xFF
#define CHUNK_SIZE      256
#define DelayTime       500
#define TimeCountMax    5

void printHexAndText(volatile const unsigned char* recvbuff, int data_len);
typedef enum {
    CMD_EXIT,
    CMD_MKDIR,
    CMD_DELETE,
    CMD_RETRIEVE,
    CMD_SEND,
    CMD_BERRY,
    CMD_MKDIR_ALL,
    CMD_MKFS,
    CMD_CD,
    CMD_SET = 0xF0,
    CMD_SHELL_START = 0xFE,
    CMD_UNKNOWN = 0xFF
} CmdType;     

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any problem */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_LOCK */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;

typedef struct Berry_CMD
{ 
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

	u8 CRC8;
	u8 Cmd;
    // DATA
    u8 data[1024];
} Berry_CMD;


typedef struct Berry_Data
{ 
    // Head
	u8 Cmd;
	u8 flag;

    // DATA
    u8 data[512];
} Berry_Data;


typedef struct Berrt_Frame
{ 
    int mode;
    string Berry_dir;
    vector<uint8_t> Berry_data;
} Berrt_Frame;



class SafeTimer
{
public:
    std::atomic<bool> stop{false};
    std::thread th;

    void start(int interval_ms, int max_count, std::function<void(int)> cb)
    {
        // 先停上一次的定时器
        stop_timer();
        stop = false;
        th = std::thread([interval_ms, max_count, cb, this]()
        {
            int cnt = 0;
            while (!stop)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                if (stop) break;
                cnt++;
                cb(cnt);
                if (cnt >= max_count)
                {
                    stop = true;
                    return;
                }
            }
        });
    }

    // 外部关闭定时器
    void stop_timer()
    {
        stop = true;
        if (th.joinable())
        {
            th.join();
        }
    }

    ~SafeTimer()
    {
        stop_timer();
    }
};

class Berry : private Robot_Hardware
{
public:
	int Get_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
    int Berry_Custom_Send(shared_ptr<Device_class> Device_P, u8 *Data, u32 len);
	int Berry_Call_Back(volatile u8 *Can_Frame);

    SafeTimer sendTimer;
    bool BerryState = false;
    bool TimeOutFlag = false;
    bool mcu_shell_flag = false;
    

    shared_ptr<Device_class> Device_my;
    int open_file(const std::string& path);

    Berrt_Frame Berry_frme;
    void mcu_shell(vector<string> args);
    void berry_shell(vector<string> args);
    void Berry_Send();
    
    bool Berry_stop_flag = false;
    string mcu_path = "/";

};



#endif