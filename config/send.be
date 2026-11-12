import Eyou_Motor_TOP as Eyou

ch =1
ID = 0x602
Eyou.Motor_Init(ch,ID)

Eyou.Send_MIT_PD_Control_Data(1)



