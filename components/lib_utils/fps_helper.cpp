#include <lib_utils/fps_helper.hpp>

#include <chrono>
#include <core_sdk/logger.hpp>

using namespace lib::utils;

void fps_helper::reset()
{
	_max_fps = 0;
	_min_fps = UINT16_MAX;
	_average_fps = 0;

	_min_frametime = 0.f;
	_max_frametime = 0.f;
	_average_frametime = 0.f;

	_last_frame_time = std::chrono::system_clock::now();
}

void fps_helper::update()
{
	const auto frame_start_time = std::chrono::system_clock::now();
	const auto frametime = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(
		frame_start_time - _last_frame_time).count()) / 1000.f;

	_last_frame_time = frame_start_time;

	update(frametime);
}

void fps_helper::update(float frametime)
{
	_current_fps = static_cast<uint16_t>(1000.f / frametime);

	_max_fps = std::max(_max_fps, _current_fps);
	_min_fps = std::min(_min_fps, _current_fps);

	_average_fps = (_average_fps + _current_fps) / 2;

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

uint16_t fps_helper::get_current_fps() const
{
	return _current_fps;
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