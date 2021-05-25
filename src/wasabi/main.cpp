
#include <iostream>
#include <vector>
#include "Scene.hpp"

class Test : public wasabi::Scene {
public:
	Test() {
		setWindow();
	}
};


int main() {
	std::vector<int> vec;
	std::cout << "Hello World qewqe\n";

	Test t{};
	t.setWindow();
	t.loop();
}
