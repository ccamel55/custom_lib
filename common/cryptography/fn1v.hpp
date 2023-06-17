#pragma once

#include <string>
#include <cstring>

namespace lib::common {

#if _WIN32
    using hash_t = uint32_t;

	class fn1v {
	public:
		// compile time hash
		constexpr static hash_t getCT(const char* szString, const hash_t uValue = OFFSET_BASIS) noexcept {
			return (szString[0] == '\0') ? uValue : getCT(&szString[1], (uValue ^ hash_t(szString[0])) * FNV_PRIME);
		}

		// runtime hash
		inline static hash_t get(const char* szString) {

			hash_t uHashed = OFFSET_BASIS;

			for (size_t i = 0; i < strlen(szString); i++) {
				uHashed ^= szString[i];
				uHashed *= FNV_PRIME;
			}

			return uHashed;
		}

	private:
		static constexpr hash_t FNV_PRIME = 0x1000193;
		static constexpr hash_t OFFSET_BASIS = 0x811c9dc5;
	};
#else
    using hash_t = uint64_t;

    class fn1v {
    public:
        //! calculate fn1v hash of \param szString during compile time
        constexpr static hash_t getCT(const char* szString, const hash_t uValue = OFFSET_BASIS) noexcept {
            return (szString[0] == '\0') ? uValue : getCT(&szString[1], (uValue ^ hash_t(szString[0])) * FNV_PRIME);
        }

        //! calculate fn1v hash of \param szString during run time
        inline static hash_t get(const char* szString) {

            hash_t uHashed = OFFSET_BASIS;

            for (size_t i = 0; i < strlen(szString); i++) {
                uHashed ^= szString[i];
                uHashed *= FNV_PRIME;
            }

            return uHashed;
        }

    private:
        static constexpr hash_t FNV_PRIME = 0x1000193;
        static constexpr hash_t OFFSET_BASIS = 0x811c9dc5;
    };
#endif

}