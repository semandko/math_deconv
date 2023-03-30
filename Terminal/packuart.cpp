#include "packuart.h"

PackUART::PackUART()
{

}

PackUART::~PackUART()
{
	
}

void PackUART::packInstruction(const quint8& instruction, quint8 (&buff)[BUFF_LENGTH])
{
	// 16 bytes of parcel
	buff[0] = START_FRAME;
	buff[1] = instruction;
	buff[14] = STOP_FRAME;
	buff[15] = END_FRAME;
}

void PackUART::packConfig(AnalogTypeDef (&element)[SIZE_OF_SYGNALS])
{
	
	const DefineConfigPack* config;
	quint32 size = sizeof(ConfigPakage)/sizeof(ConfigPakage[0]);
		
    for (quint32 i = 0; i < size; i++)
    {
        config = &(ConfigPakage[i]);
		
        element[config->sygnal].msb         = config->msb;       	// quint32
        element[config->sygnal].bitlength   = config->bitlength;  	// quint32
        element[config->sygnal].offset      = config->offset;     	// quint8
        element[config->sygnal].maxCode     = config->maxCode;   	// quint32
        element[config->sygnal].alignment   = config->alignment;  	// quint32
        element[config->sygnal].dividerADC  = config->dividerADC; 	// float
        element[config->sygnal].coeff       = config->coeff;      	// float
    }
}

void PackUART::pack(AnalogTypeDef& as, quint8 (&buff)[BUFF_LENGTH])
{
	
	float value = as.value;
	
    value = (value)*(PackPower[as.bitlength]/(as.msb)); // Binary code transforming by msb
   
    as.output = static_cast<quint16>(value); 			// convert to two bytes
    
    as.output &= PackMask[as.bitlength]; 				// reduce the accuracy
    
    quint16* exBuff = (quint16*)buff; 					// use two bytes at ones
    
    exBuff[as.offset] &= ~(PackMask[as.bitlength]); 	// clearing the bits
    
    exBuff[as.offset] |= as.output; 					// put it in the result
}

void PackUART::unpack(AnalogTypeDef& as, quint8 (&buff)[BUFF_LENGTH])
{
	
	quint16* exBuff = (quint16*)buff;
    float value = exBuff[as.offset];
    
    as.valuemeasured = (value)*((static_cast<float>(as.msb))/(PackPower[as.bitlength]));
    
    as.code = static_cast<quint32>(((as.valuemeasured*as.maxCode)/(2*as.msb)));
}

