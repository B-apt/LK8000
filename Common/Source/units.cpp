/*
   LK8000 Tactical Flight Computer -  WWW.LK8000.IT
   Released under GNU/GPL License v.2 or later
   See CREDITS.TXT file for authors and copyrights

   $Id: units.cpp,v 8.5 2010/12/13 17:37:35 root Exp root $
*/

//default       EU   UK   US   AUS
//altitude      m    ft   ft   m
//verticalspeed m/s  kts  kts  kts
//wind speed    km/  kts  mp   kts
//IAS           km/  kts  mp   kts
//distance      km   nm   ml   nm


#include "externs.h"
#include "Library/Utm.h"
#include "utils/printf.h"

namespace {

struct UnitDescriptor_t {
  const TCHAR * const Name;
  double  ToUserFact;
  double  ToUserOffset;
};

UnitDescriptor_t UnitDescriptors[unLastUnit + 1] = {
    {_T(""),   1.0,                       0},    // unUndef
    {_T("km"), 0.001,                     0},    // unKiloMeter
    {_T("nm"), 1.0 / 1852,                0},    // unNauticalMiles
    {_T("mi"), 1.0 / 1609.344,            0},    // unStatuteMiles
    {_T("kh"), 3.6,                       0},    // unKiloMeterPerHour
    {_T("kt"), 1.0 / (1852.0 / 3600.0),   0},    // unKnots
    {_T("mh"), 1.0 / (1609.344 / 3600.0), 0},    // unStatuteMilesPerHour
    {_T("ms"), 1.0                      , 0},    // unMeterPerSecond
    {_T("fm"), 1.0 / 0.3048 * 60.0,       0},    // unFeetPerMinutes
    {_T("m"),  1.0,                       0},    // unMeter
    {_T("ft"), 1.0 / 0.3048,              0},    // unFeet
    {_T("FL"), 1.0 / 0.3048 / 100,        0},    // unFligthLevel
    {_T("K"),  1.0,                       0},    // unKelvin
    {_T("°C"), 1.0,                    -273.15}, // unGradCelcius
    {_T("°F"), 9.0 / 5.0,              -459.67}, // unGradFahrenheit
    {_T("fs"), 1.0 / 0.3048,              0},    // unFeetPerSecond
    {_T(""),   1.0,                       0},    // unLastUnit
};

CoordinateFormats_t CoordinateFormat;
Units_t DistanceUnit = unKiloMeter;
Units_t AltitudeUnit = unMeter;
Units_t HorizontalSpeedUnit = unKiloMeterPerHour;
Units_t VerticalSpeedUnit = unMeterPerSecond;
Units_t WindSpeedUnit = unKiloMeterPerHour;
Units_t TaskSpeedUnit = unKiloMeterPerHour;

} // namespace

void Units::LongitudeToDMS(double Longitude,
                           int *dd,
                           int *mm,
                           int *ss,
                           bool *east) {

  int sign = Longitude<0 ? 0 : 1;
  Longitude = fabs(Longitude);

  *dd = (int)Longitude;
  Longitude = (Longitude - (*dd)) * 60.0;
  *mm = (int)(Longitude);
  Longitude = (Longitude - (*mm)) * 60.0;
  *ss = (int)(Longitude + 0.5);
  if (*ss >= 60) {
    (*mm)++;
    (*ss) -= 60;
  }
  if ((*mm) >= 60) {
    (*dd)++;
    (*mm) -= 60;
  }
  *east = (sign == 1);
}


void Units::LatitudeToDMS(double Latitude,
                          int *dd,
                          int *mm,
                          int *ss,
                          bool *north) {

  int sign = Latitude<0 ? 0 : 1;
  Latitude = fabs(Latitude);

  *dd = (int)Latitude;
  Latitude = (Latitude - (*dd)) * 60.0;
  *mm = (int)(Latitude);
  Latitude = (Latitude - (*mm)) * 60.0;
  *ss = (int)(Latitude + 0.5);
  if (*ss >= 60) {
    (*mm)++;
    (*ss) -= 60;
  }
  if ((*mm) >= 60) {
    (*dd)++;
    (*mm) -= 60;
  }
  *north = (sign==1);
}

bool Units::CoordinateToString(double Longitude, double Latitude, TCHAR *Buffer, size_t size) {
	if(CoordinateFormat == cfUTM) {
		int utmzone=0;
		char utmchar=0;
		double easting=0, northing=0;
		LatLonToUtmWGS84 ( utmzone, utmchar, easting, northing, Latitude, Longitude );
		lk::snprintf(Buffer, size, _T("UTM %d%c  %.0f  %.0f"), utmzone, utmchar, easting, northing);
	} else {
		TCHAR sLongitude[16];
		TCHAR sLatitude[16];

		Units::LongitudeToString(Longitude, sLongitude);
		Units::LatitudeToString(Latitude, sLatitude);

		lk::snprintf(Buffer,size,_T("%s  %s"), sLatitude, sLongitude);
	}
	return true;
}


bool Units::LongitudeToString(double Longitude, TCHAR *Buffer, size_t size){

  TCHAR EW[] = _T("WE");
  int dd, mm, ss;

  int sign = Longitude<0 ? 0 : 1;
  Longitude = fabs(Longitude);

  switch(CoordinateFormat){
    case cfDDMMSS:
      dd = (int)Longitude;
      Longitude = (Longitude - dd) * 60.0;
      mm = (int)(Longitude);
      Longitude = (Longitude - mm) * 60.0;
      ss = (int)(Longitude + 0.5);
      if (ss >= 60) {
        mm++;
        ss -= 60;
      }
      if (mm >= 60) {
        dd++;
        mm -= 60;
      }
      lk::snprintf(Buffer, size, _T("%c%03d°%02d'%02d\""), EW[sign], dd, mm, ss);
      break;
    case cfDDMMSSss:
      dd = (int)Longitude;
      Longitude = (Longitude - dd) * 60.0;
      mm = (int)(Longitude);
      Longitude = (Longitude - mm) * 60.0;
      lk::snprintf(Buffer, size, _T("%c%03d°%02d'%05.2f\""), EW[sign], dd, mm, Longitude);
    break;
    case cfDDMMmmm:
      dd = (int)Longitude;
      Longitude = (Longitude - dd) * 60.0;
      lk::snprintf(Buffer, size, _T("%c%03d°%06.3f'"), EW[sign], dd, Longitude);
    break;
    case cfDDdddd:
      lk::snprintf(Buffer, size, _T("%c%08.4f°"), EW[sign], Longitude);
    break;
    case cfUTM:
      _tcscpy(Buffer,_T(""));
      break;
    default:
      assert(false);
      break;
  }

  return true;

}


bool Units::LatitudeToString(double Latitude, TCHAR *Buffer, size_t size){
  TCHAR EW[] = _T("SN");
  int dd, mm, ss;

  int sign = Latitude<0 ? 0 : 1;
  Latitude = fabs(Latitude);

  switch(CoordinateFormat){
    case cfDDMMSS:
      dd = (int)Latitude;
      Latitude = (Latitude - dd) * 60.0;
      mm = (int)(Latitude);
      Latitude = (Latitude - mm) * 60.0;
      ss = (int)(Latitude + 0.5);
      if (ss >= 60) {
        mm++;
        ss -= 60;
      }
      if (mm >= 60) {
        dd++;
        mm -= 60;
      }
      lk::snprintf(Buffer, size, _T("%c%02d°%02d'%02d\""), EW[sign], dd, mm, ss);
    break;
    case cfDDMMSSss:
      dd = (int)Latitude;
      Latitude = (Latitude - dd) * 60.0;
      mm = (int)(Latitude);
      Latitude = (Latitude - mm) * 60.0;
      lk::snprintf(Buffer, size, _T("%c%02d°%02d'%05.2f\""), EW[sign], dd, mm, Latitude);
    break;
    case cfDDMMmmm:
      dd = (int)Latitude;
      Latitude = (Latitude - dd) * 60.0;
      lk::snprintf(Buffer, size, _T("%c%02d°%06.3f'"), EW[sign], dd, Latitude);
    break;
    case cfDDdddd:
      lk::snprintf(Buffer, size, _T("%c%07.4f°"), EW[sign], Latitude);
    break;
    case cfUTM:
      _tcscpy(Buffer,_T(""));
    break;
    default:
      assert(false);
    break;
  }

  return true;

}

const TCHAR *Units::GetName(Units_t Unit) {
  // JMW adjusted this because units are pretty standard internationally
  // so don't need different names in different languages.
  return UnitDescriptors[Unit].Name;
}

CoordinateFormats_t Units::GetCoordinateFormat() {
  return CoordinateFormat;
}

Units_t Units::GetDistanceUnit() {
  return DistanceUnit;
}

Units_t Units::GetAltitudeUnit() {
  return AltitudeUnit;
}

Units_t Units::GetAlternateAltitudeUnit() { // 100126
  return AltitudeUnit==unFeet?unMeter:unFeet;
}

Units_t Units::GetHorizontalSpeedUnit() {
  return HorizontalSpeedUnit;
}

Units_t Units::GetTaskSpeedUnit() {
  return TaskSpeedUnit;
}

Units_t Units::GetVerticalSpeedUnit() {
  return VerticalSpeedUnit;
}

Units_t Units::GetWindSpeedUnit() {
  return WindSpeedUnit;
}

void Units::NotifyUnitChanged() {
  switch (SpeedUnit_Config) {
    case 0 :
      HorizontalSpeedUnit = unStatuteMilesPerHour;
      WindSpeedUnit = unStatuteMilesPerHour;
      break;
    case 1 :
      HorizontalSpeedUnit = unKnots;
      WindSpeedUnit = unKnots;
      break;
    case 2 :
    default:
      HorizontalSpeedUnit = unKiloMeterPerHour;
      WindSpeedUnit = unKiloMeterPerHour;
      break;
  }

  switch(DistanceUnit_Config) {
    case 0 :
      DistanceUnit = unStatuteMiles;
      break;
    case 1 :
      DistanceUnit = unNauticalMiles;
      break;
    default:
    case 2 :
      DistanceUnit = unKiloMeter;
      break;
  }

  switch(AltitudeUnit_Config) {
    case 0 :
      AltitudeUnit = unFeet;
      break;
    default:
    case 1 :
      AltitudeUnit = unMeter;
      break;
  }

  switch(VerticalSpeedUnit_Config) {
    case 0 :
      VerticalSpeedUnit = unKnots;
      break;
    default:
    case 1 :
      VerticalSpeedUnit = unMeterPerSecond;
      break;
    case 2 :
      VerticalSpeedUnit = unFeetPerMinutes;
      break;
  }

  switch(TaskSpeedUnit_Config) {
    case 0 :
      TaskSpeedUnit = unStatuteMilesPerHour;
      break;
    case 1 :
      TaskSpeedUnit = unKnots;
      break;
    case 2 :
    default:
      TaskSpeedUnit = unKiloMeterPerHour;
      break;
  }
  
  switch(LatLonUnits_Config) {
    case 0:
    default:
      CoordinateFormat = cfDDMMSS;
      break;
    case 1:
      CoordinateFormat = cfDDMMSSss;
      break;
    case 2:
      CoordinateFormat = cfDDMMmmm;
      break;
    case 3:
      CoordinateFormat = cfDDdddd;
      break;
    case 4:
      CoordinateFormat = cfUTM;
      break;
  }
}

void Units::FormatAltitude(double Altitude, TCHAR *Buffer, size_t size){
  lk::snprintf(Buffer, size, _T("%.0f%s"), ToAltitude(Altitude), GetAltitudeName());
}

void Units::FormatAlternateAltitude(double Altitude, TCHAR *Buffer, size_t size){
  lk::snprintf(Buffer, size, _T("%.0f%s"), ToAlternateAltitude(Altitude), GetAlternateAltitudeName());
}

void Units::FormatArrival(double Altitude, TCHAR *Buffer, size_t size){
  FormatAltitude(Altitude, Buffer, size);
}

void Units::FormatDistance(double Distance, TCHAR *Buffer, size_t size) {
  int prec = 0;

  Units_t UnitIdx = GetDistanceUnit();
  double value = To(UnitIdx, Distance);
  if (value >= 100) {
    prec = 0;
  } else if (value > 10) {
    prec = 1;
  } else if (value > 1) {
    prec = 2;
  } else {
    prec = 3;
    if (UnitIdx == unKiloMeter) {
      prec = 0;
      UnitIdx = unMeter;
      value = To(UnitIdx, Distance);
    }
    if (UnitIdx == unNauticalMiles
            || UnitIdx == unStatuteMiles) {

      const double ftValue = To(unFeet, Distance);  
      if (value < 1000) {
        prec = 0;
        UnitIdx = unFeet;
        value = ftValue;
      } else {
        prec = 1;
      }
    }
  }

  lk::snprintf(Buffer, size, _T("%.*f%s"), prec, value, GetName(UnitIdx));
}

void Units::FormatMapScale(double Distance, TCHAR *Buffer, size_t size){

  int prec = 0;
  Units_t UnitIdx = GetDistanceUnit();

  double value = To(UnitIdx, Distance);

  if (value >= 9.999) {
    prec = 0;
  } else if (value >= 0.999) {
    prec = 1;
  } else {
    if (UnitIdx == unKiloMeter){
      prec = 0;
      UnitIdx = unMeter;
      value = To(UnitIdx, Distance);
    } else if ((UnitIdx == unNauticalMiles || UnitIdx == unStatuteMiles) && (value < 0.160)) {
      prec = 0;
      UnitIdx = unFeet;
      value = To(UnitIdx, Distance);
    } else {
      prec = 2;
    }
  }

  lk::snprintf(Buffer, size, _T("%.*f%s"), prec, value, GetName(UnitIdx));
}



double Units::To(Units_t unit, double value) {
  const UnitDescriptor_t *pU = &UnitDescriptors[unit];
  return value * pU->ToUserFact + pU->ToUserOffset;
}

double Units::From(Units_t unit, double value)  {
  const UnitDescriptor_t *pU = &UnitDescriptors[unit];
  return (value - pU->ToUserOffset) / pU->ToUserFact;
}

void Units::TimeToText(TCHAR* text, size_t cb, int d) {
  bool negative = (d < 0);
  int dd = abs(d) % (3600 * 24);
  int hours = (dd / 3600);
  int mins = (dd / 60 - hours * 60);
  hours = hours % 24;

  lk::snprintf(text, cb, _T("%s%02d:%02d"), (negative ? _T("-") : _T("")), hours, mins);
}

void Units::TimeToTextSimple(TCHAR* text, size_t cb, int d) {
  bool negative = (d < 0);
  int dd = abs(d) % (3600 * 24);
  int hours = (dd / 3600);
  int mins = (dd / 60 - hours * 60);
  hours = hours % 24;

  lk::snprintf(text, cb, _T("%s%02d%02d"), (negative ? _T("-") : _T("")), hours, mins);
}

// Not for displaying a clock time, good for a countdown
// will display either
// Returns true if hours, false if minutes
bool Units::TimeToTextDown(TCHAR* text, size_t cb, int d) {
  bool negative = (d < 0);
  int dd = abs(d) % (3600 * 24);
  int hours = (dd / 3600);
  int mins = (dd / 60 - hours * 60);
  hours = hours % 24;
  int seconds = (dd - mins * 60 - hours * 3600);

  if (hours == 0) {
    lk::snprintf(text, cb, _T("%s%02d:%02d"), (negative ? _T("-") : _T("")), mins, seconds);
    return false;
  } else {
    lk::snprintf(text, cb, _T("%s%02d:%02d"), (negative ? _T("-") : _T("")), hours, mins);
    return true;
  }
}

// LK8000
void Units::TimeToTextS(TCHAR* text, size_t cb, int d) {
  bool negative = (d < 0);
  int dd = abs(d) % (3600 * 24);
  int hours = (dd / 3600);
  int mins = (dd / 60 - hours * 60);
  int seconds = (dd - mins * 60 - hours * 3600);
  hours = hours % 24;

  lk::snprintf(text, cb, _T("%s%d:%02d:%02d"), (negative ? _T("-") : _T("")), hours, mins, seconds);
}

#ifndef DOCTEST_CONFIG_DISABLE
#include <doctest/doctest.h>

TEST_CASE("Units") {

  #define CHECK_APPROX_EQ(x, y) CHECK_EQ(x, doctest::Approx(y))

  SUBCASE("Length") {
    CHECK_EQ(Units::From(unFeet, 1), Units::From(unMeter, 0.3048));
    CHECK_EQ(Units::From(unNauticalMiles, 1), Units::From(unMeter, 1852));
    CHECK_EQ(Units::From(unStatuteMiles, 1), Units::From(unMeter, 1609.344));
    CHECK_EQ(Units::From(unKiloMeter, 1), Units::From(unMeter, 1000));
    CHECK_APPROX_EQ(Units::From(unFligthLevel, 125), Units::From(unFeet, 12500));
  }

  SUBCASE("Speed") {
    CHECK_EQ(Units::From(unKiloMeterPerHour, 3.6), Units::From(unMeterPerSecond, 1));
    CHECK_EQ(Units::From(unKnots, 1), Units::From(unKiloMeterPerHour, 1.852));
    CHECK_APPROX_EQ(Units::From(unStatuteMilesPerHour, 1), Units::From(unMeterPerSecond, 0.44704));
    CHECK_EQ(Units::From(unFeetPerMinutes, 1), Units::From(unMeterPerSecond, 0.00508));
    CHECK_EQ(Units::From(unFeetPerSecond, 1), Units::From(unMeterPerSecond, 0.3048));
  }

  SUBCASE("Temp") {
    CHECK_APPROX_EQ(Units::From(unGradFahrenheit, 0), Units::From(unKelvin, 255.37222222));
    CHECK_APPROX_EQ(Units::From(unGradFahrenheit, 10), Units::From(unKelvin, 260.92777778));
    CHECK_EQ(Units::From(unGradCelcius, 1), Units::From(unKelvin, 274.15));
  }

  #undef CHECK_APPROX_EQ
}
#endif