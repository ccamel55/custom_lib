#pragma once

#include <cstdint>
#include <chrono>

namespace lib::utils
{
class fps_helper
{
public:
	explicit fps_helper(std::chrono::milliseconds reset_interval = std::chrono::milliseconds(0));

    //! Reset all values
    void reset();

    //! Update fps and frametime metrics using time between calls to this function
    void update();

    //! Update fps and frametime metrics using \param frametime in ms
    void update(float frametime);

    [[nodiscard]] uint16_t get_max_fps() const;
    [[nodiscard]] uint16_t get_min_fps() const;
	[[nodiscard]] uint16_t get_average_fps() const;

	[[nodiscard]] float get_max_frametime_ms() const;
	[[nodiscard]] float get_min_frametime_ms() const;
	[[nodiscard]] float get_average_frametime_ms() const;

private:
    uint16_t _max_fps = 0;
    uint16_t _min_fps = UINT16_MAX;
    uint16_t _average_fps = 0;

    float _min_frametime = 0.f;
    float _max_frametime = 0.f;
    float _average_frametime = 0.f;

	std::chrono::milliseconds _reset_interval = std::chrono::milliseconds(0);

	std::chrono::high_resolution_clock::time_point _last_reset = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point _last_frame_time = std::chrono::high_resolution_clock::now();
};
}
