#ifndef __USBDESC_H__
#define __USBDESC_H__

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
        0x1B, 0x00, // �����ܳ���
        0x01,       // ���ýӿ���
        0x01,       // ����ֵ
        0x00,       // �����ַ�������������
        0x80,       // ������������
        0x32,       // ����100mA

        // �ӿ�������
        0x09, // ��������С
        0x04, // ���������ͣ��ǽӿ���������
        0x00, // �ӿڱ��
        0x00, // �ӿ��������ã�����
        0x00, // �۳��˵�0��Ķ˵���
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
        0x43, 0x01, // �¹�������1����
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

    0x85, HID_PD_AVERAGETIME2FULL, //     REPORT_ID (26)
    0x09, 0x6A,                    //     USAGE (AverageTimeToFull)
    0x75, 0x10,                    //     REPORT_SIZE (16)
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

    0x05, 0x84,                   //     USAGE_PAGE (Power Device) ====================
    0x85, HID_PD_CONFIGVOLTAGE,   //     REPORT_ID (10)
    0x09, 0x40,                   //     USAGE (ConfigVoltage)
    0x75, 0x10,                   //     REPORT_SIZE (16)
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

    0x09, 0x02,                 //     USAGE (PresentStatus)
    0xA1, 0x02,                 //     COLLECTION (Logical)
    0x85, HID_PD_PRESENTSTATUS, //       REPORT_ID (7)
    0x05, 0x85,                 //       USAGE_PAGE (Battery System) =================
    0x09, 0x44,                 //       USAGE (Charging)
    0x75, 0x01,                 //       REPORT_SIZE (1)
    0x15, 0x00,                 //       LOGICAL_MINIMUM (0)
    0x25, 0x01,                 //       LOGICAL_MAXIMUM (1)
    0x65, 0x00,                 //       UNIT (0)
    0x55, 0x00,                 //       UNIT_EXPONENT (0)
    0x81, 0xA3,                 //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x44,                 //       USAGE (Charging)
    0xB1, 0xA3,                 //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x45, //       USAGE (Discharging)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x45, //       USAGE (Discharging)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0xD0, //       USAGE (ACPresent)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0xD0, //       USAGE (ACPresent)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0xD1, //       USAGE (BatteryPresent)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0xD1, //       USAGE (BatteryPresent)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x42, //       USAGE (BelowRemainingCapacityLimit)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x42, //       USAGE (BelowRemainingCapacityLimit)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x43, //       USAGE (RemainingTimeLimitExpired)
    0x81, 0xA2, //       INPUT (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x43, //       USAGE (RemainingTimeLimitExpired)
    0xB1, 0xA2, //       FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x4B, //       USAGE (NeedReplacement)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x4B, //       USAGE (NeedReplacement)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0xDB, //       USAGE (VoltageNotRegulated)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0xDB, //       USAGE (VoltageNotRegulated)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x46, //       USAGE (FullyCharged)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x46, //       USAGE (FullyCharged)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x47, //       USAGE (FullyDischarged)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x47, //       USAGE (FullyDischarged)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x05, 0x84, //       USAGE_PAGE (Power Device) =================
    0x09, 0x68, //       USAGE (ShutdownRequested)
    0x81, 0xA2, //       INPUT (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x68, //       USAGE (ShutdownRequested)
    0xB1, 0xA2, //       FEATURE (Data, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x69, //       USAGE (ShutdownImminent)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x69, //       USAGE (ShutdownImminent)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x73, //       USAGE (CommunicationLost)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x73, //       USAGE (CommunicationLost)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    0x09, 0x65, //       USAGE (Overload)
    0x81, 0xA3, //       INPUT (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Bitfield)
    0x09, 0x65, //       USAGE (Overload)
    0xB1, 0xA3, //       FEATURE (Constant, Variable, Absolute, No Wrap, Linear, No Preferred, No Null Position, Volatile, Bitfield)

    // pad 2 bits
    0x95, 0x02, //       REPORT_COUNT (2)
    0x81, 0x01, //       INPUT (Constant, Array, Absolute)
    0xB1, 0x01, //       FEATURE (Constant, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Nonvolatile, Bitfield)
    0xC0,       //     END_COLLECTION
    0xC0,       //   END_COLLECTION
    0xC0,       //   END_COLLECTION
};

#endif