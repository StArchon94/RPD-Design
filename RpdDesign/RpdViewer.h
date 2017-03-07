#pragma once

#include <jni.h>
#include <opencv2/core/mat.hpp>
#include <QLabel>

using namespace std;
using namespace cv;

class Rpd;
class Tooth;

class RpdViewer: public QLabel {
	Q_OBJECT

public:
	RpdViewer(QWidget* parent, bool showBaseImage, bool showContoursImage);
	~RpdViewer();

private:
	enum RpdClass {
		AKERS_CLASP = 1,
		COMBINED_CLASP,
		DENTURE_BASE,
		EDENTULOUS_SPACE,
		LINGUAL_REST,
		OCCLUSAL_REST,
		PALATAL_PLATE,
		RING_CLASP,
		RPA,
		RPI,
		WW_CLASP
	};

	static map<string, RpdClass> rpdMapping_;
	Mat baseImage_, curImage_, designImage_;
	QSize imageSize_;
	bool showBaseImage_, showDesignImage_;
	vector<vector<Tooth>> teeth_;
	vector<Rpd*> rpds_;
	JavaVM* vm_;
	JNIEnv* env_;
	void updateRpdDesign();
	void resizeEvent(QResizeEvent* event) override;
	void refreshDisplay();

private slots:
	void loadBaseImage();
	void loadRpdInfo();
	void onShowBaseChanged(bool showBaseImage);
	void onShowDesignChanged(bool showContoursImage);
};
