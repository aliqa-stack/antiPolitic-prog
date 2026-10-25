//NAIVES BAYES ALGORITHM

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <fstream>
#include <climits>
#include <cctype>

class NaiveBayes{
    public:
    std::map<std::string, int> politicalSpeechWordCounts;
    std::map<std::string, int> nonPoliticalSpeechWordCounts;
    std::map<std::string, bool> uniqueVocabulary;
    int totalPoliticalSpeeches = 0;
    int totalNonPoliticalSpeeches = 0;
    int totalPoliticalSpeechWords = 0;
    int totalNonPoliticalSpeechWords = 0;
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
            while(firstCharacter < lastCharacter && std::ispunct(static_cast<unsigned char>(word[firstCharacter]))){
                firstCharacter++;
            }
            while(lastCharacter > firstCharacter && std::ispunct(static_cast<unsigned char>(word[lastCharacter - 1]))){
                lastCharacter--;
            }
            word = word.substr(firstCharacter, lastCharacter - firstCharacter);
            if(word.empty()){
                continue;
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
    std::string probability(std::string& speechText){
        //kumpulkan semua data yang ada menjadi 1
        float totalSpeechCount = totalPoliticalSpeeches + totalNonPoliticalSpeeches;
        //bagi data1 dengan keseluruhan data yang ada untuk menghitung probabilitas
        //laplace smoothing untuk menghindari pembagian dengan 0 tambahkan 1 ke total_pspeech
        float politicalSpeechLogProbability = std::log((float)(totalPoliticalSpeeches + 1) / (float)(totalSpeechCount + 1));
        float nonPoliticalSpeechLogProbability = std::log((float)(totalNonPoliticalSpeeches + 1) / (float)(totalSpeechCount + 1));
        
        float accumulatedPoliticalSpeechLogProbability = politicalSpeechLogProbability;
        float accumulatedNonPoliticalSpeechLogProbability = nonPoliticalSpeechLogProbability;
        
        std::vector<std::string> words = tokenizeText(speechText, false);
        
        int uniqueVocabularySize = uniqueVocabulary.size();
        int iteration = 0;
        for(auto& word : words){
            iteration++;
            int politicalSpeechWordCount = politicalSpeechWordCounts.count(word) ? politicalSpeechWordCounts[word] : 0;
            int nonPoliticalSpeechWordCount = nonPoliticalSpeechWordCounts.count(word) ? nonPoliticalSpeechWordCounts[word] : 0;
       
            //hitung likeood data 1 dan data2 
              float politicalSpeechWordLogProbability = std::log((float)(politicalSpeechWordCount + 1)/ (float)(totalPoliticalSpeechWords + uniqueVocabularySize));
              float nonPoliticalSpeechWordLogProbability = std::log((float)(nonPoliticalSpeechWordCount + 1)/ (float)(totalNonPoliticalSpeechWords + uniqueVocabularySize));

              accumulatedNonPoliticalSpeechLogProbability += nonPoliticalSpeechWordLogProbability;
              accumulatedPoliticalSpeechLogProbability += politicalSpeechWordLogProbability;

              std::cout << "Iterasi " << iteration << " - kata: " << word << "\n";
              std::cout << "Probabilitas log pidato politik: " << accumulatedPoliticalSpeechLogProbability << "\n";
              std::cout << "Probabilitas log pidato non-politik: " << accumulatedNonPoliticalSpeechLogProbability << "\n";
        }
       
        std::cout<<"Probabilitas log pidato politik: "<<accumulatedPoliticalSpeechLogProbability<<"\n";
        std::cout<<"Probabilitas log pidato non-politik: "<<accumulatedNonPoliticalSpeechLogProbability<<"\n";
        std::cout<<"Jumlah kosakata: "<<uniqueVocabularySize<<"\n";
        
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

bool loadDataset(const std::string& datasetPath, const std::string& expectedDatasetLabel, const std::string& classLabel, DataSet& data){
    std::ifstream file(datasetPath);
    if(!file.is_open()){
        std::cerr << "Gagal membuka dataset: " << datasetPath << std::endl;
        return false;
    }

    std::string line;
    if(!std::getline(file, line)){
        std::cerr << "Header dataset tidak valid: " << datasetPath << std::endl;
        return false;
    }
    if(line.compare(0, 3, "\xEF\xBB\xBF") == 0){
        line.erase(0, 3);
    }
    if(line != "text,label"){
        std::cerr << "Header dataset tidak valid: " << datasetPath << std::endl;
        return false;
    }

    int rowNumber = 1;
    while(std::getline(file, line)){
        rowNumber++;
        if(line.empty()){
            continue;
        }

        std::size_t separator = line.rfind(',');
        if(separator == std::string::npos){
            std::cerr << "error" << datasetPath << ":" << rowNumber << std::endl;
            return false;
        }

        std::string text = line.substr(0, separator);
        std::string label = line.substr(separator + 1);
        if(!label.empty() && label.back() == '\r'){
            label.pop_back();
        }
        if(label != expectedDatasetLabel){
            std::cerr << "error" << std::endl;
            return false;
        }

        data.train(text, classLabel);
    }

    return true;
}

bool loadTestDataset(const std::string& datasetPath, std::string& testText){
    std::ifstream file(datasetPath);
    if(!file.is_open()){
        std::cerr << "Gagal membuka dataset pengujian: " << datasetPath << std::endl;
        return false;
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    testText = contents.str();
    if(testText.find_first_not_of(" \t\n\r") == std::string::npos){
        std::cerr << "Dataset pengujian kosong: " << datasetPath << std::endl;
        return false;
    }

    return true;
}

int main(){
    DataSet data;
    if(!loadDataset("dataset/train/politicspeech.csv", "politicspeech", "politic speech", data) ||
       !loadDataset("dataset/train/nonpoliticspeech.csv", "nonpoliticspeech", "non politic speech", data)){
        return 1;
    }

    std::string testText;
    if(!loadTestDataset("dataset/testing/test.csv", testText)){
        return 1;
    }

    std::string classificationResult = data.probability(testText);
    std::cout << "Hasil Klasifikasi: " << classificationResult << std::endl;
    data.printWordOccurrenceExtremes("politik", data.politicalSpeechWordCounts);
    data.printWordOccurrenceExtremes("nonpolitik", data.nonPoliticalSpeechWordCounts);

    std::map<std::string, int> testWordOccurrenceCounts;
    for(const auto& word : data.tokenizeText(testText, false)){
        testWordOccurrenceCounts[word]++;
    }
    data.printWordOccurrenceExtremes("testing", testWordOccurrenceCounts);

    return 0;
}