#pragma once

// Define used to simplify development by configuring that the C/C++ side can be compiled with any option
// and that bindgen will still be able to use this file to create bindings for Rust
#ifdef HOGL_VS
extern "C" {
#endif

	typedef struct hogl_context hogl_context;
	typedef struct hogl_wnd hogl_wnd;

	/**
	 * hogl context structure contains all the needed information for all hogl operations
	 * it is used to make sure the library is initialized, keeps track of event loops etc.
	 */
	struct hogl_context {
		/**
		 * Number of windows created
		 */
		unsigned int m_wnd_count;
	};

	/**
	 * hogl window
	 */
	struct hogl_wnd {

		/**
		 * Native window of created with the help of GLFW
		 * in reality this is a type erased GLFW window
		 */
		void* mp_native_window;
	};

	/**
	 * Initializes hogl library, this function should only be called once, calling twice is undefined 
	 * behaviour
	 */
	hogl_context* hogl_init();

	/**
	 * Shuts down the hogl library, this function should only be called once, calling twice is undefined 
	 * behaviour
	 */
	void hogl_shutdown(hogl_context** context);
	
	/**
	 * Creates a hogl window
	 * @param pContext hogl context received from hogl_init function
	 * @return hogl_wnd instance or null if there was an error
	 */
	hogl_wnd* hogl_wnd_create(hogl_context* context);

	/**
	 * Destroys a hogl window
	 * @param pWnd window received from hogl_wnd_create function to destroy
	 */
	void hogl_wnd_destroy(hogl_wnd** wnd);


#ifdef HOGL_VS
}
#endif