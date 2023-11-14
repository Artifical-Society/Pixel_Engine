shader_version="0_0_0"

echo "compile shaders version: ${shader_version}:"
echo ".......  0%"

/Users/ryen/Code/Library/VulkanSDK/1.3.268.1/macOS/bin/glslc ./shaders/shader_v${shader_version}.vert -o ./shaders/build/shader_v${shader_version}.vert.spv
echo "....... 50%"

/Users/ryen/Code/Library/VulkanSDK/1.3.268.1/macOS/bin/glslc ./shaders/shader_v${shader_version}.frag -o ./shaders/build/shader_v${shader_version}.frag.spv
echo ".......100%"

chmod 742 ./shaders/build/shader_v${shader_version}.vert.spv ./shaders/build/shader_v${shader_version}.frag.spv
echo "...Finished"
