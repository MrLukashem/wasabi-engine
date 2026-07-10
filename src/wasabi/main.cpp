
#include "include/Scene.hpp"
#include "include/WasabiEngine.hpp"
#include "WindowBuilder.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


class DemoScene final: public wasabi::Scene {
public:
	void onAttach(wasabi::ecs::WorldSupervisor* world) override {
		log->info("onAttach");

		createTriangle(
			{1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{0.0, 0.0, 1.0});
	}

	void onDetach() override {
		log->info("onDetach");
	}

	void onUpdate(float dt) noexcept override {
	}
private:
	std::shared_ptr<spdlog::logger> log = spdlog::stdout_color_mt("DemoScene");
};

int main() {
	auto window = wasabi::core::WindowBuilder::create()
		.width(800)
		.height(600)
		.title("Demo Window")
		.build();

	wasabi::WasabiEngine engine{std::move(window)}; // TODO: The engine should take it as uptr
	engine.setScene(std::make_unique<DemoScene>());
	engine.loop();
}
