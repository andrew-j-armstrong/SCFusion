#include "stdafx.h"
#include "SC2Engine.h"

void CSC2Engine::BuildMinMaxString(wxString &str, const CDoubleMinMax *minmax)
{
	if(minmax->min == minmax->max)
		str = wxString::Format("%d", (int)minmax->min);
	else
		str = wxString::Format("%d to %d", (int)minmax->min, (int)minmax->max);
}

void CSC2Engine::BuildTimeMinMaxString(wxString &str, const CDoubleMinMax *minmax)
{
	if(minmax->min == minmax->max)
		str = wxString::Format("%s", TimeToString(minmax->min, 0));
	else
		str = wxString::Format("%s to %s", TimeToString(minmax->min, 0), TimeToString(minmax->max, 0));
}

void CSC2Engine::BuildMinMaxString(wxString &str, const CSizeTMinMax *minmax)
{
	if(minmax->min == minmax->max)
		str = wxString::Format("%u", minmax->min);
	else
		str = wxString::Format("%u to %u", minmax->min, minmax->max);
}

void CSC2Engine::BuildMinMaxString(wxString &str, const CDoubleOptionalMinMax *minmax)
{
	if(minmax->haveMin && minmax->haveMax)
	{
		if(minmax->min == minmax->max)
			str = wxString::Format("%g", minmax->min);
		else
			str = wxString::Format("%g to %g", minmax->min, minmax->max);
	}
	else if(minmax->haveMin)
	{
		str = wxString::Format(">= %g", minmax->min);
	}
	else if(minmax->haveMax)
	{
		str = wxString::Format("<= %g", minmax->max);
	}
	else
		str = wxT("");
}

void CSC2Engine::BuildMinMaxString(wxString &str, const CSizeTOptionalMinMax *minmax)
{
	if(minmax->haveMin && minmax->haveMax)
	{
		if(minmax->min == minmax->max)
			str = wxString::Format("%d", (int)minmax->min);
		else
			str = wxString::Format("%d to %d", (int)minmax->min, (int)minmax->max);
	}
	else if(minmax->haveMin)
	{
		str = wxString::Format(">= %d", (int)minmax->min);
	}
	else if(minmax->haveMax)
	{
		str = wxString::Format("<= %d", (int)minmax->max);
	}
	else
		str = wxT("");
}

wxString CSC2Engine::BuildMinMaxString(const CSizeTMinMax *minmax)
{
	if(minmax->min == minmax->max)
		return wxString::Format("%u", minmax->min);
	else
		return wxString::Format("%u to %u", minmax->min, minmax->max);
}

bool CSC2Engine::InterpretTimeValue(wxString value, double &time)
{
	time = 0.0;

	value = value.Trim(false).Trim(true);
	int minutesSep = value.Find(":");
	if(-1 != minutesSep)
	{
		double minutes;
		value.SubString(0, minutesSep).Trim(false).Trim(true).ToDouble(&minutes);
		if(value.Length() > (size_t)(minutesSep + 1))
			value.SubString(minutesSep+1, value.Length()).Trim(false).Trim(true).ToDouble(&time);
		time += minutes * 60.0;
		return true;
	}
	else
		return value.Trim(false).Trim(true).ToDouble(&time);
}

bool CSC2Engine::InterpretMinMaxValue(CDoubleMinMax &minmax, const wxString &value)
{
	int left, right;
	left = value.Find("to");
	if(-1 == left)
	{
		left = value.Find("-");
		if(-1 == left)
		{
			if(value.ToDouble(&minmax.min))
			{
				if(minmax.max < minmax.min)
					minmax.max = minmax.min;
				return true;
			}
			return false;
		}

		right = left + 1;
	}
	else
	{
		right = left + 2;
	}

	if(!value.SubString(0, left-1).Trim(false).Trim(true).ToDouble(&minmax.min))
		return false;
	if(!value.SubString(right, value.Length()-1).Trim(false).Trim(true).ToDouble(&minmax.max))
		return false;
	return true;
}

bool CSC2Engine::InterpretTimeMinMaxValue(CDoubleMinMax &minmax, const wxString &value)
{
	int left, right;
	left = value.Find("to");
	if(-1 == left)
	{
		left = value.Find("-");
		if(-1 == left)
		{
			if(InterpretTimeValue(value, minmax.min))
			{
				if(minmax.max < minmax.min)
					minmax.max = minmax.min;
				return true;
			}
			return false;
		}

		right = left + 1;
	}
	else
	{
		right = left + 2;
	}

	if(!InterpretTimeValue(value.SubString(0, left - 1), minmax.min))
		return false;
	if(!InterpretTimeValue(value.SubString(right, value.Length() - 1), minmax.max))
		return false;
	return true;
}

bool CSC2Engine::InterpretMinMaxValue(CSizeTMinMax &minmax, const wxString &value)
{
	int left, right;
	left = value.Find("to");
	if(-1 == left)
	{
		left = value.Find("-");
		if(-1 == left)
		{
			if(value.ToULong((unsigned long *)&minmax.min))
			{
				if(minmax.max < minmax.min)
					minmax.max = minmax.min;
				return true;
			}
			return false;
		}

		right = left + 1;
	}
	else
	{
		right = left + 2;
	}

	if(!value.SubString(0, left-1).Trim(false).Trim(true).ToULong((unsigned long *)&minmax.min))
		return false;
	if(!value.SubString(right, value.Length()-1).Trim(false).Trim(true).ToULong((unsigned long *)&minmax.max))
		return false;
	return true;
}

bool CSC2Engine::InterpretMinMaxValue(CDoubleOptionalMinMax &minmax, const wxString &value)
{
	if(value.IsEmpty())
	{
		minmax.haveMin = false;
		minmax.haveMax = false;
		return true;
	}

	int left, right;
	left = value.Find("to");
	if(-1 == left)
	{
		left = value.Find("-");
		if(-1 == left)
		{
			left = value.Find("<=");
			if(value.SubString(left + 2, value.Length()).Trim(false).Trim(true).ToDouble(&minmax.max))
			{
				minmax.haveMin = false;
				minmax.min = 0;
				minmax.haveMax = true;
				return true;
			}
			else
			{
				left = value.Find(">=");
				if(value.SubString(left + 2, value.Length()).Trim(false).Trim(true).ToDouble(&minmax.min))
				{
					minmax.haveMin = true;
					minmax.haveMax = false;
					minmax.max = 0;
					return true;
				}
				else if(value.ToDouble(&minmax.min))
				{
					minmax.haveMin = true;
					if(minmax.max < minmax.min)
						minmax.max = minmax.min;
					return true;
				}
			}
				
			return false;
		}

		right = left + 1;
	}
	else
	{
		right = left + 2;
	}

	if(!value.SubString(0, left-1).Trim(false).Trim(true).ToDouble(&minmax.min))
		return false;
	if(!value.SubString(right, value.Length()-1).Trim(false).Trim(true).ToDouble(&minmax.max))
		return false;

	minmax.haveMin = true;
	minmax.haveMax = true;

	return true;
}

bool CSC2Engine::InterpretMinMaxValue(CSizeTOptionalMinMax &minmax, const wxString &value)
{
	if(value.IsEmpty())
	{
		minmax.haveMin = false;
		minmax.haveMax = false;
		return true;
	}

	int left, right;
	left = value.Find("to");
	if(-1 == left)
	{
		left = value.Find("-");
		if(-1 == left)
		{
			left = value.Find("<=");
			if(-1 != left && value.SubString(left + 2, value.Length()).Trim(false).Trim(true).ToULong((unsigned long *)&minmax.max))
			{
				minmax.haveMin = false;
				minmax.min = 0;
				minmax.haveMax = true;
				return true;
			}
			else
			{
				left = value.Find(">=");
				if(-1 != left && value.SubString(left + 2, value.Length()).Trim(false).Trim(true).ToULong((unsigned long *)&minmax.min))
				{
					minmax.haveMin = true;
					minmax.haveMax = false;
					minmax.max = 0;
					return true;
				}
				else if(value.ToULong((unsigned long *)&minmax.min))
				{
					minmax.haveMin = true;
					if(minmax.max < minmax.min)
						minmax.max = minmax.min;
					return true;
				}
			}
			return false;
		}

		right = left + 1;
	}
	else
	{
		right = left + 2;
	}

	if(!value.SubString(0, left-1).Trim(false).Trim(true).ToULong((unsigned long *)&minmax.min))
		return false;
	if(!value.SubString(right, value.Length()-1).Trim(false).Trim(true).ToULong((unsigned long *)&minmax.max))
		return false;

	minmax.haveMin = true;
	minmax.haveMax = true;

	return true;
}

bool CSC2Engine::InterpretMinMaxValue(CBoolMinMax &minmax, int value)
{
	minmax.min = (value & 0x2) > 0;
	minmax.max = (value & 0x1) > 0;
	return true;
}

wxString CSC2Engine::TimeToString(double time, int nDecimalPlaces /* = 2 */)
{
	return wxString::Format(wxString::Format("%d:%%0%d.%df", (int)(time/60), nDecimalPlaces == 0 ? 2 : (nDecimalPlaces + 3), nDecimalPlaces), time - 60*(int)(time/60));
}

void CSC2Engine::AddTimeResult(wxPropertyGrid *pgResults, wxString name, double time, bool alwaysAdd /* = false */) const
{
	wxPGProperty *prop = pgResults->GetProperty(name);

	if(alwaysAdd || time > 0)
	{
		if(prop == 0)
		{
			prop = new wxStringProperty(name, wxPG_LABEL);
			pgResults->Append(prop);
			pgResults->SetPropertyReadOnly(prop);
		}
		prop->SetValue(TimeToString(time));
	}
	else
	{
		if(prop != 0)
			pgResults->DeleteProperty(prop);
	}
}

void CSC2Engine::AddIntResult(wxPropertyGrid *pgResults, wxString name, double value, bool alwaysAdd /* = false */) const
{
	wxPGProperty *prop = pgResults->GetProperty(name);

	if(alwaysAdd || value > 0)
	{
		if(prop == 0)
		{
			prop = new wxIntProperty(name, wxPG_LABEL);
			pgResults->Append(prop);
			pgResults->SetPropertyReadOnly(prop);
		}
		prop->SetValueFromInt((int)(value + 0.499999999));
	}
	else
	{
		if(prop != 0)
			pgResults->DeleteProperty(prop);
	}
}

void CSC2Engine::AddBoolResult(wxPropertyGrid *pgResults, wxString name, bool value, bool alwaysAdd /* = false */) const
{
	wxPGProperty *prop = pgResults->GetProperty(name);

	if(alwaysAdd || value)
	{
		if(prop == 0)
		{
			prop = new wxBoolProperty(name, wxPG_LABEL);
			pgResults->Append(prop);
			pgResults->SetPropertyReadOnly(prop);
		}
		prop->SetValueFromInt(value);
	}
	else
	{
		if(prop != 0)
			pgResults->DeleteProperty(prop);
	}
}

void CSC2Engine::GetInitialBuildOrders(wxArrayString &arrInitialBuildOrders) const
{
	for(wxStringHashMap::const_iterator iter = m_initialBuildOrders.begin(); iter != m_initialBuildOrders.end(); ++iter)
		arrInitialBuildOrders.Add(iter->first);
}

bool CSC2Engine::GetInitialBuildOrder(const wxString &buildOrderName, wxString &buildOrder) const
{
	wxStringHashMap::const_iterator iter = m_initialBuildOrders.find(buildOrderName);
	if(iter == m_initialBuildOrders.end())
		return false;

	buildOrder = iter->second;
	return true;
}

CSC2Engine::CSC2Engine(const CSC2Version *version, ESC2Race race, const CSC2RaceInfo *raceInfo)
	: m_gasMicro(eGasMicroOneBackAndForth)
	, m_allowWaitCommands(false)
	, m_minimumCommandDuration(0.0)
	, m_version(version)
	, m_race(race)
	, m_raceInfo(raceInfo)
	, m_fitnessCalc(0)
	, m_engine(0)
	, m_mutator(m_alphabet, 0.3)
	, m_maxTime(1200.0)
	, m_output(NULL)
	, m_commandWaitPoint1Second(raceInfo->GetData(), 0.1)
	, m_commandWait1Second(raceInfo->GetData(), 1.0)
	, m_commandWait5Seconds(raceInfo->GetData(), 5.0)
{
}

int CSC2Engine::GetPropertyIndex(const wxString &name)
{
	return 1;
}

int CSC2Engine::PropertySortFunction(wxPropertyGrid* propGrid, wxPGProperty* p1, wxPGProperty* p2)
{
	int index1 = GetPropertyIndex(p1->GetName()), index2 = GetPropertyIndex(p2->GetName());

	return index1 - index2;
}

int CSC2Engine::AddWaypoint(wxPropertyGrid *pgWaypoint, int waypointIndex, CPropertiesSet &setDoubleProperties, CPropertiesSet &setTimeProperties, CPropertiesSet &setSizeTMinMaxProperties, CPropertiesSet &setBoolMinMaxProperties, CPropertiesSet &setBoolProperties)
{
	AddProperties(pgWaypoint, waypointIndex * 300.0, setDoubleProperties, setTimeProperties, setSizeTMinMaxProperties, setBoolMinMaxProperties, setBoolProperties, true);
	return waypointIndex;
}

void CSC2Engine::AddProperties(wxPropertyGrid *pgTarget, double timeLimit, CPropertiesSet &setDoubleProperties, CPropertiesSet &setTimeProperties, CPropertiesSet &setSizeTMinMaxProperties, CPropertiesSet &setBoolMinMaxProperties, CPropertiesSet &setBoolProperties, bool isWaypoint)
{
	wxStringProperty *prop;

	pgTarget->Append(new wxPropertyCategory("Status"));
	if(!isWaypoint)
		prop = new wxStringProperty("Min Time", wxPG_LABEL, TimeToString(timeLimit, 0));
	else
		prop = new wxStringProperty("Max Time", wxPG_LABEL, TimeToString(timeLimit, 0));
	pgTarget->Append(prop);
	setTimeProperties.insert(prop);

	prop = new wxStringProperty("Supply", wxPG_LABEL, "");
	pgTarget->Append(prop);
	pgTarget->AppendIn(prop, new wxStringProperty("Minimum", wxPG_LABEL));
	pgTarget->AppendIn(prop, new wxStringProperty("Maximum", wxPG_LABEL));
	prop->SetExpanded(false);
	setSizeTMinMaxProperties.insert(prop);

	prop = new wxStringProperty("Bases", wxPG_LABEL, "");
	pgTarget->Append(prop);
	pgTarget->AppendIn(prop, new wxStringProperty("Minimum", wxPG_LABEL));
	pgTarget->AppendIn(prop, new wxStringProperty("Maximum", wxPG_LABEL));
	pgTarget->AppendIn(prop, new wxStringProperty("Minimum Under Construction", wxPG_LABEL));
	pgTarget->AppendIn(prop, new wxStringProperty("Maximum Under Construction", wxPG_LABEL));
	prop->SetExpanded(false);
	setSizeTMinMaxProperties.insert(prop);

	wxPropertyCategory *catResources = new wxPropertyCategory("Resources");
	pgTarget->Append(catResources);
	prop = new wxStringProperty("Minerals", wxPG_LABEL);
	pgTarget->Append(prop);
	setDoubleProperties.insert(prop);
	prop = new wxStringProperty("Gas", wxPG_LABEL);
	pgTarget->Append(prop);
	setDoubleProperties.insert(prop);
	prop = new wxStringProperty("Minerals Mined", wxPG_LABEL);
	pgTarget->Append(prop);
	setDoubleProperties.insert(prop);
	prop = new wxStringProperty("Gas Mined", wxPG_LABEL);
	pgTarget->Append(prop);
	setDoubleProperties.insert(prop);
	catResources->SetExpanded(false);

	wxHashSetString categories;
	for(size_t i=0; i < m_raceInfo->GetTargets().size(); i++)
	{
		const CSC2Target *target = m_raceInfo->GetTargets()[i];
		if(categories.find(target->GetCategory()) == categories.end())
		{
			categories.insert(target->GetCategory());
			pgTarget->Append(new wxPropertyCategory(target->GetCategory()));
		}
		else
			pgTarget->SetCurrentCategory(target->GetCategory());

		switch(target->GetRestrictionType())
		{
		case CSC2Target::eBooleanMinMaxAndUnderConstruction:
			{
				wxPGChoices choices;
				choices.Add("Ignored", 0);
				choices.Add("Optional", 1);
				choices.Add("Required", 2);
				choices.Add("Under Construction", 3);

				wxEnumProperty *prop = new wxEnumProperty(target->GetName(), wxPG_LABEL, choices);
				prop->SetChoiceSelection(1);

				pgTarget->Append(prop);
				setBoolMinMaxProperties.insert(prop);
			}
			break;
		case CSC2Target::eSizeTMinMax:
			{
				prop = new wxStringProperty(target->GetName(), wxPG_LABEL);
				pgTarget->Append(prop);
				pgTarget->AppendIn(prop, new wxStringProperty("Minimum", wxPG_LABEL));
				pgTarget->AppendIn(prop, new wxStringProperty("Maximum", wxPG_LABEL));
				prop->SetExpanded(false);
				setSizeTMinMaxProperties.insert(prop);
			}
			break;
		case CSC2Target::eSizeTMinMaxAndUnderConstruction:
			{
				prop = new wxStringProperty(target->GetName(), wxPG_LABEL);
				pgTarget->Append(prop);
				pgTarget->AppendIn(prop, new wxStringProperty("Minimum", wxPG_LABEL));
				pgTarget->AppendIn(prop, new wxStringProperty("Maximum", wxPG_LABEL));
				pgTarget->AppendIn(prop, new wxStringProperty("Minimum Under Construction", wxPG_LABEL));
				pgTarget->AppendIn(prop, new wxStringProperty("Maximum Under Construction", wxPG_LABEL));
				prop->SetExpanded(false);
				setSizeTMinMaxProperties.insert(prop);
			}
			break;
		case CSC2Target::eBoolean:
			{
				wxPGChoices choices;
				choices.Add("Optional", 0);
				choices.Add("Required", 1);

				wxEnumProperty *prop = new wxEnumProperty(target->GetName(), wxPG_LABEL, choices);
				prop->SetChoiceSelection(0);

				pgTarget->Append(prop);
				setBoolProperties.insert(prop);
			}
			break;
		}
	}
}

void CSC2Engine::UpdateBestGame()
{
	m_engine->GetBestGame(m_bestGame);
}

void CSC2Engine::PrintBestGame(wxString &output, wxPropertyGrid *pgResults) const
{
	if(!m_fitnessCalc || !m_output)
		return;

	m_output->Reset();

	CSC2State *state = m_fitnessCalc->PrintGame(*m_output, m_bestGame);
	m_output->GetOutput(output);
	
	AddResults(*state, pgResults);
	delete state;
}

void CSC2Engine::AddResults(const CSC2State &state, wxPropertyGrid *pgResults) const
{
	pgResults->SetSortFunction(PropertySortFunction);

	wxHashSetString results;

	wxHashSetString categories;
	categories.insert(wxT("Status"));
	pgResults->Append(new wxPropertyCategory("Status"));
	AddTimeResult(pgResults, "Time", state.m_time, true);
	AddIntResult(pgResults, "Supply", state.m_supply, true);
	AddIntResult(pgResults, "Bases", state.m_baseCount, true);
	results.insert(wxT("Time"));
	results.insert(wxT("Supply"));
	results.insert(wxT("Bases"));

	categories.insert(wxT("Resources"));
	pgResults->Append(new wxPropertyCategory("Resources"));
	AddIntResult(pgResults, "Minerals", state.m_minerals, true);
	AddIntResult(pgResults, "Gas", state.m_gas, true);
	AddIntResult(pgResults, "Minerals Mined", state.m_mineralsMined, true);
	AddIntResult(pgResults, "Gas Mined", state.m_gasMined, true);
	results.insert(wxT("Minerals"));
	results.insert(wxT("Gas"));
	results.insert(wxT("Minerals Mined"));
	results.insert(wxT("Gas Mined"));

	for(size_t i=0; i < m_raceInfo->GetTargets().size(); i++)
	{
		const CSC2Target *target = m_raceInfo->GetTargets()[i];
		if(target->GetStateCount(state) <= 0 || !target->DisplayInResults())
			continue;

		if(categories.find(target->GetCategory()) == categories.end())
		{
			categories.insert(target->GetCategory());
			pgResults->Append(new wxPropertyCategory(target->GetCategory()));
		}
		else
			pgResults->SetCurrentCategory(target->GetCategory());

		wxPGProperty *prop = pgResults->GetProperty(target->GetName());
		if(prop == 0)
		{
			prop = new wxStringProperty(target->GetName(), wxPG_LABEL);
			pgResults->Append(prop);
			pgResults->SetPropertyReadOnly(prop);
		}
		prop->SetValueFromInt(target->GetStateCount(state));

		results.insert(target->GetName());
	}

	wxHashSetString deletedCategories;
	CVector<wxPGProperty *> deleteProperties;
	for(wxPropertyGridIterator iter = pgResults->GetIterator(wxPG_ITERATE_CATEGORIES | wxPG_ITERATE_NORMAL); !iter.AtEnd(); ++iter)
	{
		wxPGProperty *prop = *iter;

		if(prop->IsCategory())
		{
			if(categories.end() == categories.find(prop->GetName()))
			{
				deletedCategories.insert(prop->GetName());
				deleteProperties.push_back(prop);
			}
		}
		else
		{
			if(deletedCategories.end() != deletedCategories.find(prop->GetParent()->GetName()))
				continue;

			if(results.end() == results.find(prop->GetName()))
				deleteProperties.push_back(prop);
		}
	}

	for(size_t i=0; i < deleteProperties.size(); i++)
		pgResults->DeleteProperty(deleteProperties[i]);
}

bool CSC2Engine::BuildWaypoint(const wxPropertyGrid *propertyGrid, CSC2Waypoint &waypoint)
{
	bool hasTarget = false;

	wxPGProperty *propRoot = propertyGrid->GetRoot();
	for(size_t i=0; i < propRoot->GetChildCount(); i++)
	{
		wxPGProperty *propCategory = propRoot->Item(i);
		for(size_t j=0; j < propCategory->GetChildCount(); j++)
		{
			wxPGProperty *propTarget = propCategory->Item(j);

			if(propTarget->GetName() == wxT("Min Time"))
			{
				InterpretTimeValue(propTarget->GetValue().GetString(), waypoint.m_targetTime.min);
			}
			else if(propTarget->GetName() == wxT("Max Time"))
			{
				InterpretTimeValue(propTarget->GetValue().GetString(), waypoint.m_targetTime.max);
			}
			else if(propTarget->GetName() == wxT("Supply"))
			{
				hasTarget = true;

				wxPGProperty *propMin = propTarget->GetPropertyByName(wxT("Minimum"));
				wxPGProperty *propMax = propTarget->GetPropertyByName(wxT("Maximum"));

				if(!propMin->GetValue().GetString().IsEmpty())
				{
					waypoint.m_supplyCount.haveMin = true;
					waypoint.m_supplyCount.min = (size_t)propMin->GetValue().GetLong();
				}
				if(!propMax->GetValue().GetString().IsEmpty())
				{
					waypoint.m_supplyCount.haveMax = true;
					waypoint.m_supplyCount.max = (size_t)propMax->GetValue().GetLong();
				}
			}
			else if(propTarget->GetName() == wxT("Bases"))
			{
				hasTarget = true;

				wxPGProperty *propMin = propTarget->GetPropertyByName(wxT("Minimum"));
				wxPGProperty *propMax = propTarget->GetPropertyByName(wxT("Maximum"));
				wxPGProperty *propMinUC = propTarget->GetPropertyByName(wxT("Minimum Under Construction"));
				wxPGProperty *propMaxUC = propTarget->GetPropertyByName(wxT("Maximum Under Construction"));

				if(!propMin->GetValue().GetString().IsEmpty())
				{
					waypoint.m_baseCount.haveMin = true;
					waypoint.m_baseCount.min = (size_t)propMin->GetValue().GetLong();
				}
				if(!propMax->GetValue().GetString().IsEmpty())
				{
					waypoint.m_baseCount.haveMax = true;
					waypoint.m_baseCount.max = (size_t)propMax->GetValue().GetLong();
				}
				if(!propMinUC->GetValue().GetString().IsEmpty())
				{
					waypoint.m_baseUnderConstructionCount.haveMin = true;
					waypoint.m_baseUnderConstructionCount.min = (size_t)propMinUC->GetValue().GetLong();
				}
				if(!propMaxUC->GetValue().GetString().IsEmpty())
				{
					waypoint.m_baseUnderConstructionCount.haveMax = true;
					waypoint.m_baseUnderConstructionCount.max = (size_t)propMaxUC->GetValue().GetLong();
				}
			}
			else if(propTarget->GetName() == wxT("Minerals"))
			{
				hasTarget = true;
				waypoint.m_minerals = propTarget->GetValue().GetDouble();
			}
			else if(propTarget->GetName() == wxT("Gas"))
			{
				hasTarget = true;
				waypoint.m_gas = propTarget->GetValue().GetDouble();
			}
			else if(propTarget->GetName() == wxT("Minerals Mined"))
			{
				hasTarget = true;
				waypoint.m_mineralsMined = propTarget->GetValue().GetDouble();
			}
			else if(propTarget->GetName() == wxT("Gas Mined"))
			{
				hasTarget = true;
				waypoint.m_gasMined = propTarget->GetValue().GetDouble();
			}
			else
			{
				hasTarget = true;

				for(size_t k=0; k < m_raceInfo->GetTargets().size(); k++)
				{
					if(m_raceInfo->GetTargets()[k]->GetName() == propTarget->GetName())
					{
						switch(m_raceInfo->GetTargets()[k]->GetRestrictionType())
						{
						case CSC2Target::eBooleanMinMaxAndUnderConstruction:
							{
								int val = propTarget->GetValue().GetInteger();
								if(val == 0)
									waypoint.m_maxRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], 0));
								else if(val == 2)
									waypoint.m_minRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], 1));
								else if(val == 3)
									waypoint.m_minUnderConstructionRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], 1));
							}
							break;
						case CSC2Target::eSizeTMinMax:
							{
								wxPGProperty *propMin = propTarget->GetPropertyByName(wxT("Minimum"));
								wxPGProperty *propMax = propTarget->GetPropertyByName(wxT("Maximum"));
								if(!propMin->GetValue().GetString().IsEmpty())
									waypoint.m_minRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], (size_t)propMin->GetValue().GetLong()));
								if(!propMax->GetValue().GetString().IsEmpty())
									waypoint.m_maxRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], (size_t)propMax->GetValue().GetLong()));
							}
							break;
						case CSC2Target::eSizeTMinMaxAndUnderConstruction:
							{
								wxPGProperty *propMin = propTarget->GetPropertyByName(wxT("Minimum"));
								wxPGProperty *propMax = propTarget->GetPropertyByName(wxT("Maximum"));
								wxPGProperty *propMinUC = propTarget->GetPropertyByName(wxT("Minimum Under Construction"));
								wxPGProperty *propMaxUC = propTarget->GetPropertyByName(wxT("Maximum Under Construction"));
								if(!propMin->GetValue().GetString().IsEmpty())
									waypoint.m_minRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], (size_t)propMin->GetValue().GetLong()));
								if(!propMax->GetValue().GetString().IsEmpty())
									waypoint.m_maxRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], (size_t)propMax->GetValue().GetLong()));
								if(!propMinUC->GetValue().GetString().IsEmpty())
									waypoint.m_minUnderConstructionRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], (size_t)propMinUC->GetValue().GetLong()));
								if(!propMaxUC->GetValue().GetString().IsEmpty())
									waypoint.m_maxUnderConstructionRequirements.push_back(CSC2Waypoint::STargetRequirement(m_raceInfo->GetTargets()[k], (size_t)propMaxUC->GetValue().GetLong()));
							}
							break;
						case CSC2Target::eBoolean:
							{
								int val = propTarget->GetValue().GetInteger();
								if(val == 1)
									waypoint.m_stateDurationRequirements.push_back(m_raceInfo->GetTargets()[k]);
							}
							break;
						}

						break;
					}
				}
			}
		}
	}

	return hasTarget;
}

bool CSC2Engine::InitialiseFitnessCalc(const CVector<wxPropertyGrid *> &pgWaypoints, const wxPropertyGrid *pgTarget, EScoutingWorker scout /* = eNoScout */, int scoutTime /* = 0 */, int scoutEndTime /* = 0 */)
{
	if(m_engine && m_engine->IsRunning())
		return false;

	delete m_engine;
	delete m_fitnessCalc;

	bool hasTarget = false;

	m_fitnessCalc = new CSC2FitnessCalc(m_race, *m_raceInfo, m_maxTime, m_gasMicro, m_minimumCommandDuration);

	for(size_t i=0; i < pgWaypoints.size(); i++)
	{
		CSC2Waypoint waypoint(*m_raceInfo);
		if(!BuildWaypoint(pgWaypoints[i], waypoint))
			continue;

		waypoint.m_targetTime.min = 0.0;
		m_fitnessCalc->AddWaypoint(waypoint);
		hasTarget = true;
	}

	CSC2Waypoint waypointTarget(*m_raceInfo);
	if(BuildWaypoint(pgTarget, waypointTarget))
	{
		waypointTarget.m_targetTime.max = DBL_MAX;
		m_fitnessCalc->AddWaypoint(waypointTarget);
		hasTarget = true;
	}

	if(!hasTarget)
	{
		delete m_fitnessCalc;
		m_fitnessCalc = 0;
		m_engine = 0;
		return false;
	}

	m_alphabet.clear();
	m_fitnessCalc->BuildAlphabet(m_alphabet);

	if(m_allowWaitCommands)
	{
		m_alphabet.push_back(&m_commandWaitPoint1Second);
		m_alphabet.push_back(&m_commandWait1Second);
		m_alphabet.push_back(&m_commandWait5Seconds);
	}

	switch(scout)
	{
	case eScoutStays:
		m_fitnessCalc->AddCustomEvent(CSC2Event(scoutTime, CSC2Event::eSendScout));
		break;
	case eScoutReturns:
		m_fitnessCalc->AddCustomEvent(CSC2Event(scoutTime, CSC2Event::eSendScout));
		m_fitnessCalc->AddCustomEvent(CSC2Event(scoutEndTime, CSC2Event::eReturnScout));
		break;
	case eScoutDies:
		m_fitnessCalc->AddCustomEvent(CSC2Event(scoutTime, CSC2Event::eSendScout));
		m_fitnessCalc->AddCustomEvent(CSC2Event(scoutEndTime, CSC2Event::eKillScout));
		break;
	}

	for(size_t i=0; i < m_events.size(); i++)
		m_fitnessCalc->AddCustomEvent(m_events[i]);

	m_fitnessCalc->SetInitialBuildOrder(m_initialBuildOrder);
	m_fitnessCalc->InitialiseBuildOrder();

	return true;
}

bool CSC2Engine::Start(const CVector<wxPropertyGrid *> &pgWaypoints, const wxPropertyGrid *pgTarget, EScoutingWorker scout /* = eNoScout */, int scoutTime /* = 0 */, int scoutEndTime /* = 0 */)
{
	if(m_engine && m_engine->IsRunning())
		return false;

	if(!InitialiseFitnessCalc(pgWaypoints, pgTarget, scout, scoutTime, scoutEndTime))
		return false;

	m_engine = new CGAEngine<CSC2NewChromosome, CSC2NewMutator, CSC2FitnessCalc, CFitnessValue, CGAPopulationPartialSortAndRemoveDuplicates>();

	m_engine->InitConfiguration(m_mutator, *m_fitnessCalc, 1000);

	m_engine->AddVillage(200, 100);
	m_engine->AddVillage(200, 100);
	m_engine->AddVillage(200, 100);
	m_engine->AddVillage(50, 30);
	m_engine->AddVillage(10, 8);

	m_engine->AddSeed(m_bestGame);

	m_engine->Start();

	return true;
}

void CSC2Engine::SetSeed(const std::vector<wxString> &seed)
{
	m_bestGame.GetValue().clear();
	for(size_t i=0; i < seed.size(); i++)
	{
		bool commandFound = false;
		for(size_t j=0; j < m_raceInfo->GetCommands().size(); j++)
		{
			if(seed[i] == m_raceInfo->GetCommands()[j]->GetName())
			{
				m_bestGame.GetValue().push_back(m_raceInfo->GetCommands()[j]);
				commandFound = true;
				break;
			}
		}

		if(!commandFound)
		{
			wxASSERT(false);
		}
	}
}

void CSC2Engine::Stop()
{
	if(!m_engine || !m_engine->IsRunning())
		return;

	m_engine->Stop();
}

void CSC2Engine::GetCityStats(size_t &population, size_t &evolution, size_t &stagnationCount, unsigned long long &gameCount, double &bestFitness) const
{
	population = m_engine->CityPopulationCount();
	evolution = m_engine->CityEvolution();
	stagnationCount = m_engine->CityStagnationCount();
	gameCount = m_engine->CityGameCount();
	bestFitness = m_engine->CityBestFitness();
}

void CSC2Engine::GetVillageStats(size_t villageIndex, size_t &population, size_t &evolution, size_t &stagnationCount, unsigned long long &gameCount, double &bestFitness) const
{
	population = m_engine->VillagePopulationCount(villageIndex);
	evolution = m_engine->VillageEvolution(villageIndex);
	stagnationCount = m_engine->VillageStagnationCount(villageIndex);
	gameCount = m_engine->VillageGameCount(villageIndex);
	bestFitness = m_engine->VillageBestFitness(villageIndex);
}

bool CSC2Engine::SetInitialBuildOrder(const wxString &buildOrder)
{
	m_initialBuildOrder.clear();

	wxStringTokenizer tokenizer(buildOrder, wxT("\r\n"), wxTOKEN_DEFAULT);
	while(tokenizer.HasMoreTokens())
	{
		wxString commandName = tokenizer.GetNextToken().Trim(true).Trim(false);

		int commandCount = 1;
		if(commandName.Contains("*"))
		{
			commandCount = wxAtoi(commandName.Mid(0, commandName.Index('*')).Trim(true));
			commandName = commandName.Mid(commandName.Index('*') + 1).Trim(false);
		}

		const CSC2Command *command = NULL;

		if(commandName == m_commandWait1Second.GetName())
			command = &m_commandWait1Second;
		else if(commandName == m_commandWaitPoint1Second.GetName())
			command = &m_commandWaitPoint1Second;
		else if(commandName == m_commandWait5Seconds.GetName())
			command = &m_commandWait5Seconds;
		else
		{
			for(size_t i=0; i < m_raceInfo->GetCommands().size(); i++)
			{
				if(m_raceInfo->GetCommands()[i]->GetName() == commandName)
				{
					command = m_raceInfo->GetCommands()[i];
					break;
				}
			}
		}

		if(command == NULL)
			return false;
		
		for(int i=0; i < commandCount; i++)
			m_initialBuildOrder.push_back(command);
	}

	return true;
}
