#ifndef SPACEPOINTER_LIB_SPACEPOINTER_SGP4_STATE_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_SGP4_STATE_H_

// SGP4 state record, used as part of the SGP4 propagator.
//
// Based on David Vallado's implementation of SGP4.h,
// "companion code for fundamentals of astrodynamics and applications"
// See https://celestrak.org/publications/AIAA/2006-6753/

namespace SGP4 {

  enum class OperationMode {
    AFSPC,
    IMPROVED,
  };
  enum class Method {
    DEEP_SPACE,
    NORMAL,
  };
  enum class Resonance {
    NONE,
    ONE_DAY,
    HALF_DAY,
  };

  class Sgp4GeodeticConstants {
    public:
      // Radius of earth, km
      double radiusearthkm;
      // Time units per minute.
      // A time unit seems to be the length of time it takes to orbit the earth at exactly the
      // earth's surface, divided by 2*PI (so, the time to travel one radian), because this is
      // sometimes calculated as: 1/sqrt(radiusearthkm^3/mu)
      // See also https://en.wikipedia.org/wiki/Standard_gravitational_parameter
      double xke;
      // Un-normalized zonal harmonic values
      double j2, j3, j4;
      // j3 divided by j2
      double j3oj2;
  };

  /**
   * The initialised SGP4 state, which can be passed along with a timestamp to fine the position
   * and velocity of the satellite.
   *
   * This is the equivalent of the "satrec" structure in David Vallado's SGP4 implementation.
   * I've tried to keep the variable names the same, but document whatever I can, so that this can
   * be easily compared to the original but might be more comprehensible.
   */
  class Sgp4State {
    public:
      Sgp4State(const Sgp4GeodeticConstants geo)
          : geo(geo) {};

      OperationMode operationMode;
      bool initialising;
      Method method;

      // Epoch, in julian days since 0th January 1950
      double epoch;

      // Near earth parameters

      // For perigee less than 220km, this flag is set and the equations are truncated to linear
      // variation in sqrt(a) and quadratic variation in mean anomaly. Also, the C3, delta-omega,
      // and delta-m terms are dropped.
      bool isimp;
      double aycof;
      double con41;
      double cc1, cc4, cc5;
      double d2, d3, d4;
      double delmo, eta;
      // Argument of perigee dot (rate)
      double argpdot;
      double omgcof;
      // sin(mo) mo = Mean Anomaly
      double sinmao;
      // time since the epoch, in minutes, the argument to SGP4
      double t;
      // t^n coefficients
      double t2cof, t3cof, t4cof, t5cof;

      double x1mth2, x7thm1;
      // Mean anomaly dot (rate)
      double mdot;
      // Right ascension of ascending node dot (rate)
      double nodedot;
      double xlcof, xmcof, nodecf;

      // Deep space parameters

      // Flag for resonance: 0=none, 1=one day, 2=half day
      Resonance irez;
      double d2201, d2211, d3210, d3222, d4410, d4422, d5220, d5232, d5421, d5433;
      double dedt;
      // (seems to be used for near - synchronous resonance terms)
      double del1, del2, del3;
      // Change in inclination over time.
      double didt;
      // Change in mean anomaly over time.
      double dmdt;
      // Change in mean motion over time.
      double dnodt;
      // Change in argument of perigee over time.
      double domdt;

      // Solar terms.
      double se2, se3;
      double sgh2, sgh3, sgh4;
      double sh2, sh3;
      double si2, si3;
      double sl2, sl3, sl4;

      // Greenwich sidereal time, radians.
      double gsto;

      double xfact;

      // Lunar terms.
      double e3, ee2;
      double xgh2, xgh3, xgh4;
      double xh2, xh3;
      double xi2, xi3;
      double xl2, xl3, xl4;

      double xlamo;
      double zmol, zmos;
      double atime;
      double xli;
      // Mean motion
      double xni;


      // SGP4 type drag coefficient, kg/m2er
	    double bstar;

      // Inclination - needed for lyddane modification
      double inclo;
      // Right ascension of ascending node, radians
      double nodeo;
      // Eccentricity
      double ecco;
      // Argument of perigee, radians
      double argpo;
      // Mean anomaly, radians
      double mo;
      // Mean motion, radians/minute
      double no_kozai;
      // Mean motion, radians/minute, not-kozai'd.
      double no_unkozai;

      // Singly-averaged mean elements:
      double am; // Averaged semi-major axis (earth radii)
      double em; // Averaged eccentricity
      double im; // Averaged inclination, radians
      double Om; // Averaged right ascension of ascending node, radians
      double om; // Averaged argument of perigee, radians
      double mm; // Averaged mean anomaly, radians
      double nm; // Averaged mean motion, radians/minute

      Sgp4GeodeticConstants geo;
  };

  class Sgp4InitState {
    public:
      double ao;
      double con42;
      double cosio, sinio, cosio2;
      double eccsq, omeosq, posq;
      double rp;
      double rteosq;
      double cnodm, snodm;
      double cosim, sinim;
      double em, emsq;
      double inclm;
      double nm;
      double s1, s2, s3, s4, s5, s6, s7;
      double ss1, ss2, ss3, ss4, ss5, ss6, ss7;
      double sz1, sz2, sz3;
      double sz11, sz12, sz13;
      double sz21, sz22, sz23;
      double sz31, sz32, sz33;
      double xpidot;
      double z1, z2, z3;
      double z11, z12, z13;
      double z21, z22, z23;
      double z31, z32, z33;
      double ss;
  };

  class Sgp4DpperOutputs {
    public:
      double ep;
      double inclp;
      double nodep;
      double argpp;
      double mp;
  };

  class Sgp4DspaceOutputs {
    public:
      double atime;
      double em;
      double argpm;
      double inclm;
      double xli;
      double mm;
      double xni;
      double nodem;
      double dndt;
      double nm;
  };
}

#endif
