/*
progress_bar.h
==============
A progress bar for the terminal.

Usage
-----
    ProgressBar bar(total);
    for (int i = 0; i < total; i++)
    {
        bar.update(i);
        // do something
    }

    // or

    ProgressBar bar(total);
    for (int i = 0; i < total; i++)
    {
        bar.increment();
        // do something
    }
*/

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

/**
 * @brief      Convert seconds to time string (hours, minutes, seconds)
 * @param[in]  seconds  The number of seconds
 * @return     The time string.
*/
std::string seconds_to_time(int seconds)
{
    if (seconds < 0)
        seconds = 0;

    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    seconds = seconds % 60;
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
}

/**
 * @brief      Class for progress bar.
*/
class ProgressBar
{
private:
    int bar_width = 70;
    int progress = 0;
    int total = 0;
    std::chrono::time_point<std::chrono::system_clock> start;

public:
    ProgressBar(int total)
    {
        this->total = total;
        this->start = std::chrono::system_clock::now();
    }

    /**
     * @brief      Update the progress bar to a specific progress
     * @param[in]  progress  The progress
    */
    void update(int progress)
    {
        this->progress = progress;
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        double elapsed = elapsed_seconds.count();
        double time_per_iteration = elapsed / progress;

        double eta = elapsed / progress * (total - progress);
        std::cerr << " |";
        int pos = bar_width * progress / total;
        for (int i = 0; i < bar_width; ++i)
        {
            if (i < pos)
                // put a full square
                std::cerr << "\u2588";
            else if (i == pos)
                // put a half square
                std::cerr << "\u258C";
            else
                std::cerr << " ";
        }
        std::cerr << "| " << progress << " / " << total; 
        std::cerr << " [" << seconds_to_time(elapsed) << " -> ";
        std::cerr << seconds_to_time(eta) << "] ";
        std::cerr << (double)((int)(time_per_iteration*100))/100.0 << "s/it \t\t\r";
        std::cerr.flush();

        if (progress == total)
        {
            std::cerr << std::endl;
        }
    }

    /**
     * @brief      Increment the progress bar by one
    */
    void increment()
    {
        update(progress + 1);
    }
};
