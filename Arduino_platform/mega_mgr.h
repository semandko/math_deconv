/***********************************************************************************************************
*  \copyright 
*  \file mega_mgr.h
*  \brief file containes MegaMng class defenition and methonds prototypes declaration for Arduino Mega board
*  \author Serhii Shahan
*  \date 1.04.2023
*
*  \cond
*   Revision History
*   Date        Author          Description
*   ----------  -------------   ---------------------------------------------------------------------------
*   01.04.2023  Serhii Shahan   Initialization of MegaMng class and methods prototypes
*   28.04.2023  Serhii Shahan   Implement main functionalities of Mega Platform
*
*  \endcond
***********************************************************************************************************/

#ifndef _MEGA_MNG_H_
#define _MEGA_MNG_H_

#include "packuart.h"
#include "Adafruit_APDS9960.h"
#include "PWFusion_TCA9548A.h"

typedef enum
{
    FIRST_EVENT = 0,
    CONFIG_EVENT,
    CONFIG_SUCCESS_EVENT,
    CONFIG_FAILURE_EVENT,
    CAPTURE_DATA_APDS1_EVENT,
    CAPTURE_DATA_APDS2_EVENT,
    SEND_CAPTURED_DATA_EVENT,
    EVENT_2,
    //EVENT_3,
    LAST_EVENT
}MM_EVENT;

typedef enum
{
    FIRST_STATE = 0,
    IDLE,
    CONFIG_APDS_MODULES,
    READY_TO_CAPTURE_DATA,
    DATA_CAPTURED,
    LAST_STATE
}MM_STATE;

typedef struct
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
}RGBC_STRUCT;

class MegaMng
{
public:
    MegaMng();
    ~MegaMng();

    // Common methods    
    virtual MM_EVENT    checkInstructionType    (const quint8 (&buff)[SERIAL_BUFFER_LENGTH]);
    virtual void        executeFSM              (MM_EVENT event);
    virtual void        mainLoop                (void);
    virtual bool        checkForSerialData      (void);
    virtual bool        getSerialData           (void);
    virtual void        initMega                (void);
    virtual bool        selectI2cMuxChannel     (uint8_t channel);    

    // State Machine methods
    virtual void        apdsConfiguration       (void);
    virtual void        sendConfigFailure       (void);
    virtual void        sendConfigSuccess       (void);
    virtual void        captureDataAPDS1        (void);
    virtual void        captureDataAPDS2        (void);
    virtual void        sendCapturedData        (void);

    //Getters
    virtual MM_STATE    getCurrentState         (void)  const;

    //Setters
    virtual void        setCurrentState         (MM_STATE state);
private:
    MM_STATE            current_state_;
    quint8              serial_data_buffer_[SERIAL_BUFFER_LENGTH];
    PackUART            packer_uart_;
    Adafruit_APDS9960   apds1_;
    Adafruit_APDS9960   apds2_;
    TCA9548A            i2c_mux_;
    RGBC_STRUCT         rgbc_data_;
};

#endif