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

