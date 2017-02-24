#pragma once

#include <jni.h>

#include "Tooth.h"

class Rpd {
public:
	struct Position {
		explicit Position(int zone = 0, int ordinal = 0);
		int zone, ordinal;
	};

	virtual ~Rpd() = default;
	virtual void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const = 0;
};

class RpdWithSingleSlot: public virtual Rpd {
protected:
	explicit RpdWithSingleSlot(const Position& position);
	static void extractToothInfo(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual, Position& position);
	Position position_;
};

class RpdWithRangedSlots: public virtual Rpd {
protected:
	RpdWithRangedSlots(const Position& startPosition, const Position& endPosition);
	static void extractToothInfo(JNIEnv* env, jmethodID midGetInt, jmethodID midHasNext, jmethodID midListProperties, jmethodID midNext, jmethodID midStatementGetProperty, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual, Position& startPosition, Position& endPosition);
	Position startPosition_, endPosition_;
};

class RpdWithMaterial: public virtual Rpd {
public:
	enum Material {
		CAST,
		WROUGHT_WIRE,
		UNSPECIFIED
	};

protected:
	explicit RpdWithMaterial(const Material& material);
	RpdWithMaterial(const Material& buccalMaterial, const Material& lingualMaterial);
	static void extractMaterial(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jobject dpClaspMaterial, jobject individual, Material& claspMaterial);
	static void extractMaterial(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jobject dpClaspMaterial, jobject dpBuccalClaspMaterial, jobject dpLingualClaspMaterial, jobject individual, Material& claspMaterial, Material& buccalClaspMaterial, Material& lingualClaspMaterial);
	Material material_, buccalMaterial_, lingualMaterial_;
};

class RpdWithDirection: public virtual Rpd {
public:
	enum Direction {
		MESIAL,
		DISTAL
	};

protected:
	explicit RpdWithDirection(const Direction& direction);
	static void extractDirection(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jobject dpClaspTipDirection, jobject individual, Direction& claspTipDirection);
	Direction direction_;
};

class AkersClasp: public RpdWithSingleSlot, public RpdWithMaterial, public RpdWithDirection {
public:
	AkersClasp(const Position& position, const Material& material, const Direction& direction);
	AkersClasp(const Position& position, const Material& buccalMaterial, const Material& lingualMaterial, const Direction& direction);
	static AkersClasp* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpClaspTipDirection, jobject dpClaspMaterial, jobject dpBuccalClaspMaterial, jobject dpLingualClaspMaterial, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class Clasp: public RpdWithSingleSlot, public RpdWithMaterial, public RpdWithDirection {
public:
	Clasp(const Position& position, const Material& material, const Direction& direction);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class CombinedClasp: public RpdWithRangedSlots, public RpdWithMaterial {
public:
	CombinedClasp(const Position& startPosition, const Position& endPosition, const Material& material);
	static CombinedClasp* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midHasNext, jmethodID midListProperties, jmethodID midNext, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpClaspMaterial, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class DentureBase: public RpdWithRangedSlots {
public:
	DentureBase(const Position& startPosition, const Position& endPosition);
	static DentureBase* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midHasNext, jmethodID midListProperties, jmethodID midNext, jmethodID midStatementGetProperty, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class EdentulousSpace: public RpdWithRangedSlots {
public:
	EdentulousSpace(const Position& startPosition, const Position& endPosition);
	static EdentulousSpace* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midHasNext, jmethodID midListProperties, jmethodID midNext, jmethodID midStatementGetProperty, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class GuidingPlate: public RpdWithSingleSlot {
public:
	explicit GuidingPlate(const Position& position);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class HalfClasp: public RpdWithSingleSlot, public RpdWithMaterial, public RpdWithDirection {
public:
	enum Side {
		BUCCAL,
		LINGUAL
	};

	HalfClasp(const Position& position, const Material& material, const Direction& direction, const Side& side);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
private:
	Side side_;
};

class IBar: public RpdWithSingleSlot {
public:
	explicit IBar(const Position& position);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class LingualRest: public RpdWithSingleSlot, public RpdWithDirection {
public:
	LingualRest(const Position& position, const Direction& direction);
	static LingualRest* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpRestMesialOrDistal, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class OcclusalRest: public RpdWithSingleSlot, public RpdWithDirection {
public:
	OcclusalRest(const Position& position, const Direction& direction);
	static OcclusalRest* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpRestMesialOrDistal, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class PalatalPlate: public RpdWithRangedSlots {
public:
	PalatalPlate(const Position& startPosition, const Position& endPosition, const vector<Position>& lingualConfrontations);
	static PalatalPlate* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midHasNext, jmethodID midListProperties, jmethodID midNext, jmethodID midStatementGetProperty, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject opMajorConnectorKeyPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
private:
	vector<Position> lingualConfrontations_;
};

class Plating: public RpdWithSingleSlot {
public:
	explicit Plating(const Position& position);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class RingClasp: public RpdWithSingleSlot, public RpdWithMaterial {
public:
	RingClasp(const Position& position, const Material& material);
	static RingClasp* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpClaspMaterial, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class Rpa: public RpdWithSingleSlot, public RpdWithMaterial {
public:
	Rpa(const Position& position, const Material& material);
	static Rpa* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpClaspMaterial, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class Rpi: public RpdWithSingleSlot {
public:
	explicit Rpi(const Position& position);
	static Rpi* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};

class WwClasp: public RpdWithSingleSlot, public RpdWithDirection {
public:
	WwClasp(const Position& position, const Direction& direction);
	static WwClasp* createFromIndividual(JNIEnv* env, jmethodID midGetInt, jmethodID midResourceGetProperty, jmethodID midStatementGetProperty, jobject dpClaspTipDirection, jobject dpToothZone, jobject dpToothOrdinal, jobject opComponentPosition, jobject individual);
	void draw(Mat& designImage, const vector<vector<Tooth>>& teeth) const override;
};