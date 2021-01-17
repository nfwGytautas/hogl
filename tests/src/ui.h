#pragma once

#include "hogl_ui/hogl_ui.hpp"

void ui() {
	hogl_ui::application* app = new hogl_ui::application();
	app->show();
}
