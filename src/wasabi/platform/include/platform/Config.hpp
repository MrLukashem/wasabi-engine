
#pragma

namespace wasabi {

enum class Platform {
	WINDOWS,
	ANDROID,
	LINUX,
};

Platform getPlatform();

} // namespace wasabi

#ifdef _WIN32
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
//#define VK_EXT_debug_report
#endif // _WIN32
