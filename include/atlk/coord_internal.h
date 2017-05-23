/* Copyright (C) 2016 Autotalks Ltd. */
#ifndef _ATLK_COORD_INTERNAL_H
#define _ATLK_COORD_INTERNAL_H

#include <math.h>       /* isnan, NAN */

#ifndef NAN
#define NAN (0.0/0.0)
#endif /* NAN */

#include <atlk/sdk.h>
#include <atlk/coord.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   Coordinates API
*/

/** Geographic LLA coordinates, WGS-84 datum */
typedef struct {
  /** Latitude in degrees */
  double latitude_deg;

  /** Longitude in degrees */
  double longitude_deg;

  /** Altitude in meters */
  double altitude_m;

} coord_lla_t;

/** Geographic LLA coordinates default initializer */
#define COORD_LLA_INIT {    \
  .latitude_deg = NAN,      \
  .longitude_deg = NAN,     \
  .altitude_m = NAN         \
}

/**
   Displace LLA coordinates.

   Given "reference" LLA coordinates and the orientation of the EGO
   coordinate system (relative to true north, clockwise), displace
   "reference" LLA coordinates with respect to EGO coordinate system
   by given EGO coordinate.

   @param[out] lla Displaced LLA coordinates
   @param[in] ref_lla Reference LLA coordinates
   @param[in] ref_heading_deg Reference heading in degrees
   @param[in] ego Displace according to this vector

   @retval ::ATLK_OK if succeeded
   @retval ::ATLK_E_OUT_OF_DOMAIN if calculation resulted in NAN value
   @return Error code if failed
*/
atlk_rc_t atlk_must_check
coord_displace(coord_lla_t *lla,
            const coord_lla_t *ref_lla,
            double ref_heading_deg,
            const coord_ego_t *ego);

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_COORD_INTERNAL_H */
