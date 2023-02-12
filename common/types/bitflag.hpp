#pragma once

#include <cstdint>

namespace lib::common {

#if _WIN32
using bitflag_t = uint32_t;
#else
using bitflag_t = uint64_t;
#endif

    class bitflag {
    public:
	  	inline void SetFlag(bitflag_t flag) {
		  _flags |= flag;
		}

		inline void RemoveFlag(bitflag_t flag) {
		  _flags &= ~flag;
		}

		inline bool HasFlag(bitflag_t flag) {
		  return _flags & flag;
		}

		inline void ToggleFlag(bitflag_t flag) {
		  _flags ^= flag;
		}
    private:
	  	bitflag_t _flags{};
    };
}