#pragma once
#ifndef SLM1_CHARACTERSET
#define SLM1_CHARACTERSET

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>

#include <SlrLib/Types/Types.hpp>

#include "Misc.hpp"

SLM1_NAMESPACE_BEGIN

class CharacterSet
{
public:
	friend class Model;

	static CharacterSet& GetInstance()
	{
		static CharacterSet instance;
		return instance;
	}

	void LoadToSet(const std::string& _text)
	{
		for (const char character : _text)
		{
			characterSet[character] = currentCharacterIndex;
			++currentCharacterIndex;
		}
	}

	u32 operator[](const char _value) const
	{
		if (characterSet.contains(_value))
		{
			return characterSet.at(_value);
		}

		return characterSet.at('#');
	}

	char GetByValue(const u32 _value)
	{
		auto it = std::find_if(
			std::begin(characterSet),
			std::end(characterSet),
			[_value](auto&& setIterator)
			{
				return setIterator.second == _value;
			}
		);

		if (it == std::end(characterSet))
		{
			return '#';
		}

		return it->first;
	}

	size GetSize() const
	{
		return characterSet.size();
	}
private:
	CharacterSet()
	{
		LoadToSet("#");
	}

	u32 currentCharacterIndex = 0;

	std::unordered_map<char, u32> characterSet;
};

class Sequence
{
public:
	friend Sequence StringToSequence(const std::string& _value);
	friend class Model;

	Sequence() = default;

	void AppendByCharacter(const char _value)
	{
		values.push_back(characterSet[_value]);
	}

	void AppendByValue(const u32 _value)
	{
		values.push_back(_value);
	}

	std::string ToString() const
	{
		std::string result;

		for (const u32 value : values)
		{
			result += characterSet.GetByValue(value);
		}

		return result;
	}

	size GetHash() const
	{
		const size primes[10] = {
			99546989,
			24833777,
			38882117,
			76505003,
			47100689,
			25369543,
			24355349,
			26822123,
			38251723,
			46912531
		};

		size result = 1;

		for (size index = 0; index < values.size(); ++index)
		{
			size xorWith;

			if (index == values.size() - 1)
			{
				xorWith = primes[index % 10];
			}
			else
			{
				xorWith = values[index + 1];
			}

			result *= (values[index] ^ xorWith) ^ 0xFFFFFFFF;
			size firstDigit = ((size)std::pow(10, (size)std::log(result)));
			size primeIndex = firstDigit > 0 ? result / firstDigit : 1;
			result *= primes[primeIndex];
		}

		return result;
	}

	Sequence GetSubSequence(const size _firstIndex, const size _lastIndex) const
	{
		Sequence result;

		for (size index = _firstIndex; index < _lastIndex; ++index)
		{
			result.AppendByValue(values[index]);
		}

		return result;
	}

	size GetSize() const
	{
		return values.size();
	}

	u32 operator[](const size _value) const
	{
		return values[_value];
	}

	Sequence& operator=(const Sequence& _other)
	{
		this->values = _other.values;

		return *this;
	}
private:
	CharacterSet& characterSet = CharacterSet::GetInstance();

	std::vector<u32> values;
};

inline Sequence StringToSequence(const std::string& _value)
{
	Sequence result;

	for (const char character : _value)
	{
		result.values.push_back(CharacterSet::GetInstance()[character]);
	}

	return result;
}

SLM1_NAMESPACE_END

#endif // ifndef SLM1_CHARACTERSET