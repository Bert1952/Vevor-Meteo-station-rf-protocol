/* ----------------------------------------------------------------------------
 *
 *       ___    _      ______            __             __
 *      /   |  (_)____/ ____/___  ____  / /__________  / /
 *     / /| | / / ___/ /   / __ \/ __ \/ __/ ___/ __ \/ / 
 *    / ___ |/ / /  / /___/ /_/ / / / / /_/ /  / /_/ / /  
 *   /_/  |_/_/_/   \____/\____/_/ /_/\__/_/   \____/_/   
 *
 * 090329-1 (c)2010 Elektor International Media B.V.
 * ------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------
 * This file is the climatology toolbox of the Aircontrol,
 * source code written by Antoine Authier (Elektor LABs).
 *
 * Changes Log
 *  - 0.1.0 : initial source code
 * ------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
 * Include
 * ------------------------------------------------------------------------- */
#include <math.h>
#include "tlkRH.h"


/* ============================================================================
 * Public Functions
 * ========================================================================= */
/* ------------------------------------------------------------------------- */
/**@author Antoine Authier
 * @date   2010/02/21
 * @brief  calculates dew point value
 * @param[in] RH the relative humidity (%RH)
 * @param[in] T the temperature in Celcius (&#65533;C)
 * @return the Dew Point temperature in Celcius (&#65533;C)
 * @note   this function only applies above water in temperature range 0 to 50 &#65533;C
 * @todo   also calculate the Dew Point above ice
 */
float
tlkRH_calculate_dewpoint (float RH, float T)
{
    // calculation formula parameters for Temperature range 0 to 50 &#65533;C 'above water'
    const float Tn = +243.12;
    const float m = +17.62;
    const float ln10 = +2.302585;
    // H is an intermediary value, see 'Humidity at a glance' application note
    float H, Tdp;

    // calculate the Dew Point temperature
    H = (log10(RH) - 2.0)*ln10 + (m*T)/(Tn + T);
    Tdp = Tn*H/(m - H);

    return(Tdp);
}

/* ------------------------------------------------------------------------- */
/**@author Antoine Authier
 * @date   2010/02/21
 * @brief  calculates absolute humidity value (in g/m3)
 * @param[in] RH the relative humidity (%RH)
 * @param[in] T the temperature in Celcius (&#65533;C)
 * @return the Absolute Humidity in g/m3
 * @note   this function only applies above water in temperature range 0 to 50 &#65533;C
 *         TODO check the temperature range
 */
 /*
float
tlkRH_calculate_abs_humidity (float RH, float T)
{
    // calculation formula parameters for Temperature range 0 to 50 &#65533;C 'above water'
    const float Tn = +243.12;
    const float m = +17.62;
    const float A = +6.112;
    // dv is the Absolute Humidity:
    // the mass of water vapor in a particular volume of dry air
    float dv;

    // calculate the absolute humidity
    dv = 216.7 * (RH / 100.0 * A * exp(m*T / (Tn + T)) / (273.15 + T));

    return(dv);
}
 */
/* ------------------------------------------------------------------------- */
/**@author Antoine Authier
 * @date   2010/02/21
 * @brief  calculates the mixing ratio (in g/kg)
 * @param[in] RH the relative humidity (%RH)
 * @param[in] T the temperature in Celcius (&#65533;C)
 * @param[in] P the barometric air pressure in hPa
 * @return the Mixing Ration in g/kg
 * @note   this function only applies above water in temperature range 0 to 50 &#65533;C
 *         TODO check the temperature range
 */
 /*
float
tlkRH_calculate_mixing_ratio (float RH, float T, float P)
{
    // calculation formula parameters for Temperature range 0 to 50 &#65533;C 'above water'
    const float Tn = +243.12;
    const float m = +17.62;
    const float A = +6.112;
    // e in an intermediate value, r is the Mixing Ratio:
    // the mixing ratio is the mass of water vapor in a particular mass of dry air.
    float e, r;

    // calculate the Mixing Ratio
    e = RH/100.0*A*exp(m*T/(Tn + T));
    r = 622.0*e/(P - e);

    return(r);
}
*/
/* ------------------------------------------------------------------------- */
/**@author Antoine Authier
 * @date   2010/02/21
 * @brief  calculates Heat Index in Celcius (as defined by the NOAA)
 *         HI indicates how the human body feels temperature, this is subjective
 * @param[in] RH the relative humidity (%RH)
 * @param[in] T the temperature in Celcius (&#65533;C)
 * @return the heat index
 * @note   this function only applies above water in temperature range 0 to 50 &#65533;C
 *         TODO check the temperature range
 */
float
tlkRH_calculate_heat_index (float RH, float T)
{
    // calculation formula parameters for Temperature range 0 to 50 &#65533;C 'above water'
    const float Tn = +243.12;
    const float m = +17.62;
    // HI is the Heat Index, p an intermediate value:
    float p, HI;

    // calculate the absolute humidity
    p  = RH/100.0*exp(m*T/(Tn + T));
    HI = T + 5.0/9.0*(p - 10.0);

    return(HI);
}

/* ------------------------------------------------------------------------- */
/**@author Antoine Authier
 * @date   2010/02/21
 * @brief  calculates the cloud altiture
 * @param[in] DP the Dew Point temperature (&#65533;C)
 * @param[in] T the temperature in Celcius (&#65533;C)
 * @return the cloud base altitude
 * @note   TODO check the temperature range
 */
float
tlkRH_calculate_cloud_altitude (float DP, float T)
{
    float alt;

    alt = (T - DP) * (100/.65);

    return(alt);
}

