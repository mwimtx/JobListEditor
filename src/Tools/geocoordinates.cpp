/**
 * \file *.cpp
 *
 * \brief -
 *
 * -
 *
 * (c) 2008 by Metronix Messgereate und Elektronik GmbH
 *
 * \author -
 * \date -
 * \version 01.00
 *
 * - changed at: -
 * - changed by: -
 * - changes:    -
 */



#include "geocoordinates.h"




geocoordinates::geocoordinates ( QObject *parent )
    : QObject ( parent )
{


    this->latitude = 0.0L;
    this->longitude =0.0L;
    this->eqradius = 0.0L;
    this->eccSquared = 0.0L;

  this->set_internals();
  is_init = false;



}


geocoordinates::geocoordinates(const geocoordinates &ma) {
    this->latitude = 0.0L;
    this->longitude =0.0L;
    this->eqradius = 0.0L;
    this->eccSquared = 0.0L;

    this->set_internals();
    is_init = false;

    this->set_lon_lat_rad (ma.longitude,  ma.latitude,  ma.elevation,  ma.utm_meridian,  ma.gk_meridian);


}


geocoordinates &geocoordinates::operator = ( const geocoordinates& ma) {

    this->set_lon_lat_rad (ma.longitude,  ma.latitude,  ma.elevation,  ma.utm_meridian,  ma.gk_meridian);
    return *this;
}

void geocoordinates::set_internals() {
    this->clear();

    myname = "geocoorinates";
    EquatorialRadius["Airy"]                  = 6377563.0L;
    EquatorialRadius["Australian National"]   = 6378160.0L;
    EquatorialRadius["Bessel 1841"]           = 6377397.0L;
    EquatorialRadius["Bessel 1841 (Nambia)"]  = 6377484.0L;
    EquatorialRadius["Clarke 1866"]           = 6378206.0L;
    EquatorialRadius["Clarke 1880"]           = 6378249.0L;
    EquatorialRadius["Everest"]               = 6377276.0L;
    EquatorialRadius["Fischer 1960 (Mercury)"]= 6378166.0L;
    EquatorialRadius["Fischer 1968"]          = 6378150.0L;
    EquatorialRadius["GRS 1967"]              = 6378160.0L;
    EquatorialRadius["GRS 1980"]              = 6378137.0L;
    EquatorialRadius["Helmert 1906"]          = 6378200.0L;
    EquatorialRadius["Hough"]                 = 6378270.0L;
    EquatorialRadius["International"]         = 6378388.0L;
    EquatorialRadius["Krassovsky"]            = 6378245.0L;
    EquatorialRadius["Modified Airy"]         = 6377340.0L;
    EquatorialRadius["Modified Everest"]      = 6377304.0L;
    EquatorialRadius["Modified Fischer 1960"] = 6378155.0L;
    EquatorialRadius["South American 1969"]   = 6378160.0L;
    EquatorialRadius["WGS 60"]                = 6378165.0L;
    EquatorialRadius["WGS 66"]                = 6378145.0L;
    EquatorialRadius["WGS-72"]                = 6378135.0L;
    EquatorialRadius["WGS-84"]                = 6378137.0L;

    EccentricitySquared["Airy"]                 = 0.00667054L;
    EccentricitySquared["Australian National"]  = 0.006694542L;
    EccentricitySquared["Bessel 1841"]          = 0.006674372L;
    EccentricitySquared["Bessel 1841 (Nambia)"] = 0.006674372L;
    EccentricitySquared["Clarke 1866"]          = 0.006768658L;
    EccentricitySquared["Clarke 1880"]          = 0.006803511L;
    EccentricitySquared["Everest"]              = 0.006637847L;
    EccentricitySquared["Fischer 1960 (Mercury)"]  = 0.006693422L;
    EccentricitySquared["Fischer 1968"]         = 0.006693422L;
    EccentricitySquared["GRS 1967"]             = 0.006694605L;
    EccentricitySquared["GRS 1980"]             = 0.00669438L;
    EccentricitySquared["Helmert 1906"]         = 0.006693422L;
    EccentricitySquared["Hough"]                = 0.00672267L;
    EccentricitySquared["International"]        = 0.00672267L;
    EccentricitySquared["Krassovsky"]           = 0.006693422L;
    EccentricitySquared["Modified Airy"]        = 0.00667054L;
    EccentricitySquared["Modified Everest"]     = 0.006637847L;
    EccentricitySquared["Modified Fischer 1960"]= 0.006693422L;
    EccentricitySquared["South American 1969"]  = 0.006694542L;
    EccentricitySquared["WGS 60"]               = 0.006693422L;
    EccentricitySquared["WGS 66"]               = 0.006694542L;
    EccentricitySquared["WGS-72"]               = 0.006694318L;
    EccentricitySquared["WGS-84"]               = 0.00669438L;

    ellipsoid = "WGS-84";

}

geocoordinates::~geocoordinates()
{
    is_init = false;
    //--qDebug() << "coordinates deleted";
}




void geocoordinates::set_lon_lat_rad (  long double rad_lon,  long double rad_lat,  double televation = 0.,  qint64 tutm_meridian = LONG_MAX,  qint64 tgk_meridian = LONG_MAX )
{

  myfunc = "geocoordinates::set_lon_lat_rad ";
  latitude = rad_lat;
  deglat = ( latitude * 180. ) / M_PI;

  longitude = rad_lon;
  deglon = ( longitude * 180. ) / M_PI;


  if ( longitude > M_PI )
  {
    longitude -= ( 2. * M_PI );
    deglon -=360.,
             LonH = "W";
  }
  else if ( longitude < 0 )
    LonH = "W";
  else
    LonH = "E";

  if ( latitude < 0 )
    LatH = "S";
  else
    LatH = "N";

  rad2grad<long double, qint64> ( latitude, this->deg_la, this->mi_la, this->se_la );
  rad2grad<long double, qint64> ( longitude, this->deg_lo, this->mi_lo, this->se_lo );
  this->flip();


  deglat = ( latitude * 180. ) / M_PI;
  deglon = ( longitude * 180. ) / M_PI;

  //--qDebug() << ( double ) deglon << ( double ) deglat;

  this->elevation = televation;


  if ( tutm_meridian != LONG_MAX )
  {
    if ( ( ( tutm_meridian + 3 ) % 6 ) == 0 )
    {
      this->utm_meridian = tutm_meridian;
      if ( this->utm_meridian >  180 )
        this->utm_meridian -= 360;
    }
    else
      qWarning() << myfunc << "UTM Merdian not valid, using auto calculation";
      this->utm_meridian = LONG_MAX;

  }
  if ( gk_meridian == LONG_MAX ) this->gk_meridian = this->utm_meridian;

  if ( ( tgk_meridian % 3 ) == 0 )
    this->gk_meridian = tgk_meridian;
  else
    qWarning() << myfunc <<  "Gauss Krueger Merdian not valid, using auto calculation" << endl;
  if ( this->gk_meridian < 0 )
    this->gk_meridian += 360;



  //--qDebug() << "class values " << deg_lo << mi_lo << (double)se_lo << " <lo la>  " << deg_la << mi_la << (double)se_la;
  //--qDebug() << LonH << "  " << LatH;

  is_init = true;

  this->utm_meridian  = LONG_MAX;

}

void geocoordinates::flip()
{

  if ( latitude < 0 )
  {
    this->deg_la *= -1;
    this->mi_la *= -1;
    this->se_la *= -1.0;
    LatH = "S";

  }

  if ( longitude < 0 )
  {
    this->deg_lo *= -1;
    this->mi_lo *= -1;
    this->se_lo *= -1.;
    LonH = "W";

  }

}

void geocoordinates::set_lon_lat_rad (  double rad_lon,  double rad_lat,  double televation = 0.,  qint64 tutm_meridian = LONG_MAX,  qint64 tgk_meridian = LONG_MAX )
{
  long double d2 = rad_lat;
  long double d1 = rad_lon;
  this->set_lon_lat_rad ( d1, d2, televation, tutm_meridian, tgk_meridian );
}



void geocoordinates::clear()
{
  latitude = longitude = rechts = hoch = deglat = deglon = 0.;
  northing = easting = 0.;
  this->deg_lo = this->mi_lo = this->deg_la = this->mi_la = 0;
  this->se_lo =  this->se_la = 0.;
  elevation = 0.;

  gk_meridian = LONG_MAX;
  utm_meridian = LONG_MAX;


  LatH = "empty";
  LonH = "empty";
  UTMzone = "empty";

}



QString geocoordinates::set_lon_lat_msec_qstr (  QString qslon,  QString qslat,  QString qs_cm_elevation )
{
  latitude = ( ( ( long double ) ( qslat.toLong() ) ) * M_PI ) / ( 3600000. * 180. );
  longitude = ( ( ( long double ) ( qslon.toLong() ) ) * M_PI ) / ( 3600000. * 180. );
  elevation = ( ( long double ) ( qs_cm_elevation.toLong() ) ) / 100.;
  this->set_lon_lat_rad ( longitude, latitude, elevation, LONG_MAX, LONG_MAX);
  this->get_lon_lat_str ( QSLongitude, QSLatitude );
  //emit get_lon_lat_qstr ( QSLongitude,  QSLatitude );
  return QSLongitude + "  " + QSLatitude;
}


void geocoordinates::get_lon_lat(  qint64& log,   qint64& lom,   double& los,   QString& LonH,
                    qint64& lag,   qint64& lam,   double& las,   QString& LatH,
                    double& televation,   qint64& tutm_meridian,   qint64& tgk_meridian) {

    log =  deg_lo;             /*! readable coodinates, grad */
    lom =  mi_lo;              /*! readable coodinates, grad */
    lag =  deg_la;             /*! readable coodinates, grad */
    lam =  mi_la;              /*! readable coodinates, grad */




    los = (double)se_lo;         /*! readable seconds longitude DEG */
    las = (double)se_la;         /*! readable seconds longitude DEG */

    LonH = this->LonH;
    LatH = this->LatH;
    televation = this->elevation;
    tutm_meridian = this->utm_meridian;
    tgk_meridian = this->gk_meridian;


}

void geocoordinates::get_lon_lat32(  int& log,   int& lom,   double& los,   QString& LonH,
                    int& lag,   int& lam,   double& las,   QString& LatH,
                    double& televation,   int& tutm_meridian,   int& tgk_meridian) {
    log =  (int)deg_lo;             /*! readable coodinates, grad */
    lom =  (int)mi_lo;              /*! readable coodinates, grad */
    lag =  (int)deg_la;             /*! readable coodinates, grad */
    lam =  (int)mi_la;              /*! readable coodinates, grad */




    los = (double)se_lo;         /*! readable seconds longitude DEG */
    las = (double)se_la;         /*! readable seconds longitude DEG */

    LonH = this->LonH;
    LatH = this->LatH;
    televation = this->elevation;
    tutm_meridian = (int)this->utm_meridian;
    tgk_meridian = (int)this->gk_meridian;


}


QString geocoordinates::get_lon_lat_str ( QString& qslon, QString& qslat )
{

  deg2QString ( deg_la, mi_la, se_la, qslat );
  deg2QString ( deg_lo, mi_lo, se_lo, qslon );
  QSLatitude = LatH + " " + qslat;
  QSLongitude = LonH + " " + qslon;

  return QSLongitude + "  " + QSLatitude;

}

QString geocoordinates::get_lon_lat_str_google_maps() {

    QString mylat, mylon;
    deg2QStringGoogle ( deg_la, mi_la, se_la, mylat );
    deg2QStringGoogle ( deg_lo, mi_lo, se_lo, mylon );

    return LatH + " " + mylat + " " + LonH + " " + mylon;

}



QString geocoordinates::get_elevation_str ( QString& selevation )
{

  selevation.setNum ( ( double ) elevation );

  return selevation + " m";

}

QString geocoordinates::get_utm_meridian_str ( QString& sutm_meridian )
{

  sutm_meridian.setNum ( utm_meridian );

  return sutm_meridian + " UTM Meridian";


}

QString geocoordinates::get_gk_meridian_str ( QString& sgk_meridian )
{

  sgk_meridian.setNum ( gk_meridian );

  return sgk_meridian + " Gauss Krueger Meridian";
}

//void geocoordinates::set_lon_lat32(  int log,   int lom,   double los,   QString LonH,
//                                   int lag,   int lam,   double las,   QString LatH,
//                                   double televation,   int tutm_meridian = INT_MAX,   int tgk_meridian = INT_MAX) {
//
//    qint64 xtutm_meridian = LONG_MAX;
//    qint64 xtgk_meridian = LONG_MAX;
//
//    if (tutm_meridian != INT_MAX)  xtutm_meridian = tutm_meridian;
//    if (tgk_meridian != INT_MAX)   xtgk_meridian = tgk_meridian;
//
//
//    this->set_lon_lat((qint64)log, (qint64)lom, (double)los, LonH,
//                      (qint64)lag, (qint64)lam, (double)las, LatH, televation, xtutm_meridian, xtgk_meridian);
//
//}


void geocoordinates::set_lon_lat (   qint64 log,   qint64 lom,   double los,   QString LonH,
                                     qint64 lag,   qint64 lam,   double las,   QString LatH,
                                     double televation ,   qint64 tutm_meridian  ,  qint64 tgk_meridian,
                                     bool min_sec_positive_only = false )
{

    if (min_sec_positive_only) {

        if (log < 0) {
            lom *= -1;
            los *= -1.;
        }

        if (lag < 0) {

            lam *= -1;
            las *= -1.;
        }


    }
  long double rad_lon = degrees2rad ( log, lom, los );
  long double rad_lat = degrees2rad ( lag, lam, las );
  if ( LonH == "W" ) rad_lon *= -1.;
  if ( LatH == "S" ) rad_lat *= -1.;
  this->set_lon_lat_rad ( rad_lon, rad_lat, televation, tutm_meridian, tgk_meridian );
  this->elevation = televation;



}

void geocoordinates::set_lon_lat (  const QString log,  const QString lom,  const QString los,  QString LonH,
                                    const QString lag,  const QString lam,  const QString las,  QString LatH )
{

  bool ok = true;
  qint64 errors = 0;
  myfunc = myname+"::set_lon_lat";
  quint64 tlog = log.toLong ( &ok, 10 );
  if ( !ok ) {qWarning() << myfunc << "can't convert"<< log << " to number"; errors++;}
  ok = true;
  quint64 tlom = lom.toLong ( &ok, 10 );
  if ( !ok ) {qWarning() << myfunc << "can't convert"<< lom << " to number"; errors++;}
  ok = true;
  double  tlos = los.toDouble ( &ok );
  if ( !ok ) {qWarning() << myfunc << "can't convert"<< los << " to number"; errors++;}
  ok = true;
  quint64 tlag = lag.toLong ( &ok, 10 );
  if ( !ok ) {qWarning() << myfunc << "can't convert"<< lag << " to number"; errors++;}
  ok = true;
  quint64 tlam = lam.toLong ( &ok, 10 );
  if ( !ok ) {qWarning() << myfunc << "can't convert"<< lam << " to number"; errors++;}
  ok = true;
  double  tlas = las.toDouble ( &ok );
  if ( !ok ) {qWarning() << myfunc << "can't convert"<< las << " to number"; errors++;}
  ok = true;

  if ( errors )
  {

    qFatal ( "could not initialize data" );
  }
  long double rad_lon = degrees2rad ( tlog, tlom, tlos );
  long double rad_lat = degrees2rad ( tlag, tlam, tlas );
  if ( LonH == "W" ) rad_lon *= -1.;
  if ( LatH == "S" ) rad_lat *= -1.;
  this->set_lon_lat_rad ( rad_lon, rad_lat );


}

bool geocoordinates::fetch_arguments (  QStringList& qargs )
{


  myfunc = myname+"::fetch_arguments";
  qint64 cmdpos;
  quint64 tutm_meridian = LONG_MAX;
  quint64 tgk_meridian  = LONG_MAX;
  quint64 televation  = 0;
  quint64 tlog = 0;
  quint64 tlom = 0;
  double  tlos = 0.;
  quint64 tlag = 0;
  quint64 tlam = 0;
  double  tlas = 0.;
  long double rad_lon = 0;
  long double rad_lat = 0;


  if ( ( ( cmdpos = qargs.indexOf ( "-deg" ) ) > 0 ) && ( qargs.size() > cmdpos + 8 ) )
  {
    tlog = qargs[cmdpos+1].toLong ( &is_init, 10 );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";

    tlom = qargs[cmdpos+2].toLong ( &is_init, 10 );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+2] << " to number";

    tlos = qargs[cmdpos+3].toDouble ( &is_init );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+3] << " to number";

    LonH = qargs[cmdpos+4];
    tlag = qargs[cmdpos+5].toLong ( &is_init, 10 );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+4] << " to number";

    tlam = qargs[cmdpos+6].toLong ( &is_init, 10 );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+5] << " to number";

    tlas = qargs[cmdpos+7].toDouble ( &is_init );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+6] << " to number";

    LatH = qargs[cmdpos+8];

  }
  else if (( cmdpos = qargs.indexOf ( "-deg" ) ) > 0 ) {
    qFatal ( "fetch_arguments could not initialize data, to few arguments likely" ) ;
    is_init = false;
    return is_init;
  }

  if ( is_init )
  {
    rad_lon = degrees2rad ( tlog, tlom, tlos );
    rad_lat = degrees2rad ( tlag, tlam, tlas );
    if ( LonH == "W" ) rad_lon *= -1.;
    if ( LatH == "S" ) rad_lat *= -1.;
  }


  if ( ( cmdpos = qargs.indexOf ( "-utm_meridian" ) ) > 0 )
  {

    tutm_meridian= qargs[cmdpos+1].toLong ( &is_init, 10 );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";

  }

  if ( ( cmdpos = qargs.indexOf ( "-gk_meridian" ) ) > 0 )
  {
    tgk_meridian = qargs[cmdpos+1].toLong ( &is_init, 10 );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";
  }
  if ( ( cmdpos = qargs.indexOf ( "-elevation" ) ) > 0 )
  {
    televation = qargs[cmdpos+1].toDouble ( &is_init );
    if ( !is_init ) qWarning() << myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";
  }

  if ( is_init ) set_lon_lat_rad ( rad_lon, rad_lat, televation, tutm_meridian, tgk_meridian );

  return is_init;
}


void geocoordinates::set_lon_lat_msec ( const qint64 msec_lon, const qint64 msec_lat, const qint64 cm_elevation )
{


  long double rad_lat = ( msec_lat * M_PI ) / ( 3600000. * 180. );
  long double rad_lon = ( msec_lon * M_PI ) / ( 3600000. * 180. );
  long double telev = cm_elevation / 100.;

 // //--qDebug() << msec_lon << msec_lat;
  this->set_lon_lat_rad ( rad_lon, rad_lat, telev );
}

void geocoordinates::get_lon_lat_msec (  qint64& msec_lon,  qint64& msec_lat,  qint64& cm_elevation )
{

    msec_lon = (longitude * ( 3600000. * 180. )) / M_PI;
    msec_lat = (latitude * ( 3600000. * 180. )) / M_PI;
    cm_elevation = elevation;

//  long double rad_lat = ( msec_lat * M_PI ) / ( 3600000. * 180. );
//  long double rad_lon = ( msec_lon * M_PI ) / ( 3600000. * 180. );
//  long double telev = cm_elevation / 100.;

 // //--qDebug() << msec_lon << msec_lat;
 // this->set_lon_lat_rad ( rad_lon, rad_lat, telev );
}


QChar geocoordinates::UTMLetterDesignator()
{
  //This routine determines the correct UTM letter designator for the given latitude
  //returns 'Z' if latitude is outside the UTM limits of 80N to 80S
  //Written by Chuck Gantz- chuck.gantz@globalstar.com
  QChar LetterDesignator;

  if ( ( 80 >= deglat ) && ( deglat > 72 ) )
    LetterDesignator = 'X';
  else if ( ( 72 >= deglat ) && ( deglat > 64 ) )
    LetterDesignator = 'W';
  else if ( ( 64 >= deglat ) && ( deglat > 56 ) )
    LetterDesignator = 'V';
  else if ( ( 56 >= deglat ) && ( deglat > 48 ) )
    LetterDesignator = 'U';
  else if ( ( 48 >= deglat ) && ( deglat > 40 ) )
    LetterDesignator = 'T';
  else if ( ( 40 >= deglat ) && ( deglat > 32 ) )
    LetterDesignator = 'S';
  else if ( ( 32 >= deglat ) && ( deglat > 24 ) )
    LetterDesignator = 'R';
  else if ( ( 24 >= deglat ) && ( deglat > 16 ) )
    LetterDesignator = 'Q';
  else if ( ( 16 >= deglat ) && ( deglat > 8 ) )
    LetterDesignator = 'P';
  else if ( ( 8 >= deglat ) && ( deglat >= 0 ) )
    LetterDesignator = 'N';
  else if ( ( 0 >= deglat ) && ( deglat > -8 ) )
    LetterDesignator = 'M';
  else if ( ( -8>= deglat ) && ( deglat > -16 ) )
    LetterDesignator = 'L';
  else if ( ( -16 >= deglat ) && ( deglat > -24 ) )
    LetterDesignator = 'K';
  else if ( ( -24 >= deglat ) && ( deglat > -32 ) )
    LetterDesignator = 'J';
  else if ( ( -32 >= deglat ) && ( deglat > -40 ) )
    LetterDesignator = 'H';
  else if ( ( -40 >= deglat ) && ( deglat > -48 ) )
    LetterDesignator = 'G';
  else if ( ( -48 >= deglat ) && ( deglat > -56 ) )
    LetterDesignator = 'F';
  else if ( ( -56 >= deglat ) && ( deglat > -64 ) )
    LetterDesignator = 'E';
  else if ( ( -64 >= deglat ) && ( deglat > -72 ) )
    LetterDesignator = 'D';
  else if ( ( -72 >= deglat ) && ( deglat > -80 ) )
    LetterDesignator = 'C';
  else
    LetterDesignator = 'Z'; //This is here as an error flag to show that the Latitude is outside the UTM limits

  return LetterDesignator;
}



QString geocoordinates::geo2UTM ( double& teasting, double& tnorthing, qint64& tutm_meridian, QString& tUTMzone )
{
  //converts lat/long to UTM coords.  Equations from USGS Bulletin 1532
  //East Longitudes are positive, West longitudes are negative.
  //North latitudes are positive, South latitudes are negative
  //Lat and Long are in decimal degrees
  //Does not take into account thespecial UTM zones between 0 degrees and
  //36 degrees longitude above 72 degrees latitude and a special zone 32
  //between 56 degrees and 64 degrees north latitude
  //Written by Chuck Gantz- chuck.gantz@globalstar.com


  if ( !is_init )
  {

    return "geocoordinates::geo2UTM no values defined";
  }


  eqradius = EquatorialRadius.value(ellipsoid);
  eccSquared = EccentricitySquared.value(ellipsoid);

  long double k0 = 0.9996;

  qint64 LongOrigin;
  long double eccPrimeSquared;
  long double N, T, C, A, M;


  long double LongOriginRad;

  if ( deglon > -6 && deglon < 0 )
    LongOrigin = -3;
  else if ( deglon < 6 && deglon >= 0 )
    LongOrigin = 3; //arbitrarily set origin at 0 longitude to 3W
  else if ( deglon == -180 || deglon == 180 )
    LongOrigin = -177;   // also forces at 180 deg to 177 W
  else
    LongOrigin = qint64 ( deglon/6. ) *  6 + 3 * qint64 ( deglon/6.) /  abs ( qint64 ( deglon / 6. ) );
  // if meridian was defined
  if ( this->utm_meridian != LONG_MAX )
    LongOrigin = this->utm_meridian;
  else
    this->utm_meridian = LongOrigin ;

  // set also Gauss Krueger meridian if not defined before
  if ( gk_meridian == LONG_MAX )
    gk_meridian = this->utm_meridian;
  if ( gk_meridian < 0 )
    gk_meridian += 360;

  LongOriginRad = LongOrigin * M_PI / 180.;
  //  char* utmzone;
  //compute the UTM Zone from the latitude and longitude
  //  sprintf(UTMzone, "%d%c", qint64((deglon + 180)/6) + 1, UTMLetterDesignator());

  long ut = long ( ( ( deglon + 180 ) /6 ) + 1 );
  UTMzone.setNum ( ut );
  UTMzone += UTMLetterDesignator();


  eccPrimeSquared = ( eccSquared ) / ( 1-eccSquared );

  N = eqradius / sqrt ( 1. - eccSquared * sin ( latitude ) * sin ( latitude ) );
  T = tan ( latitude ) * tan ( latitude );
  C = eccPrimeSquared * cos ( latitude ) * cos ( latitude );
  A = cos ( latitude ) * ( longitude - LongOriginRad );

  M = eqradius * ( ( 1.  - eccSquared/4. - 3. * eccSquared*eccSquared/64.
                     - 5. * eccSquared * eccSquared * eccSquared/256. ) * latitude
                   - ( 3. * eccSquared/8. + 3. * eccSquared * eccSquared/32.
                       + 45. * eccSquared * eccSquared*eccSquared / 1024. ) * sin ( 2. * latitude )
                   + ( 15. * eccSquared * eccSquared/256. + 45. * eccSquared * eccSquared * eccSquared/1024. )
                   * sin ( 4. * latitude )
                   - ( 35. * eccSquared * eccSquared * eccSquared/3072. ) * sin ( 6. * latitude ) );

  easting = ( long double ) ( k0 * N * ( A + ( 1 - T + C ) * A * A * A/6.
                                         + ( 5. - 18. * T + T * T + 72. * C - 58. * eccPrimeSquared ) * A * A * A * A * A/120. )
                              + 500000.0 );

  northing = ( long double ) ( k0 * ( M + N * tan ( latitude ) * ( A * A/2. + ( 5. -T +9. * C + 4. * C *C ) * A * A * A *A/24.
                                      + ( 61.- 58. * T + T * T + 600. * C - 330. * eccPrimeSquared ) * A * A * A * A * A * A/720. ) ) );

  if ( deglat < 0 )
    northing += 10000000.0; //10000000 meter offset for southern hemisphere

  QString strret, str;

  teasting = easting;
  tnorthing = northing;
  tutm_meridian = utm_meridian;
  tUTMzone = UTMzone;

  strret = "Easting: ";
  strret +=  str.setNum ( ( double ) easting, 'f', 2 );
  strret += " Northing: ";
  strret +=  str.setNum ( ( double ) northing, 'f', 2 );
  strret += " Zone: ";
  strret +=  UTMzone;
  strret += " Meridian ";
  strret +=  str.setNum ( utm_meridian );
  return  strret;
}

QString  geocoordinates::geo2gauss ( double& trechts, double& thoch, qint64& tgk_meridian )
{
  /*
  Die Laender der Bundesrepublik Deutschland verwenden als amtliche
  Bezugsflaeche das Bessel-Ellipsoid (s. /SCH81/, S.2).
  BESSEL-Ellipsoidkonstanlten:
  a : grosse Halbachse                    a  = 6 377 397,155 m
  b : kleine Halbachse                    b  = 6 356 078,963 m
  c : Polkruemmungsradius                 c  = sqr(a)/b
  e': zweite numerische Exzentrizitaet    e2 := e'2 = (a2-b2)/b2

  internationales Ellipsoid anders definiert, etwa
  a = 6378388
  b = 6356911.946
  f = 1/127 usw.

  */

  if ( !is_init )
  {

    return "geocoordinates::geo2gauss no values defined";
  }

  long double a, b, c, e2, Q, ef2, ef4, ef6,
  G, eta2, N, y,  cL, dL, c2, t2;




  // fix the problem that this algorithm works with 0...360 deg instead of
  // -180 .. 180
  // this applies also for the Meridian

  long double temp_long = longitude;
  long double three = 3.0;
  if ( longitude < 0 )
    temp_long = 2 * M_PI + longitude;


  a = 6377397.15508;
  b = 6356078.96290;
  c =  ( a * a) / b;
  e2= ( ( a * a) -  ( b * b ) ) / ( b * b );

  /*  Abgeleitete Konstanten, s. /SCH81/, A.3.9, S.14
  Q : meridianbogenlaenge vom Aequator bis zum Pol (meridianviertelkreis)
  */
  Q   =  c * ( 1. -3./4. * e2 * ( 1. -15./16. * e2 * ( 1. -35./36. * e2 * ( 1.-63./64. * e2 ) ) ) );
  ef2  =  - c * 3./8. * e2 * ( 1. -5./4. * e2 * ( 1. -35./32. * e2 * ( 1.-21./20. *e2 ) ) );
  ef4  =  c * 15./256. * e2 * e2 * ( 1. -7./4. * e2 * ( 1. -21./16. * e2 ) );
  ef6  =  - c * 35./3072. * pow ( e2, three ) * ( 1. -9./4. * e2 );




  //   G : meridianbogenlaenge zur gegebenen geographischen Breite latitude
  G = Q * latitude + ef2 * sin ( 2. * latitude ) + ef4 * sin ( 4. * latitude ) +
      ef6 * sin ( 6. * latitude );
  c2  =  ( cos ( latitude )) * ( cos ( latitude ));
  t2  =  ( tan ( latitude ) ) * ( tan ( latitude ) );
  eta2  =  e2*c2;

  //  N : Querkruemmungsradius, s. /SCH81/, A.3.6, S.11
  N  =  c / sqrt ( 1. + eta2 );
  dL  =  temp_long * 180. / M_PI - gk_meridian;
  cL  =  pow( ( dL * M_PI / 180. ), 2. ) * c2/2.;
  y  =  M_PI / 180. * N * cos ( latitude ) * dL * ( 1. +cL/3. * ( 1. - t2 + eta2 + cL/10. * ( 5. - t2 * ( 18. - t2 ) ) ) );

  rechts  =  gk_meridian / 3E-6 + 5E+5 + y;
  hoch  =  G + N * tan ( latitude ) * cL * ( 1. + cL/6. * ( 5.- t2 + 9. * eta2 ) );

  trechts = rechts;
  thoch = hoch;
  tgk_meridian = gk_meridian;

  QString strret, str;

  strret = "Rechtswert: ";
  strret +=  str.setNum ( ( double ) rechts , 'f', 2);
  strret += " Hochwert: ";
  strret +=  str.setNum ( ( double ) hoch, 'f', 2 );
  strret += " Gauss Krueger Meridian: ";
  strret +=  str.setNum ( gk_meridian );
  return  strret;

}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                          todo: implement me                             *
*                                                                         *
*                                                                         *
***************************************************************************/

void geocoordinates::gauss2geo()
{

  //rechtswert, hochwert, laenge & breite im Bogenmass als ergebnis



  /*
    Unterprogramm zur Umrechnung von Gauss-Krueger-Koordinaten (rechts,
    hoch) in geographische Koordinaten (latitude, longitude).
    Die geographischen Koordinaten latitude und longitude (Breite und Laenge)
    werden im Bogenmass berechnet.
    Im Wesentlichen geschieht die Berechnung nach SCHOEDLBAUER, Albert,
   'Rechenformeln und Rechenbeispiele zur Landesvermessung',
         Teil 1, Karlsruhe 1981: /SCH81/
     und Teil 2, Karlsruhe 1982: /SCH82/.

   Konstanten  werden im Programm mit extended Precision berechnet
   CONST e2 = 6.719219E-3;
   c = 6.398786849E6;
   Q = 6366742.520261453;
   f2 = 2.5112734435E-3;
   f4 = 3.678654E-6;
   f6 = 7.45559E-9;
   s.o.
  */


  long double  a, b, c, e2, Q, eta2, N, y,  c2, t2,
  f2, f4, f6, lh, sigma, bf, yN;
  long double three = 3.0;


  a = 6377397.15508;
  b = 6356078.96290;
  c = ( a * a ) / b;
  e2 = ( ( a* a ) - ( b * b ) ) / ( b * b );


  /*
  c  Abgeleitete Konstanten, siehe SCHOEDL., Teil 1, S.15
  c  Q : meridianbogenlaenge vom Aequator bis zum Pol (meridianviertelkreis)
  */
  Q   =  c * ( 1. -3./ 4. * e2 * ( 1. -15./ 16. * e2 * ( 1. - 35./ 36. * e2 * ( 1. -63./ 64. * e2 ) ) ) );
  f2  =  3E+0 / 8. * e2 * ( 1.-e2 / 2. * ( 1. -71./128. * e2 ) );
  f4  =  21./ 256. * e2 * e2 * ( 1. - e2 );
  f6  =  151.0 / 6144.0 * pow ( e2, three ); //{e2*e2*e2}

  //c  bf: Geographische Breite zur meridianbogenleange, s. /SCH81/, S.14f

  sigma = hoch / Q;
  bf = sigma + f2 * sin ( 2. * sigma ) + f4 * sin ( 4. * sigma ) + f6 * sin ( 6. * sigma );
  c2 = pow ( (cos ( bf )) , 2. );
  t2 = pow ( (tan ( bf )) , 2. );
  eta2 = c2 * e2;

  //c  N : Querkruemmungsradius (/SCH81/, A.3.6, S.11)
  N = c / ( sqrt ( 1. + eta2 ) );
  if ( rechts < 0.0001 )
    y = 0; // numerical error
  //   else y = rechts - floor(rechts / 1E6) * 1E6 - 5E5; // ! vgl. /SCH82/, B.2.1, S.79}
  // floor rounds up?
  else
    y = rechts - long ( rechts / 1E6 ) * 1E6 - 5E5; // ! vgl. /SCH82/, B.2.1, S.79}
  yN = pow ( (y/N) , 2. ) /2.;


  //(c  lh: Bezugsmeridian (in Bogenmass, s. /SCH82/, B.2.1, S.79
  //{     lh  = floor(rechts/1E6) * atanl(1E+0)/15;}
  //   lh  = floor(rechts/1E6) * 3. * M_PI/180.;
  // floor round up?
  lh  = long ( rechts/1E6 ) * 3. * M_PI/180.;


  latitude = bf - yN * tan ( bf ) * ( 1. + eta2 - yN/6. * ( 5. + 3. * t2 + 6. * eta2 * ( 1. - t2 )
                                      - yN * ( 4. + 3. * t2 * ( 2. + t2 ) ) ) );
  longitude = lh + y/N/cos ( bf ) * ( 1. - yN/3. *
                                      ( 1. + 2. * t2 + eta2 - yN / 10. * ( 5. + t2 * ( 28. + 24. * t2 ) ) ) );


  if ( longitude >= M_PI )
    longitude -= ( 2 * M_PI );
//  lon = ( longitude * 180. ) / M_PI;

  rad2grad<long double, qint64> ( latitude, this->deg_la, this->mi_la, this->se_la );
  rad2grad<long double, qint64> ( longitude, this->deg_lo, this->mi_lo, this->se_lo );
  flip();
}

void geocoordinates::UTM2geo() {
    //converts UTM coords to lat/long.  Equations from USGS Bulletin 1532
    //East Longitudes are positive, West longitudes are negative.
    //North latitudes are positive, South latitudes are negative
    //Lat and Long are in decimal degrees.
    //Does not take into account the special UTM zones between 0 degrees
    //and 36 degrees longitude above 72 degrees latitude and a special
    //zone 32 between 56 degrees and 64 degrees north latitude
    //Written by Chuck Gantz- chuck.gantz@globalstar.com

  long double k0 = 0.9996;
  
  eqradius = EquatorialRadius[ellipsoid];
  eccSquared = EccentricitySquared[ellipsoid];

  long double eccPrimeSquared;
  long double e1 = (1-sqrt(1-eccSquared))/(1+sqrt(1-eccSquared));
  long double N1, T1, C1, R1, D, M;
  long double LongOrigin;
  long double mu, phi1Rad;
  long double x, y;
  long double one_point_five = 1.5;
  int ZoneNumber;
  QString qsZoneNumber;
  char ZoneLetter;
  QChar qZoneLetter;

  x = easting - 500000.0; //remove 500,000 meter offset for longitude
  y = northing;

  qsZoneNumber =   UTMzone.left(2);
  ZoneNumber = qsZoneNumber.toInt();
 // ZoneNumber = atoi(UTMzone.c_str());
  
  qZoneLetter = UTMzone[UTMzone.size()-1];
  ZoneLetter = qZoneLetter.toLatin1();

    //  cerr << ZoneNumber << " " << ZoneLetter << endl;
  //--qDebug() << "PLEASE CHECK ME";
  
  if((ZoneLetter - 'N') >= 0)
  {

  }
  else {
    y -= 10000000.0;        //remove 10,000,000 meter offset used for southern hemisphere
  }

  LongOrigin = ((ZoneNumber - 1) * 6 - 180) + 3;  // +3 puts origin in middle of zone

  eccPrimeSquared = (eccSquared)/(1. - eccSquared);

  M = y / k0;
  mu = M / (eqradius * (1.- eccSquared/4. -3. * eccSquared*eccSquared/64.-
      5 * eccSquared * eccSquared * eccSquared/256.));

  phi1Rad = mu  + (3. * e1/2. -27. *e1 * e1 * e1/32.)*sin(2. * mu)
      + (21. * e1 * e1/16. - 55. * e1 * e1 * e1 * e1/32.)*sin(4. * mu)
      +(151. * e1 * e1 * e1/96.)*sin(6. * mu);

  N1 = eqradius / sqrt(1. - eccSquared * sin(phi1Rad) * sin(phi1Rad));
  T1 = tan(phi1Rad) * tan(phi1Rad);
  C1 = eccPrimeSquared * cos(phi1Rad) * cos(phi1Rad);
  R1 = eqradius * (1. - eccSquared) / pow((1. - eccSquared * sin(phi1Rad) * sin(phi1Rad)), one_point_five);
  D = x/(N1 * k0);

  latitude = phi1Rad - (N1*tan(phi1Rad)/R1)
      * (D * D/2. - (5. + 3. * T1 + 10. * C1 - 4. * C1 * C1 - 9. * eccPrimeSquared)
      * D * D * D * D/24.
      + (61. + 90. * T1 + 298. * C1 + 45.* T1 * T1 - 252. * eccPrimeSquared - 3. * C1 * C1)
      * D * D * D * D * D * D/720.);
  

  longitude = (D - ( 1.+ 2. * T1 + C1) * D * D * D/6
      + (5. - 2. * C1 + 28. * T1 - 3. * C1 * C1 + 8. *eccPrimeSquared + 24. * T1 * T1)
      * D * D * D * D * D/120.)/ cos(phi1Rad);

  long double xlon = LongOrigin + longitude * 180. / M_PI;
  longitude = (xlon * M_PI) / 180.;

  
  this->set_lon_lat_rad(longitude, latitude, this->elevation, this->utm_meridian, this->gk_meridian);
  //rad2grad<long double, qint64> ( latitude, this->deg_la, this->mi_la, this->se_la );
  //rad2grad<long double, qint64> ( longitude, this->deg_lo, this->mi_lo, this->se_lo );
  
  //--qDebug() << "PLEASE CHECK ME";
  //flip();

}


QString geocoordinates::geo2UTM32(double& teasting, double& tnorthing,  int& tutm_meridian, QString& tUTMzone) {

    qint64 xtutm_meridian = (qint64)tutm_meridian;
    QString ret;

    ret = this->geo2UTM(teasting, tnorthing, xtutm_meridian, tUTMzone);
    tutm_meridian = (int) xtutm_meridian;
    return ret;
}

QString geocoordinates::geo2gauss32 (double& trechts, double& thoch, int& tgk_meridian) {

    qint64 xtgk_meridian = (qint64) tgk_meridian;
    QString ret;

    ret = this->geo2gauss(trechts, thoch, xtgk_meridian);
    tgk_meridian = (int) xtgk_meridian;
    return ret;


}

void geocoordinates::set_utm(double tnorthing, double teasting) {


    this->northing = tnorthing;
    this->easting = teasting;

}


QString geocoordinates::get_lon_str_google_maps ()
{
    // declaration of variables
    QString qstrTmp;

    deg2QStringGoogle (deg_lo, mi_lo, se_lo, qstrTmp);

    qstrTmp = this->LonH + " " + qstrTmp;

    return (qstrTmp);
}


QString geocoordinates::get_lat_str_google_maps ()
{
    // declaration of variables
    QString qstrTmp;

    deg2QStringGoogle (deg_la, mi_la, se_la, qstrTmp);

    qstrTmp = this->LatH + " " + qstrTmp;

    return (qstrTmp);
}

