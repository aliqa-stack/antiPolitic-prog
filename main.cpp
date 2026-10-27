// Naive Bayes algorithm

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <fstream>
#include <climits>
#include <cctype>
#include <algorithm>
#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif
#include "../include/modelpersistence.h"
#include "../include/ModelEvaluation.h"

class NaiveBayes{
    public:
    std::map<std::string, int> politicalSpeechWordCounts;
    std::map<std::string, int> nonPoliticalSpeechWordCounts;
    std::map<std::string, bool> uniqueVocabulary;
    int totalPoliticalSpeeches = 0;
    int totalNonPoliticalSpeeches = 0;
    int totalPoliticalSpeechWords = 0;
    int totalNonPoliticalSpeechWords = 0;
    double smoothingAlpha = 1.0;
    PreprocessingConfig preprocessingConfig;
    // std::map<std::string, int> countOf_data2;

};

class DataSet : public NaiveBayes{
    public:
    //tahap tokenisasi dari data 
    std::vector<std::string> tokenizeText(const std::string& speechText, bool updateVocabulary){
        std::stringstream textStream(speechText);
        std::string word;
        std::vector<std::string> words;
        while(textStream >> word){
            std::size_t firstCharacter = 0;
            std::size_t lastCharacter = word.size();
            if(preprocessingConfig.stripEdgePunctuation){
                while(firstCharacter < lastCharacter && std::ispunct(static_cast<unsigned char>(word[firstCharacter]))){
                    firstCharacter++;
                }
                while(lastCharacter > firstCharacter && std::ispunct(static_cast<unsigned char>(word[lastCharacter - 1]))){
                    lastCharacter--;
                }
            }
            word = word.substr(firstCharacter, lastCharacter - firstCharacter);
            if(word.empty()){
                continue;
            }
            if(preprocessingConfig.lowercaseTokens){
                for(std::string::iterator character = word.begin(); character != word.end(); ++character){
                    *character = static_cast<char>(std::tolower(static_cast<unsigned char>(*character)));
                }
            }

            words.push_back(word);
            if(updateVocabulary){
                uniqueVocabulary[word] = true;
            }
        }
      
        return words;
    }


    //bedakan dari label data 
    void train(std::string& speechText, std::string speechClass){
        std::vector<std::string> words = tokenizeText(speechText, true);
        if(speechClass == "politic speech"){
            totalPoliticalSpeeches++;
            for (auto& word : words){
                totalPoliticalSpeechWords++;
                politicalSpeechWordCounts[word]++;
            }
        }else if(speechClass == "non politic speech"){
            totalNonPoliticalSpeeches++;
            for (auto& word : words){
                totalNonPoliticalSpeechWords++;
                nonPoliticalSpeechWordCounts[word]++;
            }
        }
    }

    //rumus menghitung teorema bayes untuk data yang ada
    std::string probability(
        std::string& speechText,
        bool showProgress = true,
        bool showSummary = true){
        //kumpulkan semua data yang ada menjadi 1
        double totalSpeechCount = totalPoliticalSpeeches + totalNonPoliticalSpeeches;
        double politicalSpeechLogProbability = std::log(
            (totalPoliticalSpeeches + smoothingAlpha) /
            (totalSpeechCount + 2.0 * smoothingAlpha));
        double nonPoliticalSpeechLogProbability = std::log(
            (totalNonPoliticalSpeeches + smoothingAlpha) /
            (totalSpeechCount + 2.0 * smoothingAlpha));
        
        double accumulatedPoliticalSpeechLogProbability = politicalSpeechLogProbability;
        double accumulatedNonPoliticalSpeechLogProbability = nonPoliticalSpeechLogProbability;
        
        std::vector<std::string> words = tokenizeText(speechText, false);
        
        std::size_t uniqueVocabularySize = uniqueVocabulary.size();
        int iteration = 0;
        for(auto& word : words){
            iteration++;
            int politicalSpeechWordCount = politicalSpeechWordCounts.count(word) ? politicalSpeechWordCounts[word] : 0;
            int nonPoliticalSpeechWordCount = nonPoliticalSpeechWordCounts.count(word) ? nonPoliticalSpeechWordCounts[word] : 0;
       
            //hitung likeood data 1 dan data2 
              double politicalSpeechWordLogProbability = std::log(
                  (politicalSpeechWordCount + smoothingAlpha) /
                  (totalPoliticalSpeechWords + smoothingAlpha * uniqueVocabularySize));
              double nonPoliticalSpeechWordLogProbability = std::log(
                  (nonPoliticalSpeechWordCount + smoothingAlpha) /
                  (totalNonPoliticalSpeechWords + smoothingAlpha * uniqueVocabularySize));

              accumulatedNonPoliticalSpeechLogProbability += nonPoliticalSpeechWordLogProbability;
              accumulatedPoliticalSpeechLogProbability += politicalSpeechWordLogProbability;

              if(showProgress){
                  std::cout << "Iterasi " << iteration << " - kata: " << word << "\n";
                  std::cout << "Probabilitas log pidato politik: " << accumulatedPoliticalSpeechLogProbability << "\n";
                  std::cout << "Probabilitas log pidato non-politik: " << accumulatedNonPoliticalSpeechLogProbability << "\n";
              }
        }
       
        if(showSummary){
            std::cout<<"Probabilitas log pidato politik: "<<accumulatedPoliticalSpeechLogProbability<<"\n";
            std::cout<<"Probabilitas log pidato non-politik: "<<accumulatedNonPoliticalSpeechLogProbability<<"\n";
            std::cout<<"Jumlah kosakata: "<<uniqueVocabularySize<<"\n";
        }
        
        //hitung apakah data1 lebih besar dari data2, maka klasifikasikan sebagai "pspeech"
        return accumulatedPoliticalSpeechLogProbability > accumulatedNonPoliticalSpeechLogProbability ? "politic speech" : "non politic speech";
            }

    std::string OutputData(std::string& data1){
        std::cout<< "Data 1: " << data1 << std::endl;
        return data1;
    }

    void printWordOccurrenceExtremes(const std::string& datasetName, const std::map<std::string, int>& wordOccurrenceCounts){
        std::string leastFrequentWord, mostFrequentWord;
        int leastOccurrenceCount = INT_MAX;
        int mostOccurrenceCount = 0;

        for (auto& wordOccurrence : wordOccurrenceCounts){
            if(wordOccurrence.second > mostOccurrenceCount){
                mostOccurrenceCount = wordOccurrence.second;
                mostFrequentWord = wordOccurrence.first;
            } 
            if(wordOccurrence.second < leastOccurrenceCount){
                leastOccurrenceCount = wordOccurrence.second;
                leastFrequentWord = wordOccurrence.first;

            }
        }

        std::cout << "\nDataset " << datasetName << ":\n";
        if(wordOccurrenceCounts.empty()){
            std::cout << "Tidak ada kata untuk dihitung." << std::endl;
            return;
        }
        std::cout << "Kata dengan jumlah kemunculan terbanyak: " << mostFrequentWord << " (" << mostOccurrenceCount << " kali)" << std::endl;
        std::cout << "Kata dengan jumlah kemunculan paling sedikit: " << leastFrequentWord << " (" << leastOccurrenceCount << " kali)" << std::endl;
    }

};

bool loadTrainingFile(const std::string& datasetPath, const std::string& classLabel, DataSet& data){
    std::ifstream file(datasetPath.c_str());
    if(!file.is_open()){
        std::cerr << "Gagal membuka dataset training: " << datasetPath << std::endl;
        return false;
    }

    std::string header;
    if(!std::getline(file, header)){
        std::cerr << "Header dataset tidak valid: " << datasetPath << std::endl;
        return false;
    }
    if(header.compare(0, 3, "\xEF\xBB\xBF") == 0){
        header.erase(0, 3);
    }
    if(!header.empty() && header.back() == '\r'){
        header.pop_back();
    }
    if(header != "text"){
        std::cerr << "Header dataset tidak valid: " << datasetPath << std::endl;
        return false;
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    std::string speechText = contents.str();
    if(speechText.find_first_not_of(" \t\n\r") == std::string::npos){
        std::cerr << "Dataset training kosong: " << datasetPath << std::endl;
        return false;
    }

    data.train(speechText, classLabel);
    return true;
}

bool loadTrainingFolder(const std::string& directoryPath, const std::string& classLabel, DataSet& data){
    std::vector<std::string> datasetPaths;
#ifdef _WIN32
    std::string searchPath = directoryPath + "\\*.csv";
    struct _finddata_t fileInfo;
    intptr_t searchHandle = _findfirst(searchPath.c_str(), &fileInfo);
    if(searchHandle == -1){
        std::cerr << "Gagal membaca folder dataset: " << directoryPath << std::endl;
        return false;
    }

    do{
        if((fileInfo.attrib & _A_SUBDIR) == 0){
            datasetPaths.push_back(directoryPath + "\\" + fileInfo.name);
        }
    }while(_findnext(searchHandle, &fileInfo) == 0);
    _findclose(searchHandle);
#else
    DIR* directory = opendir(directoryPath.c_str());
    if(directory == NULL){
        std::cerr << "Gagal membaca folder dataset: " << directoryPath << std::endl;
        return false;
    }

    struct dirent* fileInfo;
    while((fileInfo = readdir(directory)) != NULL){
        std::string fileName = fileInfo->d_name;
        if(fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".csv"){
            datasetPaths.push_back(directoryPath + "/" + fileName);
        }
    }
    closedir(directory);
#endif

    std::sort(datasetPaths.begin(), datasetPaths.end());
    if(datasetPaths.empty()){
        std::cerr << "Tidak ada file CSV di folder dataset: " << directoryPath << std::endl;
        return false;
    }

    for(std::size_t i = 0; i < datasetPaths.size(); i++){
        if(!loadTrainingFile(datasetPaths[i], classLabel, data)){
            return false;
        }
    }

    std::cout << "Dataset training dimuat dari " << directoryPath
              << " (" << datasetPaths.size() << " file)" << std::endl;
    return true;
}

std::string classifyForEvaluation(const std::string& testText, void* classifierData){
    DataSet* data = static_cast<DataSet*>(classifierData);
    std::string text = testText;
    return data->probability(text, false, false);
}

int main(int argc, char* argv[]){
    DataSet data;
    const std::string modelPath = "models/models-naivebayes-smoothingAlpha.bin";
    bool evaluationOnly = argc == 2 &&
        std::string(argv[1]) == "--load-model-evaluation";
    if(argc > 2 ||
       (argc == 2 &&
        std::string(argv[1]) != "--load-model-evaluation")){
        std::cerr << "Penggunaan: " << argv[0]
                  << " [--load-model-evaluation]" << std::endl;
        return 1;
    }

    std::string modelError;
    if(evaluationOnly){
        if(!ModelPersistence::loadModel(
               modelPath,
               data.politicalSpeechWordCounts,
               data.nonPoliticalSpeechWordCounts,
               data.uniqueVocabulary,
               data.totalPoliticalSpeeches,
               data.totalNonPoliticalSpeeches,
               data.totalPoliticalSpeechWords,
               data.totalNonPoliticalSpeechWords,
               data.smoothingAlpha,
               data.preprocessingConfig,
               modelError)){
            std::cerr << modelError << std::endl;
            return 1;
        }
        std::cout << "Model berhasil dimuat dari " << modelPath << std::endl;
    }else{
        if(!loadTrainingFolder("dataset/train/politicspeech", "politic speech", data) ||
           !loadTrainingFolder("dataset/train/nonpoliticspeech", "non politic speech", data)){
            return 1;
        }
        if(!ModelPersistence::saveModel(
               modelPath,
               data.politicalSpeechWordCounts,
               data.nonPoliticalSpeechWordCounts,
               data.uniqueVocabulary,
               data.totalPoliticalSpeeches,
               data.totalNonPoliticalSpeeches,
               data.totalPoliticalSpeechWords,
               data.totalNonPoliticalSpeechWords,
               data.smoothingAlpha,
               data.preprocessingConfig,
               modelError)){
            std::cerr << modelError << std::endl;
            return 1;
        }
        std::cout << "Model berhasil disimpan ke " << modelPath << std::endl;
    }

    if(!evaluationOnly){
        data.printWordOccurrenceExtremes("politik", data.politicalSpeechWordCounts);
        data.printWordOccurrenceExtremes("nonpolitik", data.nonPoliticalSpeechWordCounts);
    }

    ModelEvaluation::EvaluationSummary evaluationSummary;
    std::string evaluationError;
    if(!ModelEvaluation::evaluateDirectory(
           "dataset/testing",
           classifyForEvaluation,
           &data,
           evaluationSummary,
           std::cout,
           evaluationError)){
        std::cerr << evaluationError << std::endl;
        return 1;
    }

    return 0;
}