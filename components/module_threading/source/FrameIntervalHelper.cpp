#include <module_threading/FrameIntervalHelper.hpp>

using namespace lib::threading;

void FrameIntervalHelper::reset() {
    _max_fps = 0;
    _min_fps = std::numeric_limits<uint16_t>::max();

    _average_fps = 0;

    _min_frame_time     = std::chrono::microseconds(0);
    _max_frame_time     = std::chrono::microseconds(0);
    _average_frame_time = std::chrono::microseconds(0);

    _last_frame_time = std::chrono::system_clock::now();
}

void FrameIntervalHelper::emplace() {
    const auto frame_start_time = std::chrono::system_clock::now();
    const auto frame_time       = std::chrono::duration_cast<std::chrono::microseconds>(
        frame_start_time - _last_frame_time
    );

    _last_frame_time = frame_start_time;
    emplace(frame_time);
}

void FrameIntervalHelper::emplace(std::chrono::microseconds frame_time) {
    _current_fps = static_cast<uint16_t>(1000.f / (static_cast<float>(frame_time.count()) / 1000.f));

    _max_fps = std::max(_max_fps, _current_fps);
    _min_fps = std::min(_min_fps, _current_fps);

    _average_fps = (_average_fps + _current_fps) / 2;

    _max_frame_time = std::max(_max_frame_time, frame_time);
    _min_frame_time = std::min(_min_frame_time, frame_time);

    _average_frame_time = (_average_frame_time + frame_time) / 2;
}

uint16_t FrameIntervalHelper::get_max_fps() const {
    return _max_fps;
}

uint16_t FrameIntervalHelper::get_min_fps() const {
    return _min_fps;
}

uint16_t FrameIntervalHelper::get_average_fps() const {
    return _average_fps;
}

uint16_t FrameIntervalHelper::get_current_fps() const {
    return _current_fps;
}

std::chrono::microseconds FrameIntervalHelper::get_max_frame_time() const {
    return _max_frame_time;
}

std::chrono::microseconds FrameIntervalHelper::get_min_frame_time() const {
    return _min_frame_time;
}

std::chrono::microseconds FrameIntervalHelper::get_average_frame_time() const {
    return _average_frame_time;
}
