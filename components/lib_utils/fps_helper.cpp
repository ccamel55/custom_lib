#include <lib_utils/fps_helper.hpp>

#include <chrono>
#include <core_sdk/logger.hpp>

using namespace lib::utils;

fps_helper::fps_helper(std::chrono::milliseconds reset_interval)
	: _reset_interval(reset_interval)
{
	reset();
}

void fps_helper::reset()
{
	_max_fps = 0;
	_min_fps = UINT16_MAX;
	_average_fps = 0;

	_min_frametime = 0.f;
	_max_frametime = 0.f;
	_average_frametime = 0.f;

	_last_reset = std::chrono::high_resolution_clock::now();
	_last_frame_time = std::chrono::high_resolution_clock::now();
}

void fps_helper::update()
{
	const auto frametime = std::chrono::high_resolution_clock::now() - _last_frame_time;
	_last_frame_time = std::chrono::high_resolution_clock::now();

	update(static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(frametime).count()));
}

void fps_helper::update(float frametime)
{
	if (_reset_interval > std::chrono::milliseconds(0))
	{
		if (const auto time_since_reset = std::chrono::high_resolution_clock::now() - _last_reset;
			time_since_reset > _reset_interval)
		{
			reset();
		}
	}

	const auto current_fps = static_cast<uint16_t>(1000.f / frametime);

	_max_fps = std::max(_max_fps, current_fps);
	_min_fps = std::min(_min_fps, current_fps);

	_average_fps = (_average_fps + current_fps) / 2;

	_max_frametime = std::max(_max_frametime, frametime);
	_min_frametime = std::min(_min_frametime, frametime);

	_average_frametime = (_average_frametime + frametime) / 2;
}

uint16_t fps_helper::get_max_fps() const
{
	return _max_fps;
}

uint16_t fps_helper::get_min_fps() const
{
	return _min_fps;
}

uint16_t fps_helper::get_average_fps() const
{
	return _average_fps;
}

float fps_helper::get_max_frametime_ms() const
{
	return _max_frametime;
}

float fps_helper::get_min_frametime_ms() const
{
	return _min_frametime;
}

float fps_helper::get_average_frametime_ms() const
{
	return _average_frametime;
}