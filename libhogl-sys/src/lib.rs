#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

// Include hogl c library
include!("bindings.rs");

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

    #[test]
    fn hogl() {
        unsafe {
            // Create context and assert if couldn't create one
            let mut context : *mut crate::hogl_context = crate::hogl_init();
            assert!(!context.is_null(), "context null ptr");

            // Create window and assert if couldn't create one
            let mut window : *mut crate::hogl_wnd = crate::hogl_wnd_create(context);
            assert!(!context.is_null(), "window null ptr");

            // Shutdown window and assert if deleted pointer
            crate::hogl_wnd_destroy(&mut window);
            assert!(window.is_null(), "window wasn't set to nullptr");

            // Shutdown hogl and assert if deleted pointer
            crate::hogl_shutdown(&mut context);
            assert!(context.is_null(), "context wasn't set to nullptr");
        }
    }
}
