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
*   ---------   -------------   ---------------------------------------------------------------------------
*   1.04.2023   Serhii Shahan   Initialization of MegaMng class and methods prototypes
*
*  \endcond
***********************************************************************************************************/

#ifndef _MEGA_MNG_H_
#define _MEGA_MNG_H_

#include "packuart.h"

typedef void (*MM_FUNC_PTR)(void);

typedef enum
{
    FIRST_EVENT = 0,
    CONFIG_EVENT,
    EVENT_2,
    //EVENT_3,
    LAST_EVENT
}MM_EVENT;

typedef enum
{
    FIRST_STATE = 0,
    IDLE,
    CONFIG_HW_MODULES,
    READY_TO_CAPTURE_DATA,
    CAPTURE_DATA_FIRST_SENSOR,
    CAPTURE_DATA_SECOND_SENSOR,
    CAPTURE_DATA_BOTH_SENSORS,
    SEND_CAPTURED_DATA,
    LAST_STATE
}MM_STATE;

typedef struct
{
    MM_STATE State;
    MM_EVENT event;
    MM_STATE nextState;
    MM_FUNC_PTR funcPtr;
}MM_TABLE;

class MegaMng
{
public:
    MegaMng();
    ~MegaMng();

    virtual MM_EVENT    checkInstructionType    (const quint8 (&buff)[BUFF_LENGTH]);
    virtual void        executeFSM              (MM_EVENT event);

    // State Machine methods
    static  void        hardwareConfiguration   (void);

    //Getters
    virtual MM_STATE    getCurrentState         (void)  const;

    //Setters
    virtual void        setCurrentState         (MM_STATE state);
private:
    MM_STATE current_state_;
};



#endif