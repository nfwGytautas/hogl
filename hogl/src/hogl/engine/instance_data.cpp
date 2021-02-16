#include "instance_data.hpp"

namespace hogl {

	/**
	 * @brief Starting index of transformation attributes
	*/
	constexpr int csexpr_tsi = 12;

	gpu_buffer* transformation_desc() {
		hogl_vbo_desc ret = {};
		hogl_ap_desc apd[4];

		for (int i = 0; i < 4; i++) {
			apd[i].divisor = 1;
			apd[i].ecount = 4;
			apd[i].index = csexpr_tsi + i;
			apd[i].normalized = false;
			apd[i].offset = i * sizeof(glm::vec4);
			apd[i].type = HOGL_ET_FLOAT;
		}

		ret.data = nullptr;
		ret.data_size = 0;
		ret.usage = HOGL_VBOU_DYNAMIC;
		ret.type = HOGL_VBOT_ARRAY_BUFFER;
		ret.stride = sizeof(glm::mat4x4);
		ret.desc_size = 4;
		ret.ap_desc = apd;

		return new gpu_buffer(ret);
	}

	instance_data::instance_data() {
		m_model = transformation_desc();
	}

	instance_data::~instance_data() {
		delete m_model;
	}

	void instance_data::push_transformations(std::vector<glm::mat4>& transformations) {
		m_model->set_data(transformations);
	}

	void instance_data::bind() {
		m_model->bind();
	}

}
