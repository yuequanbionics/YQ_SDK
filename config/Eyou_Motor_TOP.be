class Eyou_Motor_PH25_config
  static var Motor_Reduction_Ratio = 121
  static var Eyou_Motor_Set_Offest = 30
  static var Eyou_Motor_Set_Speed = 18
  static var Eyou_Motor_dxfKp = 10240
  static var Encoder_Res = 65536
end


var Eyou_Motor_EN_In_Init = true
var Set_Zero_In_Init = false
var Eyou_Motor_Zero_Write = false
var Eyou_Motor_Config = true
var Eyou_Motor_config = Eyou_Motor_PH25_config()


Write_4_Byte_Data = 0x23
Write_3_Byte_Data = 0x27
Write_2_Byte_Data = 0x2B
Write_1_Byte_Data = 0x2F

Read_Back = 0x40
Read_Back_With_Code = 0x80

Set_Can_Id = 0x01
Save_To_Motor = 0x10
Back_Tem = 0x12
Control_Word = 0x40
Mode_Of_Work = 0x60
Read_Motor_Pos = 0x64
Read_Motor_Speed = 0x6C
Set_Zero_Mode = 0x7C
Planning_Speed = 0x81
Planning_Acceleration = 0x83
Planning_Deceleration = 0x84
Target_Pos = 0x7A


class Eyou_Motor
  static send_buf = []
  def Send_Control_Data(data)

    self.send_buf[0] = 8
    self.send_buf[3] = Write_4_Byte_Data
    self.send_buf[4] = 0x7A
    self.send_buf[5] = 0x60
    self.send_buf[6] = 0x00
    self.send_buf[7] = data
    self.send_buf[8] = data>>8
    self.send_buf[9] = data>>16
    self.send_buf[10] = data>>24

    var i = 0
    while i < self.send_buf[0]+3
        can_send(self.send_buf[i])
        i += 1
    end
  end
  def Write__Data(Write_Byte_Data,cmd,cmd1,index,data0,data1,data2,data3)
    self.send_buf[0] = 8
    self.send_buf[3] = Write_Byte_Data
    self.send_buf[4] = cmd
    self.send_buf[5] = cmd1
    self.send_buf[6] = index
    self.send_buf[7] = data0
    self.send_buf[8] = data1
    self.send_buf[9] = data2
    self.send_buf[10] = data3

    var i = 0
    while i < self.send_buf[0]+3
        can_send(self.send_buf[i])
        i += 1
    end
  end
  def Motor_Init(ch,ID)
    var i = 0
    while i < 64
        self.send_buf.push(0)
        i += 1
    end
    self.send_buf[1] = ch
    self.send_buf[2] = ID
    if Eyou_Motor_Config == true
      delay_ms(1)
      self.Motor_EN(false)
      delay_ms(1)
      self.Write__Data(Write_1_Byte_Data,0x60,0x60,0x00,0x08,0x00,0x00,0x00)
      Speed_Angle_S = real(Eyou_Motor_config.Eyou_Motor_Set_Speed) * 360 / 60
      Eyou_Kp = Eyou_Motor_config.Eyou_Motor_dxfKp
      delay_ms(1)
      data = 9999999
      self.Write__Data(Write_4_Byte_Data,0x65,0x60,0x00,data,data>>8,data>>16,data>>24)
      delay_ms(1)
      # can_send(self.ch,self.ID,len,Write_2_Byte_Data,0x13,0x20,0x03,Eyou_Kp,Eyou_Kp>>8,0,0)
      delay_ms(1)
      data = real(Speed_Angle_S) / 360 * Eyou_Motor_config.Motor_Reduction_Ratio * Eyou_Motor_config.Encoder_Res
      data = int(data)
      self.Write__Data(Write_4_Byte_Data,0x7F,0x60,0x00,data,data>>8,data>>16,data>>24)
      delay_ms(1)
    end
    if Eyou_Motor_EN_In_Init == true
      delay_ms(1)
      self.Motor_EN(true)
      delay_ms(1)
    end
  end
  def Motor_EN(en)
    if en == true
      self.Write__Data(Write_2_Byte_Data,Control_Word,0x60,0x00,0x06,0x00,0x00,0x00)
      self.Write__Data(Write_2_Byte_Data,Control_Word,0x60,0x00,0x07,0x00,0x00,0x00)
      self.Write__Data(Write_2_Byte_Data,Control_Word,0x60,0x00,0x0F,0x00,0x00,0x00)
    else
      self.Write__Data(Write_2_Byte_Data,Control_Word,0x60,0x00,0x06,0x00,0x00,0x00)
    end
  end
  def Read_Motor_Data()
    self.Write__Data(0x40,Read_Motor_Pos,0x60,0x00,0x00,0x00,0x00,0x00)
  end
  def Send_MIT_PD_Control_Data(target_pos)
    can_receive(1)
    self.Read_Motor_Data()
    delay_ms(1)
    can_receive(0)

    buf = []
    var i = 0
    while i < 64
        buf.push(get_can(i))
        i = i + 1
    end
    # can_send(self.ch,0x101,8,Write_4_Byte_Data,0x7A,0x60,0x00,send_buf[10],send_buf[11],send_buf[12],send_buf[13])
    pos = buf[10] + buf[11] * 256 + buf[12] * 65536 + buf[13] * 16777216
    var start_pos = real(pos) / (Eyou_Motor_config.Motor_Reduction_Ratio*Eyou_Motor_config.Encoder_Res) * 360
    plan_pos = target_pos - start_pos
    fang = 0

    if start_pos < target_pos
      fang = 1
    else
      fang = -1
    end

    if plan_pos < -90 || plan_pos > 90
      target_pos = start_pos + fang * 90
    else
      target_pos = start_pos + plan_pos
    end
    data = real(target_pos) / 360 * Eyou_Motor_config.Motor_Reduction_Ratio * Eyou_Motor_config.Encoder_Res
    data = int(data)
    self.Send_Control_Data(data)
  end
end

return Eyou_Motor()


