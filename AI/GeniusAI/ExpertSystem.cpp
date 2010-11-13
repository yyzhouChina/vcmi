#include "GeneralAI.h"
#include "../../CCallback.h"
#include "ExpertSystem.h"

/*
 * ExpertSystem.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

template <typename ruleType, typename facts> template <typename cond> void ExpertSystemShell<ruleType, facts>::DataDrivenReasoning(runType type)
{
	std::set<ruleType>::iterator ir;
	std::list<fact>::iterator iF;
	std::set<std::pair<cond, input*>>::iterator ic;
	bool factWasAdded = false; //carry it over inner loop
	switch (type)
	{
		case ANY_GOAL: //first produced decision ends reasoning
		{
			int goalCounter = 0;
			while(!goalCounter) //we reach goal or can't modify knowledge anymore
			{
				for (ir = knowledge.begin(); ir != knowledge.end(); ir++)
				{
					for (iF = factList.begin(); iF != factList.end(); iF++)
					{
						for (ic = ir->conditions.begin(); ic != ir->conditions.end(), ic++)
						{
							if (ic->first.object.matchesFact(iF->object)) //condition matches held object
							{
								(ic->second = *iF);
								++(ir->conditionCounter);
							}
						}
						if (ir->conditions.size() >= ir->conditionCounter());
						{
							ir->fireRule(); //all conditions matched
							ir->conditionCounter = 0; //do it here or in rule itself?
							if (dynamic_cast<&Goal>(*ir))
								++goalCounter;
						}
					}
					matchedConditions = 0;
					//modify set until something happens (hopefully!)
					for (iF = factsToErase.begin(); iF != factsToErase.end(); iF++)
						factList.erase(knowledge.find(*iF));
					factsToErase.clear(); //TODO: what if fact is remembered by rule, yet already erased?
					for (iF = factsToAdd.begin(); iF != factsToAdd.end(); iF++)
						factList.insert(*iF);
					if (factsToAdd.size())
					{
						factsToAdd.clear();
						factWasAdded = true;
					}
				}
				for (ir = rulesToErase.begin(); ir != rulesToErase.end(); ir++)
					knowledge.erase(knowledge.find(*ir));
				rulesToErase.clear();
				for (ir = rulesToAdd.begin(); ir != rulesToAdd.end(); ir++)
					knowledge.insert(*ir);
				if (!(factWasAdded || rulesToAdd.size())) //we can't do anything more
					break;
				rulesToAdd.clear();	
			};
		}
		break;
	}
}
void BonusRule::fireRule()
{
	for (std::set<std::pair<BonusCondition, BonusHolder*>>::iterator it = cons.begin(); it != cons.end(); it++)
	{
		switch (it->first.parameter)
		{ //compare fact with condition
			case BonusCondition::type:
				if (!it->first.functor(it->second->object->type, it->first.value)) return;
			break;
			case BonusCondition::subtype: //probably suprfluous, Selector already handles that
				if (!it->first.functor(it->second->object->subtype, it->first.value)) return;
			break;
			case BonusCondition::val:
				if (!it->first.functor(it->second->object->val, it->first.value)) return;
			break;
			case BonusCondition::duration:
				if (!it->first.functor(it->second->object->duration, it->first.value)) return;
			break;
			case BonusCondition::source:
				if (!it->first.functor(it->second->object->source, it->first.value)) return;
			break;
			case BonusCondition::id:
				if (!it->first.functor(it->second->object->id, it->first.value)) return;
			break;
			case BonusCondition::valType:
				if (!it->first.functor(it->second->object->valType, it->first.value)) return;
			break;
			case BonusCondition::additionalInfo:
				if (!it->first.functor(it->second->object->additionalInfo, it->first.value)) return;
			break;
			case BonusCondition::effectRange:
				if (!it->first.functor(it->second->object->effectRange, it->first.value)) return;
			break;
			default: //ignore or accept?
			break;
		};
	}
	//TODO: add new fact or modify existing one
}
//template <typename input, typename conType> void Rule<input, conType>::refreshRule(std::set<conType> &conditionSet)
//{
//	cons.clear();
//	for (std::set<conType>::iterator it = conditionSet.begin(); it != conditionSet.end(); it++)
//		cons.insert(std::make_pair<conType,input*>(*it, NULL)); //pointer to condition and null fact
//}
bool BonusCondition::matchesFact(Bonus &fact)
{
	if (object(fact)) //Bonus(fact) matches local Selector(object)
		return true;
	return false;
}
