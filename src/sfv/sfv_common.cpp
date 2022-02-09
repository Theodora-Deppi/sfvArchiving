/** 
 *  @file   sfv_common.cpp
 *  @brief  Handles common functions used by all SFV related things
 *  @author Jonathan Mackie
 *  @date   13/11/2021
 ***********************************************/

#include <sfv/SFVCommon.h>
#include <iostream>

void SFV::logResult(const LogType log, const std::string &message) const {
    if (b_FinalResultsOnly && (log == LogType::Passed || log == LogType::Failed || log == LogType::Processed)) {return;}
    std::string full_message;
    switch (log) {
        case LogType::Passed:
            full_message = "[Passed] " + message + "\n";
            break;
        case LogType::Failed:
            full_message = "[Failed] " + message + "\n";
            break;
        case LogType::Completed:
            full_message = "[Completed] " + message + "\n";
            break;
        case LogType::CompletedPerfect:
            full_message = "[Success] All " + message + " files successfully validated"  + "\n";
            break;
        case LogType::Error:
            full_message = "[Error] " + message + "\n";
            break;
        case LogType::Critical:
            full_message = "[Critical Error] " + message + "\n";
            break;
        case LogType::Processed:
            full_message = "[Processed] " + message + "\n";
            break;
    }

    std::cout << full_message;
}

uint32_t SFV::toHex(const uint64_t num, char *s, const bool lower_alpha)
{
    uint64_t x = num;

    // use bitwise-ANDs and bit-shifts to isolate
    // each nibble into its own byte
    // also need to position relevant nibble/byte into
    // proper location for little-endian copy
    x = ((x & 0xFFFF) << 32) | ((x & 0xFFFF0000) >> 16);
    x = ((x & 0x0000FF000000FF00) >> 8) | (x & 0x000000FF000000FF) << 16;
    x = ((x & 0x00F000F000F000F0) >> 4) | (x & 0x000F000F000F000F) << 8;

    // Now isolated hex digit in each byte
    // Ex: 0x1234FACE => 0x0E0C0A0F04030201

    // Create bitmask of bytes containing alpha hex digits
    // - add 6 to each digit
    // - if the digit is a high alpha hex digit, then the addition
    //   will overflow to the high nibble of the byte
    // - shift the high nibble down to the low nibble and mask
    //   to create the relevant bitmask
    //
    // Using above example:
    // 0x0E0C0A0F04030201 + 0x0606060606060606 = 0x141210150a090807
    // >> 4 == 0x0141210150a09080 & 0x0101010101010101
    // == 0x0101010100000000
    //
    uint64_t mask = ((x + 0x0606060606060606) >> 4) & 0x0101010101010101;

    // convert to ASCII numeral characters
    x |= 0x3030303030303030;

    // if there are high hexadecimal characters, need to adjust
    // for uppercase alpha hex digits, need to add 0x07
    //   to move 0x3A-0x3F to 0x41-0x46 (A-F)
    // for lowercase alpha hex digits, need to add 0x27
    //   to move 0x3A-0x3F to 0x61-0x66 (a-f)
    // it's actually more expensive to test if mask non-null
    //   and then run the following stmt
    x += ((lower_alpha) ? 0x27 : 0x07) * mask;

    //copy string to output buffer
    *(uint64_t *)s = x;

    return 0;
}
