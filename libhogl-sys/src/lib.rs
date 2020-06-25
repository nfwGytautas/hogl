#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

// Include hogl c library
mod bindings;

// Used for keeping test state
use lazy_static::lazy_static;

lazy_static! {
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }

    unsafe extern fn mouse_key_callback(user_pointer: *mut ::std::os::raw::c_void, event: *const crate::bindings::hogl_e_mkey) {
        println!("Mouse key: {0}", (*event).button);
    }

    unsafe extern fn key_callback(user_pointer: *mut ::std::os::raw::c_void, event: *const crate::bindings::hogl_e_key) {
        println!("Key: {0}", (*event).key);
    }

    #[test]
    fn hogl() {
        unsafe {
            // Create context and assert if couldn't create one
            let mut context : *mut crate::bindings::hogl_context = crate::bindings::hogl_init();
            assert!(!context.is_null(), "context null ptr");

            // Create window and assert if couldn't create one
            let mut window : *mut crate::bindings::hogl_wnd = crate::bindings::hogl_wnd_create(context);
            assert!(!context.is_null(), "window null ptr");

            // Create input reference
            let mut input_interface : *mut crate::bindings::hogl_e_interface = crate::bindings::hogl_input_new_interface(context);
            assert!(!input_interface.is_null(), "input interface null ptr");

            (*input_interface).cb_mkey = Some(mouse_key_callback);
            (*input_interface).cb_key = Some(key_callback);

            // Wait for window to close
            while !((*window).is_closed == 1)
            {
                // Updates window state this includes is_closed flag
                crate::bindings::hogl_wnd_state_update(window);

                // Polls hogl input
                crate::bindings::hogl_input_update(context);
            }

            // Shutdown window and assert if deleted pointer
            crate::bindings::hogl_wnd_destroy(&mut window);
            assert!(window.is_null(), "window wasn't set to nullptr");

            // Shutdown hogl and assert if deleted pointer
            crate::bindings::hogl_shutdown(&mut context);
            assert!(context.is_null(), "context wasn't set to nullptr");
        }
    }
}
