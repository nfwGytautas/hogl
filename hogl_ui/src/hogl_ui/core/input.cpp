#include "input.hpp"

namespace hogl_ui {

	bool __ccb_close(void* usrp) {
		input_manager* im = static_cast<input_manager*>(usrp);
		return im->on_close();
	}

	input_manager::input_manager() {

	}

	input_manager::~input_manager() {

	}

	void hogl_ui::input_manager::add_wi(hogl_wi* pwi) {
		pwi->usrp = this;
		pwi->close_cb = __ccb_close;
		m_wis.push_back(pwi);
	}

	bool input_manager::on_close() {
		return false;
	}

}
