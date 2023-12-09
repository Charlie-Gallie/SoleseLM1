#include <iostream>
#include <filesystem>
#include <fstream>

#include "Model.hpp"
#include "CharacterSet.hpp"
#include "PredictionSet.hpp"

using namespace SLM1;

Model& slm1 = Model::GetInstance();
CharacterSet& characterSet = CharacterSet::GetInstance();

std::string title = R"(
  ____        _                _     __  __ _ 
 / ___|  ___ | | ___  ___  ___| |   |  \/  / |
 \___ \ / _ \| |/ _ \/ __|/ _ \ |   | |\/| | |
  ___) | (_) | |  __/\__ \  __/ |___| |  | | |
 |____/ \___/|_|\___||___/\___|_____|_|  |_|_|
)";

void LoadModel()
{
	std::cout << "Model path: ";

	std::string modelPath;
	std::getline(std::cin, modelPath);

	std::vector<byte> data;

	std::ifstream modelImport(modelPath, std::ios::binary);

	if (!modelImport.good())
	{
		std::cout << "Could not load model file" << std::endl;
		return;
	}

	std::cout << "Importing model..." << std::endl;

	byte currentChar;
	while (modelImport >> std::noskipws >> currentChar)
	{
		data.push_back(currentChar);
	}

	std::cout << "Loading model..." << std::endl;

	slm1.ImportModel(data);

	std::cout << "Model loaded\n" << std::endl;

	while (1)
	{
		std::string userIn;
		std::cout << "> ";
		std::getline(std::cin, userIn);
		std::cout << slm1.Answer(userIn) << std::endl;
		std::cout << "\n";
	}
}

void TrainModel()
{
	std::cout << R"(
Ensure the training data is structured as shown below:

TrainingRoot
 +
  \--[d]set_0
  |      +
  |       \--[f]sample_0
  |       |--[f]sample_1
  |       |--[f]sample_2
  |       |--[f]sample_*
  |		
  |--[d]set_0
  |      +
  |       \--[f]sample_0
  |       |--[f]sample_1
  |       |--[f]sample_2
  |       |--[f]sample_*
  |
  |--[d]set_*

)";

	std::cout << "Training root directory: ";

	std::string trainingRoot;
	std::getline(std::cin, trainingRoot);

	if (!std::filesystem::exists(trainingRoot))
	{
		std::cout << "Could not find path specified" << std::endl;
		return;
	}

	std::cout << "Output file name (no extension): ";
	std::string exportFileName;
	std::getline(std::cin, exportFileName);

	std::filesystem::path exportPath = trainingRoot;
	exportPath = exportPath.parent_path();

	std::ofstream exportFile(exportPath.string() + "/" + exportFileName + ".slm", std::ios::binary | std::ios_base::app);
	if (!exportFile.good())
	{
		std::cout << "Couldn't create output file" << std::endl;
		return;
	}

	std::filesystem::path root = trainingRoot;

	size amountOfSets = std::distance(std::filesystem::directory_iterator(root), {});
	size counter = 1;

	for (const auto& sample : std::filesystem::directory_iterator(root))
	{
		std::cout << "Training with sample " << counter << " of " << amountOfSets << " - " << sample.path().filename() << std::endl;
		++counter;

		std::ifstream trainingFile(sample.path(), std::ios_base::binary);

		std::string trainingString = "";

		while (!trainingFile.eof())
		{
			std::string buffer;
			std::getline(trainingFile, buffer);
			trainingString += buffer;
		}

		slm1.TrainWithSample(trainingString);

		std::vector<byte> data = slm1.ExportModel();
		for (byte b : data)
		{
			exportFile << b;
		}

		slm1.ClearModel();
	}

	exportFile.close();

	std::cout << "Training done" << std::endl;
}

int main()
{
	characterSet.LoadToSet("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ,.!?\"'(){}[]:;-=_+\\/\n\r");

	std::cout << title << std::endl;

	while (1)
	{
		size mode = 0;
		std::cout << R"(

Options:
	1. Load Model
	2. Train Model

> )";
		std::string stringInput;
		std::getline(std::cin, stringInput);

		try
		{
			mode = std::stoi(stringInput);
		}
		catch (...)
		{
			mode = 0;
		}

		switch (mode)
		{
		case 1:
			LoadModel();
			break;
		case 2:
			TrainModel();
			break;
		default:
			std::cout << "Invalid input" << std::endl;
			break;
		}
	}

	size amountOfSets = std::distance(std::filesystem::directory_iterator("Training_Samples/dev_samples"), {});
	size counter = 1;

	/*
	for (const auto& set : std::filesystem::directory_iterator("Training_Samples/dev_samples"))
	{
		std::cout << "Training with set " << counter << " of " << amountOfSets << std::endl;
		++counter;

		for (const auto& sample : std::filesystem::directory_iterator(set.path()))
		{
			std::ifstream trainingFile(sample.path(), std::ios_base::binary);

			std::string trainingString = "";

			while (!trainingFile.eof())
			{
				std::string buffer;
				std::getline(trainingFile, buffer);
				trainingString += buffer;
			}

			slm1.TrainWithSample(trainingString.substr(0, trainingString.size() - 1), trainingString.back());
		}
	}

	std::ofstream exported("exported.slm", std::ios::binary);
	
	std::vector<byte> data = slm1.ExportModel();
	for (byte b : data)
	{
		exported << b;
	}

	exported.close();

	data.clear();
	*/

	std::vector<byte> data;

	std::ifstream modelImport("exported.slm", std::ios::binary);

	if (!modelImport.good())
	{
		std::cout << "Could not load model file" << std::endl;
	}

	byte currentChar;
	while (modelImport >> std::noskipws >> currentChar)
	{
		data.push_back(currentChar);
	}

	slm1.ImportModel(data);

	while (1)
	{
		std::string userIn;
		std::cout << "> ";
		std::getline(std::cin, userIn);
		std::cout << slm1.Answer(userIn) << std::endl;
		std::cout << "\n";
	}

	std::cin.get();
	return 0;
}