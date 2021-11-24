
#include "platform/Config.hpp"


namespace wasabi {

#ifdef _WIN32

Platform getPlatform() {
	return Platform::WINDOWS;
}

#endif // _WIN32

} // namespace wasabi
