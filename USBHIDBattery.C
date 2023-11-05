#include "./Lib/CH552.H"
#include "./Lib/Debug.H"
#include <stdio.h>
#include <string.h>

#include "HID_DEFINES.h"

#define THIS_ENDP0_SIZE 64
#define ENDP2_IN_SIZE 64
#define ENDP2_OUT_SIZE 64

UINT8X Ep0Buffer[MIN(64, THIS_ENDP0_SIZE + 2)] _at_ 0x0000;                                                   // �˵�0 OUT&IN��������������ż��ַ
UINT8X Ep2Buffer[MIN(64, ENDP2_IN_SIZE + 2) + MIN(64, ENDP2_OUT_SIZE + 2)] _at_ MIN(64, THIS_ENDP0_SIZE + 2); // �˵�2 IN&OUT������,������ż��ַ

UINT8 SetupReq, Ready, Count, FLAG, UsbConfig;
UINT16 SetupLen;
PUINT8 pDescr;             // USB���ñ�־
USB_SETUP_REQ SetupReqBuf; // �ݴ�Setup��
#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

#pragma NOAREGS
/*�豸������*/
UINT8C DevDesc[18] = {
    0x12,            // ��������С
    0x01,            // ����������
    0x10, 0x01,      // USB�淶�汾�ţ�USB1.1��
    0x00,            // �豸�ࣨHID�豸������ȫ0��
    0x00,            // �豸����
    0x00,            // �豸Э��
    THIS_ENDP0_SIZE, // EP0��С
    0xb2, 0x38,      // VID
    0x01, 0x80,      // PID
    0x01, 0x00,      // �豸�汾��
    IMANUFACTURER,   // �������ַ�������������
    IPRODUCT,        // ��Ʒ�ַ�������������
    ISERIAL,         // ���кŲ�Ʒ�ַ�������������
    0x01             // ������Ŀ
};

UINT8C CfgDesc[] =
    {
        // ����������
        0x09,       // ��������С
        0x02,       // ���������ͣ���������������
        0x29, 0x00, // �����ܳ���
        0x01,       // ���ýӿ���
        0x01,       // ����ֵ
        0x00,       // �����ַ�������������
        0xA0,       // ����������
        0x32,       // ����100mA

        // �ӿ�������
        0x09, // ��������С
        0x04, // ���������ͣ��ǽӿ���������
        0x00, // �ӿڱ��
        0x00, // �ӿ��������ã�����
        0x02, // �۳��˵�0��Ķ˵���
        0x03, // �ӿ��ࣨ��HID��
        0x00, // �ӿ����ࣨ�ǲ�֧����������ģ�
        0x00, // ��֧���������񣬴�����Ч
        0x00, // �ӿ��ַ�������������

        // HID��������
        0x09,       // ��������С
        0x21,       // ���������ͣ���HID����������
        0x00, 0x01, // HID�汾����1.0��
        0x00,       // ���Ҵ���
        0x01,       // �¹�HID����������
        0x22,       // �¹�������1���ͣ��Ǳ�����������
        0x77, 0x01, // �¹�������1����

        // �˵�������
        0x07,                // ��������С
        0x05,                // ���������ͣ��Ƕ˵���������
        0x82,                // �˵�ż����������Ϣ����EP2IN��
        0x03,                // �˵㴫�����ͣ����жϴ��䣩
        ENDP2_IN_SIZE, 0x00, // �˵��С
        0x01,                // �˵���ѯ�������1֡��

        // �˵�������
        0x07,                 // ��������С
        0x05,                 // ���������ͣ��Ƕ˵���������
        0x02,                 // �˵�ż����������Ϣ����EP2OUT��
        0x03,                 // �˵㴫�����ͣ����жϴ��䣩
        ENDP2_OUT_SIZE, 0x00, // �˵��С
        0x01                  // �˵���ѯ�������1֡��
};
/*�ַ���������*/
UINT8C LangDes[] = {
    // ����������
    0x04,      // ��������С
    0x03,      // ���������ͣ����ַ�����������
    0x09, 0x04 // ���Ա��루��Ӣ�
};
UINT8C StrDes1[] = {
    // �ַ��������� ������
    0x0A,                                      // ��������С
    0x03,                                      // ���������ͣ����ַ�����������
    'M', 0x00, 'Z', 0x00, 'Y', 0x00, '7', 0x00 // �ַ�������
};
UINT8C StrDes2[] = {
    // �ַ��������� ��Ʒ
    0x20, // ��������С
    0x03, // ���������ͣ����ַ�����������
    'U', 0x00, 'S', 0x00, 'B', 0x00, ' ', 0x00, 'H', 0x00, 'I', 0x00, 'D', 0x00, ' ', 0x00,
    'B', 0x00, 'a', 0x00, 't', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'y', 0x00 // �ַ�������
};
UINT8C StrDes3[] = {
    // �ַ��������� ���к�
    0x1C, // ��������С
    0x03, // ���������ͣ����ַ�����������
    'T', 0x00, 'S', 0x00, 'H', 0x00, 'E', 0x00, '-', 0x00,
    '0', 0x00, '0', 0x00, '1', 0x00, '1', 0x00, '4', 0x00, '5', 0x00, '1', 0x00, '4', 0x00, // �ַ�������
};
UINT8C StrDes4[] = {
    // �ַ��������� HID-��ѧ����
    0x0A,                                      // ��������С
    0x03,                                      // ���������ͣ����ַ�����������
    'L', 0x00, 'I', 0x00, 'O', 0x00, 'N', 0x00 // �ַ�������
};

/*HID�౨��������*/
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

    0x85, HID_PD_WARNCAPACITYLIMIT, //     REPORT_ID (15)
    0x09, 0x8C,                     //     USAGE (WarningCapacityLimit)
    0xB1, 0xA2,                     //     FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_REMNCAPACITYLIMIT, //     REPORT_ID (17)
    0x09, 0x29,                     //     USAGE (RemainingCapacityLimit)
    0xB1, 0xA2,                     //     FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_MANUFACTUREDATE, //     REPORT_ID (9)
    0x09, 0x85,                   //     USAGE (ManufacturerDate)
    0x75, 0x10,                   //     REPORT_SIZE (16)
    0x27, 0xFF, 0xFF, 0x00, 0x00, //     LOGICAL_MAXIMUM (65534)
    0xB1, 0xA3,                   //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

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

    0x85, HID_PD_REMAINTIMELIMIT, //     REPORT_ID (8)
    0x09, 0x2A,                   //     USAGE (RemainingTimeLimit)
    0x75, 0x10,                   //     REPORT_SIZE (16)
    0x27, 0x64, 0x05, 0x00, 0x00, //     LOGICAL_MAXIMUM (1380)
    0x16, 0x78, 0x00,             //     LOGICAL_MINIMUM (120)
    0x81, 0x22,                   //     INPUT (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x2A,                   //     USAGE (RemainingTimeLimit)
    0xB1, 0xA2,                   //     FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)
    0x05, 0x84,                   //     USAGE_PAGE (Power Device) ====================

    0x85, HID_PD_DELAYBE4SHUTDOWN, //     REPORT_ID (18)
    0x09, 0x57,                    //     USAGE (DelayBeforeShutdown)
    0x16, 0x00, 0x80,              //     LOGICAL_MINIMUM (-32768)
    0x27, 0xFF, 0x7F, 0x00, 0x00,  //     LOGICAL_MAXIMUM (32767)
    0xB1, 0xA2,                    //     FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_DELAYBE4REBOOT, //     REPORT_ID (19)
    0x09, 0x55,                  //     USAGE (DelayBeforeReboot)
    0xB1, 0xA2,                  //     FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x85, HID_PD_CONFIGVOLTAGE,   //     REPORT_ID (10)
    0x09, 0x40,                   //     USAGE (ConfigVoltage)
    0x15, 0x00,                   //     LOGICAL_MINIMUM (0)
    0x27, 0xFF, 0xFF, 0x00, 0x00, //     LOGICAL_MAXIMUM (65535)
    0x67, 0x21, 0xD1, 0xF0, 0x00, //     UNIT (Centivolts)
    0x55, 0x05,                   //     UNIT_EXPONENT (5)
    0xB1, 0x23,                   //     FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Nonvolatile, Bitfield)

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

UINT8X UserEp2Buf[64]; // �û����ݶ���
UINT8 Endp2Busy = 0;

/*******************************************************************************
 * Function Name  : USBDeviceInit()
 * Description    : USB�豸ģʽ����,�豸ģʽ�������շ��˵����ã��жϿ���
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USBDeviceInit()
{
    IE_USB = 0;
    USB_CTRL = 0x00;             // ���趨USB�豸ģʽ
    UDEV_CTRL = bUD_PD_DIS;      // ��ֹDP/DM��������
    UDEV_CTRL &= ~bUD_LOW_SPEED; // ѡ��ȫ��12Mģʽ��Ĭ�Ϸ�ʽ
    USB_CTRL &= ~bUC_LOW_SPEED;
    UEP2_DMA = Ep2Buffer;                       // �˵�2���ݴ����ַ
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;    // �˵�2���ͽ���ʹ��
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;               // �˵�2�շ���64�ֽڻ�����
    UEP0_DMA = Ep0Buffer;                       // �˵�0���ݴ����ַ
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN); // �˵�0��64�ֽ��շ�������
    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN; // ����USB�豸��DMA�����ж��ڼ��жϱ�־δ���ǰ�Զ�����NAK
    UDEV_CTRL |= bUD_PORT_EN;                              // ����USB�˿�
    USB_INT_FG = 0xFF;                                     // ���жϱ�־
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}

/*******************************************************************************
 * Function Name  : Enp2BlukIn()
 * Description    : USB�豸ģʽ�˵�2�������ϴ�
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Enp2BlukIn()
{
    memcpy(Ep2Buffer + MAX_PACKET_SIZE, UserEp2Buf, sizeof(UserEp2Buf)); // �����ϴ�����
    UEP2_T_LEN = THIS_ENDP0_SIZE;                                        // �ϴ���������
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;             // ������ʱ�ϴ����ݲ�Ӧ��ACK
}

/*******************************************************************************
 * Function Name  : DeviceInterrupt()
 * Description    : CH559USB�жϴ�������
 *******************************************************************************/
void DeviceInterrupt(void) interrupt INT_NO_USB using 1 // USB�жϷ������,ʹ�üĴ�����1
{
    UINT8 i;
    UINT16 len;
    if (UIF_TRANSFER) // USB������ɱ�־
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:       // endpoint 2# �˵������ϴ�
            UEP2_T_LEN = 0;          // Ԥʹ�÷��ͳ���һ��Ҫ���
            UEP2_CTRL ^= bUEP_T_TOG; // �ֶ���ת
            Endp2Busy = 0;
            UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK; // Ĭ��Ӧ��NAK
            break;
        case UIS_TOKEN_OUT | 2: // endpoint 2# �˵������´�
            if (U_TOG_OK)       // ��ͬ�������ݰ�������
            {
                len = USB_RX_LEN;        // �������ݳ��ȣ����ݴ�Ep2Buffer�׵�ַ��ʼ���
                UEP2_CTRL ^= bUEP_R_TOG; // �ֶ���ת
                for (i = 0; i < len; i++)
                {
                    Ep2Buffer[MAX_PACKET_SIZE + i] = Ep2Buffer[i] ^ 0xFF; // OUT����ȡ����IN�ɼ������֤
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; // �����ϴ�
            }
            break;
        case UIS_TOKEN_SETUP | 0: // SETUP����
            UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
            len = USB_RX_LEN;
            if (len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = ((UINT16)UsbSetupBuf->wLengthH << 8) | (UsbSetupBuf->wLengthL);
                len = 0; // Ĭ��Ϊ�ɹ������ϴ�0����
                SetupReq = UsbSetupBuf->bRequest;
                if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) /*HID������*/
                {
                    switch (SetupReq)
                    {
                    case 0x01:                           // GetReport
                        pDescr = UserEp2Buf;             // ���ƶ˵��ϴ����
                        if (SetupLen >= THIS_ENDP0_SIZE) // ���ڶ˵�0��С����Ҫ���⴦��
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
                    case 0x03: // GetProtocol
                        break;
                    case 0x09: // SetReport
                        break;
                    case 0x0A: // SetIdle
                        break;
                    case 0x0B: // SetProtocol
                        break;
                    default:
                        len = 0xFF; /*���֧��*/
                        break;
                    }
                    if (SetupLen > len)
                    {
                        SetupLen = len; // �����ܳ���
                    }
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // ���δ��䳤��
                    memcpy(Ep0Buffer, pDescr, len);                                 // �����ϴ�����
                    SetupLen -= len;
                    pDescr += len;
                }
                else // ��׼����
                {
                    switch (SetupReq) // ������
                    {
                    case USB_GET_DESCRIPTOR:
                        switch (UsbSetupBuf->wValueH)
                        {
                        case 1:                // �豸������
                            pDescr = &DevDesc; // ���豸�������͵�Ҫ���͵Ļ�����
                            len = sizeof(DevDesc);
                            break;
                        case 2:                // ����������
                            pDescr = &CfgDesc; // ���豸�������͵�Ҫ���͵Ļ�����
                            len = sizeof(CfgDesc);
                            break;
                        case 3: // �ַ���������
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
                                len = 0xFF; // ��֧�ֵ��ַ���������
                                break;
                            }
                            break;
                        case 0x22:               // ����������
                            pDescr = HIDRepDesc; // ����׼���ϴ�
                            len = sizeof(HIDRepDesc);
                            break;
                        default:
                            len = 0xff; // ��֧�ֵ�������߳���
                            break;
                        }
                        if (SetupLen > len)
                        {
                            SetupLen = len; // �����ܳ���
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // ���δ��䳤��
                        memcpy(Ep0Buffer, pDescr, len);                                 // �����ϴ�����
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL; // �ݴ�USB�豸��ַ
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
                            Ready = 1; // set config����һ�����usbö����ɵı�־
                        }
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                                         // Clear Feature
                        if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // �˵�
                        {
                            switch (UsbSetupBuf->wIndexL)
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF; // ��֧�ֵĶ˵�
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFF; // ���Ƕ˵㲻֧��
                        }
                        break;
                    case USB_SET_FEATURE:                               /* Set Feature */
                        if ((UsbSetupBuf->bRequestType & 0x1F) == 0x00) /* �����豸 */
                        {
                            if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01)
                            {
                                if (CfgDesc[7] & 0x20)
                                {
                                    /* ���û���ʹ�ܱ�־ */
                                }
                                else
                                {
                                    len = 0xFF; /* ����ʧ�� */
                                }
                            }
                            else
                            {
                                len = 0xFF; /* ����ʧ�� */
                            }
                        }
                        else if ((UsbSetupBuf->bRequestType & 0x1F) == 0x02) /* ���ö˵� */
                        {
                            if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00)
                            {
                                switch (((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL)
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; /* ���ö˵�2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; /* ���ö˵�2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; /* ���ö˵�1 IN STALL */
                                    break;
                                default:
                                    len = 0xFF; /* ����ʧ�� */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF; /* ����ʧ�� */
                            }
                        }
                        else
                        {
                            len = 0xFF; /* ����ʧ�� */
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
                        len = 0xff; // ����ʧ��
                        break;
                    }
                }
            }
            else
            {
                len = 0xff; // �����ȴ���
            }
            if (len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            }
            else if (len <= THIS_ENDP0_SIZE) // �ϴ����ݻ���״̬�׶η���0���Ȱ�
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Ĭ�����ݰ���DATA1������Ӧ��ACK
            }
            else
            {
                UEP0_T_LEN = 0;                                                      // ��Ȼ��δ��״̬�׶Σ�������ǰԤ���ϴ�0�������ݰ��Է�������ǰ����״̬�׶�
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // Ĭ�����ݰ���DATA1,����Ӧ��ACK
            }
            break;
        case UIS_TOKEN_IN | 0: // endpoint0 IN
            switch (SetupReq)
            {
            case USB_GET_DESCRIPTOR:
            case HID_GET_REPORT:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; // ���δ��䳤��
                memcpy(Ep0Buffer, pDescr, len);                                 // �����ϴ�����
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG; // ͬ����־λ��ת
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0; // ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
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
            UEP0_CTRL ^= bUEP_R_TOG; // ͬ����־λ��ת
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0; // д0����ж�
    }
    if (UIF_BUS_RST) // �豸ģʽUSB���߸�λ�ж�
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Endp2Busy = 0;
        UIF_BUS_RST = 0; // ���жϱ�־
    }
    if (UIF_SUSPEND) // USB���߹���/�������
    {
        UIF_SUSPEND = 0;
        if (USB_MIS_ST & bUMS_SUSPEND) // ����
        {
            ; // zzz
        }
    }
    else
    {                      // ������ж�,�����ܷ��������
        USB_INT_FG = 0xFF; // ���жϱ�־
    }
}

main()
{
    UINT8 i;
    CfgFsys();
    mDelaymS(5); // �޸���Ƶ�ȴ��ڲ������ȶ�,�ؼ�
    // mInitSTDIO( );                                                        //����0��ʼ��
    for (i = 0; i < 64; i++) // ׼����ʾ����
    {
        UserEp2Buf[i] = i;
    }
    USBDeviceInit(); // USB�豸ģʽ��ʼ��
    EA = 1;          // ������Ƭ���ж�
    UEP1_T_LEN = 0;  // Ԥʹ�÷��ͳ���һ��Ҫ���
    UEP2_T_LEN = 0;  // Ԥʹ�÷��ͳ���һ��Ҫ���
    FLAG = 0;
    Ready = 0;
    while (1)
    {
        if (Ready)
        {
            while (Endp2Busy)
                ; // ���æ����һ������û�д���ȥ������ȴ���
            // Endp2Busy = 1; // ����Ϊæ״̬
            // Enp2BlukIn();
            mDelaymS(100);
        }
        mDelaymS(100); // ģ�ⵥƬ����������
    }
}