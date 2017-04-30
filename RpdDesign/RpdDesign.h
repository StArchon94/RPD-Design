#pragma once

#include <jni.h>
#include <opencv2/core/mat.hpp>
#include <QTranslator>

#include "ui_RpdDesign.h"
#include "GlobalVariables.h"

class Rpd;
class RpdViewer;
class Tooth;

class RpdDesign : public QWidget {
	Q_OBJECT
public:
	explicit RpdDesign(QWidget* const& parent = nullptr);
	~RpdDesign();
private:
	void changeEvent(QEvent* event) override;
	void updateViewer();
	bool isEnglish_ = true;
	bool justLoadedRpds_ = false, showBaseImage_, showDesignImage_;
	JavaVM* vm_;
	JNIEnv* env_;
	Mat baseImage_, designImages_[2], remediedDesignImages_[2];
	QTranslator chsTranslator_, engTranslator_;
	RpdViewer* rpdViewer_;
	Ui::RpdDesignClass ui_;
	vector<Rpd*> rpds_;
	vector<Tooth> teeth_[nZones], remediedTeeth_[nZones];
private slots:
	void loadBaseImage();
	void loadRpdInfo();
	void onRemedyImageChanged(const bool& thisRemedyImage);
	void onShowBaseChanged(const bool& showBaseImage);
	void onShowDesignChanged(const bool& showContoursImage);
	void saveDesign();
	void switchLanguage(bool* const& isEnglish = nullptr);
};
