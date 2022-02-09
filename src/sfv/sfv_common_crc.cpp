/** 
 *  @file   sfv_common_crc.cpp
 *  @brief  Handles all CRC related functions
 *  @author Jonathan Mackie
 *  @date   13/11/2021
 ***********************************************/

#include <sfv/SFVCommon.h>
#include <filesystem>
#include <sstream>
#include <crc/Crc32.h>
#include <thread>
#include <future>
#include <mio/mio.hpp>

std::string SFV::calculateCrc(const std::string &file_path) const
{
    unsigned int crc{ 0x0 };
    if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path)) {
        logResult(LogType::Critical, file_path + "File doesn't exist or is not a regular file");
        return "openError";
    }
    std::error_code file_error_code;
    const unsigned long long int file_size = std::filesystem::file_size(file_path, file_error_code);
    if (file_error_code) {
        logResult(LogType::Critical, "Can't get file size of " + file_path);
        return "sizeError";
    }

    if (m_Threads == 1) { // NON MT
        // Calculate chunks
        unsigned int total_chunks = static_cast<unsigned>(file_size) / buffer_size;
        unsigned int last_chunk_size = file_size % buffer_size;
        last_chunk_size != 0 ? ++total_chunks : (last_chunk_size = buffer_size);

        for (unsigned int chunk = 0; chunk < total_chunks; ++chunk)
        {
            unsigned int this_chunk_size;
            if (chunk == total_chunks - 1) { this_chunk_size = last_chunk_size;}
            else { this_chunk_size = buffer_size;}
            const unsigned int offset = chunk * buffer_size;

            std::error_code error; mio::mmap_source mmap;
            mmap.map(file_path, offset, this_chunk_size, error);
            if (error) { throw std::exception("mmap failed to map"); }
            crc = crc32_16bytes_prefetch( mmap.data(), this_chunk_size, crc );
        }
    } else { // MT
        unsigned int numThreads = m_Threads;
        if (m_Threads == 0) {
            numThreads = std::thread::hardware_concurrency();
        }
        const unsigned int default_blocksize = static_cast<unsigned>(file_size + numThreads - 1) / numThreads;

        if (file_size > file_limit) {
            // if (default_blocksize > file_limit) { default_blocksize = file_limit; } // Might work :/ UPDATE TODO it doesn't work
            // Calculate our crc
            crc = asyncChunkCrcMmap(file_path, 0, file_size, default_blocksize);
        } else {
            std::error_code error; mio::mmap_source mmap;
            mmap.map(file_path, 0, mio::map_entire_file, error);
            if (error) { throw std::exception("mmap failed to map"); }

            // Calculate our crc
            crc = asyncChunkCrc(mmap.data(), static_cast<unsigned>(file_size), default_blocksize);
        }
    }

    // Convert unsigned int to hex string logic
    char *hex = new char[8];
    toHex(crc,hex,false);
    std::string str_hex{hex, 8};
    delete[] hex;
    return str_hex;
}

unsigned int SFV::asyncChunkCrc(const char *data, unsigned int numBytes, unsigned int maxBlockSize)  {
    std::stringstream str_strm(data);
    // last block ?
    if (numBytes <= maxBlockSize)
        return crc32_16bytes(data, numBytes, 0); // we're done

    // compute CRC of the remaining bytes in a separate thread
    auto data_left  = data + maxBlockSize;
    auto bytes_left =          numBytes - maxBlockSize;
    auto remainder = std::async(std::launch::async, asyncChunkCrc, data_left, bytes_left, maxBlockSize);

    // compute CRC of the current block
    const auto current_crc   = crc32_16bytes(data, maxBlockSize, 0);
    // get CRC of the remainder
    const auto remainder_crc = remainder.get();
    // and merge both
    return crc32_combine(current_crc, remainder_crc, bytes_left);
}

unsigned SFV::asyncChunkCrcMmap(const std::string& file_path, const unsigned long long offset, const unsigned long long num_bytes,
	unsigned max_block_size)
{
    std::error_code error; mio::mmap_source mmap;
    mmap.map(file_path, static_cast<unsigned>(offset), max_block_size, error);
    if (error) { throw std::exception("mmap failed to map"); }

    std::stringstream str_data(mmap.data());
    // last block ?
    if (num_bytes <= max_block_size)
        return crc32_16bytes(mmap.data(), static_cast<unsigned>(num_bytes), 0); // we're done

    // compute CRC of the remaining bytes in a separate thread
    auto bytes_left =          num_bytes - max_block_size;
    auto remainder = std::async(std::launch::async, asyncChunkCrcMmap, file_path, (offset + max_block_size), bytes_left, max_block_size);

    // compute CRC of the current block
    const auto current_crc   = crc32_16bytes(mmap.data(), max_block_size, 0);
    // get CRC of the remainder
    const auto remainder_crc = remainder.get();
    // and merge both
    return crc32_combine(current_crc, remainder_crc, static_cast<unsigned>(bytes_left));
}




