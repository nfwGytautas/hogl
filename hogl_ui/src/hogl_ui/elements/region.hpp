#pragma once

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
		virtual void transform(const tinfo& t) override;
		virtual bool add_child(element* e, const std::string& region);

		/**
		 * @brief Sets the transform of the region
		 * @param t New transform
		*/
		void set_transform(const tinfo& t);

		/**
		 * @brief Returns a reference to the region transformation
		 * @return Reference to region transform
		*/
		tinfo& get_transform();
	private:
		tinfo m_transform;
		element* m_child;
	};

}
