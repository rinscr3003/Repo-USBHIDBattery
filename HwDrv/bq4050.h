#ifndef __BQ4050_H__
#define __BQ4050_H__

#include "../Lib/CH552.H"
#include "../Lib/Debug.H"

void bq_Init();
UINT8 bq_testComm();                 // Return CommTest result
UINT8 bq_BattState();                // Return CHG/DSG(0xF?/0x0?), OK/Bad(0x?0/0x?F)
UINT16 bq_GetAdvState();             // Return XDSG/XCHG/PF/SS/FC/FD/TAPR/VCT/CB/SMTH/SU/IN/VDQ/FCCX/EDV2/EDV1
UINT16 bq_GetDesignVoltage();        // Unit: mV
UINT16 bq_GetVoltage();              // Unit: mV
UINT8 bq_GetRSOC();                  // Unit: %
UINT16 bq_GetR2E();                  // Unit: min
UINT16 bq_GetT2E();                  // Unit: min
UINT16 bq_GetT2F();                  // Unit: min
UINT8 bq_GetHealth();                // Unit: %
UINT16 bq_GetCellVolt(UINT8 cellNo); // Unit: mV
UINT16 bq_GetCycleCnt();

void bq_GetLifetimeBlock(UINT8 blockNo, UINT8 *blockBuf);

#endif
