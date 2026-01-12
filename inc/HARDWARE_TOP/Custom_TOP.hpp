#ifndef Custom_TOP_H_
#define Custom_TOP_H_
#include "HARDWARE_TOP.hpp"

#define Custom_Data_Struct_Len sizeof(Custom_Data)

typedef struct
{ // CMD数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u16 Data0;
	u16 Data1;
	u16 Data2;
	u16 Data3;
} Custom_Data;


class Custom : private Robot_Hardware
{
public:
    int Custom_Send_Msg(shared_ptr<Device_Struct> Device_P, u8 *Msg_Frame);
	int (*Custom_Msg_CallBack)(volatile u8 *);
	
	Custom(void);
};

#endif