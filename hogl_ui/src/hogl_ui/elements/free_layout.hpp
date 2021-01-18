#pragma once

#include <unordered_map>
#include <string>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/element.hpp"
#include "hogl_ui/elements/region.hpp"

namespace hogl_ui {

	/**
	 * @brief Free layout is the main layout object, it allows to describe specific regions
	 * where children will be stored, automatically keeping track of the viewport and size.
	 * It doesn't have a constraint on the size and layout unlike other types like a grid.
	*/
	class HOGL_UI_API free_layout : public element {
	public:
		free_layout(element* parent, const std::string& name);
		virtual ~free_layout();

		// Inherited via element
		virtual void queue_vertices(const vertex_lists& vlist) override;
		virtual void render() override;
		virtual void transform(const tinfo& t) override;
		virtual bool add_child(element* e, const std::string& region) override;
	private:
		std::vector<region*> m_regions;
	};

}
