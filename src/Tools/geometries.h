#ifndef GEOMETRIES_H
#define GEOMETRIES_H
/**
 * \file geometries.h
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



#include <climits>
#include <cmath>
#include <QString>
#include <QDebug>
// constants and defines


// class defintion

template <class T, class S> void rad2grad(const T& rad, S& gr, S& mi, T& se) {
  T h1, diff = 1.E-4;

  gr = qint64( (rad * 180.) /M_PI);      // INT SHOULD not ROUND UP ... 2.6 -> 2
  h1 = rad * 180./M_PI;
  h1 -= gr;                              // rest 0.6
  mi = qint64 (h1 * 60);                 // we should get the fraction here
  se = h1 * 3600 - mi * 60;


      // round up if we get 12:59:59.99999 to 13:00:00
  if (gr >= 0) {
    if (se <  diff) se = +0;
    h1 = 60. - se;
    if (h1 < diff) {
      se = 0.;
      if (mi != 59) mi++;
      else {
        mi = 0;
        gr++;
      }
    }
  }

  if (gr < 0) {
    if  ( -1. * se <  diff) se = -0.;
    h1 = 60. + se;
    if (h1 < diff) {
      se = 0.;
      if (mi != -59) mi--;
      else {
        mi = 0;
        gr--;
      }
    }
  }
  ////--qDebug() << "templ" << gr << mi << (double)se;
}

template <class T, class S> void deg2QString(const S& gr, const S& mi, const T& se, QString& qsdeg) {
  double setmp = (double)se;
  QString qstmp;
  //qstmp.
 //  qsdeg = QString ( "%1:" ).arg ( gr, 3, 10, QLatin1Char( '0' )  );
 //  qsdeg = QString ( "%1:" ).arg ( mi, 2, 10, QLatin1Char( '0' )  );
//   if (setmp < 10.) qsdeg += "0";
//   qsdeg += QString::number(setmp);


  qsdeg = QString::number(gr) + ":";
  if (mi < (S)10) qsdeg += "0";
  qsdeg += QString::number(mi) + ":";
  if (setmp < 10.) qsdeg += "0";
  qsdeg += QString::number(setmp);

}

template <class T, class S> void deg2QStringGoogle(const S& gr, const S& mi, const T& se, QString& qsdeg) {
  double setmp = (double)se;
  QString qstmp;
  //qstmp.
 //  qsdeg = QString ( "%1:" ).arg ( gr, 3, 10, QLatin1Char( '0' )  );
 //  qsdeg = QString ( "%1:" ).arg ( mi, 2, 10, QLatin1Char( '0' )  );
//   if (setmp < 10.) qsdeg += "0";
//   qsdeg += QString::number(setmp);


  qsdeg = QString::number(gr) + QString::fromUtf8("° ");
  if (mi < (S)10) qsdeg += "0";
  qsdeg += QString::number(mi) + "' ";
  if (setmp < 10.) qsdeg += "0";
  qsdeg += QString::number(setmp) + "''";


}

template <class T, class S> T degrees2rad(const S& gr, const S& mi, const T& se) {
  T deg = gr + mi/60. + se/3600.;
  T rad = (deg * M_PI) / 180.;
  return rad;
}


#endif // GEOMETRIES_H
