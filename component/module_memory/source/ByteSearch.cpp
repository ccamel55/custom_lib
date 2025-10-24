#include <module_memory/ByteSearch.hpp>

using namespace lib::memory;

ByteSearch::ByteSearch(memory_section memory)
    : _memory(memory) {
}

std::optional<address> ByteSearch::find(const std::vector<int>& bytes) const {
    // Has to be smaller than memory section duh...
    if (bytes.size() >= _memory.size() || bytes.empty()) {
        return std::nullopt;
    }

    const auto& section_span = _memory.section;

    for (size_t i = 0; i < section_span.size() - bytes.size();) {
        bool matches = true;
        size_t bytes_to_skip = 0;

        for (size_t j = 0; j < bytes.size(); j++) {
            const int sig_byte = bytes.at(j);
            const int cur_byte = static_cast<int>(section_span[i + j]);

            // If we re-discover our starting byte mid-way through a match, then we can only
            // skip to that
            if (bytes_to_skip == 0 && j > 0 && (sig_byte == -1 || cur_byte == bytes.at(0))) {
                bytes_to_skip = j;
            }

            // Only allow the code bellow to execute once we fail to match
            if (sig_byte == -1 || cur_byte == sig_byte) {
                continue;
            }

            // Only set bytes_to_skip if we didn't discover our starting byte during our scan.
            // If we start from j, we might end up missing some data
            if (bytes_to_skip == 0) {
                bytes_to_skip = j;
            }

            matches = false;
            break;
        }

        if (matches) {
            return address(&section_span[i]);
        }

        // Skip bytes we already checked and know are bad.
        i += std::max<size_t>(1, bytes_to_skip);
    }

    return std::nullopt;
}