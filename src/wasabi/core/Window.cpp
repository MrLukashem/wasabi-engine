
#include "Window.hpp"

#include <SFML/Window.hpp>


namespace {

template <typename W>
void prepareWindow(W& window) {
	window.setSize({ 800, 600 });
	window.setTitle("Wasabi Engine hello!");
}

} // namespace

namespace wasabi::core {

} // namespace wasabi::core

