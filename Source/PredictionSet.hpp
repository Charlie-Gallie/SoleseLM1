#pragma once
#ifndef SLM1_PREDICTIONSET
#define SLM1_PREDICTIONSET

#include <unordered_map>

#include "Misc.hpp"
#include "CharacterSet.hpp"

SLM1_NAMESPACE_BEGIN

class PredictionSet
{
public:
	friend class Model;

	PredictionSet() = default;

	u32 GetMostLikelyValue() const
	{
		CharacterInstance currentMaximum = { characterSet['#'], 0, 0.0};

		for (const std::pair<u32, CharacterInstance>& characterLikelihood : likelihood)
		{
			if (characterLikelihood.second.likelihood > currentMaximum.likelihood)
			{
				currentMaximum = characterLikelihood.second;
			}
		}

		return currentMaximum.value;
	}

	void IncrementOccurrencesFor(const u32 _value)
	{
		CharacterInstance& incrementFor = likelihood[_value];

		incrementFor.value = _value;
		incrementFor.occurrences += 1;

		size currentTotalOccurences = 0;

		for (const std::pair<u32, CharacterInstance>& characterLikelihood : likelihood)
		{
			currentTotalOccurences += characterLikelihood.second.occurrences;
		}

		for (const std::pair<u32, CharacterInstance>& characterLikelihood : likelihood)
		{
			CharacterInstance& currentInstance = likelihood[characterLikelihood.second.value];
			currentInstance.likelihood = double(currentInstance.occurrences) / double(currentTotalOccurences);
		}
	}

	PredictionSet& operator=(const PredictionSet& _other)
	{
		this->likelihood = _other.likelihood;

		return *this;
	}

private:
	struct CharacterInstance
	{
		u32 value = 0;
		size occurrences = 0;
		double likelihood = 0.0;
	};

	CharacterSet& characterSet = CharacterSet::GetInstance();

	std::unordered_map<u32, CharacterInstance> likelihood;
};

class PredictionState
{
public:
	friend class Model;

	static PredictionState& GetInstance()
	{
		static PredictionState instance;
		return instance;
	}

	PredictionSet& GetPredictionForSequence(const Sequence& _sequence)
	{
		Sequence attemptSequence = _sequence;

		while (attemptSequence.GetSize() > 0)
		{
			size sequenceHash = attemptSequence.GetHash();
			if (predictionSets.contains(sequenceHash))
			{
				return predictionSets.at(sequenceHash);
			}

			attemptSequence = attemptSequence.GetSubSequence(1, attemptSequence.GetSize());
		}

		// New sequence
		PredictionSet& newPredictionSet = predictionSets[_sequence.GetHash()];
		newPredictionSet = PredictionSet{};

		return newPredictionSet;
	}

private:
	PredictionState() = default;

	// Sequence hash, predictions for next
	std::unordered_map<size, PredictionSet> predictionSets;
};

SLM1_NAMESPACE_END

#endif // ifndef SLM1_PREDICTIONSET