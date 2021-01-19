#pragma once

#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/element.hpp"

namespace hogl_ui {

	/**
	 * @brief Root element of the hogl ui
	*/
	class HOGL_UI_API root : public element {
	public:
		root();
		~root();

		// Inherited via element
		virtual void queue_vertices(const vertex_lists& vlist) override;
		virtual void render() override;
		virtual void transform() override;
		virtual bool add_child(element* e, const std::string& region) override;
	private:
		element* m_layout = nullptr;		
	};

}
