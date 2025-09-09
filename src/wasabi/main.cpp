
#include <iostream>
#include <vector>
#include "Scene.hpp"
#include "WindowBuilder.hpp"

class ExampleScene : public wasabi::Scene {
public:
	ExampleScene() {
		setWindow();
	}

	void setWindow() noexcept {
		auto window = wasabi::core::WindowBuilder::create()
			.width(800)
			.height(600)
			.title("Wasabi Window")
			.build();

		Scene::setWindow(std::move(window));
	}
};


int main() {
	std::vector<int> vec;

	ExampleScene scene{};
	scene.loop();
}
