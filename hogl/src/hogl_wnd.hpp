#pragma once

#include <vector>

// Common hogl library types
#include "hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl window managing system
*/
class HOGL_API hogl_cs_wndmanager
{
public:
	~hogl_cs_wndmanager();

	/**
	 * @brief Update all active windows, also destroys windows marked for that
	*/
	void update();

	/**
	 * @brief Creates a hogl window
	 * @return hogl_wnd instance or null if there was an error
	*/
	hogl_wnd* create();

private:
	/**
	 * @brief Updates the specified window
	 * @param wnd Window to update
	*/
	void update_wnd(hogl_wnd* wnd);

	/**
	 * @brief Destroy specified window
	 * @param wnd Window to destroy
	*/
	void destroy_wnd(hogl_wnd*& wnd);
private:
	/**
	 * @brief Windows created by the manager
	*/
	std::vector<hogl_wnd*> m_windows;
};

/**
 * @brief hogl window
*/
struct HOGL_API hogl_wnd {
	/**
	 * @brief [STATE] Specifies if this window is closed, 0 for NO otherwise YES, default NO
	*/
	int is_closed;

	/**
	 * @brief Specifies if this window should be destroyed on the next update, 0 for NO otherwise YES, default NO
	*/
	int destroy;

	/**
	 * @brief Specifies if this window should be updated or not, 0 for NO otherwise YES, default YES
	*/
	int active;

	/**
	 * @brief Native window of created with the help of GLFW
	 * in reality this is a type erased GLFW window
	*/
	void* native_window;
};

HOGL_NSPACE_END
