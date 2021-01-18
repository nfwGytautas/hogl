#pragma once

#include "hogl_ui/hogl_ui.hpp"

void ui() {
	hogl_ui::application app;
	if (!app.load_layout_file("res/test_layout.xml")) {
		return;
	}
	app.show();
}
