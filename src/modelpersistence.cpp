#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <cerrno>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
#include "../include/modelpersistence.h"

namespace {

const char modelMagic[8] = {'N', 'B', 'M', 'O', 'D', 'E', 'L', '\0'};
const std::uint32_t currentFormatVersion = 1;
const std::uint32_t maximumEntryCount = 1000000;
const std::uint32_t maximumStringLength = 1024 * 1024;

bool makeDirectory(const std::string& directoryPath) {
#ifdef _WIN32
    if (_mkdir(directoryPath.c_str()) != 0 && errno != EEXIST) {
        return false;
    }
    struct _stat directoryInfo;
    return _stat(directoryPath.c_str(), &directoryInfo) == 0 &&
        (directoryInfo.st_mode & _S_IFDIR) != 0;
#else
    if (mkdir(directoryPath.c_str(), 0777) != 0 && errno != EEXIST) {
        return false;
    }
    struct stat directoryInfo;
    return stat(directoryPath.c_str(), &directoryInfo) == 0 &&
        S_ISDIR(directoryInfo.st_mode);
#endif
}

bool makeParentDirectory(const std::string& path, std::string& error) {
    const std::size_t separator = path.find_last_of("/\\");
    if (separator == std::string::npos) {
        return true;
    }

    const std::string directoryPath = path.substr(0, separator);
    if (directoryPath.empty() || makeDirectory(directoryPath)) {
        return true;
    }

    error = "Gagal membuat folder model: " + directoryPath;
    return false;
}

bool writeByte(std::ostream& output, std::uint8_t value) {
    output.put(static_cast<char>(value));
    return static_cast<bool>(output);
}

bool readByte(std::istream& input, std::uint8_t& value) {
    char byte;

    if (!input.get(byte)) {
        return false;
    }

    value = static_cast<std::uint8_t>(static_cast<unsigned char>(byte));

    return true;
}
bool writeUint32(std::ostream& output, std::uint32_t value) {
    for (unsigned int shift = 0; shift < 32; shift += 8) {
        if (!writeByte(output, static_cast<std::uint8_t>(value >> shift))) {
            return false;
        }
    }

    return true;
}

bool readUint32(std::istream& input, std::uint32_t& value) {
    value = 0;
    for (unsigned int shift = 0; shift < 32; shift += 8) {
        std::uint8_t byte;
        if (!readByte(input, byte)) {
            return false;
        }
        value |= static_cast<std::uint32_t>(byte) << shift;
    }

    return true;
}

bool writeUint64(std::ostream& output, std::uint64_t value) {
 
    for (unsigned int shift = 0; shift < 64; shift += 8) {

        if (!writeByte(output, static_cast<std::uint8_t>(value >> shift))) {
            return false;
        }
    }

    return true;
}

bool readUint64(std::istream& input, std::uint64_t& value) {
    value = 0;
    for (unsigned int shift = 0; shift < 64; shift += 8) {
        std::uint8_t byte;
        if (!readByte(input, byte)) {
            return false;
        }

        value |= static_cast<std::uint64_t>(byte) << shift;
    }

    return true;
}


bool writeString(std::ostream& output, const std::string& value) {

    //panjang teks ikut disimpan sebelum isi katanya
    if (value.size() > maximumStringLength ||
        !writeUint32(output, static_cast<std::uint32_t>(value.size()))) {
        return false;
    }

    output.write(value.data(), static_cast<std::streamsize>(value.size()));

    return static_cast<bool>(output);
}


bool readString(std::istream& input, std::string& value) {
    std::uint32_t length;

    if (!readUint32(input, length) || length > maximumStringLength) {
        return false;
    }

    value.resize(length);
    if (length > 0) {
        input.read(&value[0], static_cast<std::streamsize>(length));
    }

    return static_cast<bool>(input);
}
bool writeWordCounts(
    std::ostream& output,
    const std::map<std::string, int>& wordCounts) {
    if (wordCounts.size() > maximumEntryCount ||
        !writeUint32(output, static_cast<std::uint32_t>(wordCounts.size()))) {
        return false;
    }

    for (std::map<std::string, int>::const_iterator entry = wordCounts.begin();
         entry != wordCounts.end(); ++entry) {
        if (entry->second <= 0 || !writeString(output, entry->first) ||
            !writeUint64(output, static_cast<std::uint64_t>(entry->second))) {
            return false;
        }
    }

    return true;
}

bool readWordCounts(
    std::istream& input,
    std::map<std::string, int>& wordCounts) {
    std::uint32_t entryCount;

    
    if (!readUint32(input, entryCount) || entryCount > maximumEntryCount) {
        return false;
    }

    for (std::uint32_t i = 0; i < entryCount; ++i) {
        std::string word;
        std::uint64_t count;

        if (!readString(input, word) || !readUint64(input, count) ||
            count == 0 ||
            count > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
            !wordCounts.insert(std::make_pair(word, static_cast<int>(count))).second) {
            return false;
        }
    }

    return true;
}


bool writeVocabulary(
    std::ostream& output,
    const std::map<std::string, bool>& vocabulary) {


    if (vocabulary.size() > maximumEntryCount ||
        !writeUint32(output, static_cast<std::uint32_t>(vocabulary.size()))) {
        return false;
    }

    for (std::map<std::string, bool>::const_iterator entry = vocabulary.begin();
         entry != vocabulary.end(); ++entry) {
        if (!entry->second || !writeString(output, entry->first) ||
            !writeByte(output, 1)) {
            return false;
        }
    }

    return true;
}

bool readVocabulary(
    std::istream& input,
    std::map<std::string, bool>& vocabulary) {
    std::uint32_t entryCount;
    if (!readUint32(input, entryCount) || entryCount > maximumEntryCount) {
        return false;
    }
    for (std::uint32_t i = 0; i < entryCount; ++i) {
        std::string word;
        std::uint8_t present;

        if (!readString(input, word) || !readByte(input, present) ||
            present != 1 ||
            !vocabulary.insert(std::make_pair(word, true)).second) {
            return false;
        }
    }

    return true;
}


bool validateModel(
    const std::map<std::string, int>& politicalSpeechWordCounts,
    const std::map<std::string, int>& nonPoliticalSpeechWordCounts,
    const std::map<std::string, bool>& uniqueVocabulary,
    int totalPoliticalSpeeches,
    int totalNonPoliticalSpeeches,
    int totalPoliticalSpeechWords,
    int totalNonPoliticalSpeechWords,
    double smoothingAlpha) {

    //cek nilai hitungan dan smoothing sebelum model dipakai
    if (totalPoliticalSpeeches < 0 || totalNonPoliticalSpeeches < 0 ||
        totalPoliticalSpeechWords < 0 || totalNonPoliticalSpeechWords < 0 ||
        !std::isfinite(smoothingAlpha) || smoothingAlpha <= 0.0) {
        return false;
    }

    std::uint64_t politicalWordTotal = 0;
    std::uint64_t nonPoliticalWordTotal = 0;

    //jumlah hitungan kata harus sama dengan total yang dicatat
    for (std::map<std::string, int>::const_iterator entry = politicalSpeechWordCounts.begin();
         entry != politicalSpeechWordCounts.end(); ++entry) {

        if (entry->second <= 0 || uniqueVocabulary.count(entry->first) == 0) {
            return false;
        }
        politicalWordTotal += static_cast<std::uint64_t>(entry->second);
    }

    for (std::map<std::string, int>::const_iterator entry = nonPoliticalSpeechWordCounts.begin();
         entry != nonPoliticalSpeechWordCounts.end(); ++entry) {


        if (entry->second <= 0 || uniqueVocabulary.count(entry->first) == 0) {
            return false;
        }

        nonPoliticalWordTotal += static_cast<std::uint64_t>(entry->second);
    }

    if (politicalWordTotal != static_cast<std::uint64_t>(totalPoliticalSpeechWords) ||
        nonPoliticalWordTotal != static_cast<std::uint64_t>(totalNonPoliticalSpeechWords)) {
        return false;
    }

    //pastikan semua kata yang disimpan masuk ke kosakata
    for (std::map<std::string, bool>::const_iterator entry = uniqueVocabulary.begin();
         entry != uniqueVocabulary.end(); ++entry) {

        if (!entry->second ||
            (politicalSpeechWordCounts.count(entry->first) == 0 &&
             nonPoliticalSpeechWordCounts.count(entry->first) == 0)) {
            return false;
        }
    }

    return true;
}

}

PreprocessingConfig::PreprocessingConfig()
    : stripEdgePunctuation(true), lowercaseTokens(false) {
}

namespace ModelPersistence {

//fungsi menyimpan data model naivebayes ke binary
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
    std::string& error) {

    if (!validateModel(
            politicalSpeechWordCounts, nonPoliticalSpeechWordCounts, uniqueVocabulary,
            totalPoliticalSpeeches, totalNonPoliticalSpeeches,
            totalPoliticalSpeechWords, totalNonPoliticalSpeechWords, smoothingAlpha)) {

        error = "Data model tidak konsisten atau nilai smoothing tidak valid.";
        return false;
    }

    if (!makeParentDirectory(path, error)) {
        return false;
    }

    std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);

    if (!output.is_open()) {
        error = "Gagal membuka file model untuk ditulis: " + path;
        return false;
    }

    //double disalin dulu ke bentuk angka agar bisa masuk ke file binary
    std::uint64_t alphaBits;
    std::memcpy(&alphaBits, &smoothingAlpha, sizeof(alphaBits));

    const std::uint8_t preprocessingFlags =
        (preprocessingConfig.stripEdgePunctuation ? 1 : 0) |
        (preprocessingConfig.lowercaseTokens ? 2 : 0);

    //bagian awal file berisi penanda format dan nilai-nilai model
    output.write(modelMagic, sizeof(modelMagic));

    const bool written =
        static_cast<bool>(output) &&
        writeUint32(output, currentFormatVersion) &&
        writeUint64(output, static_cast<std::uint64_t>(totalPoliticalSpeeches)) &&
        writeUint64(output, static_cast<std::uint64_t>(totalNonPoliticalSpeeches)) &&
        writeUint64(output, static_cast<std::uint64_t>(totalPoliticalSpeechWords)) &&
        writeUint64(output, static_cast<std::uint64_t>(totalNonPoliticalSpeechWords)) &&
        writeUint64(output, alphaBits) &&
        writeByte(output, preprocessingFlags) &&
        writeWordCounts(output, politicalSpeechWordCounts) &&
        writeWordCounts(output, nonPoliticalSpeechWordCounts) &&
        writeVocabulary(output, uniqueVocabulary);

    output.flush();

    if (!written || !output) {
        error = "Gagal menulis file model: " + path;
        return false;
    }

    return true;
}


//fungsi untuk membaca lagi model naivebayes dari binary
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
    std::string& error) {         

    //buka file model sebagai binary
    std::ifstream input(path.c_str(), std::ios::binary);
    if (!input.is_open()) {
        error = "Gagal membuka file model: " + path;
        return false;
    }

    //baca penanda file, kalau beda berarti bukan format model ini
    char magic[sizeof(modelMagic)];
    input.read(magic, sizeof(magic));

    if (!input || std::memcmp(magic, modelMagic, sizeof(modelMagic)) != 0) {
        error = "Format file model tidak valid.";
        return false;
    }

    std::uint32_t formatVersion;
    std::uint64_t politicalDocuments;
    std::uint64_t nonPoliticalDocuments;
    std::uint64_t politicalWords;
    std::uint64_t nonPoliticalWords;
    std::uint64_t alphaBits;
    std::uint8_t preprocessingFlags;

    //ambil versi file dan jumlah-jumlah yang diperlukan
    if (!readUint32(input, formatVersion) ||
        formatVersion != currentFormatVersion ||
        !readUint64(input, politicalDocuments) ||
        !readUint64(input, nonPoliticalDocuments) ||
        !readUint64(input, politicalWords) ||
        !readUint64(input, nonPoliticalWords) ||
        !readUint64(input, alphaBits) ||
        !readByte(input, preprocessingFlags)) {

        error = "Versi atau metadata file model rusak.";
        return false;
    }

    //hindari mengubah angka besar menjadi int kalau tidak muat
    if (politicalDocuments > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
        nonPoliticalDocuments > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
        politicalWords > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
        nonPoliticalWords > static_cast<std::uint64_t>(std::numeric_limits<int>::max()) ||
        (preprocessingFlags & ~static_cast<std::uint8_t>(3)) != 0) {

        error = "Metadata file model berada di luar rentang yang valid.";
        return false;
    }

    double loadedSmoothingAlpha;
    std::memcpy(&loadedSmoothingAlpha, &alphaBits, sizeof(loadedSmoothingAlpha));
    PreprocessingConfig loadedPreprocessingConfig;
    loadedPreprocessingConfig.stripEdgePunctuation = (preprocessingFlags & 1) != 0;
    loadedPreprocessingConfig.lowercaseTokens = (preprocessingFlags & 2) != 0;

    std::map<std::string, int> loadedPoliticalWordCounts;
    std::map<std::string, int> loadedNonPoliticalWordCounts;
    std::map<std::string, bool> loadedVocabulary;

    if (!readWordCounts(input, loadedPoliticalWordCounts) ||
        !readWordCounts(input, loadedNonPoliticalWordCounts) ||
        !readVocabulary(input, loadedVocabulary) ||
        input.peek() != std::char_traits<char>::eof()) {

        error = "Isi file model rusak atau memiliki data tambahan yang tidak dikenal.";
        return false;
    }

    //ubah angka yang sudah dicek tadi ke int
    const int loadedPoliticalDocuments = static_cast<int>(politicalDocuments);
    const int loadedNonPoliticalDocuments = static_cast<int>(nonPoliticalDocuments);
    const int loadedPoliticalWords = static_cast<int>(politicalWords);
    const int loadedNonPoliticalWords = static_cast<int>(nonPoliticalWords);

    if (!validateModel(
            loadedPoliticalWordCounts, loadedNonPoliticalWordCounts, loadedVocabulary,
            loadedPoliticalDocuments, loadedNonPoliticalDocuments,
            loadedPoliticalWords, loadedNonPoliticalWords, loadedSmoothingAlpha)) {

        error = "Isi file model tidak konsisten atau nilai smoothing tidak valid.";
        return false;
    }

    //baru pindahkan ke model utama kalau semua pengecekan berhasil
    politicalSpeechWordCounts.swap(loadedPoliticalWordCounts);
    nonPoliticalSpeechWordCounts.swap(loadedNonPoliticalWordCounts);
    uniqueVocabulary.swap(loadedVocabulary);
    totalPoliticalSpeeches = loadedPoliticalDocuments;
    totalNonPoliticalSpeeches = loadedNonPoliticalDocuments;
    totalPoliticalSpeechWords = loadedPoliticalWords;
    totalNonPoliticalSpeechWords = loadedNonPoliticalWords;
    smoothingAlpha = loadedSmoothingAlpha;
    preprocessingConfig = loadedPreprocessingConfig;

    return true;
}

}
