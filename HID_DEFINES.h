#ifndef __USBHIDDEF_H__
#define __USBHIDDEF_H__

// HID Request Type HID1.11 Page 51 7.2.1 Get_Report Request
#define HID_REPORT_TYPE_INPUT 1
#define HID_REPORT_TYPE_OUTPUT 2
#define HID_REPORT_TYPE_FEATURE 3

#define HID_PD_IPRODUCT 0x01     // FEATURE ONLY
#define HID_PD_SERIAL 0x02       // FEATURE ONLY
#define HID_PD_MANUFACTURER 0x03 // FEATURE ONLY
#define IPRODUCT 0x02
#define ISERIAL 0x03
#define IMANUFACTURER 0x01
#define IDEVICECHEMISTRY 0x04
#define IOEMVENDOR 0x00

#define HID_PD_RECHARGEABLE 0x06  // FEATURE ONLY
#define HID_PD_PRESENTSTATUS 0x07 // INPUT OR FEATURE(required by Windows)
#define HID_PD_REMAINTIMELIMIT 0x08
#define HID_PD_MANUFACTUREDATE 0x09
#define HID_PD_CONFIGVOLTAGE 0x0A     // 10 FEATURE ONLY
#define HID_PD_VOLTAGE 0x0B           // 11 INPUT (NA) OR FEATURE(implemented)
#define HID_PD_REMAININGCAPACITY 0x0C // 12 INPUT OR FEATURE(required by Windows)
#define HID_PD_RUNTIMETOEMPTY 0x0D
#define HID_PD_FULLCHRGECAPACITY 0x0E // 14 INPUT OR FEATURE. Last Full Charge Capacity
#define HID_PD_WARNCAPACITYLIMIT 0x0F
#define HID_PD_CPCTYGRANULARITY1 0x10
#define HID_PD_REMNCAPACITYLIMIT 0x11
#define HID_PD_CURRENT 0x15          // 21 FEATURE ONLY
#define HID_PD_CAPACITYMODE 0x16
#define HID_PD_DESIGNCAPACITY 0x17
#define HID_PD_CPCTYGRANULARITY2 0x18
#define HID_PD_AVERAGETIME2FULL 0x1A
#define HID_PD_AVERAGECURRENT 0x1B
#define HID_PD_AVERAGETIME2EMPTY 0x1C

#define HID_PD_IDEVICECHEMISTRY 0x1F // Feature
#define HID_PD_IOEMINFORMATION 0x20  // Feature

// PresenStatus dynamic flags
#define PRESENTSTATUS_CHARGING 0x00
#define PRESENTSTATUS_DISCHARGING 0x01
#define PRESENTSTATUS_ACPRESENT 0x02
#define PRESENTSTATUS_BATTPRESENT 0x03
#define PRESENTSTATUS_BELOWRCL 0x04
#define PRESENTSTATUS_RTLEXPIRED 0x05
#define PRESENTSTATUS_NEEDREPLACE 0x06
#define PRESENTSTATUS_VOLTAGENR 0x07
#define PRESENTSTATUS_FULLCHARGE 0x08
#define PRESENTSTATUS_FULLDISCHARGE 0x09
#define PRESENTSTATUS_SHUTDOWNREQ 0x0A
#define PRESENTSTATUS_SHUTDOWNIMNT 0x0B
#define PRESENTSTATUS_COMMLOST 0x0C
#define PRESENTSTATUS_OVERLOAD 0x0D

#endif
