#ifndef MODEL_EVALUATION_H
#define MODEL_EVALUATION_H

#include <iosfwd>
#include <string>

namespace ModelEvaluation {

typedef std::string (*ClassifierFunction)(const std::string&, void*);

struct EvaluationSummary {
    int totalFiles;
    int correctPredictions;

    EvaluationSummary();
};

bool evaluateDirectory(
    const std::string& directoryPath,
    ClassifierFunction classify,
    void* classifierData,
    EvaluationSummary& summary,
    std::ostream& output,
    std::string& error);

}

#endif
