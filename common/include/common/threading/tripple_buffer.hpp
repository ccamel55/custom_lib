#pragma once

#include <atomic>

namespace lib::common
{
//! Copy and lock free triple buffer.
template <class t> class triple_buffer
{
  public:
	triple_buffer(t &read_buffer, t &write_buffer, t &ready_buffer) :
		_reading(&read_buffer), _writing(&write_buffer), _ready(&ready_buffer)
	{
		// dont swap by default
		_updated.exchange(true);
	}

	//! Return a pointer to the write buffer.
	const t *get_write_buffer() const
	{
		return _writing.load();
	}

	//! Called when you have finished writing to the write buffer.
	void finish_write()
	{
		// swap ready and write buffers
		auto p = _ready.exchange(_writing);
		_writing.store(p);

		// mark updated info
		_updated.exchange(false);
	}

	//! Return a pointer to the read buffer.
	const t *get_read_buffer() const
	{
		return _reading.load();
	}

	//! Called when you have finished reading from the read buffer.
	void finish_read()
	{
		// if we didnt update then dont swap
		if (_updated.exchange(true))
		{
			return;
		}

		// swap only if we updated
		auto p = _ready.exchange(_reading);
		_reading.store(p);
	}

  private:
	std::atomic<t *> _reading = nullptr;
	std::atomic<t *> _writing = nullptr;
	std::atomic<t *> _ready = nullptr;
	std::atomic_bool _updated = false;
};
}  // namespace lib::common