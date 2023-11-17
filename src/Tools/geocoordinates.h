/**
 * \file geocoordinates.h
 *
 * \brief -
 *
 * -
 *
 * (c) 2010 by Metronix Messgereate und Elektronik GmbH
 *
 * \author - Bernhard Friedrichs
 * \date -
 * \version 01.00
 *
 * - changed at: -
 * - changed by: -
 * - changes:    -
 */

#ifndef GEOCOORDINATES_H
#define GEOCOORDINATES_H

#include <QObject>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

#include <climits>
#include <cmath>


#include "geometries.h"


/**
     @author B. Friedrichs <bfr@mtx>
     internal calculation will use -180 to 180 deg lon
                                  and -90 to 90 deg lat

    Routines for coordinate transformation <br>
    (first version K. Ivory)<br>
    getestet:<br>
    Longitude        Latitude         Meridian<br>
    10 42' 59.3215'' 48 26' 45.4355''  12<br>
    rechts: 4.405057628849e+06  hoch: 5.368263247964e+06<br>
    Ergebinis 25.6.97<br>

    Longitude         Latitude<br>
    10 31' 55.9''  52 15' 39.5''<br>
    Soll 4399796.57      5792799.64 rechts hoch<br>
    Ist  4399796.266583  5792800.787294<br>

Braunschweig St. Andreas Curch, tower top middle
"52:16:09.4416", "10:31:15.8414"

Gauss Krueger:  Rechts 3 603820.13   Hoch  5 793801.08   Meridian 9
                       4 399055.56         5 793741.52            12
UTM           Northing 5792079     Easting 603791    Zone 32U   Meridian 9

Loewenwall Braunschweig
52:15:39.5, 10:31:55.9
Gauss Krueger:  Rechts 4 399796.57   Hoch  5 792799.64   Meridian 12
WGS-80 Lat Lon 52:15:34 10:31:53


CN Tower Toronto:
43:38:33.24N 79:23:13.7W. Zone 17  
             4833438    630084 from Wikipedia
UTM   17 "T" 4832413    630104  my software with -81 -81 Meridian
             4832409    630107  (internet tool)


The internal data is RADIANS longitude and latitude; all input must be converted into that.
From these longitude and latitude degrees and UTM are calculated.

 */

/*!
  geocoorinates is using long double;


*/
class  geocoordinates : public QObject{


    Q_OBJECT
public:
    geocoordinates(QObject *parent = 0);
    geocoordinates(const geocoordinates &ma);

    ~geocoordinates();

    geocoordinates& operator =  ( const geocoordinates& );



public slots:

    /*!
      clear and re-set all variables
    */
    void clear();

    /*!
      this is THE function to setup the geocoordinates; all other functions will sooner or later call this this function
      from the radians value all other values will be re-calculated;
      the long double is needed for interchange with other coordinates

    */
    void set_lon_lat_rad(long double rad_lon, long double rad_lat, double televation, qint64 tutm_meridian, qint64 tgk_meridian);

    /*!
      same version with double (e.g. this is used to set data from outside      
    */
    void set_lon_lat_rad( double rad_lon,  double rad_lat,  double televation,  qint64 tutm_meridian,  qint64 tgk_meridian);

    void set_lon_lat(  qint64 log,   qint64 lom,   double los,   QString LonH,
                       qint64 lag,   qint64 lam,   double las,   QString LatH,
                       double televation,   qint64 tutm_meridian,   qint64 tgk_meridian,  bool min_sec_positive_only);
    
//    void set_lon_lat32(  int log,   int lom,   double los,   QString LonH,
//                       int lag,   int lam,   double las,   QString LatH,
//                       double televation,   int tutm_meridian,   int tgk_meridian);

    void get_lon_lat(  qint64& log,   qint64& lom,   double& los,   QString& LonH,
                        qint64& lag,   qint64& lam,   double& las,   QString& LatH,
                        double& televation,   qint64& tutm_meridian,   qint64& tgk_meridian);

    void get_lon_lat32(  int& log,   int& lom,   double& los,   QString& LonH,
                        int& lag,   int& lam,   double& las,   QString& LatH,
                        double& televation,   int& tutm_meridian,   int& tgk_meridian);

    /*!< specify positive number(strings) only; indicate if West/East North/South; if empty East / North is assumed
     *

     */
    void set_lon_lat( const QString log,  const QString lom,  const QString los,  QString LonH,
                      const QString lag,  const QString lam,  const QString las,  QString LatH);


    bool fetch_arguments(QStringList& qargs );
    
    QString set_lon_lat_msec_qstr( QString qslon,  QString qslat,  QString qs_cm_elevation);
    void set_lon_lat_msec( const qint64 msec_lon,  const qint64 msec_lat, const qint64 cm_elevation);
    void get_lon_lat_msec( qint64& msec_lon,  qint64& msec_lat,  qint64& cm_elevation);

    
    QString get_lon_lat_str(QString& qslon, QString& qslat);

    QString get_lon_lat_str_google_maps ();
    QString get_lon_str_google_maps     ();
    QString get_lat_str_google_maps     ();


    QString get_elevation_str(QString& elevation);
    QString get_utm_meridian_str(QString& utm_meridian);
    QString get_gk_meridian_str(QString& gk_meridian);
    
    QChar UTMLetterDesignator();    
    
    QString geo2UTM(double& teasting, double& tnorthing,  qint64& tutm_meridian, QString& tUTMzone);
    QString geo2gauss (double& trechts, double& thoch, qint64& tgk_meridian);
    QString geo2UTM32(double& teasting, double& tnorthing,  int& tutm_meridian, QString& tUTMzone);
    QString geo2gauss32 (double& trechts, double& thoch, int& tgk_meridian);

    void gauss2geo();
    void UTM2geo() ;
    void set_utm(double tnorthing, double teasting);



//signals:
//    void get_lon_lat_qstr(QString& QSLongitude, QString& QSLatitude);
//    void utm_calculated(double& teasting, double& tnorthing,  qint64& tutm_meridian, QString& tUTMzone);

private:
    
    bool is_init;
    void flip();

    void set_internals();

    long double latitude;      /*! latitude  in RAD   this is the base of all*/
    long double longitude;     /*! longitude in RAD   this is the base of all*/
    long double deglat;        /*! latitude  in DEG  */
    long double deglon;        /*! longitude in DEG */
    long double rechts;        /*! Gauss Krueger */
    long double hoch;          /*! Gauss Krueger */
    long double northing;      /*! UTM coordianate */
    long double easting;       /*! UTM coordinates */
    long double elevation;     /*! elevation sea (NN) in meter */

    qint64 deg_lo;             /*! readable coodinates, grad */
    qint64 mi_lo;              /*! readable coodinates, grad */
    qint64 deg_la;             /*! readable coodinates, grad */
    qint64 mi_la;              /*! readable coodinates, grad */




    long double se_lo;         /*! readable seconds longitude DEG */
    long double se_la;         /*! readable seconds longitude DEG */

    qint64 gk_meridian;        /*! meridian for Gauss Krueger  in DEG */
    qint64 utm_meridian;       /*! meridian for UTM   in DEG */

    QString UTMzone;           /*! UTM zone */
    QString  LatH, LonH;          // LatH can be N or S, LonH can be E or W
    QString ellipsoid;         /*! store the Ellispoid name like WGS-84 */
    QString QSLatitude;
    QString QSLongitude;
    QHash<QString, long double> EquatorialRadius;
    QHash<QString, long double> EccentricitySquared;
    long double eqradius;      /*! used Equatorial Radius */
    long double eccSquared;    /*! used Eccentricity Squared */
    
    
    QString myname;            /*!  use for debug and other messages */
    QString myfunc;            /*!  use for debug and other messages */

};

#endif // GEOCOORDINATES_H

/*

/// @brief The usual PI/180 constant
static const double DEG_TO_RAD = 0.017453292519943295769236907684886;
/// @brief Earth's quatratic mean radius for WGS-84
static const double EARTH_RADIUS_IN_METERS = 6372797.560856;

** @brief Computes the arc, in radian, between two WGS-84 positions.
  *
  * The result is equal to <code>Distance(from,to)/EARTH_RADIUS_IN_METERS</code>
  *    <code>= 2*asin(sqrt(h(d/EARTH_RADIUS_IN_METERS )))</code>
  *
  * where:<ul>
  *    <li>d is the distance in meters between 'from' and 'to' positions.</li>
  *    <li>h is the haversine function: <code>h(x)=sin²(x/2)</code></li>
  * </ul>
  *
  * The haversine formula gives:
  *    <code>h(d/R) = h(from.lat-to.lat)+h(from.lon-to.lon)+cos(from.lat)*cos(to.lat)</code>
  *
  * @sa http://en.wikipedia.org/wiki/Law_of_haversines


*
double ArcInRadians(const Position& from, const Position& to) {
    double latitudeArc  = (from.lat - to.lat) * DEG_TO_RAD;
    double longitudeArc = (from.lon - to.lon) * DEG_TO_RAD;
    double latitudeH = sin(latitudeArc * 0.5);
    latitudeH *= latitudeH;
    double lontitudeH = sin(longitudeArc * 0.5);
    lontitudeH *= lontitudeH;
    double tmp = cos(from.lat*DEG_TO_RAD) * cos(to.lat*DEG_TO_RAD);
    return 2.0 * asin(sqrt(latitudeH + tmp*lontitudeH));
}

* @brief Computes the distance, in meters, between two WGS-84 positions.
  *
  * The result is equal to <code>EARTH_RADIUS_IN_METERS*ArcInRadians(from,to)</code>
  *
  * @sa ArcInRadians




double DistanceInMeters(const Position& from, const Position& to) {
    return EARTH_RADIUS_IN_METERS*ArcInRadians(from, to);
}

Approximate distance in miles:

    sqrt(x * x + y * y)

where x = 69.1 * (lat2 - lat1)
and y = 53.0 * (lon2 - lon1)

You can improve the accuracy of this approximate distance calculation by adding the cosine math function:

Improved approximate distance in miles:

    sqrt(x * x + y * y)

where x = 69.1 * (lat2 - lat1)
and y = 69.1 * (lon2 - lon1) * cos(lat1/57.3)

If you need greater accuracy, you can use the Great Circle Distance Formula. This formula requires use of spherical geometry and a high level of floating point mathematical accuracy - about 15 digits of accuracy (sometimes called "double-precision"). In order to use this formula properly make sure your software application or programming language is capable of double-precision floating point calculations. In addition, the trig math functions used in this formula require conversion of the latitude and longitude values from decimal degrees to radians. To convert latitude or longitude from decimal degrees to radians, divide the latitude and longitude values in this database by 180/pi, or approximately 57.29577951. The radius of the Earth is assumed to be 6,378.8 kilometers, or 3,963.0 miles.

If you convert all latitude and longitude values in the database to radians before the calculation, use this equation:

Great Circle Distance Formula using radians:

    3963.0 * arccos[sin(lat1) *  sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1)]

If you do NOT first convert the latitude and longitude values in the database to radians, you must include the degrees-to-radians conversion in the calculation. Substituting degrees for radians, the formula becomes:

Great Circle Distance Formula using decimal degrees:

    3963.0 * arccos[sin(lat1/57.2958) * sin(lat2/57.2958) + cos(lat1/57.2958) * cos(lat2/57.2958) *  cos(lon2/57.2958 -lon1/57.2958)]

OR

    r * acos[sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1)]

Where r is the radius of the earth in whatever units you desire.
r=3437.74677 (nautical miles)
r=6378.7 (kilometers)
r=3963.0 (statute miles)

If the software application or programming language you are using has no arccosine function, you can calculate the same result using the arctangent function, which most applications and languages do support. Use the following equation:

    3963.0 * arctan[sqrt(1-x^2)/x]

where

    x = [sin(lat1/57.2958) * sin(lat2/57.2958)] + [cos(lat1/57.2958) * cos(lat2/57.2958) * cos(lon2/57.2958 - lon1/57.2958)]




*/
