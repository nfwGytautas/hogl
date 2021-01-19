#pragma once

#include <vector>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/element.hpp"

namespace hogl_ui {

	class HOGL_UI_API region : public element {
	public:
		region(element* parent, const std::string name);
		virtual ~region();

		// Inherited via element
		virtual void queue_vertices(const vertex_lists& vlist) override;
		virtual void render() override;
		virtual void transform() override;
		virtual bool add_child(element* e, const std::string& region) override;
		virtual void set_background(color c) override;
	private:
		element* m_child;

		std::vector<float> vertices = {};
		std::vector<unsigned int> indices = {};
	};

}
