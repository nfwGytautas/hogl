#pragma once

#include "hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief Rendering abstraction that provides basic rendering abstractions
*/
class hogl_cs_renderer
{
public:
	/**
	 * @brief Set the renderer render target, for more check the different hogl render target classes, the targets
	 * are class that inherit the hogl_i_render_target interface, do note, that their lifetime is managed by the user
	 * @param target hogl render target to set
	*/
	void change_target(hogl_i_render_target* target);

	/**
	 * @brief Begins a new frame
	*/
	void begin_frame();

	/**
	 * @brief Simple call to render a hogl_object, for more advanced rendering render buffers will need to be used
	 * @param object hogl_object instance to render
	*/
	void render_object(hogl_object* object);

	/**
	 * @brief Ends the current frame
	*/
	void end_frame();

	/**
	 * @brief Flushes current contents to the flush target
	*/
	void flush();
private:
	hogl_i_render_target* m_target = nullptr;
};

/**
 * @brief hogl render target interface, these targets also allow for finer control of the rendering process
*/
class hogl_i_render_target
{
public:
	virtual ~hogl_i_render_target() {};

	/**
	 * @brief Check if the render target is valid or not
	 * @return True if the render target is valid, False otherwise
	*/
	virtual bool valid() = 0;

	/**
	 * @brief Bind this render target, for windows this sets the OpenGL context to the context of the specified window
	*/
	virtual void bind() = 0;

	/**
	 * @brief Clear the render target
	*/
	virtual void clear() = 0;

	/**
	 * @brief Flush to the render target, for windows this causes the buffers to be swapped
	*/
	virtual void flush() = 0;
};

/**
 * @brief Render target where the target is a window, for more info check the hogl_i_render_target class,
 * this target is created using a hogl window, and the target is valid as long as the window is valid itself
*/
class hogl_wnd_render_target : public hogl_i_render_target
{
public:
	/**
	 * @brief Create a new instance of window rendering target
	 * @param wnd Window to render to
	*/
	hogl_wnd_render_target(hogl_wnd* wnd);
	
	/**
	 * @brief Set the clear color of the render target, channel values are from 0 to 255, 
	 * other values keep the color unchanged, e.g. in order to only change the red channel just write 256 or more to g, b and a channels
	 * @param r Red channel value of the clear color
	 * @param g Green channel value of the clear color
	 * @param b Blue channel value of the clear color
	 * @param a Alpha channel value of the clear color
	*/
	void set_clear_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a);

	// Inherited via hogl_i_render_target
	virtual bool valid() override;

	virtual void bind() override;

	virtual void clear() override;

	virtual void flush() override;
private:
	hogl_wnd* m_wnd;
	float m_clearColor[4];
};

HOGL_NSPACE_END
