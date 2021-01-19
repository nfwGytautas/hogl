#pragma once

#include <unordered_map>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/element.hpp"
#include "hogl_ui/elements/root.hpp"
#include "hogl_ui/core/vertex_list.hpp"
#include "hogl_core/graphics/hogl_wnd.h"

namespace hogl_ui {

	/**
	 * @brief The main hogl ui object that defines the application
	*/
	class HOGL_UI_API application {
	public:
		/**
		 * @brief Creates a new application object, there can only exist one application per program, once constructed
		 * the application can be reached using get_instance static method
		*/
		application();
		virtual ~application();

		/**
		 * @brief Loads the specified layout file from the specified path
		 * @param path Path to the layout file
		 * @return True if the layout loaded correctly, false otherwise
		*/
		bool load_layout_file(const char* path);

		/**
		 * @brief Shows the application window this is a blocking call and everything after it
		 * will be called once the application exits
		*/
		void show();

		/**
		 * @brief Returns the application instance
		 * @return Pointer to application instance
		*/
		static application* get_instance();

		/**
		 * @brief Returns the window height
		*/
		unsigned int get_window_height() const;

		/**
		 * @brief Returns the window width
		*/
		unsigned int get_window_width() const;
	private:
		void setup_vlists();

	private:
		hogl_wnd* m_window = nullptr;
		hogl_wi* m_wi = nullptr;

		root* m_root = nullptr;
		std::unordered_map<std::string, element*> m_elements;

		vertex_lists m_vlist = {};
	};

}
