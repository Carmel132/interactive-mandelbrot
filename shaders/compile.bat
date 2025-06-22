set f=mandelbrot
glslc source/%f%.frag -o compiled/%f%.frag.spv
glslc source/%f%.vert -o compiled/%f%.vert.spv