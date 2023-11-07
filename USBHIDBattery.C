#include "./Lib/CH552.H"
#include "./Lib/Debug.H"
#include <stdio.h>
#include <string.h>
#include "./HwDrv/ch55x_soft_i2c.h"
#include "./HwDrv/bq4050.h"

#include "HID_DEFINES.h"

#define THIS_ENDP0_SIZE 64

UINT8X Ep0Buffer[MIN(64, THIS_ENDP0_SIZE + 2)] _at_ 0x0000; // 端点0 OUT&IN缓冲区，必须是偶地址

UINT8 SetupReq, Ready, Count, FLAG, UsbConfig;
UINT16 SetupLen;
PUINT8 pDescr;             // USB配置标志
USB_SETUP_REQ SetupReqBuf; // 暂存Setup包
#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

#pragma NOAREGS

/*******************************************************************************
* Function Name  : Port1Cfg()
* Description    : 端口1配置
* Input          : Mode  0 = 浮空输入，无上拉
                         1 = 推挽输入输出
                         2 = 开漏输入输出，无上拉
                         3 = 类51模式，开漏输入输出，有上拉，内部电路可以加速由低到高的电平爬升
                   ,UINT8 Pin	(0-7)
* Output         : None
* Return         : None
*******************************************************************************/
extern void Port1Cfg(UINT8 Mode, UINT8 Pin);

#include "USBDesc.h"

UINT8X UserReportBuf[64]; // 用户数据定义
UINT8 ReportBusy = 0;

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
    UEP0_DMA = Ep0Buffer;                       // 端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN); // 端点0单64字节收发缓冲区
    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN; // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                              // 允许USB端口
    USB_INT_FG = 0xFF;                                     // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}

void USBDeviceReset()
{
    IE_USB = 0;
    USB_CTRL = 0x06;
    UDEV_CTRL = bUD_PD_DIS; // 禁止DP/DM下拉电阻
    mDelaymS(100);
    USBDeviceInit();
}

volatile UINT16X batteryPresentStatus = PRESENTSTATUS_BATTPRESENT;
volatile UINT16X runtimeToEmpty = 0x0;
volatile UINT16X averageToEmpty = 0x0;
volatile UINT16X averageToFull = 0x0;
volatile UINT16X designVolt = 1110;
volatile UINT16X currentVolt = 1110;
UINT8XV batteryHealth = 100;
UINT8XV batteryRSOC = 44;

/*******************************************************************************
 * Function Name  : DeviceInterrupt()
 * Description    : CH552USB中断处理函数
 *******************************************************************************/
void DeviceInterrupt(void) interrupt INT_NO_USB using 1 // USB中断服务程序,使用寄存器组1
{
    UINT16 len;
    if (UIF_TRANSFER) // USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
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
                        switch (UsbSetupBuf->wValueL)
                        {
                        case HID_PD_PRESENTSTATUS:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = batteryPresentStatus & 0x00ff;
                            UserReportBuf[2] = (batteryPresentStatus & 0xff00) >> 8;
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_RUNTIMETOEMPTY:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = runtimeToEmpty & 0x00ff;
                            UserReportBuf[2] = (runtimeToEmpty & 0xff00) >> 8;
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_RECHARGEABLE:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = 1;
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_CAPACITYMODE:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = 2;   // Capacity in %
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_CONFIGVOLTAGE:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = designVolt & 0x00ff;
                            UserReportBuf[2] = (designVolt & 0xff00) >> 8;
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_VOLTAGE:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = currentVolt & 0x00ff;
                            UserReportBuf[2] = (currentVolt & 0xff00) >> 8;
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_DESIGNCAPACITY:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = 100; // Capacity in %
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        case HID_PD_FULLCHRGECAPACITY:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = batteryHealth; // Capacity in %
                            pDescr = UserReportBuf;           // 控制端点上传输据
                            break;
                        case HID_PD_REMAININGCAPACITY:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = batteryRSOC; // Capacity in %
                            pDescr = UserReportBuf;         // 控制端点上传输据
                            break;
                        case HID_PD_CPCTYGRANULARITY1:
                        case HID_PD_CPCTYGRANULARITY2:
                            UserReportBuf[0] = UsbSetupBuf->wValueL;
                            UserReportBuf[1] = 1;
                            pDescr = UserReportBuf; // 控制端点上传输据
                            break;
                        default:
                            len = 0;
                        }
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
            if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) /*HID类命令*/
            {
                ;
            }
            UEP0_CTRL ^= bUEP_R_TOG; // 同步标志位翻转
            break;
        case UIS_TOKEN_SOF:
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0; // 写0清空中断
    }
    if (UIF_BUS_RST) // 设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
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

sbit PIN_LED = P1 ^ 5;

main()
{
    CfgFsys();
    mDelaymS(5);     // 修改主频等待内部晶振稳定,必加
    USBDeviceInit(); // USB设备模式初始化
    EA = 1;          // 允许单片机中断
    UEP2_T_LEN = 0;  // 预使用发送长度一定要清空
    FLAG = 0;
    Ready = 0;
    Port1Cfg(1, 5);
    bq_Init();
    if (!bq_testComm())
    {
        mDelaymS(500);
        while (!bq_testComm())
        {
            swi2c_bus_reset();
            mDelaymS(500);
            PIN_LED = !(PIN_LED);
        }
    }
    PIN_LED = 0;

    while (1)
    {
        if (Ready)
        {
            ;
            mDelaymS(100);
        }
        mDelaymS(100); // 模拟单片机做其它事
    }
}
