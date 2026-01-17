#ifndef CORSAT_H
#define CORSAT_H
#endif



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>



/*
* Library manager for a cubesat state machine
*
*
*/




/* ERRORS THAT CAN BE RETURNED */
#define COR_OK 0

#define COR_ERR_INVALID_PARAM 1
#define COR_ERR_DATE_CALC_ERROR 2






/* EPS DEFINITIONS */
/* specific channel or PDU is 3.3V */
#define COR_EPS_VOLT_3V3 0
/* specifc channel or PDU is 5V */
#define COR_EPS_VOLT_5V 1
/* specifc channel or PDU is 12V */
#define COR_EPS_VOLT_12V 2
/* specific channel or PDU is VBAT */
#define COR_EPS_VOLT_VBAT 3
/* specifc channel or PDU is 24V */
#define COR_EPS_VOLT_24V 4

/* channel on/off definitions */
#define COR_EPS_CH_ON 1
#define COR_EPS_CH_OFF 0

/* PDUs on/off definitions */
#define COR_EPS_PDU_ON 1
#define COR_EPS_PDU_OFF 0

/* EPS batteries is charging or discharging */
/* EPS_batteries_mA should be treated as positive */
#define COR_EPS_BAT_CHARGING 1
/* EPS_batteries_mA should be treated as negative*/
#define COR_EPS_BAT_DISCHARGING 0

/* EPS heater status on/off */

/* battery heaters are on */
#define COR_EPS_BAT_HEATER_ON 1
/* battery heaters are off */
#define COR_EPS_BAT_HEATER_OFF 0

/**
 * @brief This function return the value between 0 to 65535 respect to the state of charge in percentage from 0 to 100%
 * 
 * @param[in] percentage is the input percentage you want to transform, is a number form 0 to 100, any number > 100 will be set to 100
 * 
 * @return uint16_t returns a value between 0 to 65,535 proportional to percentage input 
 * 
 */
static inline uint16_t COR_BAT_SOC(uint8_t percentage)
{
    if(percentage > 100U)
    {
        percentage = 100U;
    }
    return (uint16_t)(((uint32_t)percentage * 65535U) / 100U );
}

/* ---------------*/

/* MAIN DEFINITIONS THAT THE USER CAN CHANGE */

#define COR_DEF_MAX_TRANSITIONS_RECORD 64



/**  TIME MANAGEMENT REGION 
 * 
 * ---------------------------------------------------------------------------------------------------------------
 */


/* SPACECRAFT TIME MANAGEMENT 
* J2000: milliseconds passed from 00:00:00 1 January 2000
*/
uint64_t COR_UNIX_TIME_ms;


/**
 * @brief This struct represent a date time for the computer
 */
typedef struct 
{
    uint16_t year;      /**< Year (yyyy) from 0 to 65,535 */
    uint8_t month;      /**< Mont (mm) from 1 to 12 */
    uint8_t day;        /**< Day (dd) from 1 to 31 (depending on month)*/
    uint8_t hour;       /**< Hour (hh) from 0 to 23 */
    uint8_t minute;     /**< Minute  from 0 to 59 */
    uint8_t second;     /**< Second from 0 to 59 */
    uint16_t millisecond;   /**< Milliseconds from 0 to 999*/
} COR_DateTime;


/**
 * @brief This function calculate if one year is leap or not
 * 
 * The function takes one year as input and returns 0 if it is not leap or 1 if it is
 * 
 * @param[in] year is the year to check from 0 to 65,535
 * 
 * @retval 0 = if it is not leap
 * @retval 1 = if it is leap
 *  
 */
static inline uint8_t COR_NU_is_leap_year(uint16_t year)
{
    if(year % 4U != 0)
    {
        return 0;
    }
    if(year % 100U != 0U)
    {
        return 1;
    }
    if(year % 400U == 0U)
    {
        return 1;
    }
    return 0;
}


/**
* @brief Calculate date and time from timestamp in millisecond
*
* This function calculate the DateTime from the COR_UNIX_TIME_ms (uint64_t) variable
* which contains the milliseconds passed from 01/01/2000 00:00:00.000 (J2000)
*
* @param[out] dateTime is the COR_DateTime pointer where the data will be written
*
* @return int16_t is the return value
*
* @retval COR_OK if everything went ok
* @retval COR_ERR_INVALID_PARAM: if dateTime pointer is NULL
* @retval COR_ERR_DATE_CALC_ERROR if the calculation went wrong 
*/
int16_t COR_calculate_DateTime(COR_DateTime *const dateTime) 
{
    /* check if the parameter is ok */
    if (NULL == dateTime)
    {
        return COR_ERR_INVALID_PARAM;
    }

    /* variable used locally. days in each month */
    static const uint8_t month_len[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* calcualte the milliseconds */
    dateTime->millisecond = (uint16_t)(COR_UNIX_TIME_ms % 1000U);
    uint64_t var_tmp = COR_UNIX_TIME_ms / 1000U;
    /* calculate the seconds */
    dateTime->second = (uint8_t)(var_tmp % 60U);
    var_tmp /= 60U;
    /* calculate the minutes */
    dateTime->minute = (uint8_t)(var_tmp % 60U);
    var_tmp /= 60U;
    /* calculate the hours */
    dateTime->hour = (uint8_t)(var_tmp % 24U);
    var_tmp /= 24U; 

    /* year calculations */
    dateTime->year = 2000;
    /* is it leap 2000? */
    uint16_t d_in_y = COR_NU_is_leap_year(dateTime->year) ? 366U : 365U;
    
    /* while loop for calculation of the year */
    while (var_tmp >= d_in_y) 
    {
        var_tmp -= d_in_y;
        dateTime->year++;
        d_in_y = COR_NU_is_leap_year(dateTime->year) ? 366U : 365U;
    }

    /* month calculations with for loop */
    dateTime->month = 0; // tmp for cycle
    for (uint8_t i = 0; i < 12; i++)
    {
        uint8_t d_in_m = month_len[i];
        
        // correction if february is 29 days
        if ((1U == i) && (COR_NU_is_leap_year(dateTime->year)))
        {
            d_in_m = 29U;
        }

        if (var_tmp < d_in_m)
        {
            /* month has been found */
            dateTime->month = i + 1U; 
            /* the rest are the days */
            dateTime->day = (uint8_t)(var_tmp + 1U); 
            /* end of the for loop */
            break;
        }

        var_tmp -= d_in_m;
    }

    /* everything is ok */
    return COR_OK;
}



/**  END TIME MANAGEMENT REGION 
 * 
 * ---------------------------------------------------------------------------------------------------------------
 */






/* states machine list */
enum COR_STATE
{
    COR_STATE_NOMINAL,
    COR_STATE_COMM,
    COR_STATE_STARTUP,
    COR_STATE_SAFE,
    COR_STATE_MAINTENANCE,
    COR_STATE_HYPERSAFE,
};

/* definition of types */
typedef struct cor_state_transition COR_StateTransition;
typedef struct cor_state_transtion_instance COR_StateTransition_Instance;


/* 
* state transition record struct
* Fields:
* - from_state: the state that the software is exiting
* - to_state: the state that the software is entering
* - reason: a number that indicates the reason of the transition
* - time: time when this transition is happening (seconds from boot up, or unix time)
* Notes:
* This struct is created whenever a state transition happens. All these records are reported in the COR_StateTransition_Instance struct
*/
struct cor_state_transition
{
    uint8_t from_state;
    uint8_t to_state;
    uint16_t reason;
    uint32_t time;
};


/*
* state transition instance records
* Fields:
* - COR_StateTransition: array where the transitions record are memorized
* - max_index: maximum number of elements that can be memorized
* - index: number of elements memorized in the structure
* Notes:
* When this struct is populated, the suggestion is to reset the index whenever this telemetry is given to the ground. If the elments
* are more than max_index the new transition is recorded at the first index (circular array)
*/
struct cor_state_transition_instance
{
    COR_StateTransition transitions[COR_DEF_MAX_TRANSITIONS_RECORD];
    size_t max_index;
    size_t index;
};



struct COR_EPS_status
{
    /* PDU 1 */

    uint8_t PDU1_VOLT :2;
    uint8_t PDU1_STATE :1;

    uint8_t PDU1_CH1_STATE :1;
    uint16_t PDU1_CH1_Voltage_mV;
    uint16_t PDU1_CH1_Current_mA;

    uint8_t PDU1_CH2_STATE :1;
    uint16_t PDU1_CH2_Voltage_mV;
    uint16_t PDU1_CH2_Current_mA;

    uint8_t PDU1_CH3_STATE :1;
    uint16_t PDU1_CH3_Voltage_mV;
    uint16_t PDU1_CH3_Current_mA;
    
    /* PDU 2 */

    uint8_t PDU2_VOLT :2;
    uint8_t PDU2_STATE :1;

    
    uint8_t PDU2_CH1_STATE :1;
    uint16_t PDU2_CH1_Voltage_mV;
    uint16_t PDU2_CH1_Current_mA;

    uint8_t PDU2_CH2_STATE :1;
    uint16_t PDU2_CH2_Voltage_mV;
    uint16_t PDU2_CH2_Current_mA;

    uint8_t PDU2_CH3_STATE :1;
    uint16_t PDU2_CH3_Voltage_mV;
    uint16_t PDU2_CH3_Current_mA;

    /* PDU 3 */

    uint8_t PDU3_VOLT :2;
    uint8_t PDU3_STATE :1;


    uint8_t PDU3_CH1_STATE :1;
    uint16_t PDU3_CH1_Voltage_mV;
    uint16_t PDU3_CH1_Current_mA;

    uint8_t PDU3_CH2_STATE :1;
    uint16_t PDU3_CH2_Voltage_mV;
    uint16_t PDU3_CH2_Current_mA;

    uint8_t PDU3_CH3_STATE :1;
    uint16_t PDU3_CH3_Voltage_mV;
    uint16_t PDU3_CH3_Current_mA;


    /* temperature of the EPS */
    int16_t Temp_EPS;
    /* temperature of the batteries */
    int16_t Temp_BAT;
    /* current status of the EPS */
    uint16_t Status;
    /* status if batteries are charging or discharging */
    uint8_t EPS_CH_DISCH_STATE :1;
    /* batteries voltage in mV which can go up to +65V*/
    uint16_t EPS_Batteries_mV;
    /* batteries current in/out which can go up to +65A 1mA sensitivity
    * the direction of the current is taken from EPS_CH_DISCH_STATE
    */
    uint16_t EPS_Batteries_mA;

    /* state of charge of the batteries
    *  0% = 0
    *  25% = 16383
    *  50% = 32766
    *  75% = 49149
    *  100% = 65535
    *  use BATT_SOC_PERC(x) to get the integer from uint8_t percentage
    */
    uint16_t EPS_BAT_SOC;



    /* batteries heater state */
    uint8_t Battery_Heater_State :1;
    /* current used by heaters */
    uint16_t Battery_Heater_Current_mA;
};



struct cor_state
{
    uint8_t current_state;
};









/*
* cor_init
* ---------------
* Parameters:
* Returns:
*/
int cor_init();











#ifdef __cplusplus
}
#endif