mkdir spvShaders

C:/VulkanSDK/1.3.236.0/Bin/glslc.exe vulkanFB.vert -o spvShaders/FBOshader_vert.spv
C:/VulkanSDK/1.3.236.0/Bin/glslc.exe vulkanFB.frag -o spvShaders/FBOshader_frag.spv
pause