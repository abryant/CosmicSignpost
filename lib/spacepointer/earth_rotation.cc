#include "earth_rotation.h"

#include <cmath>

#include "quaternion.h"
#include "time_utils.h"
#include "vector.h"


Vector EarthRotation::earthEquatorialToEarthFixed(Vector v, long timeUtcMillis) {
  double timeJulianCenturiesSinceJ2000 = daysSinceJ2000(timeUtcMillis) / 36525.0;
  std::pair<double, double> deltaPsiAndDeltaEpsilon =
      getDeltaPsiAndDeltaEpsilon(timeJulianCenturiesSinceJ2000);
  v = applyPrecession(v, timeJulianCenturiesSinceJ2000);
  v = applyNutation(v, deltaPsiAndDeltaEpsilon, timeJulianCenturiesSinceJ2000);
  v = applySiderealRotation(v, deltaPsiAndDeltaEpsilon, timeUtcMillis);
  return v;
}

Vector EarthRotation::applyPrecession(Vector v, double timeJulianCenturiesSinceJ2000) {
  double t = timeJulianCenturiesSinceJ2000;
  // Using https://gssc.esa.int/navipedia/index.php/ICRF_to_CEP
  // The following three angles are all in arcseconds:
  double z = 2306.2181 * t + 1.09468 * t * t + 0.018203 * t * t * t;
  double theta = 2004.3109 * t - 0.42665 * t * t - 0.041833 * t * t * t;
  double zeta = 2306.2181 * t + 0.30188 * t * t + 0.017998 * t * t * t;
  // Note that the rotation matrices on the ESA navipedia page all have their angles negated
  // compared to normal rotation matrices, and the parameters to them are also negated.
  // (because cos(x) = cos(-x) and sin(-x) = -sin(x), and each rotation matrix on the page should
  // have the two sin terms swapped)
  // So we really have R3(z) * R2(-theta) * R3(zeta)
  Quaternion zQuaternion = Quaternion::rotateZ(M_PI * (z / 3600) / 180);
  Quaternion thetaQuaternion = Quaternion::rotateY(M_PI * (-theta / 3600) / 180);
  Quaternion zetaQuaternion = Quaternion::rotateZ(M_PI * (zeta / 3600) / 180);
  Quaternion precession = zQuaternion * thetaQuaternion * zetaQuaternion;
  return precession.rotate(v);
}

std::pair<double, double> EarthRotation::getDeltaPsiAndDeltaEpsilon(
    double timeJulianCenturiesSinceJ2000) {
  double t = timeJulianCenturiesSinceJ2000;
  // Using https://gssc.esa.int/navipedia/index.php/ICRF_to_CEP
  // The following three angles are all in arcseconds:
  double r = 1296000; // 1 full rotation in arcseconds
  double alpha1 = 485866.733 + (1325*r + 715922.633)*t + 31.310*t*t + 0.064*t*t*t;
  double alpha2 = 1287099.804 + (99*r + 1292581.224)*t - 0.577*t*t - 0.012*t*t*t;
  double alpha3 = 335778.877 + (1342*r + 295263.137)*t - 13.257*t*t + 0.011*t*t*t;
  double alpha4 = 1072261.307 + (1236*r + 1105601.328)*t - 6.891*t*t + 0.019*t*t*t;
  double alpha5 = 450160.280 - (5*r + 482890.539)*t + 7.455*t*t + 0.008*t*t*t;
  double deltaPsi = 0;
  double deltaEpsilon = 0;
  for (int i = 0; i < 106; ++i) {
    double alphaTerm =
        NUTATION_ALPHA_COEFFICIENTS_MATRIX[i][0] * alpha1
        + NUTATION_ALPHA_COEFFICIENTS_MATRIX[i][1] * alpha2
        + NUTATION_ALPHA_COEFFICIENTS_MATRIX[i][2] * alpha3
        + NUTATION_ALPHA_COEFFICIENTS_MATRIX[i][3] * alpha4
        + NUTATION_ALPHA_COEFFICIENTS_MATRIX[i][4] * alpha5;
    double alphaTermRadians = M_PI * (alphaTerm / 3600) / 180;
    deltaPsi +=
        (NUTATION_AB_COEFFICIENTS_MATRIX[i][0] + t*NUTATION_AB_COEFFICIENTS_MATRIX[i][1]) * 1e-4
            * std::sin(alphaTermRadians);
    deltaEpsilon +=
        (NUTATION_AB_COEFFICIENTS_MATRIX[i][2] + t*NUTATION_AB_COEFFICIENTS_MATRIX[i][3]) * 1e-4
            * std::cos(alphaTermRadians);
  }
  // Results are in arcseconds.
  return std::make_pair(deltaPsi, deltaEpsilon);
}

Vector EarthRotation::applyNutation(
    Vector v,
    std::pair<double, double> deltaPsiAndDeltaEpsilon,
    double timeJulianCenturiesSinceJ2000) {
  double t = timeJulianCenturiesSinceJ2000;
  // Using https://gssc.esa.int/navipedia/index.php/ICRF_to_CEP
  // The following three angles are all in arcseconds:
  double epsilon = 23*3600 + 26*60 + 21.448 - 46.8150*t - 0.00059*t*t + 0.001813*t*t*t;
  double deltaPsi = deltaPsiAndDeltaEpsilon.first;
  double deltaEpsilon = deltaPsiAndDeltaEpsilon.second;
  // Note that the rotation matrices on the ESA navipedia page all have their angles negated
  // compared to normal rotation matrices (because cos(x) = cos(-x) and sin(-x) = -sin(x), and each
  // rotation matrix on the page should have the two sin terms swapped).
  // So we really have R1(epsilon + deltaEpsilon) * R3(deltaPsi) * R1(-epsilon)
  Quaternion epsilonPlusDeltaRotation =
      Quaternion::rotateX(M_PI * ((epsilon + deltaEpsilon) / 3600) / 180);
  Quaternion deltaPsiRotation = Quaternion::rotateZ(M_PI * (deltaPsi / 3600) / 180);
  Quaternion epsilonRotation = Quaternion::rotateX(M_PI * (-epsilon / 3600) / 180);
  Quaternion nutation = epsilonPlusDeltaRotation * deltaPsiRotation * epsilonRotation;
  return nutation.rotate(v);
}

Vector EarthRotation::applySiderealRotation(
    Vector v,
    std::pair<double, double> deltaPsiAndDeltaEpsilon,
    long timeUtcMillis) {
  // Account for the earth's rotation using Greenwich Mean Sidereal Time.
  // See https://gssc.esa.int/navipedia/index.php/CEP_to_ITRF
  // Greenwich Mean Sidereal Time is defined in terms of UT1 and not UTC. It's easy to find an
  // approximate value for UT1, because UTC is adjusted using leap seconds to be less than 0.9s
  // away from UT1, but it's much more difficult to find it exactly. For this use-case, we can live
  // with the approximation.
  double ut1JulianDays =
      millisToJulianDays(unixTimeToApproxUt1(timeUtcMillis - JANUARY_1_2000_MIDNIGHT_UTC_MILLIS));
  double tu = ut1JulianDays / 36525;
  double gmst0 =
          (6 + (41.0/60)
           + ((50.54841 + 8640184.812866*tu + 0.093104*tu*tu - 0.000006210*tu*tu*tu) / 3600))
       / 24;
  double gmst = 1.002737909350795 * ut1JulianDays + gmst0;

  // Find alphaE, using nutation parameters.
  double timeJulianCenturiesSinceJ2000 = daysSinceJ2000(timeUtcMillis) / 36525.0;
  double t = timeJulianCenturiesSinceJ2000;
  double epsilon = 23*3600 + 26*60 + 21.448 - 46.8150*t - 0.00059*t*t + 0.001813*t*t*t;
  double deltaPsi = deltaPsiAndDeltaEpsilon.first;
  double n12 = - std::cos(M_PI * (epsilon / 3600) / 180) * std::sin(M_PI * (deltaPsi / 3600) / 180);
  double n11 = std::cos(M_PI * (deltaPsi / 3600) / 180);
  double alphaE = std::atan(n12/n11);

  double gast = gmst + alphaE;
  // Note that the rotation matrices on the ESA navipedia page all have their angles negated
  // compared to normal rotation matrices (because cos(x) = cos(-x) and sin(-x) = -sin(x), and each
  // rotation matrix on the page should have the two sin terms swapped).
  // So we really have Rs = R3(-gast)
  Quaternion rotation = Quaternion::rotateZ(-gast * 2 * M_PI);
  return rotation.rotate(v);
}

// From https://gssc.esa.int/navipedia/index.php/ICRF_to_CEP
const int8_t EarthRotation::NUTATION_ALPHA_COEFFICIENTS_MATRIX[106][5] = {
  {  0,  0,  0,  0,  1},
  {  0,  0,  2, -2,  2},
  {  0,  0,  2,  0,  2},
  {  0,  0,  0,  0,  2},
  {  0, -1,  0,  0,  0},
  {  1,  0,  0,  0,  0},
  {  0,  1,  2, -2,  2},
  {  0,  0,  2,  0,  1},
  {  1,  0,  2,  0,  2},
  {  0, -1,  2, -2,  2},
  { -1,  0,  0,  2,  0},
  {  0,  0,  2, -2,  1},
  { -1,  0,  2,  0,  2},
  {  1,  0,  0,  0,  1},
  {  0,  0,  0,  2,  0},
  { -1,  0,  2,  2,  2},
  { -1,  0,  0,  0,  1},
  {  1,  0,  2,  0,  1},
  { -2,  0,  0,  2,  0},
  { -2,  0,  2,  0,  1},
  {  0,  0,  2,  2,  2},
  {  2,  0,  2,  0,  2},
  {  2,  0,  0,  0,  0},
  {  1,  0,  2, -2,  2},
  {  0,  0,  2,  0,  0},
  {  0,  0,  2, -2,  0},
  { -1,  0,  2,  0,  1},
  {  0,  2,  0,  0,  0},
  {  0,  2,  2, -2,  2},
  { -1,  0,  0,  2,  1},
  {  0,  1,  0,  0,  1},
  {  1,  0,  0, -2,  1},
  {  0, -1,  0,  0,  1},
  {  2,  0, -2,  0,  0},
  { -1,  0,  2,  2,  1},
  {  1,  0,  2,  2,  2},
  {  0, -1,  2,  0,  2},
  {  0,  0,  2,  2,  1},
  {  1,  1,  0, -2,  0},
  {  0,  1,  2,  0,  2},
  { -2,  0,  0,  2,  1},
  {  0,  0,  0,  2,  1},
  {  2,  0,  2, -2,  2},
  {  1,  0,  0,  2,  0},
  {  1,  0,  2, -2,  1},
  {  0,  0,  0, -2,  1},
  {  0, -1,  2, -2,  1},
  {  2,  0,  2,  0,  1},
  {  1, -1,  0,  0,  0},
  {  1,  0,  0, -1,  0},
  {  0,  0,  0,  1,  0},
  {  0,  1,  0, -2,  0},
  {  1,  0, -2,  0,  0},
  {  2,  0,  0, -2,  1},
  {  0,  1,  2, -2,  1},
  {  1,  1,  0,  0,  0},
  {  1, -1,  0, -1,  0},
  { -1, -1,  2,  2,  2},
  {  0, -1,  2,  2,  2},
  {  1, -1,  2,  0,  2},
  {  3,  0,  2,  0,  2},
  { -2,  0,  2,  0,  2},
  {  1,  0,  2,  0,  0},
  { -1,  0,  2,  4,  2},
  {  1,  0,  0,  0,  2},
  { -1,  0,  2, -2,  1},
  {  0, -2,  2, -2,  1},
  { -2,  0,  0,  0,  1},
  {  2,  0,  0,  0,  1},
  {  3,  0,  0,  0,  0},
  {  1,  1,  2,  0,  2},
  {  0,  0,  2,  1,  2},
  {  1,  0,  0,  2,  1},
  {  1,  0,  2,  2,  1},
  {  1,  1,  0, -2,  1},
  {  0,  1,  0,  2,  0},
  {  0,  1,  2, -2,  0},
  {  0,  1, -2,  2,  0},
  {  1,  0, -2,  2,  0},
  {  1,  0, -2, -2,  0},
  {  1,  0,  2, -2,  0},
  {  1,  0,  0, -4,  0},
  {  2,  0,  0, -4,  0},
  {  0,  0,  2,  4,  2},
  {  0,  0,  2, -1,  2},
  { -2,  0,  2,  4,  2},
  {  2,  0,  2,  2,  2},
  {  0, -1,  2,  0,  1},
  {  0,  0, -2,  0,  1},
  {  0,  0,  4, -2,  2},
  {  0,  1,  0,  0,  2},
  {  1,  1,  2, -2,  2},
  {  3,  0,  2, -2,  2},
  { -2,  0,  2,  2,  2},
  { -1,  0,  0,  0,  2},
  {  0,  0, -2,  2,  1},
  {  0,  1,  2,  0,  1},
  { -1,  0,  4,  0,  2},
  {  2,  1,  0, -2,  0},
  {  2,  0,  0,  2,  0},
  {  2,  0,  2, -2,  1},
  {  2,  0, -2,  0,  1},
  {  1, -1,  0, -2,  0},
  { -1,  0,  0,  1,  1},
  { -1, -1,  0,  2,  1},
  {  0,  1,  0,  1,  0},
};

// From https://gssc.esa.int/navipedia/index.php/ICRF_to_CEP
const double EarthRotation::NUTATION_AB_COEFFICIENTS_MATRIX[106][4] = {
  // All values below have been premultiplied by 10^4, and need dividing by 10^4 before use.
  // A[0][i]    A[1][i]    B[0][i]    B[1][i]
  //  arcsec  arcsec/ct     arcsec  arcsec/ct (ct=Julian century)
  {-171996.0,    -174.2,   92025.0,       8.9},
  { -13187.0,      -1.6,    5736.0,      -3.1},
  {  -2274.0,      -0.2,     977.0,      -0.5},
  {   2062.0,       0.2,    -895.0,       0.5},
  {  -1426.0,       3.4,      54.0,      -0.1},
  {    712.0,       0.1,      -7.0,       0.0},
  {   -517.0,       1.2,     224.0,      -0.6},
  {   -386.0,      -0.4,     200.0,       0.0},
  {   -301.0,       0.0,     129.0,      -0.1},
  {    217.0,      -0.5,     -95.0,       0.3},
  {    158.0,       0.0,      -1.0,       0.0},
  {    129.0,       0.1,     -70.0,       0.0},
  {    123.0,       0.0,     -53.0,       0.0},
  {     63.0,       0.1,     -33.0,       0.0},
  {     63.0,       0.0,      -2.0,       0.0},
  {    -59.0,       0.0,      26.0,       0.0},
  {    -58.0,      -0.1,      32.0,       0.0},
  {    -51.0,       0.0,      27.0,       0.0},
  {    -48.0,       0.0,       1.0,       0.0},
  {     46.0,       0.0,     -24.0,       0.0},
  {    -38.0,       0.0,      16.0,       0.0},
  {    -31.0,       0.0,      13.0,       0.0},
  {     29.0,       0.0,      -1.0,       0.0},
  {     29.0,       0.0,     -12.0,       0.0},
  {     26.0,       0.0,      -1.0,       0.0},
  {    -22.0,       0.0,       0.0,       0.0},
  {     21.0,       0.0,     -10.0,       0.0},
  {     17.0,      -0.1,       0.0,       0.0},
  {    -16.0,       0.1,       7.0,       0.0},
  {     16.0,       0.0,      -8.0,       0.0},
  {    -15.0,       0.0,       9.0,       0.0},
  {    -13.0,       0.0,       7.0,       0.0},
  {    -12.0,       0.0,       6.0,       0.0},
  {     11.0,       0.0,       0.0,       0.0},
  {    -10.0,       0.0,       5.0,       0.0},
  {     -8.0,       0.0,       3.0,       0.0},
  {     -7.0,       0.0,       3.0,       0.0},
  {     -7.0,       0.0,       3.0,       0.0},
  {     -7.0,       0.0,       0.0,       0.0},
  {      7.0,       0.0,      -3.0,       0.0},
  {     -6.0,       0.0,       3.0,       0.0},
  {     -6.0,       0.0,       3.0,       0.0},
  {      6.0,       0.0,      -3.0,       0.0},
  {      6.0,       0.0,       0.0,       0.0},
  {      6.0,       0.0,      -3.0,       0.0},
  {     -5.0,       0.0,       3.0,       0.0},
  {     -5.0,       0.0,       3.0,       0.0},
  {     -5.0,       0.0,       3.0,       0.0},
  {      5.0,       0.0,       0.0,       0.0},
  {     -4.0,       0.0,       0.0,       0.0},
  {     -4.0,       0.0,       0.0,       0.0},
  {     -4.0,       0.0,       0.0,       0.0},
  {      4.0,       0.0,       0.0,       0.0},
  {      4.0,       0.0,      -2.0,       0.0},
  {      4.0,       0.0,      -2.0,       0.0},
  {     -3.0,       0.0,       0.0,       0.0},
  {     -3.0,       0.0,       0.0,       0.0},
  {     -3.0,       0.0,       1.0,       0.0},
  {     -3.0,       0.0,       1.0,       0.0},
  {     -3.0,       0.0,       1.0,       0.0},
  {     -3.0,       0.0,       1.0,       0.0},
  {     -3.0,       0.0,       1.0,       0.0},
  {      3.0,       0.0,       0.0,       0.0},
  {     -2.0,       0.0,       1.0,       0.0},
  {     -2.0,       0.0,       1.0,       0.0},
  {     -2.0,       0.0,       1.0,       0.0},
  {     -2.0,       0.0,       1.0,       0.0},
  {     -2.0,       0.0,       1.0,       0.0},
  {      2.0,       0.0,      -1.0,       0.0},
  {      2.0,       0.0,       0.0,       0.0},
  {      2.0,       0.0,      -1.0,       0.0},
  {      2.0,       0.0,      -1.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       1.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       1.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {     -1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,      -1.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,      -1.0,       0.0},
  {      1.0,       0.0,      -1.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,      -1.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0},
  {      1.0,       0.0,       0.0,       0.0}
};