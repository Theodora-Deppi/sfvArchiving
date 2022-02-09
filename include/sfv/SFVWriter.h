/** 
 *  @file   SFVWriter.h
 *  @brief  Parses a file or folder and creates a SFV file from the results
 *  @author Jonathan Mackie
 *  @date   10/10/2021
 ***********************************************/

#ifndef SFV_ARCHIVING_SFV_WRITER_H
#define SFV_ARCHIVING_SFV_WRITER_H

#include <sfv/SFVCommon.h>

class SFVWriter final : public SFV {
public:
	/**
     * \brief Constructor. Checks if the target path exists
     * \param path relative path to file or folder
     * \param final_results_only If printing results is desired
     */
    explicit SFVWriter(const std::string &path, const bool final_results_only = false) : SFV(final_results_only), m_Path(path) {
        if (!exists(m_Path)) {
            logResult(LogType::Critical, "Can't find target : " + path);
            return; // TODO add a shutdown for if it can't find the file
        }
    }

    /**
     * \brief Creates a SFV file based on the target
     */
    void process() override {
        if (!preProcess()) {return;}

        // If folder
        if (is_directory(m_Path) && !is_empty(m_Path)) {
            for (auto & entry : std::filesystem::recursive_directory_iterator(m_Path ))
            {
                if (b_Error) break;
                if (is_regular_file( entry ))
                    calculateFile(entry.path().string(), m_Path.string() );
            }
        }

        // If file
        if (is_regular_file(m_Path)) calculateFile(m_Path.string());

        if (b_Error || m_SFVLines.empty()) return;

        // Creates file name for the SFV file
        std::string pathname = m_Path.string();
        pathname.erase(pathname.find(m_Path.extension().string()), m_Path.extension().string().size());
        pathname += ".sfv";

        if (std::ofstream file(pathname); file.good() || !file.fail())
        {
            // Populates the new SFV file with our results
            for (const auto& [file_str, crc_str] : m_SFVLines) {
                file << file_str.c_str() << " " << crc_str.c_str() << std::endl;
            }
            file.close();
            logResult(LogType::Completed, "File written to " + pathname);
        }

        finishedProcessing();
    }

private:

	/**
     * \brief Calculates the hash for a file then stores in the line list
     * \param file target file
     * \param dir Currently Unused
     */
    void calculateFile(const std::string& file, const std::string& dir = "") {
	    if (const std::string crc = calculateCrc(file); crc == "error") {
            logResult(LogType::Failed, file);
            b_Error = true;
        } else {
            m_SFVLines.emplace_back(SFVLine{file, crc});
            logResult(LogType::Processed, file);
        }
    }

    struct SFVLine {
        std::string file;
        std::string crc;
    };
    std::filesystem::path m_Path;
    std::vector<SFVLine> m_SFVLines;
    bool b_Error = false;
};

#endif //SFV_ARCHIVING_SFV_WRITER_H
