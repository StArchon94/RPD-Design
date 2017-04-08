#pragma once

#include <QImage>

#include "Rpd.h"

QImage matToQImage(const Mat& inputMat);

QPixmap matToQPixmap(const Mat& inputMat);

Size qSizeToSize(const QSize& size);

QSize sizeToQSize(const Size& size);

float degreeToRadian(const float& degree);

float radianToDegree(const float& radian);

template <typename T>
Point2f rotate(const Point_<T>& point, const float& angle) { return Point2f(point.x * cos(angle) - point.y * sin(angle), point.y * cos(angle) + point.x * sin(angle)); }

template <typename T>
Point2f normalize(const Point_<T>& point) { return static_cast<Point2f>(point) / norm(point); }

template <typename T>
Point roundToPoint(const Point_<T>& point) { return Point(round(point.x), round(point.y)); }

void catPath(string& path, const string& searchDirectory, const string& extension);

string getClsSig(const char* const& clsStr);

bool isDentureBase(const RpdAsLingualBlockage::LingualBlockage& lingualBlockage);

const Tooth& getTooth(const vector<Tooth> teeth[nZones], const Rpd::Position& position);

Tooth& getTooth(vector<Tooth> teeth[nZones], const Rpd::Position& position);

bool findLingualBlockage(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions);

void computeStringCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, vector<Point>& curve, float& sumOfRadii, int& nTeeth);

void computeStringCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, vector<Point>& curve, float& avgRadius);

void computeStringCurves(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, const vector<float>& distanceScales, const bool& keepStartEndPoints, const bool& considerDistalPoints, vector<vector<Point>>& curves, float* const& avgRadius = nullptr, vector<Point>* const& distalPoints = nullptr);

void computeStringCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, const float& distanceScale, const bool& keepStartEndPoints, const bool& considerDistalPoints, vector<Point>& curve, float* const& avgRadius = nullptr, vector<Point>* const& distalPoints = nullptr);

void computeInscribedCurve(const vector<Point>& cornerPoints, vector<Point>& curve, const float& smoothness = 0.5F, const bool& shouldAppend = true);

void computeSmoothCurve(const vector<Point>& curve, vector<Point>& smoothCurve, const bool& isClosed = false, const float& smoothness = 0.5F);

void computePiecewiseSmoothCurve(const vector<Point>& curve, vector<Point>& piecewiseSmoothCurve, const bool& shouldSmoothStart = true, const bool& shouldSmoothEnd = true);

void computeLingualCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, vector<Point>& curve, vector<vector<Point>>& curves, vector<Point>* const& distalPoints = nullptr);

void computeLingualCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, vector<Point>& curve, vector<vector<Point>>& curves, Point* const& distalPoint);

void computeMesialCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, vector<Point>& curve, vector<Point>* const& innerCurve = nullptr);

void computeDistalCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, const vector<Point>& distalPoints, vector<Point>& curve, vector<Point>* const& innerCurve = nullptr);

void computeInnerCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, const float& avgRadius, const bool hasLingualConfrontations[nZones][nTeethPerZone], vector<Point>& curve, const bool& isSelfCall = false);

void computeOuterCurve(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, vector<Point>& curve, float* const& avgRadius = nullptr);

void computeLingualConfrontationCurves(const vector<Tooth> teeth[nZones], const vector<Rpd::Position>& positions, const bool hasLingualConfrontations[nZones][nTeethPerZone], vector<vector<Point>>& curves);

Point2f computeNormalDirection(const Point2f& point, float* const& angle = nullptr);

vector<RpdAsLingualBlockage::LingualBlockage> tipDirectionsToLingualBlockages(const vector<Rpd::Direction>& tipDirections);
