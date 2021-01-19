#pragma once

#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/element.hpp"

namespace hogl_ui {

	/**
	 * @brief Button class that provides a standard button functionality to the ui
	*/
	class HOGL_UI_API button : public element {
	public:
		button(element* parent, const std::string& name);
		virtual ~button();

		// Inherited via element
		virtual void queue_vertices(const vertex_lists& vlist) override;
		virtual void render() override;
		virtual void transform() override;
		virtual void set_background(color c) override;
	private:
		std::vector<float> vertices = {};
		std::vector<unsigned int> indices = {};
	};

}
