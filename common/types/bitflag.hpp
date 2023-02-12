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

        //! Set the bit specified by \param flag
	  	inline void set_flag(bitflag_t flag) {
		  _flags |= flag;
		}

        //! Unset the bit specified by \param flag
		inline void remove_flag(bitflag_t flag) {
		  _flags &= ~flag;
		}

        //! Set the bit specified by \param flag
		inline bool has_flag(bitflag_t flag) {
		  return _flags & flag;
		}

        //! Flip the bit specified by \param flag
		inline void toggle_flag(bitflag_t flag) {
		  _flags ^= flag;
		}
    private:
	  	bitflag_t _flags = 0;
    };
}