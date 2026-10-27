#ifndef MODEL_PERSISTENCE_H
#define MODEL_PERSISTENCE_H

#include <map>
#include <string>

struct PreprocessingConfig {
    PreprocessingConfig();
    bool stripEdgePunctuation;
    bool lowercaseTokens;
};

namespace ModelPersistence {

bool saveModel(
    const std::string& path,
    const std::map<std::string, int>& politicalSpeechWordCounts,
    const std::map<std::string, int>& nonPoliticalSpeechWordCounts,
    const std::map<std::string, bool>& uniqueVocabulary,
    int totalPoliticalSpeeches,
    int totalNonPoliticalSpeeches,
    int totalPoliticalSpeechWords,
    int totalNonPoliticalSpeechWords,
    double smoothingAlpha,
    const PreprocessingConfig& preprocessingConfig,
    std::string& error);

bool loadModel(
    const std::string& path,
    std::map<std::string, int>& politicalSpeechWordCounts,
    std::map<std::string, int>& nonPoliticalSpeechWordCounts,
    std::map<std::string, bool>& uniqueVocabulary,
    int& totalPoliticalSpeeches,
    int& totalNonPoliticalSpeeches,
    int& totalPoliticalSpeechWords,
    int& totalNonPoliticalSpeechWords,
    double& smoothingAlpha,
    PreprocessingConfig& preprocessingConfig,
    std::string& error);

}

#endif
