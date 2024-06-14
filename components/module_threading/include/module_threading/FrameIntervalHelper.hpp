#pragma once

#include <chrono>
#include <limits>

namespace lib::threading {
class FrameIntervalHelper {
public:
    //! Reset all values
    void reset();

    //! Update fps and frame time metrics using time between calls to this function
    void emplace();

    //! Update fps and frame time metrics using
    //! \param frame_time Frame time in microseconds.
    void emplace(std::chrono::microseconds frame_time);

    [[nodiscard]] uint16_t get_max_fps() const;
    [[nodiscard]] uint16_t get_min_fps() const;
    [[nodiscard]] uint16_t get_average_fps() const;
    [[nodiscard]] uint16_t get_current_fps() const;

    [[nodiscard]] std::chrono::microseconds get_max_frame_time() const;
    [[nodiscard]] std::chrono::microseconds get_min_frame_time() const;
    [[nodiscard]] std::chrono::microseconds get_average_frame_time() const;

private:
    uint16_t _max_fps = 0;
    uint16_t _min_fps = std::numeric_limits<uint16_t>::max();

    uint16_t _average_fps = 0;
    uint16_t _current_fps = 0;

    std::chrono::microseconds _min_frame_time     = { };
    std::chrono::microseconds _max_frame_time     = { };
    std::chrono::microseconds _average_frame_time = { };

    std::chrono::system_clock::time_point _last_frame_time = std::chrono::system_clock::now();

};
}
