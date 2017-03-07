#include <Windows.h>
#include <opencv2/imgproc.hpp>
#include <QMessageBox>

#include "Utilities.h"
#include "EllipticCurve.h"
#include "Tooth.h"

Mat qImageToMat(const QImage& inputImage) {
	switch (inputImage.format()) {
			// 8-bit, 4 channel
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied:
			return Mat(inputImage.height(), inputImage.width(), CV_8UC4, const_cast<uchar*>(inputImage.bits()), static_cast<size_t>(inputImage.bytesPerLine())).clone();
			// 8-bit, 3 channel
		case QImage::Format_RGB32:
		case QImage::Format_RGB888: {
			auto tmp = inputImage.rgbSwapped();
			if (tmp.format() == QImage::Format_RGB32)
				tmp = tmp.convertToFormat(QImage::Format_RGB888);
			return Mat(tmp.height(), tmp.width(), CV_8UC3, const_cast<uchar*>(tmp.bits()), static_cast<size_t>(tmp.bytesPerLine())).clone();
		}
			// 8-bit, 1 channel
		case QImage::Format_Indexed8:
			return Mat(inputImage.height(), inputImage.width(), CV_8UC1, const_cast<uchar*>(inputImage.bits()), static_cast<size_t>(inputImage.bytesPerLine())).clone();
		default:
			QMessageBox::information(nullptr, "", "qImageToMat() - QImage format not handled: " + QString(inputImage.format()));
			return Mat();
	}
}

QImage matToQImage(const Mat& inputMat) {
	switch (inputMat.type()) {
			// 8-bit, 4 channel
		case CV_8UC4:
			return QImage(inputMat.data, inputMat.cols, inputMat.rows, static_cast<int>(inputMat.step), QImage::Format_ARGB32);
			// 8-bit, 3 channel
		case CV_8UC3:
			return QImage(inputMat.data, inputMat.cols, inputMat.rows, static_cast<int>(inputMat.step), QImage::Format_RGB888).rgbSwapped();
			// 8-bit, 1 channel
		case CV_8UC1: {
			static auto isColorTableReady = false;
			static QVector<QRgb> colorTable(256);
			if (!isColorTableReady) {
				for (auto i = 0; i < 256; ++i)
					colorTable[i] = qRgb(i, i, i);
				isColorTableReady = true;
			}
			QImage image(inputMat.data, inputMat.cols, inputMat.rows, static_cast<int>(inputMat.step), QImage::Format_Indexed8);
			image.setColorTable(colorTable);
			return image;
		}
		default:
			QMessageBox::information(nullptr, "", "matToQImage() - Mat type not handled: " + QString(to_string(inputMat.type()).data()));
			return QImage();
	}
}

Mat qPixmapToMat(const QPixmap& inputPixmap) { return qImageToMat(inputPixmap.toImage()); }

QPixmap matToQPixmap(const Mat& inputMat) { return QPixmap::fromImage(matToQImage(inputMat)); }

Size qSizeToSize(const QSize& size) { return Size(size.width(), size.height()); }

QSize sizeToQSize(const Size& size) { return QSize(size.width, size.height); }

float degreeToRadian(float degree) { return degree / 180 * CV_PI; }

float radianToDegree(float radian) { return radian / CV_PI * 180; }

void catPath(string& path, const string& searchDirectory, const string& extension) {
	auto searchPattern = searchDirectory + extension;
	WIN32_FIND_DATA findData;
	auto hFind = FindFirstFile(searchPattern.c_str(), &findData);
	do {
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			path.append(searchDirectory + findData.cFileName + ';');
	}
	while (FindNextFile(hFind, &findData));
	FindClose(hFind);
}

string getClsSig(const char* clsStr) { return 'L' + string(clsStr) + ';'; }

void computeStringingCurve(const vector<vector<Tooth>>& teeth, const Rpd::Position& startPosition, const Rpd::Position& endPosition, vector<Point>& curve, float& avgRadius, bool* hasLingualBlockage) {
	Point lastPoint;
	float sumOfRadii = 0;
	auto nTeeth = 0;
	if (hasLingualBlockage)
		*hasLingualBlockage = false;
	if (startPosition.zone == endPosition.zone) {
		for (auto zone = startPosition.zone, ordinal = startPosition.ordinal; ordinal <= endPosition.ordinal; ++ordinal) {
			++nTeeth;
			auto tooth = teeth[zone][ordinal];
			sumOfRadii += tooth.getRadius();
			if (ordinal == startPosition.ordinal)
				curve.push_back(tooth.getAnglePoint(0));
			else
				curve.push_back((tooth.getAnglePoint(0) + lastPoint) / 2);
			curve.push_back(tooth.getCentroid());
			lastPoint = tooth.getAnglePoint(180);
			if (ordinal == endPosition.ordinal)
				curve.push_back(lastPoint);
			if (hasLingualBlockage)
				if (tooth.getLingualBlockage())
					*hasLingualBlockage = true;
		}
	}
	else {
		auto step = -1;
		for (auto zone = startPosition.zone, ordinal = startPosition.ordinal; zone == startPosition.zone || ordinal <= endPosition.ordinal; ordinal += step) {
			++nTeeth;
			auto tooth = teeth[zone][ordinal];
			sumOfRadii += tooth.getRadius();
			if (zone == startPosition.zone && ordinal == startPosition.ordinal)
				curve.push_back(tooth.getAnglePoint(180));
			else
				curve.push_back((tooth.getAnglePoint(step ? 90 * (1 - step) : 0) + lastPoint) / 2);
			curve.push_back(tooth.getCentroid());
			lastPoint = tooth.getAnglePoint(step ? 90 * (1 + step) : 180);
			if (zone == endPosition.zone && ordinal == endPosition.ordinal)
				curve.push_back(lastPoint);
			if (ordinal == 0) {
				if (step == -1)
					++zone;
				++step;
			}
			if (hasLingualBlockage)
				if (tooth.getLingualBlockage())
					*hasLingualBlockage = true;
		}
	}
	avgRadius = sumOfRadii / nTeeth;
}

Point2f computeNormalDirection(const Point2f& point, float* angle) {
	auto direction = point - teethEllipse.center;
	auto thisAngle = atan2(direction.y, direction.x) - degreeToRadian(teethEllipse.angle);
	if (thisAngle < -CV_PI)
		thisAngle += CV_2PI;
	if (angle)
		*angle = thisAngle;
	auto normalDirection = Point2f(pow(teethEllipse.size.height, 2) * cos(thisAngle), pow(teethEllipse.size.width, 2) * sin(thisAngle));
	return normalDirection / norm(normalDirection);
}

void computeInscribedCurve(const vector<Point>& cornerPoints, float maxRadius, vector<Point>& curve, bool shouldAppend) {
	Point2f v1 = cornerPoints[0] - cornerPoints[1], v2 = cornerPoints[2] - cornerPoints[1];
	auto l1 = norm(v1), l2 = norm(v2);
	auto d1 = v1 / l1, d2 = v2 / l2;
	auto sinTheta = d1.cross(d2);
	auto theta = asin(abs(sinTheta));
	if (d1.dot(d2) < 0)
		theta = CV_PI - theta;
	auto radius = min({maxRadius, static_cast<float>(min({l1, l2}) * tan(theta / 2) / 2)});
	vector<Point> thisCurve;
	auto isValidCurve = EllipticCurve(cornerPoints[1] + roundToInt(normalize(d1 + d2) * radius / sin(theta / 2)), roundToInt(Size(radius, radius)), radianToDegree(sinTheta > 0 ? atan2(d2.x, -d2.y) : atan2(d1.x, -d1.y)), 180 - radianToDegree(theta), sinTheta > 0).getCurve(thisCurve);
	if (!shouldAppend)
		curve.clear();
	if (isValidCurve)
		curve.insert(curve.end(), thisCurve.begin(), thisCurve.end());
	else
		curve.push_back(cornerPoints[1]);
}

void computeSmoothCurve(const vector<Point> curve, vector<Point>& smoothCurve, bool isClosed, float maxRadius) {
	smoothCurve.clear();
	for (auto point = curve.begin(); point < curve.end(); ++point) {
		if (point == curve.begin())
			if (isClosed)
				computeInscribedCurve({curve.back(),*point, *(point + 1)}, maxRadius, smoothCurve);
			else
				smoothCurve.insert(smoothCurve.end(), *point);
		else if (point == curve.end() - 1)
			if (isClosed)
				computeInscribedCurve({smoothCurve.back(), *point, curve[0]}, maxRadius, smoothCurve);
			else
				smoothCurve.insert(smoothCurve.end(), *point);
		else
			computeInscribedCurve({smoothCurve.back(),*point, *(point + 1)}, maxRadius, smoothCurve);
	}
}

void updateLingualBlockage(vector<vector<Tooth>>& teeth, const Rpd::Position& position, RpdWithLingualBlockage::LingualBlockage lingualBlockage) { teeth[position.zone][position.ordinal].setLingualBlockage(lingualBlockage); }

void updateLingualBlockage(vector<vector<Tooth>>& teeth, const vector<Rpd::Position>& positions, RpdWithLingualBlockage::LingualBlockage lingualBlockage, RpdWithLingualBlockage::Scope scope) {
	if (scope == RpdWithLingualBlockage::LINE)
		if (positions[0].zone == positions[1].zone)
			for (auto zone = positions[0].zone, ordinal = positions[0].ordinal; ordinal <= positions[1].ordinal; ++ordinal)
				teeth[zone][ordinal].setLingualBlockage(lingualBlockage);
		else {
			auto step = -1;
			for (auto zone = positions[0].zone, ordinal = positions[0].ordinal; zone == positions[0].zone || ordinal <= positions[1].ordinal; ordinal += step) {
				teeth[zone][ordinal].setLingualBlockage(lingualBlockage);
				if (ordinal == 0)
					if (step) {
						step = 0;
						++zone;
					}
					else
						step = 1;
			}
		}
	else {
		auto zone = positions[0].zone;
		for (auto ordinal = positions[0].ordinal; ordinal <= positions[1].ordinal; ++ordinal)
			teeth[zone][ordinal].setLingualBlockage(lingualBlockage);
		zone = positions[1].zone;
		for (auto ordinal = positions[1].ordinal; ordinal >= positions[0].ordinal; --ordinal)
			teeth[zone][ordinal].setLingualBlockage(lingualBlockage);
	}
}

const string jenaLibPath = "D:/Utilities/apache-jena-3.2.0/lib/";

const int lineThicknessOfLevel[]{1, 4, 7};

RotatedRect teethEllipse;
