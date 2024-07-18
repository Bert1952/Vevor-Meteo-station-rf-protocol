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
#ifndef __TLK_RH__
#define __TLK_RH__

/* ----------------------------------------------------------------------------
 * Global Function Prototypes
 * ------------------------------------------------------------------------- */
extern float tlkRH_calculate_dewpoint (float RH, float T);
extern float tlkRH_calculate_abs_humidity (float RH, float T);
extern float tlkRH_calculate_mixing_ratio (float RH, float T, float P);
extern float tlkRH_calculate_heat_index (float RH, float T);
extern float tlkRH_calculate_cloud_altitude (float DP, float T);

#endif // __TLK_RH__
