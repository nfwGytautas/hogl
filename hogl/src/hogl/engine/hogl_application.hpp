#pragma once

#include "hogl/shared/hogl_def.hpp"
#include "hogl/engine/object_storage.hpp"
#include "hogl/engine/ubo_manager.hpp"
#include "hogl/engine/instance_data.hpp"
#include "hogl/engine/window.hpp"
#include "hogl/io/asset_manager.hpp"
#include "hogl/core/framebuffer.hpp"
#include "hogl/core/ubo.hpp"
#include "hogl/entity/mesh.hpp"
#include "hogl/entity/material.hpp"
#include "hogl/entity/camera.hpp"
#include "hogl/entity/scene.hpp"
#include "hogl/entity/components.hpp"

namespace hogl {

	/**
	 * @brief The main class of hogl
	*/
	class HOGL_CPP_API hogl_application {
	public:
		hogl_application();
		virtual ~hogl_application();
		static hogl_application* get();

		/**
		 * @brief Initializes hogl
		*/
		void init();

		virtual void on_ready();
		virtual void on_update();
		virtual void on_render();
	protected:
		object_storage* ostorage = nullptr;
		ubo_manager* ubo_manager = nullptr;
		asset_manager* asset_manager = nullptr;
		instance_data* instance_data = nullptr;

		window* main_window = nullptr;
	private:
		void loop();

	private:
		static hogl_application* ms_instance;
	};

}