#include <hogl/hogl.hpp>

#include <iostream>

using namespace hogl;

void mouse_key_callback(void* user_pointer, const hogl_e_mkey* event)
{
    std::cout << "Mouse key: " << event->button << "\n";
}

void key_callback(void* user_pointer, const hogl_e_key* event)
{
    std::cout << "Key: " << event->key << "\n";
}

int main()
{
    // Create context
    hogl_context* context = hogl_init();

    // Create window
    hogl_wnd* window = context->window_manager->create();

    // Bind window to the input system
    context->input_system->bind_wnd(window);

    // Create input interface
    hogl_e_interface* input_interface = context->input_system->new_interface();

    // Set callbacks
    input_interface->cb_key = key_callback;
    input_interface->cb_mkey = mouse_key_callback;

    // Loop until window is closed
    while (window->is_closed == 0)
    {
        // Updates window state this includes is_closed flag
        context->window_manager->update();

        // Polls hogl input
        context->input_system->update();
    }

    // Shutdown hogl
    hogl_shutdown(context);
}
