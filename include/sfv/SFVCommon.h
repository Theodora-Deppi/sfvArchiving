/** 
 *  @file   SFVCommon.h
 *  @brief  Base class used by the reader and writer
 *  @author Jonathan Mackie
 *  @date   10/10/2021
 ***********************************************/

#ifndef SFVARCHIVING_SFV_COMMON_H
#define SFVARCHIVING_SFV_COMMON_H
#include <cstdint>
#include <string>

class SFV {
    // options
    bool b_FinalResultsOnly;
public:
    explicit SFV(const bool final_results_only) {
        b_FinalResultsOnly = final_results_only;
        m_HashType = HashType::CRC; // Defaults to crc
    }
    SFV(const SFV&) = delete; // Block all copies and moves
    SFV(SFV&&) = delete;
    SFV& operator= ( const SFV & ) = delete;
    SFV& operator= ( SFV && ) = delete;

    virtual void process() = 0;

    /**
     * \brief Sets the amount of threads allowed to be used by the hashing async
     * \param count Target thread count. Zero equals max possible
     */
    void setThreadCount(const unsigned int count) {
        if (b_HasProcessed) {logResult(LogType::Critical, "You can't set the thread count after it's processed");}
        m_Threads = count;
    }

    virtual ~SFV() = default;
protected:

    /**
     * \brief Checks if the SFV module can run
     * \return If the module can run
     */
    [[nodiscard]] bool preProcess() const {
        return b_CanRun;
    }

    enum class LogType{
        Passed = 0x00,
        Failed = 0x01,
        Completed = 0x02,
        CompletedPerfect = 0x03,
        Error = 0x04,
        Critical = 0x05,
        Processed = 0x06
    };

    enum class HashType{CRC,MD5}; // TODO add MD5 support
    /**
     * \brief Logs the string in a consistent format
     * \param log Message type
     * \param message Message as string
     */
    void logResult(LogType log, const std::string& message) const;

    /**
     * \brief Calculates the CRC of a file
     * \param file_path target file
     * \return Hash as string
     * \note The function will return a error message as string upon failure. (e.g "openError" or "sizeError")
     */
    [[nodiscard]] std::string calculateCrc(const std::string& file_path) const;

    /**
     * \brief Marks the processing has completed
     */
    void finishedProcessing() {
        b_HasProcessed = true;
    }
    HashType m_HashType;
private:
    bool b_HasProcessed = false;
    bool b_CanRun = true;
    unsigned int m_Threads = 0;
    /**
     * \brief Generates a CRC hash based on data
     * \param data File File data as char* 
     * \param num_bytes Number of bytes left
     * \param max_block_size Max target block size
     * \return CRC as unsigned int
     * \note Simply put. This function is self recursive. \n It calculates what if should process, hands the rest of the data to a another async-ed version of itself. \n Then processes a chunk of the data. Combining it with the response from the async-ed version.
     */
    static unsigned int asyncChunkCrc(const char* data, unsigned int num_bytes, unsigned int max_block_size);
    static constexpr size_t buffer_size = 20*4096;
    /**
     * \brief If the file size is over 3.899999894 GB we change the file in chunks
     */
    static constexpr unsigned long long int file_limit = 4187593000;  // 3.899999894 gb
    /**
     * \brief Opens a file in memory map and processes the chunk
     * \param file_path Target File
     * \param offset File offset
     * \param num_bytes Number of bytes left
     * \param max_block_size Max target block size
     * \return CRC as unsigned int
     * \note Edited version of asyncChunkCrc just for memory mapped files
     */
    static unsigned int asyncChunkCrcMmap(const std::string& file_path, unsigned long long int offset, unsigned long long int num_bytes, unsigned int max_block_size);

    // https://johnnylee-sde.github.io/Fast-unsigned-integer-to-hex-string/
    static uint32_t toHex(uint64_t num, char *s, bool lower_alpha);

};

#endif //SFVARCHIVING_SFV_COMMON_H
