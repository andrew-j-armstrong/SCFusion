#pragma once

class CFitnessValue
{
public:
	inline CFitnessValue();
	inline CFitnessValue(const CFitnessValue &value);

	inline void StartRecalc() {}

	inline int Compare(const CFitnessValue &value) const
	{
		if(value < *this)
			return 1;
		else if(*this < value)
			return -1;
		else
			return 0;
	}

	inline void operator =(const CFitnessValue &value);
	inline bool operator <(const CFitnessValue &value) const;
	inline bool operator==(const CFitnessValue &value) const;
	operator double() const { return (m_requirementsValue + 0.01 * (m_optionalValue + 0.01 * m_economyValue)) / (m_requirementsTime > 0 ? (200.0 + m_requirementsTime) : 1); }

	bool m_succeeded;
	double m_requirementsValue;
	double m_requirementsTime;
	double m_optionalValue;
	double m_economyValue;
	double m_resourceValue;
};

CFitnessValue::CFitnessValue()
	: m_succeeded(false)
	, m_requirementsValue(0.0)
	, m_requirementsTime(0.0)
	, m_optionalValue(0.0)
	, m_economyValue(0.0)
	, m_resourceValue(0.0)
{
}

CFitnessValue::CFitnessValue(const CFitnessValue &value)
{
	m_succeeded = value.m_succeeded;
	m_requirementsValue = value.m_requirementsValue;
	m_requirementsTime = value.m_requirementsTime;
	m_optionalValue = value.m_optionalValue;
	m_economyValue = value.m_economyValue;
	m_resourceValue = value.m_resourceValue;
}

void CFitnessValue::operator =(const CFitnessValue &value)
{
	m_requirementsValue = value.m_requirementsValue;
	m_succeeded = value.m_succeeded;
	m_requirementsTime = value.m_requirementsTime;
	m_optionalValue = value.m_optionalValue;
	m_economyValue = value.m_economyValue;
	m_resourceValue = value.m_resourceValue;
}

#define VERY_SMALL_DOUBLE	0.000001

bool CFitnessValue::operator <(const CFitnessValue &value) const
{
	if(!m_succeeded && value.m_succeeded)
		return true;
	else if(m_succeeded != value.m_succeeded)
		return false;

	if(m_requirementsValue < value.m_requirementsValue - VERY_SMALL_DOUBLE)
		return true;
	else if(m_requirementsValue > value.m_requirementsValue + VERY_SMALL_DOUBLE)
		return false;

	if(m_requirementsTime > value.m_requirementsTime + VERY_SMALL_DOUBLE)
		return true;
	else if(m_requirementsTime < value.m_requirementsTime - VERY_SMALL_DOUBLE)
		return false;

	if(m_optionalValue < value.m_optionalValue - VERY_SMALL_DOUBLE)
		return true;
	else if(m_optionalValue > value.m_optionalValue + VERY_SMALL_DOUBLE)
		return false;

	if(m_economyValue < value.m_economyValue - VERY_SMALL_DOUBLE)
		return true;
	else if(m_economyValue > value.m_economyValue + VERY_SMALL_DOUBLE)
		return false;

	return m_resourceValue < value.m_resourceValue - VERY_SMALL_DOUBLE;
}

bool CFitnessValue::operator==(const CFitnessValue &value) const
{
	return m_succeeded == value.m_succeeded
		&& VERY_SMALL_DOUBLE > abs(m_requirementsValue - value.m_requirementsValue)
		&& VERY_SMALL_DOUBLE > abs(m_requirementsTime - value.m_requirementsTime)
		&& VERY_SMALL_DOUBLE > abs(m_optionalValue - value.m_optionalValue)
		&& VERY_SMALL_DOUBLE > abs(m_economyValue - value.m_economyValue)
		&& VERY_SMALL_DOUBLE > abs(m_resourceValue - value.m_resourceValue);
}
