This folder contains the libraries that are bundled inside the hogl C api
(Currently these libraries are all built for windows later on if I can I will include other platforms)

1. Glad and glfw3 both of these libraries are used to provide OpenGL functionality (Static linked with zlib(glfw3) and MIT(glad))

2. stb_image used to provide the ability to load image data (Compiled into the source with MIT license)

3. openal-soft used to provide sound API for the library (Must be dynamically linked because of LPGL)