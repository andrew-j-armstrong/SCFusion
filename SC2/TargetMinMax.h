#pragma once

template <typename TVal>
class CTargetMinMax
{
public:
	TVal min;
	TVal max;

	CTargetMinMax() : min(0), max(0) {}
	CTargetMinMax(TVal _min, TVal _max) : min(_min), max(_max) {}
	~CTargetMinMax() {}

	inline const TVal &GetValue(bool getMax) const { return getMax ? max : min; }
	inline bool IsWithinRange(TVal val) const { return min <= val && val <= max; }
	inline void Combine(const CTargetMinMax &value) { min = mymax(min, value.min); max = mymax(max, value.max); }

	double GetRequirementValue(TVal stateVal, double valuePerUnit) const
	{
		if(stateVal <= min)
			return stateVal * valuePerUnit;
		else if(stateVal > max)
			return (min + max - (double)stateVal) * valuePerUnit;
		else
			return min * valuePerUnit;
	}
};

//template<>
//bool CTargetMinMax<bool>::IsWithinRange(bool value) const;

typedef CTargetMinMax<size_t> CSizeTMinMax;
typedef CTargetMinMax<double> CDoubleMinMax;
typedef CTargetMinMax<bool> CBoolMinMax;


template <typename TVal>
class CTargetOptionalMinMax
{
public:
	bool haveMin;
	bool haveMax;
	TVal min;
	TVal max;

	CTargetOptionalMinMax() : haveMin(false), haveMax(false), min(0), max(0) {}
	CTargetOptionalMinMax(TVal _min, TVal _max) : min(_min), max(_max) {}
	~CTargetOptionalMinMax() {}

	inline bool IsWithinRange(TVal val) const { return (!haveMin || min <= val) && (!haveMax || val <= max); }
	inline void Combine(const CTargetMinMax<TVal> &value) { haveMin ||= value.haveMin; min = mymax(min, value.min); haveMax ||= value.haveMax; max = mymax(max, value.max); }

	double GetRequirementValue(TVal stateVal, double valuePerUnit) const
	{
		if(stateVal <= min)
			return stateVal * valuePerUnit;
		else if(stateVal > max)
			return (min + max - (double)stateVal) * valuePerUnit;
		else
			return min * valuePerUnit;
	}
};

typedef CTargetOptionalMinMax<size_t> CSizeTOptionalMinMax;
typedef CTargetOptionalMinMax<double> CDoubleOptionalMinMax;
typedef CTargetOptionalMinMax<bool> CBoolOptionalMinMax;
