import streamlit as st
import numpy as np
import time

# 移除：旧版Streamlit不支持server.autoReload，删除这行报错代码
# st.set_option('server.autoReload', False)

# 设置页面标题和宽布局（旧版Streamlit也支持）
st.set_page_config(page_title="60Hz示波器", layout="wide")

# 初始化会话状态（保存参数和消息，避免刷新丢失）
if 'messages' not in st.session_state:
    st.session_state.messages = []
if 'wave_params' not in st.session_state:
    st.session_state.wave_params = {
        "wave_type": "正弦波",
        "frequency": 3.0,
        "amplitude": 1.0,
        "offset": 0.0,
        "noise": 0.1
    }

# 生成波形数据（优化：80个数据点，平衡平滑度和性能）
def generate_wave_data(params):
    x = np.linspace(0, 2 * np.pi * 2, 200)  # 80个点，1屏显示2个周期
    t = time.time() * 2  # 时间因子，控制波形移动速度
    
    # 根据波形类型生成核心数据
    if params["wave_type"] == "正弦波":
        y = np.sin(x + t * params["frequency"])
    elif params["wave_type"] == "方波":
        y = np.sign(np.sin(x + t * params["frequency"]))
    elif params["wave_type"] == "三角波":
        y = 2 * np.abs(2 * ((x + t * params["frequency"]) / (2 * np.pi) % 1) - 1) - 1
    else:  # 锯齿波
        y = 2 * ((x + t * params["frequency"]) / (2 * np.pi) % 1) - 1
    
    # 应用振幅、偏移和噪声
    y = y * params["amplitude"] + params["offset"]
    y += np.random.normal(0, params["noise"], len(y))
    return y

# 页面标题和说明
st.title("📊 60Hz 高频示波器（兼容旧版Streamlit）")
st.caption("波形刷新率：≈60次/秒（16.7ms/帧）| 支持正弦/方波/三角波/锯齿波")
st.divider()

# 左右分栏：左侧控制区，右侧显示区
col_control, col_display = st.columns([1, 3])

# ---------------------- 左侧：波形控制区 ----------------------
with col_control:
    st.subheader("🔧 波形参数控制")
    
    # 1. 波形类型选择
    wave_type = st.selectbox(
        "选择波形类型",
        ["正弦波", "方波", "三角波", "锯齿波"],
        index=["正弦波", "方波", "三角波", "锯齿波"].index(st.session_state.wave_params["wave_type"])
    )
    
    # 2. 频率调节（0.5~10Hz）
    frequency = st.slider(
        "频率 (Hz)",
        min_value=0.5,
        max_value=10.0,
        value=st.session_state.wave_params["frequency"],
        step=0.5,
        help="数值越大，波形越密集"
    )
    
    # 3. 振幅调节（0.1~5）
    amplitude = st.slider(
        "振幅",
        min_value=0.1,
        max_value=5.0,
        value=st.session_state.wave_params["amplitude"],
        step=0.1,
        help="数值越大，波形越高"
    )
    
    # 4. 垂直偏移调节（-2~2）
    offset = st.slider(
        "垂直偏移",
        min_value=-2.0,
        max_value=2.0,
        value=st.session_state.wave_params["offset"],
        step=0.1,
        help="正数上移，负数下移"
    )
    
    # 5. 噪声调节（0~1）
    noise = st.slider(
        "噪声强度",
        min_value=0.0,
        max_value=1.0,
        value=st.session_state.wave_params["noise"],
        step=0.05,
        help="数值越大，波形越不稳定"
    )
    
    # 更新会话状态（实时生效）
    st.session_state.wave_params = {
        "wave_type": wave_type,
        "frequency": frequency,
        "amplitude": amplitude,
        "offset": offset,
        "noise": noise
    }

    # ---------------------- 消息交互区 ----------------------
    st.divider()
    st.subheader("💬 消息交互")
    new_message = st.text_input("输入消息", placeholder="例如：波形显示流畅！")
    
    if st.button("发送消息", type="primary") and new_message.strip():
        st.session_state.messages.append(new_message.strip())
        new_message = ""  # 清空输入框
    
    # 显示最近5条消息
    if st.session_state.messages:
        st.caption("最近消息（最多5条）：")
        for i, msg in enumerate(reversed(st.session_state.messages[-5:])):
            st.text(f"[{len(st.session_state.messages)-i}] {msg}")
    else:
        st.caption("暂无消息，可输入并发送~")

# ---------------------- 右侧：60Hz波形显示区 ----------------------
with col_display:
    st.subheader(f"📈 实时波形 | {wave_type}（{frequency} Hz）")
    wave_placeholder = st.empty()  # 占位容器，用于高频刷新
    
    # 核心：60Hz刷新逻辑（1/60秒≈16.7ms间隔）
    target_interval = 1 / 60  # 60Hz的目标间隔
    while True:
        # 记录开始时间，补偿计算耗时
        start_time = time.time()
        
        # 1. 生成当前波形数据
        wave_data = generate_wave_data(st.session_state.wave_params)
        
        # 2. 绘制波形（width='stretch'替代use_container_width，兼容新旧版）
        wave_placeholder.line_chart(
            wave_data,
            width='stretch',  # 波形充满容器（消除旧警告）
            height=400,       # 固定高度，避免闪烁
            color="#1E88E5"   # 示波器经典蓝色
        )
        
        # 3. 动态计算sleep时间（确保总间隔接近16.7ms）
        elapsed_time = time.time() - start_time  # 本次循环耗时
        sleep_time = max(0, target_interval - elapsed_time)  # 避免负睡眠
        time.sleep(sleep_time)
