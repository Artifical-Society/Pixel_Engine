shader_version="0_0_0"

#echo "compile shaders version: ${shader_version}:"
#echo ".......  0%"

/usr/local/bin/glslc ./shaders/shader_v${shader_version}.vert -o ./shaders/build/shader_v${shader_version}.vert.spv
#echo "....... 50%"

/usr/local/bin/glslc ./shaders/shader_v${shader_version}.frag -o ./shaders/build/shader_v${shader_version}.frag.spv
#echo ".......100%"
#echo "...Finished"
