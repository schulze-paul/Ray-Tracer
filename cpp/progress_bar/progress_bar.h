#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

std::string seconds_to_time(int seconds)
{
    if (seconds < 0)
        seconds = 0;

    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    seconds = seconds % 60;
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
}

class ProgressBar
{
private:
    int barWidth = 70;
    int progress = 0;
    int total = 0;
    std::chrono::time_point<std::chrono::system_clock> start;

public:
    ProgressBar(int total)
    {
        this->total = total;
        this->start = std::chrono::system_clock::now();
    }
    void update(int progress)
    {
        this->progress = progress;
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        double elapsed = elapsed_seconds.count();
        double eta = elapsed / progress * (total - progress);
        std::cerr << "[";
        int pos = barWidth * progress / total;
        for (int i = 0; i < barWidth; ++i)
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
        std::cerr << "] " << int(progress * 100.0 / total) << " %\t";
        std::cerr << "Elapsed: " << seconds_to_time(elapsed) << " \t";
        std::cerr << "ETA: " << seconds_to_time(eta) << " \r";
        std::cerr.flush();
    }
    void increment()
    {
        update(progress + 1);
    }
};
