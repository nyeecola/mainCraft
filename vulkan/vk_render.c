#include <stdlib.h>
#include "vk_render.h"
#include "utils.h"

VkRenderPass
create_render_pass(VkDevice logical_device, struct swapchain_info state)
{
	VkRenderPass render_pass;

	VkAttachmentDescription color_attachment = {
		.format = state.surface_format.format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference color_attachment_ref = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_ref,
		.pDepthStencilAttachment = VK_NULL_HANDLE
	};

	/* This subpass needs wait for the clear-color operation and the operation
	 * of write pixels in the framebuffer have to wait this subpass
	 * */
	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	/* Currently we only have one of each ^^;
	 * But we may have more in the future...
	 * */
	VkSubpassDescription subpasses[] = { subpass };
	VkSubpassDependency subpasses_dependencies[] = { dependency };
	VkAttachmentDescription attachments[] = { color_attachment };

	VkRenderPassCreateInfo render_pass_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = array_size(attachments),
		.pAttachments = attachments,
		.subpassCount = array_size(subpasses),
		.pSubpasses = subpasses,
		.dependencyCount = array_size(subpasses_dependencies),
		.pDependencies = subpasses_dependencies
	};

	if (vkCreateRenderPass(logical_device, &render_pass_info, NULL, &render_pass) != VK_SUCCESS) {
		print_error("Failed to create render pass!");
		return VK_NULL_HANDLE;
	}

	return render_pass;
}

VkShaderModule
createShaderModule(const VkDevice logical_device, const char *code, int64_t size) {
	VkShaderModule shader_module;

	VkShaderModuleCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = size,
		.pCode = (const uint32_t *) code
	};

	if (vkCreateShaderModule(logical_device, &create_info, NULL, &shader_module) != VK_SUCCESS) {
		print_error("Failed to create shader module!");
		return VK_NULL_HANDLE;
	}

	return shader_module;
}

int
create_graphics_pipeline(const VkDevice logical_device, struct swapchain_info *swapchain_info, struct vk_render *render)
{
	char *vert_shader_code, *frag_shader_code;
	int64_t vert_size, frag_size;
	VkPipeline pipeline;
	int ret = -1;

	vert_shader_code = read_file("shaders/vert.spv", &vert_size);
	if (!vert_shader_code)
		goto return_error;

	frag_shader_code = read_file("shaders/frag.spv", &frag_size);
	if (!frag_shader_code)
		goto destroy_vert_code;

	VkShaderModule vert_shader_module = createShaderModule(logical_device, vert_shader_code, vert_size);
	if (vert_shader_module == VK_NULL_HANDLE)
		goto destroy_frag_code;

	VkShaderModule frag_shader_module = createShaderModule(logical_device, frag_shader_code, frag_size);
	if (frag_shader_module == VK_NULL_HANDLE)
		goto destroy_vert_module;

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0
	};

	VkPipelineShaderStageCreateInfo vert_shader_stage_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vert_shader_module,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo frag_shader_stage_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = frag_shader_module,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo shader_stages[] = {
		vert_shader_stage_info,
		frag_shader_stage_info
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = swapchain_info->extent.width,
		.height = swapchain_info->extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = swapchain_info->extent
	};

	/* Now just fill the struct with the two above */
	VkPipelineViewportStateCreateInfo viewport_state = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};

	/* The rasterization node of the pipeline */
	VkPipelineRasterizationStateCreateInfo rasterizer = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		// It needs to be counter clockwise for reason that I don't understand
		 .frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f, // Optional
		.depthBiasClamp = 0.0f, // Optional
		.depthBiasSlopeFactor = 0.0f // Optional
	};

	// This is really related with multisampling anti aliasing
	// but it requires a gpu feature be enabled
	VkPipelineMultisampleStateCreateInfo multisampling = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f, // Optional
		.pSampleMask = NULL, // Optional
		.alphaToCoverageEnable = VK_FALSE, // Optional
		.alphaToOneEnable = VK_FALSE // Optional
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
						  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
		.colorBlendOp = VK_BLEND_OP_ADD, // Optional
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
		.alphaBlendOp = VK_BLEND_OP_ADD // Optional
	};

	// This is the second struct of fixed function
	VkPipelineColorBlendStateCreateInfo color_blending = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY, // Optional
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants[0] = 0.0f, // Optional
		.blendConstants[1] = 0.0f, // Optional
		.blendConstants[2] = 0.0f, // Optional
		.blendConstants[3] = 0.0f, // Optional
	};

	VkPipelineLayoutCreateInfo pipeline_layout_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = VK_NULL_HANDLE
	};

	if (vkCreatePipelineLayout(logical_device, &pipeline_layout_info, NULL, &render->pipeline_layout) != VK_SUCCESS) {
		print_error("Failed to create pipeline layout!");
		goto destroy_frag_module;
	}

	VkGraphicsPipelineCreateInfo pipeline_info = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = array_size(shader_stages),
		.pStages = shader_stages,
		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_assembly,
		.pViewportState = &viewport_state,
		.pRasterizationState = &rasterizer,
		.pMultisampleState = &multisampling,
		.pColorBlendState = &color_blending,
		.pDynamicState = NULL, // Optional
		.renderPass = render->render_pass,
		.layout = render->pipeline_layout,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE, // Optional
		.basePipelineIndex = -1 // Optional
	};

	if (vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline) != VK_SUCCESS) {
		vkDestroyPipelineLayout(logical_device, render->pipeline_layout, NULL);
		print_error("Failed to create graphics pipeline!");
		goto destroy_frag_module;
	}
	render->graphics_pipeline = pipeline;

	ret = 0;

destroy_frag_module:
	vkDestroyShaderModule(logical_device, frag_shader_module, NULL);
destroy_vert_module:
	vkDestroyShaderModule(logical_device, vert_shader_module, NULL);
destroy_frag_code:
	free(frag_shader_code);
destroy_vert_code:
	free(vert_shader_code);
return_error:
	return ret;
}
