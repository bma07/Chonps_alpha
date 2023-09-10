mkdir spvShaders

C:/VulkanSDK/1.3.236.0/Bin/glslc.exe gui.vert -o spvShaders/gui_vert.spv
C:/VulkanSDK/1.3.236.0/Bin/glslc.exe gui.frag -o spvShaders/gui_frag.spv

C:/VulkanSDK/1.3.236.0/Bin/glslc.exe preStencil.vert -o spvShaders/preStencil_vert.spv
C:/VulkanSDK/1.3.236.0/Bin/glslc.exe preStencil.frag -o spvShaders/preStencil_frag.spv

pause