// SGP4 Propagator, for finding where Earth satellites are over time.
// Incorporates time-varying parameters from the Sun and the Moon,
// and other components.
//
// Based on David Vallado's implementation of SGP4.cpp,
// "companion code for fundamentals of astrodynamics and applications"
// See https://celestrak.org/publications/AIAA/2006-6753/

#include "sgp4_propagator.h"

#include <cmath>

#include "time_utils.h"

SGP4::Sgp4State SGP4::initialiseSgp4(
    SGP4::WgsVersion wgsVersion,
    SGP4::OperationMode operationMode,
    SGP4::Sgp4OrbitalElements elements) {
  const double temp4 = 1.5e-12;
  SGP4::Sgp4GeodeticConstants geoConstants = getGravitationalConstants(wgsVersion);
  SGP4::Sgp4State state = SGP4::Sgp4State(geoConstants);
  SGP4::Sgp4InitState initState = {};
  state.epoch = elements.epoch;
  state.method = SGP4::Method::NORMAL;
  state.operationMode = operationMode;

  state.bstar = elements.bStarDragCoefficient;
  state.ecco = elements.eccentricity;
  state.argpo = elements.argumentOfPeriapsisRadians;
  state.inclo = elements.inclinationRadians;
  state.mo = elements.meanAnomalyRadians;
  state.no_kozai = elements.meanMotion;
  state.nodeo = elements.rightAscensionOfAscendingNodeRadians;

  state.initialising = true;
  state.t = 0.0;

  initl(state, initState);

  if ((initState.omeosq >= 0.0) || (state.no_unkozai >= 0.0)) {
    state.isimp = initState.rp < (220.0 / state.geo.radiusearthkm + 1.0);
    // Earth constants
    double sfour = 78.0 / state.geo.radiusearthkm + 1.0;
    double qzms2ttemp = (120.0 - 78.0) / state.geo.radiusearthkm;
    double qzms24 = qzms2ttemp * qzms2ttemp * qzms2ttemp * qzms2ttemp;
    double perige = (initState.rp - 1.0) * state.geo.radiusearthkm;

    // for perigees below 156 km, s and qoms2t are altered
    if (perige < 156.0) {
      sfour = perige - 78.0;
      if (perige < 98.0) {
        sfour = 20.0;
      }
      double qzms24temp = (120.0 - sfour) / state.geo.radiusearthkm;
      qzms24 = qzms24temp * qzms24temp * qzms24temp * qzms24temp;
      sfour = sfour / state.geo.radiusearthkm + 1.0;
    }
    double pinvsq = 1.0 / initState.posq;

    double tsi = 1.0 / (initState.ao - sfour);
    state.eta = initState.ao * state.ecco * tsi;
    double etasq = state.eta * state.eta;
    double eeta = state.ecco * state.eta;
    double psisq = fabs(1.0 - etasq);
    double coef = qzms24 * pow(tsi, 4.0);
    double coef1 = coef / pow(psisq, 3.5);
    double cc2 = coef1 * state.no_unkozai * (initState.ao * (1.0 + 1.5 * etasq + eeta *
      (4.0 + etasq)) + 0.375 * state.geo.j2 * tsi / psisq * state.con41 *
      (8.0 + 3.0 * etasq * (8.0 + etasq)));
    state.cc1 = state.bstar * cc2;
    double cc3 = 0.0;
    if (state.ecco > 1.0e-4) {
      cc3 = -2.0 * coef * tsi * state.geo.j3oj2 * state.no_unkozai * initState.sinio / state.ecco;
    }
    state.x1mth2 = 1.0 - initState.cosio2;
    state.cc4 = 2.0 * state.no_unkozai * coef1 * initState.ao * initState.omeosq *
      (state.eta * (2.0 + 0.5 * etasq) + state.ecco *
      (0.5 + 2.0 * etasq) - state.geo.j2 * tsi / (initState.ao * psisq) *
      (-3.0 * state.con41 * (1.0 - 2.0 * eeta + etasq *
      (1.5 - 0.5 * eeta)) + 0.75 * state.x1mth2 *
      (2.0 * etasq - eeta * (1.0 + etasq)) * cos(2.0 * state.argpo)));
    state.cc5 = 2.0 * coef1 * initState.ao * initState.omeosq * (1.0 + 2.75 *
      (etasq + eeta) + eeta * etasq);
    double cosio4 = initState.cosio2 * initState.cosio2;
    double temp1 = 1.5 * state.geo.j2 * pinvsq * state.no_unkozai;
    double temp2 = 0.5 * temp1 * state.geo.j2 * pinvsq;
    double temp3 = -0.46875 * state.geo.j4 * pinvsq * pinvsq * state.no_unkozai;
    state.mdot = state.no_unkozai + 0.5 * temp1 * initState.rteosq * state.con41 + 0.0625 *
      temp2 * initState.rteosq * (13.0 - 78.0 * initState.cosio2 + 137.0 * cosio4);
    state.argpdot = -0.5 * temp1 * initState.con42 + 0.0625 * temp2 *
      (7.0 - 114.0 * initState.cosio2 + 395.0 * cosio4) +
      temp3 * (3.0 - 36.0 * initState.cosio2 + 49.0 * cosio4);
    double xhdot1 = -temp1 * initState.cosio;
    state.nodedot = xhdot1 + (0.5 * temp2 * (4.0 - 19.0 * initState.cosio2) +
      2.0 * temp3 * (3.0 - 7.0 * initState.cosio2)) * initState.cosio;
    initState.xpidot = state.argpdot + state.nodedot;
    state.omgcof = state.bstar * cc3 * cos(state.argpo);
    state.xmcof = 0.0;
    if (state.ecco > 1.0e-4) {
      state.xmcof = -(2.0 / 3.0) * coef * state.bstar / eeta;
    }
    state.nodecf = 3.5 * initState.omeosq * xhdot1 * state.cc1;
    state.t2cof = 1.5 * state.cc1;
    if (fabs(initState.cosio + 1.0) > 1.5e-12) {
      state.xlcof = -0.25 * state.geo.j3oj2 * initState.sinio * (3.0 + 5.0 * initState.cosio) / (1.0 + initState.cosio);
    } else {
      state.xlcof = -0.25 * state.geo.j3oj2 * initState.sinio * (3.0 + 5.0 * initState.cosio) / temp4;
    }
    state.aycof = -0.5 * state.geo.j3oj2 * initState.sinio;
    double delmotemp = 1.0 + state.eta * cos(state.mo);
    state.delmo = delmotemp * delmotemp * delmotemp;
    state.sinmao = sin(state.mo);
    state.x7thm1 = 7.0 * initState.cosio2 - 1.0;

    // Deep space initialization
    if ((2 * M_PI / state.no_unkozai) >= 225.0) {
      state.method = SGP4::Method::DEEP_SPACE;
      state.isimp = true;
      initState.inclm = state.inclo;

      dscom(state, initState);

      SGP4::Sgp4DpperOutputs outputs {
        ep: state.ecco,
        inclp: state.inclo,
        nodep: state.nodeo,
        argpp: state.argpo,
        mp: state.mo,
      };
      dpper(state, outputs);
      state.ecco = outputs.ep;
      state.inclo = outputs.inclp;
      state.nodeo = outputs.nodep;
      state.argpo = outputs.argpp;
      state.mo = outputs.mp;

      dsinit(state, initState);
    }

    // Set variables if not deep space
    if (!state.isimp) {
      double cc1sq = state.cc1 * state.cc1;
      state.d2 = 4.0 * initState.ao * tsi * cc1sq;
      double temp = state.d2 * tsi * state.cc1 / 3.0;
      state.d3 = (17.0 * initState.ao + sfour) * temp;
      state.d4 = 0.5 * temp * initState.ao * tsi * (221.0 * initState.ao + 31.0 * sfour) *
        state.cc1;
      state.t3cof = state.d2 + 2.0 * cc1sq;
      state.t4cof = 0.25 * (3.0 * state.d3 + state.cc1 *
        (12.0 * state.d2 + 10.0 * cc1sq));
      state.t5cof = 0.2 * (3.0 * state.d4 +
        12.0 * state.cc1 * state.d3 +
        6.0 * state.d2 * state.d2 +
        15.0 * cc1sq * (2.0 * state.d2 + cc1sq));
    }
  }

  // Propagate to zero to initialise everything else.
  runSgp4(state, 0.0);

  state.initialising = false;
  return state;
}

void SGP4::initl(SGP4::Sgp4State &state, SGP4::Sgp4InitState &initState) {
  initState.eccsq = state.ecco * state.ecco;
  initState.omeosq = 1.0 - initState.eccsq;
  initState.rteosq = sqrt(initState.omeosq);
  initState.cosio = cos(state.inclo);
  initState.cosio2 = initState.cosio * initState.cosio;

  // Un-kozai the mean motion.
  double ak = pow(state.geo.xke / state.no_kozai, (2.0 / 3.0));
  double d1 = 0.75 * state.geo.j2 * (3.0 * initState.cosio2 - 1.0) / (initState.rteosq * initState.omeosq);
  double del = d1 / (ak * ak);
  double adel = ak * (1.0 - del * del - del * (1.0 / 3.0 + 134.0 * del * del / 81.0));
  del = d1 / (adel * adel);
  state.no_unkozai = state.no_kozai / (1.0 + del);

  initState.ao = pow(state.geo.xke / (state.no_unkozai), (2.0 / 3.0));
  initState.sinio = sin(state.inclo);
  double po = initState.ao * initState.omeosq;
  initState.con42 = 1.0 - 5.0 * initState.cosio2;
  state.con41 = -initState.con42 - initState.cosio2 - initState.cosio2;
  initState.posq = po * po;
  initState.rp = initState.ao * (1.0 - state.ecco);

  // count integer number of days from 0 jan 1970
  double ts70 = state.epoch - 7305.0;
  double ds70 = floor(ts70 + 1.0e-8);
  double tfrac = ts70 - ds70;
  // find greenwich location at epoch
  double c1 = 1.72027916940703639e-2;
  double thgr70 = 1.7321343856509374;
  double fk5r = 5.07551419432269442e-15;
  double c1p2p = c1 + (2 * M_PI);
  double gsto1 = fmod(thgr70 + c1*ds70 + c1p2p*tfrac + ts70*ts70*fk5r, 2 * M_PI);
  if (gsto1 < 0.0) {
    gsto1 = gsto1 + (2 * M_PI);
  }
  state.gsto = SGP4::greenwichSiderealTime(state.epoch + 2433281.5);
}

void SGP4::dscom(SGP4::Sgp4State &state, SGP4::Sgp4InitState &initState) {
  double ep = state.ecco;
  double argpp = state.argpo;
  double inclp = state.inclo;
  double nodep = state.nodeo;
  double np = state.no_unkozai;

  const double zes = 0.01675;
  const double zel = 0.05490;
  const double c1ss = 2.9864797e-6;
  const double c1l = 4.7968065e-7;
  const double zsinis = 0.39785416;
  const double zcosis = 0.91744867;
  const double zcosgs = 0.1945905;
  const double zsings = -0.98088458;

  initState.nm = np;
  initState.em = ep;
  double snodm = sin(nodep);
  double cnodm = cos(nodep);
  double sinomm = sin(argpp);
  double cosomm = cos(argpp);
  initState.sinim = sin(inclp);
  initState.cosim = cos(inclp);
  initState.emsq = initState.em * initState.em;
  double betasq = 1.0 - initState.emsq;
  double rtemsq = sqrt(betasq);

  // Initialize lunar solar terms
  double day = state.epoch + 18261.5;
  double xnodce = fmod(4.5236020 - 9.2422029e-4 * day, 2 * M_PI);
  double stem = sin(xnodce);
  double ctem = cos(xnodce);
  double zcosil = 0.91375164 - 0.03568096 * ctem;
  double zsinil = sqrt(1.0 - zcosil * zcosil);
  double zsinhl = 0.089683511 * stem / zsinil;
  double zcoshl = sqrt(1.0 - zsinhl * zsinhl);
  double gam = 5.8351514 + 0.0019443680 * day;
  double zx = 0.39785416 * stem / zsinil;
  double zy = zcoshl * ctem + 0.91744867 * zsinhl * stem;
  zx = atan2(zx, zy);
  zx = gam + zx - xnodce;
  double zcosgl = cos(zx);
  double zsingl = sin(zx);

  // Solar terms
  double zcosg = zcosgs;
  double zsing = zsings;
  double zcosi = zcosis;
  double zsini = zsinis;
  double zcosh = cnodm;
  double zsinh = snodm;
  double cc = c1ss;
  double xnoi = 1.0 / initState.nm;

  double a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
  double x1, x2, x3, x4, x5, x6, x7, x8;
  for (int lsflg = 1; lsflg <= 2; lsflg++) {
    a1 = zcosg * zcosh + zsing * zcosi * zsinh;
    a3 = -zsing * zcosh + zcosg * zcosi * zsinh;
    a7 = -zcosg * zsinh + zsing * zcosi * zcosh;
    a8 = zsing * zsini;
    a9 = zsing * zsinh + zcosg * zcosi * zcosh;
    a10 = zcosg * zsini;
    a2 = initState.cosim * a7 + initState.sinim * a8;
    a4 = initState.cosim * a9 + initState.sinim * a10;
    a5 = -initState.sinim * a7 + initState.cosim * a8;
    a6 = -initState.sinim * a9 + initState.cosim * a10;

    x1 = a1 * cosomm + a2 * sinomm;
    x2 = a3 * cosomm + a4 * sinomm;
    x3 = -a1 * sinomm + a2 * cosomm;
    x4 = -a3 * sinomm + a4 * cosomm;
    x5 = a5 * sinomm;
    x6 = a6 * sinomm;
    x7 = a5 * cosomm;
    x8 = a6 * cosomm;

    initState.z31 = 12.0 * x1 * x1 - 3.0 * x3 * x3;
    initState.z32 = 24.0 * x1 * x2 - 6.0 * x3 * x4;
    initState.z33 = 12.0 * x2 * x2 - 3.0 * x4 * x4;
    initState.z1 = 3.0 *  (a1 * a1 + a2 * a2) + initState.z31 * initState.emsq;
    initState.z2 = 6.0 *  (a1 * a3 + a2 * a4) + initState.z32 * initState.emsq;
    initState.z3 = 3.0 *  (a3 * a3 + a4 * a4) + initState.z33 * initState.emsq;
    initState.z11 = -6.0 * a1 * a5 + initState.emsq *  (-24.0 * x1 * x7 - 6.0 * x3 * x5);
    initState.z12 = -6.0 *  (a1 * a6 + a3 * a5) + initState.emsq *
      (-24.0 * (x2 * x7 + x1 * x8) - 6.0 * (x3 * x6 + x4 * x5));
    initState.z13 = -6.0 * a3 * a6 + initState.emsq * (-24.0 * x2 * x8 - 6.0 * x4 * x6);
    initState.z21 = 6.0 * a2 * a5 + initState.emsq * (24.0 * x1 * x5 - 6.0 * x3 * x7);
    initState.z22 = 6.0 *  (a4 * a5 + a2 * a6) + initState.emsq *
      (24.0 * (x2 * x5 + x1 * x6) - 6.0 * (x4 * x7 + x3 * x8));
    initState.z23 = 6.0 * a4 * a6 + initState.emsq * (24.0 * x2 * x6 - 6.0 * x4 * x8);
    initState.z1 = initState.z1 + initState.z1 + betasq * initState.z31;
    initState.z2 = initState.z2 + initState.z2 + betasq * initState.z32;
    initState.z3 = initState.z3 + initState.z3 + betasq * initState.z33;
    initState.s3 = cc * xnoi;
    initState.s2 = -0.5 * initState.s3 / rtemsq;
    initState.s4 = initState.s3 * rtemsq;
    initState.s1 = -15.0 * initState.em * initState.s4;
    initState.s5 = x1 * x3 + x2 * x4;
    initState.s6 = x2 * x3 + x1 * x4;
    initState.s7 = x2 * x4 - x1 * x3;

    // Lunar terms
    if (lsflg == 1) {
      initState.ss1 = initState.s1;
      initState.ss2 = initState.s2;
      initState.ss3 = initState.s3;
      initState.ss4 = initState.s4;
      initState.ss5 = initState.s5;
      initState.ss6 = initState.s6;
      initState.ss7 = initState.s7;
      initState.sz1 = initState.z1;
      initState.sz2 = initState.z2;
      initState.sz3 = initState.z3;
      initState.sz11 = initState.z11;
      initState.sz12 = initState.z12;
      initState.sz13 = initState.z13;
      initState.sz21 = initState.z21;
      initState.sz22 = initState.z22;
      initState.sz23 = initState.z23;
      initState.sz31 = initState.z31;
      initState.sz32 = initState.z32;
      initState.sz33 = initState.z33;
      zcosg = zcosgl;
      zsing = zsingl;
      zcosi = zcosil;
      zsini = zsinil;
      zcosh = zcoshl * cnodm + zsinhl * snodm;
      zsinh = snodm * zcoshl - cnodm * zsinhl;
      cc = c1l;
    }
  }

  state.zmol = fmod(4.7199672 + 0.22997150  * day - gam, 2 * M_PI);
  state.zmos = fmod(6.2565837 + 0.017201977 * day, 2 * M_PI);

  // Solar terms
  state.se2 = 2.0 * initState.ss1 * initState.ss6;
  state.se3 = 2.0 * initState.ss1 * initState.ss7;
  state.si2 = 2.0 * initState.ss2 * initState.sz12;
  state.si3 = 2.0 * initState.ss2 * (initState.sz13 - initState.sz11);
  state.sl2 = -2.0 * initState.ss3 * initState.sz2;
  state.sl3 = -2.0 * initState.ss3 * (initState.sz3 - initState.sz1);
  state.sl4 = -2.0 * initState.ss3 * (-21.0 - 9.0 * initState.emsq) * zes;
  state.sgh2 = 2.0 * initState.ss4 * initState.sz32;
  state.sgh3 = 2.0 * initState.ss4 * (initState.sz33 - initState.sz31);
  state.sgh4 = -18.0 * initState.ss4 * zes;
  state.sh2 = -2.0 * initState.ss2 * initState.sz22;
  state.sh3 = -2.0 * initState.ss2 * (initState.sz23 - initState.sz21);

  // Lunar terms
  state.ee2 = 2.0 * initState.s1 * initState.s6;
  state.e3 = 2.0 * initState.s1 * initState.s7;
  state.xi2 = 2.0 * initState.s2 * initState.z12;
  state.xi3 = 2.0 * initState.s2 * (initState.z13 - initState.z11);
  state.xl2 = -2.0 * initState.s3 * initState.z2;
  state.xl3 = -2.0 * initState.s3 * (initState.z3 - initState.z1);
  state.xl4 = -2.0 * initState.s3 * (-21.0 - 9.0 * initState.emsq) * zel;
  state.xgh2 = 2.0 * initState.s4 * initState.z32;
  state.xgh3 = 2.0 * initState.s4 * (initState.z33 - initState.z31);
  state.xgh4 = -18.0 * initState.s4 * zel;
  state.xh2 = -2.0 * initState.s2 * initState.z22;
  state.xh3 = -2.0 * initState.s2 * (initState.z23 - initState.z21);
}

// Provides deep space long-period periodic contributions to the mean elements.
void SGP4::dpper(const SGP4::Sgp4State &state, SGP4::Sgp4DpperOutputs &outputs) {
  double zns = 1.19459e-5;
  double zes = 0.01675;
  double znl = 1.5835218e-4;
  double zel = 0.05490;

  // Calculate time varying periodics
  double zm = state.zmos + zns * state.t;
  double zf = zm + 2.0 * zes * sin(zm);
  double sinzf = sin(zf);
  double f2 = 0.5 * sinzf * sinzf - 0.25;
  double f3 = -0.5 * sinzf * cos(zf);
  double ses = state.se2 * f2 + state.se3 * f3;
  double sis = state.si2 * f2 + state.si3 * f3;
  double sls = state.sl2 * f2 + state.sl3 * f3 + state.sl4 * sinzf;
  double sghs = state.sgh2 * f2 + state.sgh3 * f3 + state.sgh4 * sinzf;
  double shs = state.sh2 * f2 + state.sh3 * f3;

  zm = state.zmol + znl * state.t;
  zf = zm + 2.0 * zel * sin(zm);
  sinzf = sin(zf);
  f2 = 0.5 * sinzf * sinzf - 0.25;
  f3 = -0.5 * sinzf * cos(zf);
  double sel = state.ee2 * f2 + state.e3 * f3;
  double sil = state.xi2 * f2 + state.xi3 * f3;
  double sll = state.xl2 * f2 + state.xl3 * f3 + state.xl4 * sinzf;
  double sghl = state.xgh2 * f2 + state.xgh3 * f3 + state.xgh4 * sinzf;
  double shll = state.xh2 * f2 + state.xh3 * f3;
  double pe = ses + sel;
  double pinc = sis + sil;
  double pl = sls + sll;
  double pgh = sghs + sghl;
  double ph = shs + shll;

  if (!state.initialising) {
    outputs.inclp = outputs.inclp + pinc;
    outputs.ep = outputs.ep + pe;
    double sinip = sin(outputs.inclp);
    double cosip = cos(outputs.inclp);

    // Apply periodics directly
    //  sgp4fix for lyddane choice
    //  strn3 used original inclination - this is technically feasible
    //  gsfc used perturbed inclination - also technically feasible
    //  probably best to readjust the 0.2 limit value and limit discontinuity
    //  0.2 rad = 11.45916 deg
    //  use next line for original strn3 approach and original inclination
    //  if (state.inclo >= 0.2)
    //  use next line for gsfc version and perturbed inclination
    if (outputs.inclp >= 0.2) {
      ph = ph / sinip;
      pgh = pgh - cosip * ph;
      outputs.argpp = outputs.argpp + pgh;
      outputs.nodep = outputs.nodep + ph;
      outputs.mp = outputs.mp + pl;
    } else {
      // Apply periodics with lyddane modification
      double sinop = sin(outputs.nodep);
      double cosop = cos(outputs.nodep);
      double alfdp = sinip * sinop;
      double betdp = sinip * cosop;
      double dalf = ph * cosop + pinc * cosip * sinop;
      double dbet = -ph * sinop + pinc * cosip * cosop;
      alfdp = alfdp + dalf;
      betdp = betdp + dbet;
      outputs.nodep = fmod(outputs.nodep, 2 * M_PI);
      if ((outputs.nodep < 0.0) && (state.operationMode == SGP4::OperationMode::AFSPC)) {
        outputs.nodep = outputs.nodep + 2 * M_PI;
      }
      double xls = outputs.mp + outputs.argpp + cosip * outputs.nodep;
      double dls = pl + pgh - pinc * outputs.nodep * sinip;
      xls = xls + dls;
      double xnoh = outputs.nodep;
      outputs.nodep = atan2(alfdp, betdp);
      if ((outputs.nodep < 0.0) && (state.operationMode == SGP4::OperationMode::AFSPC)) {
        outputs.nodep = outputs.nodep + 2 * M_PI;
      }
      if (fabs(xnoh - outputs.nodep) > M_PI) {
        if (outputs.nodep < xnoh) {
          outputs.nodep = outputs.nodep + 2 * M_PI;
        } else {
          outputs.nodep = outputs.nodep - 2 * M_PI;
        }
      }
      outputs.mp = outputs.mp + pl;
      outputs.argpp = xls - outputs.mp - cosip * outputs.nodep;
    }
  }
}

void SGP4::dspace(const SGP4::Sgp4State &state, SGP4::Sgp4DspaceOutputs &outputs) {
  double fasx2 = 0.13130908;
  double fasx4 = 2.8843198;
  double fasx6 = 0.37448087;
  double g22 = 5.7686396;
  double g32 = 0.95240898;
  double g44 = 1.8014998;
  double g52 = 1.0508330;
  double g54 = 4.4108898;
  double rptim = 4.37526908801129966e-3; // 7.29211514668855e-5 rad/sec
  double stepp = 720.0;
  double stepn = -720.0;
  double step2 = 259200.0;

  // Calculate deep space resonance effects
  outputs.dndt = 0.0;
  double theta = fmod(state.gsto + state.t * rptim, 2 * M_PI);
  outputs.em = outputs.em + state.dedt * state.t;

  outputs.inclm = outputs.inclm + state.didt * state.t;
  outputs.argpm = outputs.argpm + state.domdt * state.t;
  outputs.nodem = outputs.nodem + state.dnodt * state.t;
  outputs.mm = outputs.mm + state.dmdt * state.t;

  // Update resonances: numerical (euler-maclaurin) integration
  // Epoch restart
  double ft = 0.0;
  if (state.irez != SGP4::Resonance::NONE) {
    if ((outputs.atime == 0.0) || (state.t * outputs.atime <= 0.0) || (fabs(state.t) < fabs(outputs.atime))) {
      outputs.atime = 0.0;
      outputs.xni = state.no_unkozai;
      outputs.xli = state.xlamo;
    }
    double delt;
    if (state.t > 0.0) {
      delt = stepp;
    } else {
      delt = stepn;
    }

    double xndt;
    double xldot;
    double xnddt;
    while (true) {
      // Dot terms calculated
      if (state.irez == SGP4::Resonance::ONE_DAY) {
        // Near-synchronous resonance terms
        xndt = state.del1 * sin(outputs.xli - fasx2) + state.del2 * sin(2.0 * (outputs.xli - fasx4)) +
          state.del3 * sin(3.0 * (outputs.xli - fasx6));
        xldot = outputs.xni + state.xfact;
        xnddt = state.del1 * cos(outputs.xli - fasx2) +
          2.0 * state.del2 * cos(2.0 * (outputs.xli - fasx4)) +
          3.0 * state.del3 * cos(3.0 * (outputs.xli - fasx6));
        xnddt = xnddt * xldot;
      } else {
        // Near-half-day resonance terms
        double xomi = state.argpo + state.argpdot * outputs.atime;
        double x2omi = xomi + xomi;
        double x2li = outputs.xli + outputs.xli;
        xndt = state.d2201 * sin(x2omi + outputs.xli - g22) + state.d2211 * sin(outputs.xli - g22) +
          state.d3210 * sin(xomi + outputs.xli - g32) + state.d3222 * sin(-xomi + outputs.xli - g32) +
          state.d4410 * sin(x2omi + x2li - g44) + state.d4422 * sin(x2li - g44) +
          state.d5220 * sin(xomi + outputs.xli - g52) + state.d5232 * sin(-xomi + outputs.xli - g52) +
          state.d5421 * sin(xomi + x2li - g54) + state.d5433 * sin(-xomi + x2li - g54);
        xldot = outputs.xni + state.xfact;
        xnddt = state.d2201 * cos(x2omi + outputs.xli - g22) + state.d2211 * cos(outputs.xli - g22) +
          state.d3210 * cos(xomi + outputs.xli - g32) + state.d3222 * cos(-xomi + outputs.xli - g32) +
          state.d5220 * cos(xomi + outputs.xli - g52) + state.d5232 * cos(-xomi + outputs.xli - g52) +
          2.0 * (state.d4410 * cos(x2omi + x2li - g44) +
          state.d4422 * cos(x2li - g44) + state.d5421 * cos(xomi + x2li - g54) +
          state.d5433 * cos(-xomi + x2li - g54));
        xnddt = xnddt * xldot;
      }

      // Integrator
      if (fabs(state.t - outputs.atime) < stepp) {
        ft = state.t - outputs.atime;
        break;
      }

      outputs.xli = outputs.xli + xldot * delt + xndt * step2;
      outputs.xni = outputs.xni + xndt * delt + xnddt * step2;
      outputs.atime = outputs.atime + delt;
    }

    outputs.nm = outputs.xni + xndt * ft + xnddt * ft * ft * 0.5;
    double xl = outputs.xli + xldot * ft + xndt * ft * ft * 0.5;
    if (state.irez == SGP4::Resonance::ONE_DAY) {
      outputs.mm = xl - outputs.nodem - outputs.argpm + theta;
      outputs.dndt = outputs.nm - state.no_unkozai;
    } else {
      outputs.mm = xl - 2.0 * outputs.nodem + 2.0 * theta;
      outputs.dndt = outputs.nm - state.no_unkozai;
    }
    outputs.nm = state.no_unkozai + outputs.dndt;
  }
}

void SGP4::dsinit(SGP4::Sgp4State &state, SGP4::Sgp4InitState &initState) {
  double no = state.no_unkozai;
  double emsq = initState.emsq;

  double q22 = 1.7891679e-6;
  double q31 = 2.1460748e-6;
  double q33 = 2.2123015e-7;
  double root22 = 1.7891679e-6;
  double root44 = 7.3636953e-9;
  double root54 = 2.1765803e-9;
  double rptim = 4.37526908801129966e-3; // this equates to 7.29211514668855e-5 rad/sec
  double root32 = 3.7393792e-7;
  double root52 = 1.1428639e-7;
  double znl = 1.5835218e-4;
  double zns = 1.19459e-5;

  // Deep space initialization
  state.irez = Resonance::NONE;
  if ((initState.nm < 0.0052359877) && (initState.nm > 0.0034906585)) {
    state.irez = Resonance::ONE_DAY;
  }
  if ((initState.nm >= 8.26e-3) && (initState.nm <= 9.24e-3) && (initState.em >= 0.5)) {
    state.irez = Resonance::HALF_DAY;
  }

  // Solar terms
  double ses = initState.ss1 * zns * initState.ss5;
  double sis = initState.ss2 * zns * (initState.sz11 + initState.sz13);
  double sls = -zns * initState.ss3 * (initState.sz1 + initState.sz3 - 14.0 - 6.0 * emsq);
  double sghs = initState.ss4 * zns * (initState.sz31 + initState.sz33 - 6.0);
  double shs = -zns * initState.ss2 * (initState.sz21 + initState.sz23);
  if ((initState.inclm < 5.2359877e-2) || (initState.inclm > M_PI - 5.2359877e-2)) {
    shs = 0.0;
  }
  if (initState.sinim != 0.0) {
    shs = shs / initState.sinim;
  }
  double sgs = sghs - initState.cosim * shs;

  // Lunar terms
  state.dedt = ses + initState.s1 * znl * initState.s5;
  state.didt = sis + initState.s2 * znl * (initState.z11 + initState.z13);
  state.dmdt = sls - znl * initState.s3 * (initState.z1 + initState.z3 - 14.0 - 6.0 * emsq);
  double sghl = initState.s4 * znl * (initState.z31 + initState.z33 - 6.0);
  double shll = -znl * initState.s2 * (initState.z21 + initState.z23);
  if ((initState.inclm < 5.2359877e-2) || (initState.inclm > M_PI - 5.2359877e-2)) {
    shll = 0.0;
  }
  state.domdt = sgs + sghl;
  state.dnodt = shs;
  if (initState.sinim != 0.0) {
    state.domdt = state.domdt - initState.cosim / initState.sinim * shll;
    state.dnodt = state.dnodt + shll / initState.sinim;
  }

  // Calculate deep space resonance effects
  double theta = fmod(state.gsto, 2 * M_PI);
  initState.em = initState.em + state.dedt * state.t;
  initState.inclm = initState.inclm + state.didt * state.t;

  // Initialize the resonance terms
  if (state.irez != Resonance::NONE) {
    double aonv = pow(initState.nm / state.geo.xke, (2.0 / 3.0));

    // Geopotential resonance for 12 hour orbits
    if (state.irez == Resonance::HALF_DAY) {
      double cosisq = initState.cosim * initState.cosim;
      double emo = initState.em;
      initState.em = state.ecco;
      double emsqo = emsq;
      emsq = initState.eccsq;
      double eoc = initState.em * emsq;
      double g201 = -0.306 - (initState.em - 0.64) * 0.440;

      double g211, g310, g322, g410, g422, g520;
      if (initState.em <= 0.65) {
        g211 = 3.616 - 13.2470 * initState.em + 16.2900 * emsq;
        g310 = -19.302 + 117.3900 * initState.em - 228.4190 * emsq + 156.5910 * eoc;
        g322 = -18.9068 + 109.7927 * initState.em - 214.6334 * emsq + 146.5816 * eoc;
        g410 = -41.122 + 242.6940 * initState.em - 471.0940 * emsq + 313.9530 * eoc;
        g422 = -146.407 + 841.8800 * initState.em - 1629.014 * emsq + 1083.4350 * eoc;
        g520 = -532.114 + 3017.977 * initState.em - 5740.032 * emsq + 3708.2760 * eoc;
      } else {
        g211 = -72.099 + 331.819 * initState.em - 508.738 * emsq + 266.724 * eoc;
        g310 = -346.844 + 1582.851 * initState.em - 2415.925 * emsq + 1246.113 * eoc;
        g322 = -342.585 + 1554.908 * initState.em - 2366.899 * emsq + 1215.972 * eoc;
        g410 = -1052.797 + 4758.686 * initState.em - 7193.992 * emsq + 3651.957 * eoc;
        g422 = -3581.690 + 16178.110 * initState.em - 24462.770 * emsq + 12422.520 * eoc;
        if (initState.em > 0.715) {
          g520 = -5149.66 + 29936.92 * initState.em - 54087.36 * emsq + 31324.56 * eoc;
        } else {
          g520 = 1464.74 - 4664.75 * initState.em + 3763.64 * emsq;
        }
      }
      double g533, g521, g532;
      if (initState.em < 0.7) {
        g533 = -919.22770 + 4988.6100 * initState.em - 9064.7700 * emsq + 5542.21  * eoc;
        g521 = -822.71072 + 4568.6173 * initState.em - 8491.4146 * emsq + 5337.524 * eoc;
        g532 = -853.66600 + 4690.2500 * initState.em - 8624.7700 * emsq + 5341.4  * eoc;
      } else {
        g533 = -37995.780 + 161616.52 * initState.em - 229838.20 * emsq + 109377.94 * eoc;
        g521 = -51752.104 + 218913.95 * initState.em - 309468.16 * emsq + 146349.42 * eoc;
        g532 = -40023.880 + 170470.89 * initState.em - 242699.48 * emsq + 115605.82 * eoc;
      }

      double sini2 = initState.sinim * initState.sinim;
      double f220 = 0.75 * (1.0 + 2.0 * initState.cosim + cosisq);
      double f221 = 1.5 * sini2;
      double f321 = 1.875 * initState.sinim * (1.0 - 2.0 * initState.cosim - 3.0 * cosisq);
      double f322 = -1.875 * initState.sinim * (1.0 + 2.0 * initState.cosim - 3.0 * cosisq);
      double f441 = 35.0 * sini2 * f220;
      double f442 = 39.3750 * sini2 * sini2;
      double f522 = 9.84375 * initState.sinim * (sini2 * (1.0 - 2.0 * initState.cosim - 5.0 * cosisq) +
        0.33333333 * (-2.0 + 4.0 * initState.cosim + 6.0 * cosisq));
      double f523 = initState.sinim * (4.92187512 * sini2 * (-2.0 - 4.0 * initState.cosim +
        10.0 * cosisq) + 6.56250012 * (1.0 + 2.0 * initState.cosim - 3.0 * cosisq));
      double f542 = 29.53125 * initState.sinim * (2.0 - 8.0 * initState.cosim + cosisq *
        (-12.0 + 8.0 * initState.cosim + 10.0 * cosisq));
      double f543 = 29.53125 * initState.sinim * (-2.0 - 8.0 * initState.cosim + cosisq *
        (12.0 + 8.0 * initState.cosim - 10.0 * cosisq));
      double xno2 = initState.nm * initState.nm;
      double ainv2 = aonv * aonv;
      double temp1 = 3.0 * xno2 * ainv2;
      double temp = temp1 * root22;
      state.d2201 = temp * f220 * g201;
      state.d2211 = temp * f221 * g211;
      temp1 = temp1 * aonv;
      temp = temp1 * root32;
      state.d3210 = temp * f321 * g310;
      state.d3222 = temp * f322 * g322;
      temp1 = temp1 * aonv;
      temp = 2.0 * temp1 * root44;
      state.d4410 = temp * f441 * g410;
      state.d4422 = temp * f442 * g422;
      temp1 = temp1 * aonv;
      temp = temp1 * root52;
      state.d5220 = temp * f522 * g520;
      state.d5232 = temp * f523 * g532;
      temp = 2.0 * temp1 * root54;
      state.d5421 = temp * f542 * g521;
      state.d5433 = temp * f543 * g533;
      state.xlamo = fmod(state.mo + state.nodeo + state.nodeo - theta - theta, 2 * M_PI);
      state.xfact = state.mdot + state.dmdt + 2.0 * (state.nodedot + state.dnodt - rptim) - no;
      initState.em = emo;
      emsq = emsqo;
    }

    // Synchronous resonance terms
    if (state.irez == Resonance::ONE_DAY) {
      double g200 = 1.0 + emsq * (-2.5 + 0.8125 * emsq);
      double g310 = 1.0 + 2.0 * emsq;
      double g300 = 1.0 + emsq * (-6.0 + 6.60937 * emsq);
      double f220 = 0.75 * (1.0 + initState.cosim) * (1.0 + initState.cosim);
      double f311 = 0.9375 * initState.sinim * initState.sinim * (1.0 + 3.0 * initState.cosim)
          - 0.75 * (1.0 + initState.cosim);
      double f330 = 1.0 + initState.cosim;
      f330 = 1.875 * f330 * f330 * f330;
      state.del1 = 3.0 * initState.nm * initState.nm * aonv * aonv;
      state.del2 = 2.0 * state.del1 * f220 * g200 * q22;
      state.del3 = 3.0 * state.del1 * f330 * g300 * q33 * aonv;
      state.del1 = state.del1 * f311 * g310 * q31 * aonv;
      state.xlamo = fmod(state.mo + state.nodeo + state.argpo - theta, 2 * M_PI);
      state.xfact = state.mdot + initState.xpidot - rptim + state.dmdt + state.domdt + state.dnodt - no;
    }

    // For sgp4, initialize the integrator
    state.xli = state.xlamo;
    state.xni = no;
    state.atime = 0.0;
    initState.nm = no;
  }
}

SGP4::Sgp4Result SGP4::runSgp4(SGP4::Sgp4State &state, double timeSinceEpochMinutes) {

  // Set mathematical constants
  const double temp4 = 1.5e-12;
  double vkmpersec = state.geo.radiusearthkm * state.geo.xke / 60.0;

  state.t = timeSinceEpochMinutes;

  // Update for secular gravity and atmospheric drag
  double xmdf = state.mo + state.mdot * state.t;
  double argpdf = state.argpo + state.argpdot * state.t;
  double nodedf = state.nodeo + state.nodedot * state.t;
  double argpm = argpdf;
  double mm = xmdf;
  double t2 = state.t * state.t;
  double nodem = nodedf + state.nodecf * t2;
  double tempa = 1.0 - state.cc1 * state.t;
  double tempe = state.bstar * state.cc4 * state.t;
  double templ = state.t2cof * t2;

  if (!state.isimp) {
    double delomg = state.omgcof * state.t;
    double delmtemp = 1.0 + state.eta * cos(xmdf);
    double delm = state.xmcof *
      (delmtemp * delmtemp * delmtemp -
      state.delmo);
    double temp = delomg + delm;
    mm = xmdf + temp;
    argpm = argpdf - temp;
    double t3 = t2 * state.t;
    double t4 = t3 * state.t;
    tempa = tempa - state.d2 * t2 - state.d3 * t3 -
      state.d4 * t4;
    tempe = tempe + state.bstar * state.cc5 * (sin(mm) -
      state.sinmao);
    templ = templ + state.t3cof * t3 + t4 * (state.t4cof +
      state.t * state.t5cof);
  }

  double nm = state.no_unkozai;
  double em = state.ecco;
  double inclm = state.inclo;
  if (state.method == SGP4::Method::DEEP_SPACE) {
    Sgp4DspaceOutputs dspaceOutputs {
      atime: state.atime,
      em: em,
      argpm: argpm,
      inclm: inclm,
      xli: state.xli,
      mm: mm,
      xni: state.xni,
      nodem: nodem,
      dndt: 0,
      nm: nm,
    };
    dspace(state, dspaceOutputs);
    state.atime = dspaceOutputs.atime;
    em = dspaceOutputs.em;
    argpm = dspaceOutputs.argpm;
    inclm = dspaceOutputs.inclm;
    state.xli = dspaceOutputs.xli;
    mm = dspaceOutputs.mm;
    state.xni = dspaceOutputs.xni;
    nodem = dspaceOutputs.nodem;
    nm = dspaceOutputs.nm;
  }

  if (nm <= 0.0) {
    return SGP4::Sgp4Result {
      code: SGP4::ResultCode::NEGATIVE_MEAN_MOTION
    };
  }
  double am = pow((state.geo.xke / nm), (2.0 / 3.0)) * tempa * tempa;
  nm = state.geo.xke / pow(am, 1.5);
  em = em - tempe;

  if ((em >= 1.0) || (em < -0.001)) {
    return SGP4::Sgp4Result {
      code: SGP4::ResultCode::BAD_MEAN_ELEMENTS
    };
  }
  if (em < 1.0e-6) {
    em = 1.0e-6;
  }
  mm = mm + state.no_unkozai * templ;
  double xlm = mm + argpm + nodem;
  double emsq = em * em;
  double temp = 1.0 - emsq;

  nodem = fmod(nodem, 2 * M_PI);
  argpm = fmod(argpm, 2 * M_PI);
  xlm = fmod(xlm, 2 * M_PI);
  mm = fmod(xlm - argpm - nodem, 2 * M_PI);

  // Recover singly averaged mean elements
  state.am = am;
  state.em = em;
  state.im = inclm;
  state.Om = nodem;
  state.om = argpm;
  state.mm = mm;
  state.nm = nm;

  // Compute extra mean quantities
  double sinim = sin(inclm);
  double cosim = cos(inclm);

  // Add lunar-solar periodics
  double ep = em;
  double xincp = inclm;
  double argpp = argpm;
  double nodep = nodem;
  double mp = mm;
  double sinip = sinim;
  double cosip = cosim;
  if (state.method == SGP4::Method::DEEP_SPACE) {
    SGP4::Sgp4DpperOutputs outputs {
      ep: ep,
      inclp: xincp,
      nodep: nodep,
      argpp: argpp,
      mp: mp,
    };
    dpper(state, outputs);
    ep = outputs.ep;
    xincp = outputs.inclp;
    nodep = outputs.nodep;
    argpp = outputs.argpp;
    mp = outputs.mp;

    if (xincp < 0.0) {
      xincp = -xincp;
      nodep = nodep + M_PI;
      argpp = argpp - M_PI;
    }
    if ((ep < 0.0) || (ep > 1.0)) {
      return SGP4::Sgp4Result {
        code: SGP4::ResultCode::BAD_PERTURBATION_ELEMENTS
      };
    }
  }

  // Long period periodics
  if (state.method == SGP4::Method::DEEP_SPACE) {
    sinip = sin(xincp);
    cosip = cos(xincp);
    state.aycof = -0.5 * state.geo.j3oj2 * sinip;
    if (fabs(cosip + 1.0) > 1.5e-12) {
      state.xlcof = -0.25 * state.geo.j3oj2 * sinip * (3.0 + 5.0 * cosip) / (1.0 + cosip);
    } else {
      state.xlcof = -0.25 * state.geo.j3oj2 * sinip * (3.0 + 5.0 * cosip) / temp4;
    }
  }
  double axnl = ep * cos(argpp);
  temp = 1.0 / (am * (1.0 - ep * ep));
  double aynl = ep* sin(argpp) + temp * state.aycof;
  double xl = mp + argpp + nodep + temp * state.xlcof * axnl;

  // Solve Kepler's equation
  double u = fmod(xl - nodep, 2 * M_PI);
  double eo1 = u;
  double sineo1, coseo1;
  double tem5 = 9999.9;
  int ktr = 1;
  while ((fabs(tem5) >= 1.0e-12) && (ktr <= 10)) {
    sineo1 = sin(eo1);
    coseo1 = cos(eo1);
    tem5 = 1.0 - coseo1 * axnl - sineo1 * aynl;
    tem5 = (u - aynl * coseo1 + axnl * sineo1 - eo1) / tem5;
    if (fabs(tem5) >= 0.95) {
      tem5 = tem5 > 0.0 ? 0.95 : -0.95;
    }
    eo1 = eo1 + tem5;
    ktr = ktr + 1;
  }

  // Short period preliminary quantities
  double ecose = axnl * coseo1 + aynl * sineo1;
  double esine = axnl * sineo1 - aynl * coseo1;
  double el2 = axnl * axnl + aynl * aynl;
  double pl = am * (1.0 - el2);
  if (pl < 0.0) {
      return SGP4::Sgp4Result {
        code: SGP4::ResultCode::BAD_SEMILATUS_RECTUM
      };
  }
  double rl = am * (1.0 - ecose);
  double rdotl = sqrt(am) * esine / rl;
  double rvdotl = sqrt(pl) / rl;
  double betal = sqrt(1.0 - el2);
  temp = esine / (1.0 + betal);
  double sinu = am / rl * (sineo1 - aynl - axnl * temp);
  double cosu = am / rl * (coseo1 - axnl + aynl * temp);
  double su = atan2(sinu, cosu);
  double sin2u = (cosu + cosu) * sinu;
  double cos2u = 1.0 - 2.0 * sinu * sinu;
  temp = 1.0 / pl;
  double temp1 = 0.5 * state.geo.j2 * temp;
  double temp2 = temp1 * temp;

  // Update for short period periodics
  if (state.method == SGP4::Method::DEEP_SPACE) {
    double cosisq = cosip * cosip;
    state.con41 = 3.0 * cosisq - 1.0;
    state.x1mth2 = 1.0 - cosisq;
    state.x7thm1 = 7.0 * cosisq - 1.0;
  }
  double mrt = rl * (1.0 - 1.5 * temp2 * betal * state.con41) +
    0.5 * temp1 * state.x1mth2 * cos2u;
  su = su - 0.25 * temp2 * state.x7thm1 * sin2u;
  double xnode = nodep + 1.5 * temp2 * cosip * sin2u;
  double xinc = xincp + 1.5 * temp2 * cosip * sinip * cos2u;
  double mvt = rdotl - nm * temp1 * state.x1mth2 * sin2u / state.geo.xke;
  double rvdot = rvdotl + nm * temp1 * (state.x1mth2 * cos2u + 1.5 * state.con41) / state.geo.xke;

  // Orientation vectors
  double sinsu = sin(su);
  double cossu = cos(su);
  double snod = sin(xnode);
  double cnod = cos(xnode);
  double sini = sin(xinc);
  double cosi = cos(xinc);
  double xmx = -snod * cosi;
  double xmy = cnod * cosi;
  double ux = xmx * sinsu + cnod * cossu;
  double uy = xmy * sinsu + snod * cossu;
  double uz = sini * sinsu;
  double vx = xmx * cossu - cnod * sinsu;
  double vy = xmy * cossu - snod * sinsu;
  double vz = sini * cossu;

  if (mrt < 1.0) {
    return SGP4::Sgp4Result {
      code: SGP4::ResultCode::SATELLITE_DECAYED
    };
  }

  // Position and velocity (in km and km/sec)
  return SGP4::Sgp4Result {
    code: SGP4::ResultCode::SUCCESS,
    x: (mrt * ux)* state.geo.radiusearthkm,
    y: (mrt * uy)* state.geo.radiusearthkm,
    z: (mrt * uz)* state.geo.radiusearthkm,
    vx: (mvt * ux + rvdot * vx) * vkmpersec,
    vy: (mvt * uy + rvdot * vy) * vkmpersec,
    vz: (mvt * uz + rvdot * vz) * vkmpersec,
  };
}

double SGP4::greenwichSiderealTime(double julianDateUt1) {
  const double deg2rad = M_PI / 180.0;
  double tut1 = (julianDateUt1 - 2451545.0) / 36525.0;
  double temp = -6.2e-6 * tut1 * tut1 * tut1 + 0.093104 * tut1 * tut1 +
    (876600.0 * 3600 + 8640184.812866) * tut1 + 67310.54841;  // sec
  temp = fmod(temp * deg2rad / 240.0, 2 * M_PI); // 360/86400 = 1/240, to deg, to rad
  if (temp < 0.0) {
    temp += 2 * M_PI;
  }
  return temp;
}

SGP4::Sgp4GeodeticConstants SGP4::getGravitationalConstants(SGP4::WgsVersion wgsVersion) {
  double mu;
  Sgp4GeodeticConstants result;
  switch (wgsVersion) {
    default:
    case SGP4::WgsVersion::WGS_72:
      mu = 398600.8;
      result.radiusearthkm = 6378.135;
      result.j2 = 0.001082616;
      result.j3 = -0.00000253881;
      result.j4 = -0.00000165597;
      break;
    case SGP4::WgsVersion::WGS_84:
      mu = 398600.5;
      result.radiusearthkm = 6378.137;
      result.j2 = 0.00108262998905;
      result.j3 = -0.00000253215306;
      result.j4 = -0.00000161098761;
      break;
  }
  result.j3oj2 = result.j3 / result.j2;
  result.xke = 60.0 / std::sqrt(result.radiusearthkm * result.radiusearthkm * result.radiusearthkm / mu);
  return result;
}

double SGP4::findTimeSinceEpochMinutes(
    const SGP4::Sgp4State &state, int64_t timeUtcMillis) {
  double timeJulianDaysSince0thJanuary1950 =
      millisToJulianDays(timeUtcMillis) + UNIX_EPOCH_JULIAN_DATE - JAN_0_1950_JULIAN_DATE;
  double julianDaysSinceEpoch = timeJulianDaysSince0thJanuary1950 - state.epoch;
  return julianDaysSinceEpoch * 24 * 60;
}
