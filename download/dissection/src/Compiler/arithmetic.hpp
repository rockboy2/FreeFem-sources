/*! \file   arithmetic.hpp
    \brief  higher precision arithmetic for Kernel Detection
    \author Atsushi Suzuki, Laboratoire Jacques-Louis Lions
    \date   Jul. 17th 2015
    \date   Nov. 30th 2016
*/

// This file is part of Dissection
// 
// Dissection is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Linking Dissection statically or dynamically with other modules is making
// a combined work based on Disssection. Thus, the terms and conditions of 
// the GNU General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Dissection give you 
// permission to combine Dissection program with free software programs or 
// libraries that are released under the GNU LGPL and with independent modules 
// that communicate with Dissection solely through the Dissection-fortran 
// interface. You may copy and distribute such a system following the terms of 
// the GNU GPL for Dissection and the licenses of the other code concerned, 
// provided that you include the source code of that other code when and as
// the GNU GPL requires distribution of source code and provided that you do 
// not modify the Dissection-fortran interface.
//
// Note that people who make modified versions of Dissection are not obligated 
// to grant this special exception for their modified versions; it is their
// choice whether to do so. The GNU General Public License gives permission to 
// release a modified version without this exception; this exception also makes
// it possible to release a modified version which carries forward this
// exception. If you modify the Dissection-fortran interface, this exception 
// does not apply to your modified version of Dissection, and you must remove 
// this exception when you distribute your modified version.
//
// This exception is an additional permission under section 7 of the GNU 
// General Public License, version 3 ("GPLv3")
//
// Dissection is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dissection.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _COMPILER_ARITHMETIC_H
# define _COMPILER_ARITHMETIC_H
# include "Compiler/OptionCompiler.hpp"
#include <float.h>
#include <string>
#include <cstdio>
#include <complex>
using std::complex;

#ifdef SX_ACE
#define LONG_DOUBLE
#endif

template<typename T, typename Z>
inline T machine_epsilon() { return T(DBL_EPSILON); };

template<>
inline double machine_epsilon<double, double>() { return DBL_EPSILON; }

template<typename T>
inline std::string tostring(const T &x) { std::string dummy; return dummy; };

  
#ifdef DD_REAL
#include "qd/dd_real.h"

typedef dd_real quadruple; // implementation of quadruple precision

#include "qd/qd_real.h"

typedef qd_real octruple; // implementation of octruple precision

inline double quad2double(const quadruple &x) { return x.x[0]; }

inline double oct2double(const octruple &x) { return x.x[0]; }

inline quadruple oct2quad(const octruple &x) { return dd_real(x.x[0], x.x[1]); }

template<>
inline quadruple machine_epsilon<quadruple, quadruple>() { // argument is dummy
  return quadruple(dd_real::_eps);                     // to define type
}

template<>
inline quadruple machine_epsilon<quadruple, double>() { // argument is dummy
  return quadruple(DBL_EPSILON);
}

template<>
inline std::string tostring<quadruple>(const quadruple &x)  {
  return x.to_string(dd_real::_ndigits);
}

template<>
inline std::string tostring<complex<quadruple> >(const complex<quadruple> &x) {
  return "( " + x.real().to_string(dd_real::_ndigits) + " "
              + x.imag().to_string(dd_real::_ndigits) + " )";
}

#else  // #ifdef DD_REAL
#ifdef LONG_DOUBLE
typedef long double quadruple;
inline double quad2double(const quadruple &x) { return (double)x; }

template<>
inline quadruple machine_epsilon<quadruple, quadruple>() {
  return (long double)1.93e-34; // need to be updated for LONG DOUBLE
}

template<>
inline quadruple machine_epsilon<quadruple, double>() {
  return (long double)DBL_EPSILON;
}

inline long double fabs(const long double x) {
  return (x > 0.0L ? x : (-x));
}
#else  // #ifdef LONG_DOUBLE
#ifdef FAST_DD
#include <fast_dd.h>
typedef dd_real quadruple; // implementation of quadruple precisio

inline double quad2double(const quadruple &x) { return to_double(x); }


template<>
inline quadruple machine_epsilon<quadruple, double>() { // argument is dummy
  return dd_real(1.93e-34);  // need to be updated for LONG DOUBLE
}

template<>
inline std::string tostring<quadruple>(const quadruple &x)  { 
 char buf[256];
  sprintf(buf, "%24.16e", quad2double(x));
  return std::string(buf);
}
template<>
inline std::string tostring<complex<quadruple> >(const complex<quadruple> &x)  {
 char buf[256];
  sprintf(buf, "( %24.16e 24.16e )",
          quad2double(x.real()), quad2double(x.imag()));
  return std::string(buf);
}
// atan2 is not defined in fast_dd : approximated by double!
inline quadruple atan2(const quadruple &y, const quadruple &x) {
  double t;
  t = atan2(quad2double(y), quad2double(x));
  return dd_real(t);
}
#else // #ifdef FAST_DD
#include <quadmath.h>
typedef __float128 quadruple;
inline double quad2double(const quadruple &x) { return (double)x; }
inline quadruple sqrt(const quadruple &x) { return sqrtq(x); }
template<>
inline std::string tostring<quadruple> (const quadruple &x) {
  char buf[256];
  quadmath_snprintf(buf, 256, "%.32Qe", x);
  return std::string(buf);
}
template<>
inline std::string tostring<complex<quadruple> >(const complex<quadruple> &x) {
  char buf[256];
  quadmath_snprintf(buf, 256, "%.32Qe %.32Qe", x.real(), x.imag());
  return std::string(buf);
}

#endif // #endif FAST_DD
#endif // #endif LONG_DOUBLE
#endif // #endif DD_REAL

template<>
inline std::string tostring<double>(const double &x)  {
  char buf[256];
  sprintf(buf, "%24.16e", x);
  return std::string(buf);
}
template<>
inline std::string tostring<complex<double> >(const complex<double> &x)  {
  char buf[256];
  sprintf(buf, "(%24.16e %24.16e)", x.real(), x.imag());
  return std::string(buf);
}

#ifndef NO_OCTRUPLE
template<>
inline std::string tostring<octruple>(const octruple &x)  { return x.to_string(); }
#endif

template<typename T, typename Z>
inline T tohigher(const Z &x){ };

template<>
inline quadruple tohigher<quadruple, double>(const double &y)
{
  return quadruple(y);
}

#ifndef NO_OCTRUPLE
template<>
inline octruple tohigher<octruple, quadruple>(const quadruple &y)
{
  return octruple(y);
}
#endif

template<>
inline complex<quadruple> tohigher<complex<quadruple>, double>(const double &y)
{
  return complex<quadruple>(quadruple(y), quadruple(0.0));
}
#ifndef NO_OCTRUPLE
template<>
inline complex<octruple> tohigher<complex<octruple>, quadruple>(const quadruple &y)
{
  return complex<octruple>(octruple(y), octruple(0.0));
}
#endif

template<typename T, typename U>
inline U tolower(const T &x) { };

template<>
inline double tolower<quadruple, double>(const quadruple &x) {
  return quad2double(x);
}

template<>
inline complex<double> tolower<complex<quadruple>, complex<double> >(const complex<quadruple> &x) {
  return complex<double>(quad2double(x.real()), quad2double(x.imag()));
}

#ifndef NO_OCTRUPLE
template<>
inline quadruple tolower<octruple, quadruple>(const octruple &x) {
  return oct2quad(x);
}
template<>
inline complex<quadruple> tolower<complex<octruple>, complex<quadruple> >(const complex<octruple> &x) {
  return complex<quadruple>(oct2quad(x.real()), oct2quad(x.imag()));
}
#endif

template<typename T>
inline double todouble(const T &x){ };

template<>
inline double todouble<double>(const double &x) { return x; }
template<>
inline double todouble<quadruple>(const quadruple &x) { return quad2double(x); }
template<>
inline double todouble<complex<double> >(const complex<double> &x) { return x.real(); }
template<>
inline double todouble<complex<quadruple> >(const complex<quadruple> &x) {
  return quad2double(x.real());
}
#ifndef NO_OCTRUPLE
template<>
inline double todouble<octruple>(const octruple &x)  { return oct2double(x); }

template<>
inline double todouble<complex<octruple> >(const complex<octruple> &x) {
  return oct2double(x.real());
}
#endif


template<typename T>
void printscalar(FILE *fp, T x);
template<>
void printscalar<double>(FILE *fp, double x);
template<>
void printscalar<quadruple>(FILE *fp, quadruple x);
template<>
void printscalar<complex<double> >(FILE *fp, complex<double> x);
template<>
void printscalar<complex<quadruple> >(FILE *fp, complex<quadruple> x);

template<typename T, typename U>
T tocomplex(const U &x);
template<>
double tocomplex<double, double>(const double &x);
template<>
quadruple tocomplex<quadruple, quadruple>(const quadruple &x);
template<>
complex<double> tocomplex<complex<double>, double>(const double &x);
template<>
complex<quadruple> tocomplex<complex<quadruple>, quadruple>(const quadruple &x);


template<typename T, typename U>
inline T tocomplex(const U &x)
{
  fprintf(stderr, "%s %d : specilized template is not defined\n",
	  __FILE__, __LINE__);
  return T(0.0);
}

template<>
inline double tocomplex<double, double>(const double &x){ return x; }

template<>
inline quadruple tocomplex<quadruple, quadruple>(const quadruple &x)
{
  return x;
}

template<>
inline complex<double> tocomplex<complex<double>, double>(const double &x)
{
  return std::complex<double>(x, 0.0);
}

template<>
inline complex<quadruple> tocomplex<complex<quadruple>,
				    quadruple>(const quadruple &x)
{
  quadruple zero(0.0);
  return std::complex<quadruple>(x, zero);
}

#ifndef NO_OCTRUPLE
template<>
inline octruple tocomplex<octruple>(const octruple &x) { return x; }

template<>
inline complex<octruple> tocomplex<complex<octruple> >(const octruple &x) {
  octruple zero(0.0);
  return std::complex<octruple>(x, zero);
}
#endif
#endif
