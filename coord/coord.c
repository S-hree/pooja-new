/* Copyright (C) 2016 Autotalks Ltd. */
#include <atlk/coord.h>
#include <atlk/coord_internal.h>

#include <platform/common.h>

/**
   Reference:
   
     J2945/1 D5.0, A.2 COORDINATE TRANSFORMATION (NORMATIVE)
*/

/* Semi-major axis of earth */
#define A 6378137.0

/* Flattening */
#define F 0.003353

/* Eccentricity: (f*(2-f))^0.5 */
#define F1 \
  (sqrt((F * (2.0 - F))))

/* Radius of earth in meridian: a*(1-f1^2)/(1-f1^2*(sin(RefLat))^2)^(3/2) */
#define F2(f1_sq, sin_ref_lat_sq) \
  (A * (1.0 - f1_sq) / pow((1.0 - f1_sq * sin_ref_lat_sq), 1.5))

/* Radius of earth in prime vertical: a/(1-f1^2*(sin(RefLat))^2)^(1/2) */
#define F3(f1_sq, sin_ref_lat_sq) \
  (A / sqrt(1.0 - f1_sq * sin_ref_lat_sq))

/* Convert degrees to/from radians */
#define DEG_TO_RAD(d) ((d) * M_PI / 180.0)
#define RAD_TO_DEG(d) ((d) * 180.0 / M_PI)

atlk_rc_t atlk_must_check
coord_displace(coord_lla_t *lla,
            const coord_lla_t *ref_lla,
            double ref_heading_deg,
            const coord_ego_t *ego)
{
  double ref_lat_rad, ref_lon_rad, ref_heading_rad;
  double f1, f1_sq, sin_ref_lat, sin_ref_lat_sq;
  double cos_ref_heading, sin_ref_heading;
  double e, n, f2, f3, new_lat_rad, new_lon_rad;
  coord_lla_t new_lla = COORD_LLA_INIT;

  if (atlk_unlikely(!lla || !ref_lla || !ego)) {
    TR_ERROR_NO_ARGS("Mandatory function argument is not specified");
    return ATLK_E_INVALID_ARG;
  }

  ref_lat_rad = DEG_TO_RAD(ref_lla->latitude_deg);
  ref_lon_rad = DEG_TO_RAD(ref_lla->longitude_deg);
  ref_heading_rad = DEG_TO_RAD(ref_heading_deg);

  f1 = F1;
  f1_sq = f1 * f1;
  sin_ref_lat = sin(ref_lat_rad);
  sin_ref_lat_sq = sin_ref_lat * sin_ref_lat;

  sin_ref_heading = sin(ref_heading_rad);
  cos_ref_heading = cos(ref_heading_rad);

  /**
     E = cos(RefHeading)*Y + sin(RefHeading)*X;
     N = cos(RefHeading)*X - sin(RefHeading)*Y;
   */
  e = cos_ref_heading * ego->y_m + sin_ref_heading * ego->x_m;
  n = cos_ref_heading * ego->x_m - sin_ref_heading * ego->y_m;

  f2 = F2(f1_sq, sin_ref_lat_sq);
  f3 = F3(f1_sq, sin_ref_lat_sq);
  
  /**
     NEW_LATITUDE (rad) = (1/f2)*N + RefLat;
     NEW_LONGITUDE (rad) = (1/(f3*cos(RefLat)))*E + RefLon;
   */
  new_lat_rad = (1.0 / f2) * n + ref_lat_rad;
  new_lon_rad = (1.0 / (f3 * cos(ref_lat_rad))) * e + ref_lon_rad;

  new_lla.latitude_deg = RAD_TO_DEG(new_lat_rad);
  new_lla.longitude_deg = RAD_TO_DEG(new_lon_rad);
  new_lla.altitude_m = ref_lla->altitude_m + ego->z_m;

  /* Verify result conains all numbers */
  if (isnan(new_lla.latitude_deg) || isnan(new_lla.longitude_deg)) {
    TR_DEBUG_NO_ARGS("Transformed LLA contains NAN values");
    return ATLK_E_OUT_OF_DOMAIN;
  }
  else if (isnan(new_lla.altitude_m)) {
    /* Ignore it, happens when altitude is not available (i.e. 2D fix) */
    TR_DEBUG_NO_ARGS("Transformed LLA altitude is NAN");
  }

  *lla = new_lla;
  return ATLK_OK;
}
