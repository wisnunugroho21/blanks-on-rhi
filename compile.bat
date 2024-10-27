mkdir -p build/shaders

glslc src/shaders/simple.vert -o build/shaders/simple.vert.spv
glslc src/shaders/simple.frag -o build/shaders/simple.frag.spv