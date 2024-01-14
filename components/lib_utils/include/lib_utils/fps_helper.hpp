#pragma once

#include <cstdint>
#include <chrono>

namespace lib::utils
{
class fps_helper
{
public:
    //! Reset all values
    void reset();

    //! Update fps and frametime metrics using time between calls to this function
    void update();

    //! Update fps and frametime metrics using \param frametime in ms
    void update(float frametime);

    [[nodiscard]] uint16_t get_max_fps() const;
    [[nodiscard]] uint16_t get_min_fps() const;
	[[nodiscard]] uint16_t get_average_fps() const;
	[[nodiscard]] uint16_t get_current_fps() const;

	[[nodiscard]] float get_max_frametime_ms() const;
	[[nodiscard]] float get_min_frametime_ms() const;
	[[nodiscard]] float get_average_frametime_ms() const;

private:
    uint16_t _max_fps = 0;
    uint16_t _min_fps = UINT16_MAX;
    uint16_t _average_fps = 0;
	uint16_t _current_fps = 0;

    float _min_frametime = 0.f;
    float _max_frametime = 0.f;
    float _average_frametime = 0.f;

	std::chrono::system_clock::time_point _last_frame_time = std::chrono::system_clock::now();
};
}
