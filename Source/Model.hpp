#pragma once
#ifndef SLM1_MODEL
#define SLM1_MODEL

#include <string>
#include <vector>
#include <thread>

#include "Types.hpp"
#include "Misc.hpp"
#include "CharacterSet.hpp"
#include "PredictionSet.hpp"

SLM1_NAMESPACE_BEGIN

class Model
{
public:
	static Model& GetInstance()
	{
		static Model instance;
		return instance;
	}

	void TrainWithSample(const std::string& _text)
	{
		std::string sample = _text;
		size totalSize = sample.size();
		size loops = 0;
		
		while (sample.size() > 1)
		{
			std::cout << "Estimated Progress: " << (double(loops) / double(totalSize)) * 100 << "%" << std::endl;
			++loops;

			const char nextCharacter = sample.back();

			std::string leftTrain = sample.substr(0, sample.size() - 1);
			predictionState.GetPredictionForSequence(StringToSequence(leftTrain)).IncrementOccurrencesFor(characterSet[nextCharacter]);

			std::string rightTrain = sample.substr(1, sample.size() - 2);
			while (rightTrain.size() > 0)
			{
				predictionState.GetPredictionForSequence(StringToSequence(rightTrain)).IncrementOccurrencesFor(characterSet[nextCharacter]);
				rightTrain = rightTrain.substr(1, rightTrain.size() - 1);
			}

			sample.pop_back();
		}
	}

	void ClearModel()
	{
		predictionState.predictionSets.clear();
	}

	std::vector<byte> ExportModel()
	{
		std::vector<byte> exportedData;

		for (const auto& set : predictionState.predictionSets)
		{
			size hash = set.first;
			for (size hashByteIndex = 0; hashByteIndex < sizeof(hash); ++hashByteIndex)
			{
				exportedData.push_back(((byte*)&hash)[hashByteIndex]);
			}

			size samples = set.second.likelihood.size();
			for (size samplesByteIndex = 0; samplesByteIndex < sizeof(samples); ++samplesByteIndex)
			{
				exportedData.push_back(((byte*)&samples)[samplesByteIndex]);
			}

			for (const auto&[value, instance] : set.second.likelihood)
			{
				for (size valueIndex = 0; valueIndex < sizeof(value); ++valueIndex)
				{
					exportedData.push_back(((byte*)&value)[valueIndex]);
				}

				double likelihood = instance.likelihood;
				for (size likelihoodIndex = 0; likelihoodIndex < sizeof(likelihood); ++likelihoodIndex)
				{
					exportedData.push_back(((byte*)&likelihood)[likelihoodIndex]);
				}
			}
		}

		return exportedData;
	}

	void ImportModel(const std::vector<byte>& _data)
	{
		predictionState.predictionSets.clear();

		size currentByte = 0;
		while (currentByte < _data.size())
		{
			size currentOffset = 0;

			size hash = *(size*)&_data[currentByte];
			currentOffset += sizeof(hash);

			size samples = *(size*)&_data[currentByte + currentOffset];
			currentOffset += sizeof(samples);

			size sampleSize = sizeof(u32) + sizeof(double);

			for (size currentSampleIndex = 0; currentSampleIndex < samples; ++currentSampleIndex)
			{
				u32 value = *(u32*)&_data[currentByte + currentOffset];
				currentOffset += sizeof(value);

				double likelihood = *(double*)&_data[currentByte + currentOffset];
				currentOffset += sizeof(likelihood);

				predictionState.predictionSets[hash].likelihood[value] = PredictionSet::CharacterInstance{
					.value = value,
					.occurrences = 0,
					.likelihood = likelihood
				};
			}

			currentByte = currentByte + currentOffset;
		}
	}

	std::string Answer(const std::string& _value)
	{
		Sequence sampleSequence = StringToSequence(_value);

		for (size index = 0; index < 800; ++index)
		{
			u32 nextValue = predictionState.GetPredictionForSequence(sampleSequence).GetMostLikelyValue();
			sampleSequence.AppendByValue(nextValue);
		}

		std::string result = "";

		for (size index = 0; index < sampleSequence.GetSize(); ++index)
		{
			result += characterSet.GetByValue(sampleSequence[index]);
		}

		return result;
	}

private:
	PredictionState& predictionState = PredictionState::GetInstance();
	CharacterSet& characterSet = CharacterSet::GetInstance();

	Model() = default;
};

SLM1_NAMESPACE_END

#endif // ifndef SLM1_MODEL