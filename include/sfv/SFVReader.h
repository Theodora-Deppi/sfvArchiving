/** 
 *  @file   SFVReader.h
 *  @brief  Reads a SFV and compares the results
 *  @author Jonathan Mackie
 *  @date   10/10/2021
 ***********************************************/

#ifndef SFVARCHIVING_SFV_READER_H
#define SFVARCHIVING_SFV_READER_H

#include <iostream>
#include <filesystem>
#include <fstream>
#include <future>
#include <sfv/SFVCommon.h>

class SFVReader final : public SFV {
public:
	/**
     * \brief Constructor
     * \param file relative path to file or folder
     * \param final_results_only If printing results is desired
     */
    explicit SFVReader(const std::string& file, const bool final_results_only = false) : SFV(final_results_only), m_FilePath(file) {
        if (!std::filesystem::exists(m_FilePath) || std::filesystem::is_directory(m_FilePath)) {
            logResult(LogType::Critical, "Can't find file : " + file);
            return;
        }
    }

    /**
     * \brief Processes the SFV file and tries to read the target files
     */
    void process() override {
        if (!preProcess()) {return;}

        std::string line;
        std::ifstream file;
        file.open(m_FilePath);

        if(!file.is_open()) {
            logResult(LogType::Critical, "Failed to open " + m_FilePath.string());
            return;
        }

        unsigned int lines = 0;
        // Reads each line then processes
        while(getline(file, line)) {
            readLine(line);
            lines++;
        }

        // Print results
        if (lines == m_Passed) {logResult(LogType::CompletedPerfect, std::to_string(lines));}
        else {
            logResult(LogType::Completed,"Completed with " + std::to_string(m_Passed) + " passes and " + std::to_string(m_Failed) + " fails.");
            for (const auto& s : m_FailedItemsStrings) {
                logResult(LogType::Failed, s);
            }
        }
        finishedProcessing();
    }
private:

    /**
     * \brief Processes a line from a sfv file. Files the target file then calls the CRC check
     * \param line File line as string
     */
    void readLine(std::string line) {
        if (line[0] == ';') {return;} // Ignore comments

        // Cleans up the line
        const auto file = line.substr(0, line.find(' '));
        line.erase(0, file.size()+1);

        // Pre added for when MD5 support is added
        if (m_HashType == HashType::MD5) {
            line.erase(0, 1);
        }

        // Makes the file relative path
        std::string full_file_path = m_FilePath.string();
        full_file_path.erase(full_file_path.find(m_FilePath.filename().string()), m_FilePath.filename().string().size());

        // Stores the original hash
        const std::string original_hash = line;
        full_file_path += file;

        // Stores the new hash
        const std::string hash = calculateCrc(full_file_path);

        if (hash == "openError") std::cout << "[Failed to open] " << file << "\n";

        // Compares hashes
        if (hash == original_hash) {
            // Good
            logResult(LogType::Passed, file);
            m_Passed++;
        } else {
            // Bad
            std::string message = file + " - CRC mismatch. Original: " + original_hash + " New: " + hash;
            m_FailedItemsStrings.emplace_back(message);
            logResult(LogType::Failed, message);
            m_Failed++;
        }
    }

    unsigned int m_Passed = 0;
    unsigned int m_Failed = 0;

    std::filesystem::path m_FilePath;
    std::vector<std::string> m_FailedItemsStrings;

};

#endif //SFVARCHIVING_SFV_READER_H
