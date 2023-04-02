/*
Semenenko Myk: semenenko_myk@ukr.net
*/


#ifndef _PACKUART_H_
#define _PACKUART_H_

#define quint8 unsigned char
#define quint16 unsigned short
#define qint32 int
#define quint32 unsigned int

#define NUMBER_POINTS           128 // number of points for data
#define BUFF_LENGTH             16 // length of the UART packet

#define START_FRAME             0x3A
#define STOP_FRAME              0x0D
#define END_FRAME               0x0A

#define START_FRAME_POS         0x00
#define INSTRUCTION_FRAME_POS   0x01
#define STOP_FRAME_POS          0x0E
#define END_FRAME_POS           0x0F


#define START_LUXMETER          14

#define CONFIG_INSTRUCTION      0xFD
#define TEST_INSTRUCTION2       0xFE
#define TEST_INSTRUCTION3       0xFF


const quint8 Instruction = START_LUXMETER;


typedef enum
{
    Default_GY1145      = 0,
    LUXMETER_UV         = 1,
    LUXMETER_VIS        = 2,
    LUXMETER_IR         = 3,

    //COMMAND_TYPE        = 4,
    
    SIZE_OF_SYGNALS
} BaseGY1145_TypeDef;

typedef struct
{
    float       divider;
    float       dividerADC;
    float       value;
    float       valuemeasured;
    float       coeff;

    quint32    code;
    quint32    msb;
    quint32    bitlength;
    qint32     output;
    quint32    maxCode;
    quint32    alignment;
    quint8     offset;
    quint8     sign;

} AnalogTypeDef;

typedef struct
{
    quint8    sygnal;
    quint32   msb;
    quint32   bitlength;
    quint8    offset; // size of the word is 2 bytes
    quint32   maxCode;
    quint32   alignment;
    float     dividerADC;
    float     coeff;
} DefineConfigPack;

const DefineConfigPack ConfigPakage[] =
{
    //quint8        //const quint32*    //quint32   //quint8   //quint32    //quint32  //float             //float
    //sygnal              msb            bitlength   offset      maxCode     Alignment   divider for ADC     coeff
    {LUXMETER_UV,   	 32768,         16,         1,         4095,       4095,        1,      			1},
    {LUXMETER_VIS,       32768,         16,         2,         4095,       4095,        1,      			1},
    {LUXMETER_IR,        32768,         16,         3,         4095,       4095,        1,      			1}
};

//const DefineConfigPack TestPackage[] =
//{
//    //quint8        //const quint32*    //quint32   //quint8   //quint32    //quint32  //float             //float
//    //sygnal              msb            bitlength   offset      maxCode     Alignment   divider for ADC     coeff
//    {COMMAND_TYPE,       32768,         16,         1,         4095,       4095,        1,                  1},
//};

//packing for 2 bytes by msb
//                             0      1      2      3      4      5      6      7      8      9     10     11     12     13     14     15     16     17 */
const quint32 PackPower[] = {0x0000,0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,0x8000};
const quint32 PackMask[] =  {0x0000,0x0001,0x0003,0x0007,0x000f,0x001f,0x003f,0x007f,0x00ff,0x01ff,0x03ff,0x07ff,0x0fff,0x1fff,0x3fff,0x7fff,0xffff};

class PackUART
{
public:

    explicit PackUART();
    virtual ~PackUART();

	virtual void packInstruction(const quint8& instruction, quint8 (&buff)[BUFF_LENGTH]);
    virtual void packConfig(AnalogTypeDef (&element)[SIZE_OF_SYGNALS]);
    virtual void pack(AnalogTypeDef& as, quint8 (&buff)[BUFF_LENGTH]);
    virtual void unpack(AnalogTypeDef& as, quint8 (&buff)[BUFF_LENGTH]);
    virtual bool isPacketStructureCorrect(const quint8 (&buff)[BUFF_LENGTH]);

private:
	float value_;

};

#endif // PACKUART_H
