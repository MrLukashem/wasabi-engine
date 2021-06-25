
#include "utils/FileUtils.hpp"

#include <algorithm>
#include <fstream>


namespace wasabi::utils {

Bytes readFile(const std::filesystem::path path) noexcept {
	std::ifstream stream(path, std::ios_base::in | std::ios_base::ate | std::ios_base::binary);
	if (stream.fail() || !stream.is_open()) {
		return {};
	}

	const std::size_t size = stream.tellg();
	std::vector<char> buffer(size);

	stream.seekg(0);
	stream.read(buffer.data(), size);
	stream.close();
	return buffer;
}

} // namespace wasabi::utils

