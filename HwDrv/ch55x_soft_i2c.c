#include "ch55x_soft_i2c.h"
#include "../Lib/CH552.H"
#include "../Lib/Debug.H"

#pragma NOAREGS

sbit PIN_SDA = P1 ^ 6;
sbit PIN_SCL = P1 ^ 7;

// about 100K
#define DELAY_TIMES (2)

#define Delay_us(x) mDelayuS(x)

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
void Port1Cfg(UINT8 Mode, UINT8 Pin)
{
    switch (Mode)
    {
    case 0:
        P1_MOD_OC = P1_MOD_OC & ~(1 << Pin);
        P1_DIR_PU = P1_DIR_PU & ~(1 << Pin);
        break;
    case 1:
        P1_MOD_OC = P1_MOD_OC & ~(1 << Pin);
        P1_DIR_PU = P1_DIR_PU | (1 << Pin);
        break;
    case 2:
        P1_MOD_OC = P1_MOD_OC | (1 << Pin);
        P1_DIR_PU = P1_DIR_PU & ~(1 << Pin);
        break;
    case 3:
        P1_MOD_OC = P1_MOD_OC | (1 << Pin);
        P1_DIR_PU = P1_DIR_PU | (1 << Pin);
        break;
    default:
        break;
    }
}

void swi2c_init(void)
{
    Port1Cfg(3, 6);
    Port1Cfg(3, 7);
    PIN_SDA = 1;
    PIN_SCL = 1;
}

void swi2c_start(void)
{
    PIN_SDA = 1;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 1;
    Delay_us(DELAY_TIMES);
    PIN_SDA = 0;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 0;
    Delay_us(DELAY_TIMES);
}

void swi2c_stop(void)
{
    PIN_SDA = 0;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 1;
    Delay_us(DELAY_TIMES);
    PIN_SDA = 1;
    Delay_us(DELAY_TIMES);
}

void swi2c_bus_reset(void)
{
	UINT8 i;
    Port1Cfg(1, 6);
    Port1Cfg(1, 7);

    PIN_SDA = 1;
    for (i = 0; i < 9; i++)
    {
        mDelayuS(20);
        PIN_SCL = 0;
        mDelayuS(20);
        PIN_SCL = 1;
    }
    mDelayuS(20);
    PIN_SCL = 0;
    PIN_SDA = 0;
    Port1Cfg(1, 6);
    Port1Cfg(1, 7);

    mDelayuS(20);
    PIN_SCL = 1;
    mDelayuS(20);
    PIN_SDA = 1;
    swi2c_init();
}

UINT8 swi2c_wait_ack(void)
{
    UINT8 ucErrTime = 0;
    PIN_SDA = 1;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 1;
    Delay_us(DELAY_TIMES);
    while (PIN_SDA == 1)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            swi2c_stop();
            return 1;
        }
    }
    PIN_SCL = 0;
    Delay_us(DELAY_TIMES);
    return 0;
}

void swi2c_ack(void)
{
    PIN_SDA = 0;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 1;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 0;
}

void swi2c_nack(void)
{
    PIN_SCL = 0;
    PIN_SDA = 1;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 1;
    Delay_us(DELAY_TIMES);
    PIN_SCL = 0;
}

void swi2c_send_byte(UINT8 txd)
{
    UINT8 t;
    PIN_SCL = 0;
    for (t = 0; t < 8; t++)
    {
        if ((txd & 0x80) == 0x80)
        {
            PIN_SDA = 1;
        }
        else
        {
            PIN_SDA = 0;
        }
        txd <<= 1;
        Delay_us(DELAY_TIMES);
        PIN_SCL = 1;
        Delay_us(DELAY_TIMES);
        PIN_SCL = 0;
        Delay_us(DELAY_TIMES);
    }
    PIN_SDA = 1;
}

UINT8 swi2c_read_byte(UINT8 ack)
{
    UINT8 i, receive = 0;
    PIN_SDA = 1;
    for (i = 0; i < 8; i++)
    {
        PIN_SCL = 0;
        Delay_us(DELAY_TIMES);
        PIN_SCL = 1;
        Delay_us(DELAY_TIMES);
        receive <<= 1;
        if (PIN_SDA == 1)
        {
            receive++;
        }
        Delay_us(DELAY_TIMES);
    }
    PIN_SCL = 0;
    if (!ack)
        swi2c_nack(); // nACK
    else
        swi2c_ack(); // ACK
    return receive;
}

void swi2c_single_write(UINT8 daddr, UINT8 addr, UINT8 buf)
{
    swi2c_start();
    swi2c_send_byte(daddr);
    swi2c_wait_ack();
    swi2c_send_byte(addr);
    swi2c_wait_ack();
    swi2c_send_byte(buf);
    swi2c_wait_ack();
    swi2c_stop();
}

UINT8 swi2c_single_read(UINT8 daddr, UINT8 addr)
{
    UINT8 buf;
    swi2c_start();
    swi2c_send_byte(daddr);
    swi2c_wait_ack();
    swi2c_send_byte(addr);
    swi2c_wait_ack();
    swi2c_start();
    swi2c_send_byte(daddr + 1);
    swi2c_wait_ack();
    buf = swi2c_read_byte(0); // no ack
    swi2c_stop();
    return buf;
}
