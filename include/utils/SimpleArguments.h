/** 
 *  @file   SimpleArguments.h
 *  @brief  Very Simple CMD argument handler
 *  @author Jonathan Mackie
 *  @date   10/10/2021
 ***********************************************/

#ifndef SFVARCHIVING_SIMPLEARGUMENTS_H
#define SFVARCHIVING_SIMPLEARGUMENTS_H

#include <vector>

class SimpleArguments {
public:
	/**
     * \brief Parses the command line arguments into a easy to use system
     * \param argc Argument count (Ideally from int main)
     * \param argv Arguments array (Ideally from int main)
     */
    SimpleArguments(const int argc, char* argv[]) : m_Arguments(std::vector<std::string>(argv + 1, argv + argc)) {}
    SimpleArguments(const SimpleArguments&) = delete; // Block all copies and moves
    SimpleArguments(SimpleArguments&&) = delete;
    SimpleArguments& operator= ( const SimpleArguments & ) = delete;
    SimpleArguments& operator= ( SimpleArguments && ) = delete;
    ~SimpleArguments() {m_Arguments.clear();}

	/**
     * \brief Checks if a possible command line argument exists
     * \param target Argument string in literal format (e.g --readSFV not readSFV)
     * \return bool True if found. False if not found.
     */
    bool find(const std::string& target) {
	    const auto result = std::find(m_Arguments.begin(), m_Arguments.end(), target);
        return result != m_Arguments.end();
    }

	/**
     * \brief Finds the argument then the string after. Useful for path input
     * \param target Argument string in literal format (e.g --readSFV not readSFV)
     * \note Please use find() to check if it exists first
     * \return string found after argument
     */
    std::string findAfter(const std::string& target) {
	    const auto result = std::find(m_Arguments.begin(), m_Arguments.end(), target);
	    const int index = static_cast<int>(std::distance(m_Arguments.begin(), result));
        return m_Arguments.at(static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(index)+1);
    }

	/**
     * \brief Gets the first argument in the list
     * \return string of first argument
     */
    std::string getFirst() {
        return m_Arguments.at(0);
    }

	/**
     * \brief Gets the amount of arguments in the list
     * \return int of argument count
     */
    [[nodiscard]] int count() const {
        return static_cast<int>(m_Arguments.size());
    }

	/**
     * \brief Prints all arguments inputted. Only to be used for debugging
     */
    void print() {
        for (const auto& s: m_Arguments) std::cout << s << std::endl;
    }

private:
    std::vector<std::string> m_Arguments;
};

#endif //SFVARCHIVING_SIMPLEARGUMENTS_H
