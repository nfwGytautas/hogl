#pragma once

#include "hogl_ui/shared/hogl_ui_def.hpp"

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
	private:
	};

}
