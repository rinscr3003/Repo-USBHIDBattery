#include "bq4050.h"
#include "../Lib/CH552.H"
#include "../Lib/Debug.H"
#include "ch55x_soft_i2c.h"

#pragma NOAREGS

void bq_Init()
{
    swi2c_init();
}

void Delay10us()
{
    mDelayuS(10);
}

#define BQ_ADDR 0x17

void WriteNbyte(UINT8 addr, PUINT8 bytes, UINT8 bytesLen)
{
    UINT8 i;
    swi2c_start();
    swi2c_send_byte(BQ_ADDR & 0xFE);
    swi2c_wait_ack();
    swi2c_send_byte(addr);
    swi2c_wait_ack();
    for (i = 0; i < bytesLen; i++)
    {
        swi2c_send_byte(bytes[i]);
        swi2c_wait_ack();
    }
    swi2c_stop();
}

void ReadNbyte(UINT8 addr, PUINT8 bytes, UINT8 bytesLen)
{
    UINT8 i;
    swi2c_start();
    swi2c_send_byte(BQ_ADDR & 0xFE);
    swi2c_wait_ack();
    swi2c_send_byte(addr);
    swi2c_wait_ack();
    swi2c_start();
    swi2c_send_byte(BQ_ADDR | 0x01);
    swi2c_wait_ack();
    for (i = 0; i < bytesLen; i++)
        bytes[i] = swi2c_read_byte(i != bytesLen - 1); // no ack
    swi2c_stop();
}

UINT8 bq_testComm()
{
    UINT8 res;
    swi2c_start();
    swi2c_send_byte(BQ_ADDR & 0xFE);
    res = !swi2c_wait_ack();
    if (res)
        swi2c_stop();
    return res;
}

void bq_MACReadBlock(UINT8 *inBlock, UINT8 inLen, UINT8 *outBlock, UINT8 outLen)
{
    while (1)
    {
        WriteNbyte(0x44, inBlock, inLen);
        Delay10us();
        ReadNbyte(0x44, outBlock, outLen);
        if (inBlock[1] == outBlock[1] && inBlock[2] == outBlock[2])
        {
            break;
        }
    }
}

UINT8 bq_BattState() // Return CHG/DSG(0xC?/0x0?), OK/Bad(0x0?/0x3?), TCTDFCFD(bit3 2 1 0)
{
    UINT8 battStatus[2], battDataBuf[7], ret = 0x00;
    UINT8 battCmd[3] = {0x02, 0x50, 0x00}; // SafetyAlert MAC Cmd
    ReadNbyte(0x16, battStatus, 2);
    ret |= (((battStatus[0] & 0x40) == 0x40) ? 0x00 : 0xc0);
    bq_MACReadBlock(battCmd, 3, battDataBuf, 7);
    battDataBuf[6] &= 0x0f; // Clear RSVD
    battDataBuf[5] &= 0xfd;
    battDataBuf[4] &= 0x7a;
    battDataBuf[3] &= 0xbf;
    if ((battDataBuf[3] | battDataBuf[4] | battDataBuf[5] | battDataBuf[6]) != 0)
    {
        ret |= 0x30;
        return ret;
    }
    battCmd[1] = 0x54;
    bq_MACReadBlock(battCmd, 3, battDataBuf, 7);
    if ((battDataBuf[4] & 0x60) != 0)
    {
        ret |= 0x30;
    }
    battCmd[1] = 0x51;
    bq_MACReadBlock(battCmd, 3, battDataBuf, 7);
    battDataBuf[6] &= 0x0f; // Clear RSVD
    battDataBuf[5] &= 0xd5;
    battDataBuf[4] &= 0x7f;
    battDataBuf[3] &= 0xff;
    if ((battDataBuf[3] | battDataBuf[4] | battDataBuf[5] | battDataBuf[6]) != 0)
    {
        ret |= 0x30;
    }
    battCmd[1] = 0x56;
    bq_MACReadBlock(battCmd, 3, battDataBuf, 5);
    ret |= (battDataBuf[3] & 0x0f);
    return ret;
}

UINT16 bq_GetAdvState()
{ // Return XDSG/XCHG/PF/SS  FC/FD/TAPR/VCT  CB/SMTH/SU/IN  VDQ/FCCX/EDV2/EDV1
    UINT16 ret = 0x0000;
    UINT8 battDataBuf[7], battCmd[3] = {0x02, 0x54, 0x00}; // OperationStatus MAC Cmd
    bq_MACReadBlock(battCmd, 3, battDataBuf, 7);
    ret |= ((battDataBuf[3] & 0x40) == 0x40) ? (UINT16)1 << 6 : 0;  // SMTH
    ret |= ((battDataBuf[4] & 0x08) == 0x08) ? (UINT16)1 << 12 : 0; // SS
    ret |= ((battDataBuf[4] & 0x10) == 0x10) ? (UINT16)1 << 13 : 0; // PF
    ret |= ((battDataBuf[4] & 0x20) == 0x20) ? (UINT16)1 << 15 : 0; // XDSG
    ret |= ((battDataBuf[4] & 0x40) == 0x40) ? (UINT16)1 << 14 : 0; // XCHG
    ret |= ((battDataBuf[6] & 0x10) == 0x10) ? (UINT16)1 << 7 : 0;  // CB
    battCmd[1] = 0x55;                                              // ChargingStatus MAC Cmd
    bq_MACReadBlock(battCmd, 3, battDataBuf, 6);
    ret |= ((battDataBuf[3] & 0x10) == 0x10) ? (UINT16)1 << 4 : 0; // IN
    ret |= ((battDataBuf[3] & 0x20) == 0x20) ? (UINT16)1 << 5 : 0; // SU
    ret |= ((battDataBuf[3] & 0x80) == 0x80) ? (UINT16)1 << 8 : 0; // VCT
    ret |= ((battDataBuf[4] & 0x80) == 0x80) ? (UINT16)1 << 9 : 0; // TAPR
    battCmd[1] = 0x56;                                             // GaugingStatus MAC Cmd
    bq_MACReadBlock(battCmd, 3, battDataBuf, 5);
    ret |= ((battDataBuf[3] & 0x01) == 0x01) ? (UINT16)1 << 10 : 0; // FD
    ret |= ((battDataBuf[3] & 0x02) == 0x02) ? (UINT16)1 << 11 : 0; // FC
    ret |= ((battDataBuf[4] & 0x04) == 0x04) ? (UINT16)1 << 2 : 0;  // FCCX
    ret |= ((battDataBuf[4] & 0x20) == 0x20) ? (UINT16)1 << 0 : 0;  // EDV1
    ret |= ((battDataBuf[4] & 0x40) == 0x40) ? (UINT16)1 << 1 : 0;  // EDV2
    ret |= ((battDataBuf[4] & 0x80) == 0x80) ? (UINT16)1 << 3 : 0;  // VDQ

    return ret;
}

UINT16 bq_GetDesignVoltage()
{ // Unit: mV
    UINT8 battBuf[2];
    UINT16 battVolt;
    ReadNbyte(0x19, battBuf, 2);
    battVolt = (battBuf[1] << 8) + battBuf[0];
    return battVolt;
}

UINT16 bq_GetVoltage()
{ // Unit: mV
    UINT8 battBuf[2];
    UINT16 battVolt;
    ReadNbyte(0x09, battBuf, 2);
    battVolt = (battBuf[1] << 8) + battBuf[0];
    return battVolt;
}

UINT8 bq_GetRSOC()
{ // Unit: %
    UINT8 battBuf[2];
    ReadNbyte(0x0D, battBuf, 2);
    return battBuf[0];
}

UINT16 bq_GetR2E()
{ // Unit: min
    UINT8 battBuf[2];
    UINT16 battT2E;
    ReadNbyte(0x11, battBuf, 2);
    battT2E = (battBuf[1] << 8) + battBuf[0];
    return battT2E;
}

UINT16 bq_GetT2E()
{ // Unit: min
    UINT8 battBuf[2];
    UINT16 battT2E;
    ReadNbyte(0x12, battBuf, 2);
    battT2E = (battBuf[1] << 8) + battBuf[0];
    return battT2E;
}

UINT16 bq_GetT2F()
{ // Unit: min
    UINT8 battBuf[2];
    UINT16 battT2F;
    ReadNbyte(0x13, battBuf, 2);
    battT2F = (battBuf[1] << 8) + battBuf[0];
    return battT2F;
}

UINT8 bq_GetHealth()
{ // Unit: %
    UINT8 battBuf[2];
    UINT16 battFCC, battDC;
    float battHealth;
    ReadNbyte(0x10, battBuf, 2);
    battFCC = (battBuf[1] << 8) + battBuf[0];
    ReadNbyte(0x18, battBuf, 2);
    battDC = (battBuf[1] << 8) + battBuf[0];
    battHealth = ((float)battFCC * 100.0f / (float)battDC);
    return (battHealth >= 100.0f ? 100 : (UINT8)battHealth);
}

UINT16 bq_GetCellVolt(UINT8 cellNo)
{ // Unit: mV
    UINT8 battBuf[2], cmd = 0x40;
    UINT16 battCellVolt;
    if (cellNo > 4 || cellNo < 1)
        return 0;
    cmd -= cellNo;
    ReadNbyte(cmd, battBuf, 2);
    battCellVolt = (battBuf[1] << 8) + battBuf[0];
    return battCellVolt;
}

UINT16 bq_GetCycleCnt()
{
    UINT8 battBuf[2];
    UINT16 battCycleCnt;
    ReadNbyte(0x17, battBuf, 2);
    battCycleCnt = (battBuf[1] << 8) + battBuf[0];
    return battCycleCnt;
}

void bq_GetLifetimeBlock(UINT8 blockNo, UINT8 *blockBuf)
{
    UINT8 blockBufLen = 3, battCmd[3] = {0x02, 0x60, 0x00}; // LifetimeDataBlockN MAC Cmd
    battCmd[1] += (blockNo - 1);
    switch (blockNo)
    {
    case 1:
    case 4:
        blockBufLen += 32;
        break;
    case 2:
        blockBufLen += 8;
        break;
    case 3:
        blockBufLen += 16;
        break;
    case 5:
        blockBufLen += 20;
        break;
    default:
        return;
    }
    bq_MACReadBlock(battCmd, 3, blockBuf, blockBufLen);
}
