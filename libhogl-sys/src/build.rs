extern crate bindgen;

fn main() {
    // Tell cargo to tell rustc to link the system bzip2
    // shared library.
    println!("cargo:rustc-link-search=native=../build/");

    if cfg!(windows) {
        // Link windows OpenGL libs
        println!("cargo:rustc-link-lib=dylib=user32");
        println!("cargo:rustc-link-lib=dylib=gdi32");
        println!("cargo:rustc-link-lib=dylib=shell32");
        println!("cargo:rustc-link-lib=dylib=vcruntime");
        println!("cargo:rustc-link-lib=dylib=msvcrt");
    }

    // Link hogl dependencies
    println!("cargo:rustc-link-lib=dylib=Glad");
    println!("cargo:rustc-link-lib=dylib=glfw3");

    // Link hogl library
    println!("cargo:rustc-link-lib=dylib=hogl");

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("../hogl-c/hogl/src/hogl.h")
        // Generate comments
        .generate_comments(true)
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the bindings.rs file.
    bindings
        .write_to_file("src/bindings.rs")
        .expect("Couldn't write bindings!");
}