import Can_Config as config

def send_can_config(can_cfg, enum_table)
    for cfg_key : can_cfg.keys()
        if enum_table.contains(cfg_key)
            can_param(enum_table[cfg_key], can_cfg[cfg_key])
        end
    end
end

def main()
    Can_Enable = config["Can_Enable"]
    Can_CMD_Data_Enum = config["Can_CMD_Data_Enum"]
    Can_config = config[Can_Enable["Can_Baud"]]
    if Can_Enable["Can_EN"] == true
        can_param(Can_CMD_Data_Enum["Can_Instance"],Can_Enable["Can_Instance"])
        if Can_Enable["Can_Instance"] == 1
            can_param(Can_CMD_Data_Enum["Can_MessageRAMOffset"],0)
        end
        if Can_Enable["Can_Instance"] == 2
            can_param(Can_CMD_Data_Enum["Can_MessageRAMOffset"],1280)
        end
        can_param(Can_CMD_Data_Enum["Can_EN"],Can_Enable["Can_EN"])
        send_can_config(Can_config, Can_CMD_Data_Enum)
        can_param(Can_CMD_Data_Enum["Can_Complete"],Can_Enable["Can_Complete"])
    end    
end

main()
