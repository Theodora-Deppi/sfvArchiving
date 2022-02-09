/** 
 *  @file   Timer.h
 *  @brief  Used for logging the time taken to complete a process
 *  @author Jonathan Mackie
 *  @date   16/11/2021
 ***********************************************/

#ifndef SFVARCHIVING_TIMER_H
#define SFVARCHIVING_TIMER_H

class Timer {
public:
    Timer() = default;
    Timer(const Timer&) = delete; // Block all copies and moves
    Timer(Timer&&) = delete;
    Timer& operator= ( const Timer & ) = delete;
    Timer& operator= ( Timer && ) = delete;
    ~Timer() = default;

	/**
     * \brief Starts the timer
     */
    void start() {m_StartTime = std::chrono::system_clock::now();}

    /**
     * \brief Stops the timer
     */
    void stop() {m_EndTime = std::chrono::system_clock::now();}

    /**
     * \brief Stops the timer and prints the results
     */
    void stopAndPrint() {
        stop();
        print();
    }

    /**
     * \brief Prints the final timer time in seconds and milliseconds
     */
    void print() {
        std::cout << "[Finished] Time taken: " << seconds() << " seconds (" << milliseconds() << " milliseconds)" << std::endl;
    }

    /**
     * \brief Gets the timer time as milliseconds
     * \return timer timer in milliseconds as unsigned int
     */
    unsigned int milliseconds() {
        if(m_Milliseconds == 0) m_Milliseconds = static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count()); // We only get the count once. Then store it to stop possible multiple calls
        return m_Milliseconds;
    }

    /**
     * \brief Gets the timer time as seconds
     * \return timer timer in seconds as float
     */
    float seconds() {return static_cast<float>(milliseconds()) / 1000;}

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime{};
    std::chrono::time_point<std::chrono::system_clock> m_EndTime{};
    unsigned int m_Milliseconds = 0;
};

#endif //SFVARCHIVING_TIMER_H
