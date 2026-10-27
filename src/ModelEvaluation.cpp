#include "../include/ModelEvaluation.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

namespace {

struct TestFile {
    std::string path;
    char expectedClass;
};

bool readTestFiles(
    const std::string& directoryPath,
    std::vector<TestFile>& testFiles,
    std::string& error) {
#ifdef _WIN32
    std::string searchPath = directoryPath + "\\*.csv";
    struct _finddata_t fileInfo;
    intptr_t searchHandle = _findfirst(searchPath.c_str(), &fileInfo);
    if (searchHandle == -1) {
        error = "Gagal membaca folder testing: " + directoryPath;
        return false;
    }

    do {
        if ((fileInfo.attrib & _A_SUBDIR) == 0) {
            std::string fileName = fileInfo.name;
            std::size_t extensionPosition = fileName.find_last_of('.');
            if (extensionPosition == std::string::npos || extensionPosition == 0) {
                _findclose(searchHandle);
                error = "Nama file testing tidak memiliki label p/n: " + fileName;
                return false;
            }
            char expectedClass = fileName[extensionPosition - 1];
            if (expectedClass != 'p' && expectedClass != 'n') {
                _findclose(searchHandle);
                error = "Label file testing harus p atau n: " + fileName;
                return false;
            }

            TestFile testFile;
            testFile.path = directoryPath + "\\" + fileName;
            testFile.expectedClass = expectedClass;
            testFiles.push_back(testFile);
        }
    } while (_findnext(searchHandle, &fileInfo) == 0);
    _findclose(searchHandle);
#else
    DIR* directory = opendir(directoryPath.c_str());
    if (directory == NULL) {
        error = "Gagal membaca folder testing: " + directoryPath;
        return false;
    }

    struct dirent* fileInfo;
    while ((fileInfo = readdir(directory)) != NULL) {
        std::string fileName = fileInfo->d_name;
        if (fileName.size() < 4 ||
            fileName.substr(fileName.size() - 4) != ".csv") {
            continue;
        }

        std::size_t extensionPosition = fileName.find_last_of('.');
        if (extensionPosition == std::string::npos || extensionPosition == 0) {
            closedir(directory);
            error = "Nama file testing tidak memiliki label p/n: " + fileName;
            return false;
        }
        char expectedClass = fileName[extensionPosition - 1];
        if (expectedClass != 'p' && expectedClass != 'n') {
            closedir(directory);
            error = "Label file testing harus p atau n: " + fileName;
            return false;
        }

        TestFile testFile;
        testFile.path = directoryPath + "/" + fileName;
        testFile.expectedClass = expectedClass;
        testFiles.push_back(testFile);
    }
    closedir(directory);
#endif

    std::sort(testFiles.begin(), testFiles.end(),
        [](const TestFile& first, const TestFile& second) {
            return first.path < second.path;
        });
    if (testFiles.empty()) {
        error = "Tidak ada file CSV untuk dievaluasi di folder: " + directoryPath;
        return false;
    }
    return true;
}

bool readTestText(const TestFile& testFile, std::string& testText, std::string& error) {
    std::ifstream file(testFile.path.c_str());
    if (!file.is_open()) {
        error = "Gagal membuka dataset pengujian: " + testFile.path;
        return false;
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    testText = contents.str();
    if (testText.find_first_not_of(" \t\n\r") == std::string::npos) {
        error = "Dataset pengujian kosong: " + testFile.path;
        return false;
    }
    return true;
}

}

namespace ModelEvaluation {

EvaluationSummary::EvaluationSummary()
    : totalFiles(0),
      correctPredictions(0) {
}

bool evaluateDirectory(
    const std::string& directoryPath,
    ClassifierFunction classify,
    void* classifierData,
    EvaluationSummary& summary,
    std::ostream& output,
    std::string& error) {
    if (classify == NULL) {
        error = "Fungsi klasifikasi belum diberikan.";
        return false;
    }

    std::vector<TestFile> testFiles;
    if (!readTestFiles(directoryPath, testFiles, error)) {
        return false;
    }

    summary = EvaluationSummary();
    for (std::size_t i = 0; i < testFiles.size(); ++i) {
        std::string testText;
        if (!readTestText(testFiles[i], testText, error)) {
            return false;
        }

        const std::string prediction = classify(testText, classifierData);
        const char predictedClass = prediction == "politic speech" ? 'p' : 'n';
        const char expectedClass = testFiles[i].expectedClass;

        summary.totalFiles++;
        if (predictedClass == expectedClass) {
            summary.correctPredictions++;
        }

        output << testFiles[i].path << std::endl;
        output << expectedClass << " : (tebakan model: " << predictedClass
               << ") - " << (predictedClass == expectedClass ? "BENAR" : "SALAH")
               << std::endl;
    }

    const int incorrectPredictions = summary.totalFiles - summary.correctPredictions;
    const double accuracy = 100.0 * summary.correctPredictions / summary.totalFiles;
    const double overallPrecision =
        100.0 * summary.correctPredictions / summary.totalFiles;
    const double overallRecall =
        100.0 * summary.correctPredictions / summary.totalFiles;
    const double overallF1Score =
        overallPrecision + overallRecall == 0.0
        ? 0.0
        : 2.0 * overallPrecision * overallRecall /
            (overallPrecision + overallRecall);

    output << "\nAkurasi testing: " << summary.correctPredictions << "/"
           << summary.totalFiles << " (" << accuracy << "%)" << std::endl;
    output << "Prediksi benar keseluruhan: " << summary.correctPredictions << std::endl;
    output << "Prediksi salah keseluruhan: " << incorrectPredictions << std::endl;
    output << "Precision keseluruhan (micro): " << overallPrecision << "%" << std::endl;
    output << "Recall keseluruhan (micro): " << overallRecall << "%" << std::endl;
    output << "F1-score keseluruhan (micro): " << overallF1Score << "%" << std::endl;
    return true;
}

}
