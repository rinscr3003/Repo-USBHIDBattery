#include "./Lib/CH552.H"
#include "./Lib/Debug.H"
#include <stdio.h>
#include <string.h>

#include "HID_DEFINES.h"

#define THIS_ENDP0_SIZE 64
#define ENDP2_IN_SIZE 64
#define ENDP2_OUT_SIZE 64

UINT8X Ep0Buffer[MIN(64, THIS_ENDP0_SIZE + 2)] _at_ 0x0000;                                                   // 端点0 OUT&IN缓冲区，必须是偶地址
UINT8X Ep2Buffer[MIN(64, ENDP2_IN_SIZE + 2) + MIN(64, ENDP2_OUT_SIZE + 2)] _at_ MIN(64, THIS_ENDP0_SIZE + 2); // 端点2 IN&OUT缓冲区,必须是偶地址

UINT8 SetupReq, Ready, Count, FLAG, UsbConfig;
UINT16 SetupLen;
PUINT8 pDescr;             // USB配置标志
USB_SETUP_REQ SetupReqBuf; // 暂存Setup包
#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

#pragma NOAREGS
/*设备描述符*/
UINT8C DevDesc[18] = {
    0x12,            // 描述符大小
    0x01,            // 描述符类型
    0x10, 0x01,      // USB规范版本号（USB1.1）
    0x00,            // 设备类（HID设备此三项全0）
    0x00,            // 设备子类
    0x00,            // 设备协议
    THIS_ENDP0_SIZE, // EP0大小
    0xb2, 0x38,      // VID
    0x01, 0x80,      // PID
    0x01, 0x00,      // 设备版本号
    IMANUFACTURER,   // 制造商字符串描述符索引
    IPRODUCT,        // 产品字符串描述符索引
    ISERIAL,         // 序列号产品字符串描述符索引
    0x01             // 配置数目
};

UINT8C CfgDesc[] =
    {
        // 配置描述符
        0x09,       // 描述符大小
        0x02,       // 描述符类型（是配置描述符）
        0x29, 0x00, // 配置总长度
        0x01,       // 配置接口数
        0x01,       // 配置值
        0x00,       // 配置字符串描述符索引
        0xA0,       // 机供，唤醒
        0x32,       // 请求100mA

        // 接口描述符
        0x09, // 描述符大小
        0x04, // 描述符类型（是接口描述符）
        0x00, // 接口编号
        0x00, // 接口替用设置（？）
        0x02, // 扣除端点0后的端点数
        0x03, // 接口类（是HID）
        0x00, // 接口子类（是不支持引导服务的）
        0x00, // 不支持引导服务，此项无效
        0x00, // 接口字符串描述符索引

        // HID类描述符
        0x09,       // 描述符大小
        0x21,       // 描述符类型（是HID类描述符）
        0x00, 0x01, // HID版本（是1.0）
        0x00,       // 国家代码
        0x01,       // 下挂HID描述符数量
        0x22,       // 下挂描述符1类型（是报表描述符）
        0x20, 0x01, // 下挂描述符1长度

        // 端点描述符
        0x07,                // 描述符大小
        0x05,                // 描述符类型（是端点描述符）
        0x82,                // 端点号及输入输出信息（是EP2IN）
        0x03,                // 端点传输类型（是中断传输）
        ENDP2_IN_SIZE, 0x00, // 端点大小
        0xFF,                // 端点轮询间隔（是255帧）

        // 端点描述符
        0x07,                 // 描述符大小
        0x05,                 // 描述符类型（是端点描述符）
        0x02,                 // 端点号及输入输出信息（是EP2OUT）
        0x03,                 // 端点传输类型（是中断传输）
        ENDP2_OUT_SIZE, 0x00, // 端点大小
        0xFF                  // 端点轮询间隔（是255帧）
};
/*字符串描述符*/
UINT8C LangDes[] = {
    // 语言描述符
    0x04,      // 描述符大小
    0x03,      // 描述符类型（是字符串描述符）
    0x09, 0x04 // 语言编码（是英语）
};
UINT8C StrDes1[] = {
    // 字符串描述符 制造商
    0x0A,                                      // 描述符大小
    0x03,                                      // 描述符类型（是字符串描述符）
    'M', 0x00, 'Z', 0x00, 'Y', 0x00, '7', 0x00 // 字符串内容
};
UINT8C StrDes2[] = {
    // 字符串描述符 产品
    0x20, // 描述符大小
    0x03, // 描述符类型（是字符串描述符）
    'U', 0x00, 'S', 0x00, 'B', 0x00, ' ', 0x00, 'H', 0x00, 'I', 0x00, 'D', 0x00, ' ', 0x00,
    'B', 0x00, 'a', 0x00, 't', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'y', 0x00 // 字符串内容
};
UINT8C StrDes3[] = {
    // 字符串描述符 序列号
    0x1C, // 描述符大小
    0x03, // 描述符类型（是字符串描述符）
    'T', 0x00, 'S', 0x00, 'H', 0x00, 'E', 0x00, '-', 0x00,
    '0', 0x00, '0', 0x00, '1', 0x00, '1', 0x00, '4', 0x00, '5', 0x00, '1', 0x00, '4', 0x00, // 字符串内容
};
UINT8C StrDes4[] = {
    // 字符串描述符 HID-化学类型
    0x0A,                                      // 描述符大小
    0x03,                                      // 描述符类型（是字符串描述符）
    'L', 0x00, 'I', 0x00, 'O', 0x00, 'N', 0x00 // 字符串内容
};

/*HID类报表描述符*/
UINT8C HIDRepDesc[] = {
    0x05, 0x84,       // USAGE_PAGE (Power Device)
    0x09, 0x04,       // USAGE (UPS)
    0xA1, 0x01,       // COLLECTION (Application)
    0x09, 0x24,       //   USAGE (Sink)
    0xA1, 0x02,       //   COLLECTION (Logical)
    0x75, 0x08,       //     REPORT_SIZE (8)
    0x95, 0x01,       //     REPORT_COUNT (1)
    0x15, 0x00,       //     LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x00, //     LOGICAL_MAXIMUM (255)

    0x85, HID_PD_IPRODUCT, //     REPORT_ID (1)
    0x09, 0xFE,            //     USAGE (iProduct)
    0x79, IPRODUCT,        //     STRING INDEX (2)
    0xB1, 0x23,            //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_SERIAL, //     REPORT_ID (2)
    0x09, 0xFF,          //     USAGE (iSerialNumber)
    0x79, ISERIAL,       //     STRING INDEX (3)
    0xB1, 0x23,          //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_MANUFACTURER, //     REPORT_ID (3)
    0x09, 0xFD,                //     USAGE (iManufacturer)
    0x79, IMANUFACTURER,       //     STRING INDEX (1)
    0xB1, 0x23,                //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x05, 0x85,                //     USAGE_PAGE (Battery System) ====================
    0x85, HID_PD_RECHARGEABLE, //     REPORT_ID (6)
    0x09, 0x8B,                //     USAGE (Rechargable)
    0xB1, 0x23,                //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_IDEVICECHEMISTRY, //     REPORT_ID (31)
    0x09, 0x89,                    //     USAGE (iDeviceChemistry)
    0x79, IDEVICECHEMISTRY,        //     STRING INDEX (4)
    0xB1, 0x23,                    //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_IOEMINFORMATION, //     REPORT_ID (32)
    0x09, 0x8F,                   //     USAGE (iOEMInformation)
    0x79, IOEMVENDOR,             //     STRING INDEX (5)
    0xB1, 0x23,                   //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_CAPACITYMODE, //     REPORT_ID (22)
    0x09, 0x2C,                //     USAGE (CapacityMode)
    0xB1, 0x23,                //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_CPCTYGRANULARITY1, //     REPORT_ID (16)
    0x09, 0x8D,                     //     USAGE (CapacityGranularity1)
    0x26, 0x64, 0x00,               //     LOGICAL_MAXIMUM (100)
    0xB1, 0x22,                     //     FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_CPCTYGRANULARITY2, //     REPORT_ID (24)
    0x09, 0x8E,                     //     USAGE (CapacityGranularity2)
    0xB1, 0x23,                     //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

    0x85, HID_PD_FULLCHRGECAPACITY, //     REPORT_ID (14)
    0x09, 0x67,                     //     USAGE (FullChargeCapacity)
    0xB1, 0x83,                     //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_DESIGNCAPACITY, //     REPORT_ID (23)
    0x09, 0x83,                  //     USAGE (DesignCapacity)
    0xB1, 0x83,                  //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_REMAININGCAPACITY, //     REPORT_ID (12)
    0x09, 0x66,                     //     USAGE (RemainingCapacity)
    0x81, 0xA3,                     //     INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x66,                     //     USAGE (RemainingCapacity)
    0xB1, 0xA3,                     //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_AVERAGETIME2FULL, //     REPORT_ID (26)
    0x09, 0x6A,                    //     USAGE (AverageTimeToFull)
    0x27, 0xFF, 0xFF, 0x00, 0x00,  //     LOGICAL_MAXIMUM (65534)
    0x66, 0x01, 0x10,              //     UNIT (Seconds)
    0x55, 0x00,                    //     UNIT_EXPONENT (0)
    0xB1, 0xA3,                    //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_AVERAGETIME2EMPTY, //     REPORT_ID (28)
    0x09, 0x69,                     //     USAGE (AverageTimeToEmpty)
    0x81, 0xA3,                     //     INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x69,                     //     USAGE (AverageTimeToEmpty)
    0xB1, 0xA3,                     //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_RUNTIMETOEMPTY, //     REPORT_ID (13)
    0x09, 0x68,                  //     USAGE (RunTimeToEmpty)
    0x81, 0xA3,                  //     INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x68,                  //     USAGE (RunTimeToEmpty)
    0xB1, 0xA3,                  //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_VOLTAGE, //     REPORT_ID (11)
    0x09, 0x30,           //     USAGE (Voltage)
    0x81, 0xA3,           //     INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x30,           //     USAGE (Voltage)
    0xB1, 0xA3,           //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_PRESENTSTATUS, //       REPORT_ID (7)
    0x05, 0x85,                 //       USAGE_PAGE (Battery System) =================
    0x09, 0x44,                 //       USAGE (Charging)
    0x75, 0x01,                 //       REPORT_SIZE (1)
    0x15, 0x00,                 //       LOGICAL_MINIMUM (0)
    0x25, 0x01,                 //       LOGICAL_MAXIMUM (1)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x44,                 //       USAGE (Charging)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x45,                 //       USAGE (Discharging)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x45,                 //       USAGE (Discharging)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0xD0,                 //       USAGE (ACPresent)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0xD0,                 //       USAGE (ACPresent)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0xD1,                 //       USAGE (BatteryPresent)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0xD1,                 //       USAGE (BatteryPresent)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x42,                 //       USAGE (BelowRemainingCapacityLimit)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x42,                 //       USAGE (BelowRemainingCapacityLimit)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x43,                 //       USAGE (RemainingTimeLimitExpired)
    0x81, 0xA2,                 //       INPUT (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x43,                 //       USAGE (RemainingTimeLimitExpired)
    0xB1, 0xA2,                 //       FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x4B,                 //       USAGE (NeedReplacement)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x4B,                 //       USAGE (NeedReplacement)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0xDB,                 //       USAGE (VoltageNotRegulated)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0xDB,                 //       USAGE (VoltageNotRegulated)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x46,                 //       USAGE (FullyCharged)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x46,                 //       USAGE (FullyCharged)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x47,                 //       USAGE (FullyDischarged)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x47,                 //       USAGE (FullyDischarged)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x05, 0x84,                 //       USAGE_PAGE (Power Device) =================
    0x09, 0x68,                 //       USAGE (ShutdownRequested)
    0x81, 0xA2,                 //       INPUT (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x68,                 //       USAGE (ShutdownRequested)
    0xB1, 0xA2,                 //       FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x69,                 //       USAGE (ShutdownImminent)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x69,                 //       USAGE (ShutdownImminent)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x73,                 //       USAGE (CommunicationLost)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x73,                 //       USAGE (CommunicationLost)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x09, 0x65,                 //       USAGE (Overload)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x65,                 //       USAGE (Overload)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x95, 0x02,                 //       REPORT_COUNT (2)
    0x81, 0x01,                 //       INPUT (Constant, Array, Absolute)
    0xB1, 0x01,                 //       FEATURE (Constant, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Nonvolatile, Bitfield)
    0xC0,                       //     END_COLLECTION
    0xC0                        // END_COLLECTION
};

UINT8X UserEp2Buf[64]; // 用户数据定义
UINT8 Endp2Busy = 0;

/*******************************************************************************
 * Function Name  : USBDeviceInit()
 * Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USBDeviceInit()
{
    IE_USB = 0;
    USB_CTRL = 0x00;             // 先设定USB设备模式
    UDEV_CTRL = bUD_PD_DIS;      // 禁止DP/DM下拉电阻
    UDEV_CTRL &= ~bUD_LOW_SPEED; // 选择全速12M模式，默认方式
    USB_CTRL &= ~bUC_LOW_SPEED;
    UEP2_DMA = Ep2Buffer;                       // 端点2数据传输地址
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;    // 端点2发送接收使能
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;               // 端点2收发各64字节缓冲区
    UEP0_DMA = Ep0Buffer;                       // 端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN); // 端点0单64字节收发缓冲区
    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN; // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                              // 允许USB端口
    USB_INT_FG = 0xFF;                                     // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}

/*******************************************************************************
 * Function Name  : Enp2BulkIn()
 * Description    : USB设备模式端点2的批量上传
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Enp2BulkIn()
{
    memcpy(Ep2Buffer + MAX_PACKET_SIZE, UserEp2Buf, sizeof(UserEp2Buf)); // 加载上传数据
    UEP2_T_LEN = THIS_ENDP0_SIZE;                                        // 上传最大包长度
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;             // 有数据时上传数据并应答ACK
}

/*******************************************************************************
 * Function Name  : DeviceInterrupt()
 * Description    : CH552USB中断处理函数
 *******************************************************************************/
void DeviceInterrupt(void) interrupt INT_NO_USB using 1 // USB中断服务程序,使用寄存器组1
{
    UINT8 i;
    UINT16 len;
    if (UIF_TRANSFER) // USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:       // endpoint 2# 端点批量上传
            UEP2_T_LEN = 0;          // 预使用发送长度一定要清空
            UEP2_CTRL ^= bUEP_T_TOG; // 手动翻转
            Endp2Busy = 0;
            UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // 默认应答NAK
            break;
        case UIS_TOKEN_OUT | 2: // endpoint 2# 端点批量下传
            if (U_TOG_OK)       // 不同步的数据包将丢弃
            {
                len = USB_RX_LEN;        // 接收数据长度，数据从Ep2Buffer首地址开始存放
                UEP2_CTRL ^= bUEP_R_TOG; // 手动翻转
                for (i = 0; i < len; i++)
                {
                    Ep2Buffer[MAX_PACKET_SIZE + i] = Ep2Buffer[i] ^ 0xFF; // OUT数据取反到IN由计算机验证
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; // 允许上传
            }
            break;
        case UIS_TOKEN_SETUP | 0: // SETUP事务
            UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
            len = USB_RX_LEN;
            if (len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = ((UINT16)UsbSetupBuf->wLengthH << 8) | (UsbSetupBuf->wLengthL);
                len = 0; // 默认为成功并且上传0长度
                SetupReq = UsbSetupBuf->bRequest;
                if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) /*HID类命令*/
                {
                    switch (SetupReq)
                    {
                    case 0x01: // GetReport

                        pDescr = UserEp2Buf;             // 控制端点上传输据
                        if (SetupLen >= THIS_ENDP0_SIZE) // 大于端点0大小，需要特殊处理
                        {
                            len = THIS_ENDP0_SIZE;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    case 0x02: // GetIdle
                        break;
                    case 0x03: // GetProtocol only in boot device
                        break;
                    case 0x09: // SetReport
                        break;
                    case 0x0A: // SetIdle
                        break;
                    case 0x0B: // SetProtocol only in boot device
                        break;
                    default:
                        len = 0xFF; /*命令不支持*/
                        break;
                    }
                    if (SetupLen > len)
                    {
                        SetupLen = len; // 限制总长度
                    }
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // 本次传输长度
                    memcpy(Ep0Buffer, pDescr, len);                                 // 加载上传数据
                    SetupLen -= len;
                    pDescr += len;
                }
                else // 标准请求
                {
                    switch (SetupReq) // 请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch (UsbSetupBuf->wValueH)
                        {
                        case 1:                // 设备描述符
                            pDescr = &DevDesc; // 把设备描述符送到要发送的缓冲区
                            len = sizeof(DevDesc);
                            break;
                        case 2:                // 配置描述符
                            pDescr = &CfgDesc; // 把配置描述符送到要发送的缓冲区
                            len = sizeof(CfgDesc);
                            break;
                        case 3: // 字符串描述符
                            switch (UsbSetupBuf->wValueL)
                            {
                            case 1:
                                pDescr = (PUINT8)(&StrDes1[0]);
                                len = sizeof(StrDes1);
                                break;
                            case 2:
                                pDescr = (PUINT8)(&StrDes2[0]);
                                len = sizeof(StrDes2);
                                break;
                            case 3:
                                pDescr = (PUINT8)(&StrDes3[0]);
                                len = sizeof(StrDes3);
                                break;
                            case 4:
                                pDescr = (PUINT8)(&StrDes4[0]);
                                len = sizeof(StrDes4);
                                break;
                            case 0:
                                pDescr = (PUINT8)(&LangDes[0]);
                                len = sizeof(LangDes);
                                break;
                            default:
                                len = 0xFF; // 不支持的字符串描述符
                                break;
                            }
                            break;
                        case 0x22:               // 报表描述符
                            pDescr = HIDRepDesc; // 数据准备上传
                            len = sizeof(HIDRepDesc);
                            break;
                        default:
                            len = 0xff; // 不支持的命令或者出错
                            break;
                        }
                        if (SetupLen > len)
                        {
                            SetupLen = len; // 限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // 本次传输长度
                        memcpy(Ep0Buffer, pDescr, len);                                 // 加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL; // 暂存USB设备地址
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if (SetupLen >= 1)
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        UsbConfig = UsbSetupBuf->wValueL;
                        if (UsbConfig)
                        {
                            Ready = 1; // set config命令一般代表usb枚举完成的标志
                        }
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                                         // Clear Feature
                        if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // 端点
                        {
                            switch (UsbSetupBuf->wIndexL)
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF; // 不支持的端点
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFF; // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                               /* Set Feature */
                        if ((UsbSetupBuf->bRequestType & 0x1F) == 0x00) /* 设置设备 */
                        {
                            if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01)
                            {
                                if (CfgDesc[7] & 0x20)
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFF; /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFF; /* 操作失败 */
                            }
                        }
                        else if ((UsbSetupBuf->bRequestType & 0x1F) == 0x02) /* 设置端点 */
                        {
                            if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00)
                            {
                                switch (((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL)
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; /* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; /* 设置端点2 OUT Stall */
                                    break;
                                default:
                                    len = 0xFF; /* 操作失败 */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF; /* 操作失败 */
                            }
                        }
                        else
                        {
                            len = 0xFF; /* 操作失败 */
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if (SetupLen >= 2)
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xff; // 操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xff; // 包长度错误
            }
            if (len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            }
            else if (len <= THIS_ENDP0_SIZE) // 上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // 默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0;                                                      // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // 默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0: // endpoint0 IN
            switch (SetupReq)
            {
            case USB_GET_DESCRIPTOR:
                // case HID_GET_REPORT:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // 本次传输长度
                memcpy(Ep0Buffer, pDescr, len);                                 // 加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG; // 同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0; // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0: // endpoint0 OUT
            len = USB_RX_LEN;
            if (SetupReq == 0x09)
            {
                if (Ep0Buffer[0])
                {
                    // printf("Light on Num Lock LED!\n");
                }
                else if (Ep0Buffer[0] == 0)
                {
                    // printf("Light off Num Lock LED!\n");
                }
            }
            UEP0_CTRL ^= bUEP_R_TOG; // 同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0; // 写0清空中断
    }
    if (UIF_BUS_RST) // 设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Endp2Busy = 0;
        UIF_BUS_RST = 0; // 清中断标志
    }
    if (UIF_SUSPEND) // USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if (USB_MIS_ST & bUMS_SUSPEND) // 挂起
        {
            ; // zzz
        }
    }
    else
    {                      // 意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF; // 清中断标志
    }
}

main()
{
    UINT8 i;
    CfgFsys();
    mDelaymS(5); // 修改主频等待内部晶振稳定,必加
    // mInitSTDIO( );                                                        //串口0初始化
    for (i = 0; i < 64; i++) // 准备演示数据
    {
        UserEp2Buf[i] = i;
    }
    USBDeviceInit(); // USB设备模式初始化
    EA = 1;          // 允许单片机中断
    UEP1_T_LEN = 0;  // 预使用发送长度一定要清空
    UEP2_T_LEN = 0;  // 预使用发送长度一定要清空
    FLAG = 0;
    Ready = 0;
    while (1)
    {
        if (Ready)
        {
            while (Endp2Busy)
                ; // 如果忙（上一包数据没有传上去），则等待。
            // Endp2Busy = 1; // 设置为忙状态
            // Enp2BulkIn();
            mDelaymS(100);
        }
        mDelaymS(100); // 模拟单片机做其它事
    }
}
