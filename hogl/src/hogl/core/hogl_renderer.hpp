#pragma once

#include <unordered_map>
#include <string>
#include <array>

#include "hogl/hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief Rendering abstraction that provides basic rendering abstractions
*/
class HOGL_API hogl_cs_renderer
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
	 * @brief Render a object
	 * @param object Object to render
	*/
	void render_object(hogl_render_object* object);

	/**
	 * @brief Executes the specified draw call
	 * @param draw_call Draw call to execute
	*/
	void execute_draw_call(hogl_render_draw_call* draw_call);

	/**
	 * @brief Ends the current frame
	*/
	void end_frame();

	/**
	 * @brief Flushes current contents to the flush target
	*/
	void flush();

	/**
	 * [TODO] Transfer to draw_call
	 * @brief Adjust the size of the rendering viewport
	 * @param width New width of the viewport
	 * @param height New height of the viewport
	*/
	void adjust_viewport(unsigned int width, unsigned int height);

	/**
	 * @brief Force the next bound draw call to override the current one.
	 * This function is used when the currently bound draw call is changed.
	*/
	void reset_draw_call();

	/**
	 * @brief Force the next bound object to override the current one
	 * This function is used when the currently bound object is changed.
	*/
	void reset_object();
private:
	/**
	 * @brief Bind the specified draw call
	 * @param draw_call hogl_render_draw_call instance to bind
	*/
	void bind_draw_call(hogl_render_draw_call* draw_call);

	/**
	 * @brief Bind the specified object
	 * @param object hogl_render_object instance to bind
	*/
	void bind_object(hogl_render_object* object);

	/**
	 * @brief Bind the specified mesh
	 * @param mesh hogl_mesh instance to bind
	*/
	void bind_mesh(hogl_mesh* mesh);

	/**
	 * @brief Bind the specified shader
	 * @param shader hogl_shader instance to bind
	*/
	void bind_shader(hogl_shader* shader);

	/**
	 * @brief Bind the specified texture
	 * @param texture hogl_texture instance to bind
	*/
	void bind_texture(hogl_texture* texture);

	/**
	 * @brief Renders the current bound
	*/
	void render_single();

	/**
	 * @brief Sets OpenGL depth test function
	 * @param func hogl_render_depth value
	*/
	void set_depth_func(hogl_render_depth func);

	/**
	 * @brief Set if cubemaps should be sampled seamlessly or not
	 * @param value 
	*/
	void set_seamless_cubemap(bool value);
private:
	hogl_i_render_target* m_target = nullptr;
	
	hogl_render_draw_call* m_currentDrawCall = nullptr;
	hogl_render_object* m_currentObject = nullptr;
	hogl_mesh* m_currentMesh = nullptr;
	hogl_shader* m_currentShader = nullptr;
	std::array<hogl_texture*, HOGL_MAX_TEXTURE_SLOTS> m_currentTexture = { nullptr };
};

/**
 * @brief hogl render target interface, these targets also allow for finer control of the rendering process
*/
class HOGL_API hogl_i_render_target
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
class HOGL_API hogl_wnd_render_target : public hogl_i_render_target
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
	hogl_wnd* m_wnd = nullptr;
	float m_clearColor[4];
};

/**
 * @brief Render target where the target is a framebuffer, for more info check the hogl_i_render_target class,
 * this target is created using a hogl framebuffer, and the target is valid as long as the framebuffer is valid itself
*/
class HOGL_API hogl_fbo_render_target : public hogl_i_render_target
{
public:
	/**
	 * @brief Create a new instance of framebuffer rendering target
	 * @param fbo Framebuffer to render to
	*/
	hogl_fbo_render_target(hogl_framebuffer* fbo);

	/**
	 * @brief Set the clear color of the render target, channel values are from 0 to 255,
	 * other values keep the color unchanged, e.g. in order to only change the red channel just write 256 or more to g, b and a channels
	 * @param r Red channel value of the clear color
	 * @param g Green channel value of the clear color
	 * @param b Blue channel value of the clear color
	 * @param a Alpha channel value of the clear color
	*/
	void set_clear_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a);
	
	/**
	 * @brief Attach a texture to the framebuffer
	 * @param texture Texture to attach
	*/
	void attach_texture(hogl_texture* texture);

	/**
	 * @brief This functions sets the attachment of the framebuffer to be a single side of a texture, so make sure that the attached texture
	 * is a cube map before using this function
	 * @param cslot Cube map side of the texture from 0 - 6
	*/
	void set_cslot(unsigned int cslot);

	/**
	 * @brief Set the mip-map level of the attached texture
	 * @param mip Mip-map level
	*/
	void set_mip(unsigned int mip);

	/**
	 * @brief Maps the attached texture to GPU memory
	*/
	void map();

	// Inherited via hogl_i_render_target
	virtual bool valid() override;

	virtual void bind() override;

	virtual void clear() override;

	virtual void flush() override;
private:
	hogl_framebuffer* m_fbo = nullptr;
	hogl_texture* m_textureAttachment = nullptr;
	float m_clearColor[4];
	unsigned int m_mip = 0;
};

/**
 * @brief A single object that is going to be rendered in a draw call
*/
struct HOGL_API hogl_render_object
{
	/**
	 * @brief Mesh to render
	*/
	hogl_mesh* mesh = nullptr;

	/**
	 * @brief Shader to render with
	*/
	hogl_shader* shader = nullptr;

	/**
	 * @brief Texture to render
	*/
	std::vector<hogl_texture*> textures;
};

/**
 * @brief Single draw call
*/
struct HOGL_API hogl_render_draw_call
{
	/**
	 * @brief Object that will be rendered during this draw call
	*/
	hogl_render_object* object = nullptr;

	/**
	 * @brief Depth test function, DISABLED to disable depth testing
	*/
	hogl_render_depth depth_test = hogl_render_depth::LESS;

	/**
	 * @brief Set true to enable seamless cube map sampling
	*/
	bool seamless_cubemap = true;

	/**
	 * @brief If this value is 0 then the object will rendered normally, if any other value, then instancing will be used 
	*/
	size_t instance_count = 0;

	/**
	 * @brief Render mode for this draw call
	*/
	hogl_render_mode render_mode = hogl_render_mode::TRIANGLES;
};

HOGL_NSPACE_END
