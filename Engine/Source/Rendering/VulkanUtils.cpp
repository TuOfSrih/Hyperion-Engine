#include "VulkanUtils.hpp"

#include "RenderContext.hpp"

namespace Hyperion::Rendering::Vulkan {

	SharingInfo::SharingInfo(const std::vector<uint32_t>& queueIndices): queueIndices(queueIndices)
	{
	}
	bool SharingInfo::isShared() const
	{
		return queueIndices.size() > 0;
	}
	vk::SharingMode SharingInfo::getSharingMode() const
	{
		return (isShared() ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive);
	}
	const uint32_t* SharingInfo::getIndices() const
	{
		return queueIndices.data();
	}
	size_t SharingInfo::getAmount() const
	{
		return queueIndices.size();
	}
	vk::CommandBuffer simpleExecuteTransfer(const std::function<void(const vk::CommandBuffer& cmdBuffer)>& func)
	{
		const vk::Device& device = RenderContext::active->getDevice();

		vk::CommandBuffer cmdBuffer = device.allocateCommandBuffers(
			{
				Rendering::RenderContext::active->getTransferPool()
			}
		).at(0);

		cmdBuffer.begin(
			{
				vk::CommandBufferUsageFlagBits::eOneTimeSubmit
			}
		);

		func(cmdBuffer);

		cmdBuffer.end();

		const vk::SubmitInfo submitInfo{
			0,
			nullptr,
			nullptr,
			1,
			&cmdBuffer,
			0,
			nullptr
		};
		RenderContext::active->getTransferQueue().submit(
			1,
			&submitInfo,
			{}
		);
		
		return cmdBuffer;
	}
}


