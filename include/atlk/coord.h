/* Copyright (C) 2016 Autotalks Ltd. */
#ifndef _ATLK_COORD_H
#define _ATLK_COORD_H

#include <math.h>       /* isnan, NAN */

#ifndef NAN
#define NAN (0.0/0.0)
#endif /* NAN */

#include <atlk/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
   @file
   Coordinates API
*/

/**
   Vehicle EGO coordinates.
   
   Coordinates in a cartesian coordinate system, centered around the vehicles
   position reference, defined per standard:

     - SAE J2745/1 - section 6.2.3.
     - ETSI EN 302 637-2 - section B.19.
   
   Axis X is positive towards vehicles front, axis Y is positive towards
   vehicles right hand side, axis Z is positive towards ground (i.e. down).
*/
typedef struct {
  /** Axis X distance in meters */
  double x_m;

  /** Axis Y distance in meters */
  double y_m;
  
  /** Axis Z distance in meters */
  double z_m;

} coord_ego_t;

/** Vehicle EGO coordinates default initializer */
#define COORD_EGO_INIT {    \
  .x_m = NAN,               \
  .y_m = NAN,               \
  .z_m = NAN                \
}

/** Vehicle EGO coordinates zeroed value initializer */
#define COORD_EGO_ZERO_INIT {   \
  .x_m = 0.0,                   \
  .y_m = 0.0,                   \
  .z_m = 0.0                    \
}

#ifdef __cplusplus
}
#endif

#endif /* _ATLK_COORD_H */
