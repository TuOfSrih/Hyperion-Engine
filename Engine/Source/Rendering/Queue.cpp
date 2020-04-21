
#include "Queue.hpp"

#include "System/Debug.hpp"
#include "RenderContext.hpp"


namespace Hyperion::Rendering {

	const vk::Queue& QueueTypeInfo::getQueue() const
	{
			switch (type) {
			case QueueType::GRAPHICS:
				return RenderContext::active->getGraphicsQueue();
				break;
			case QueueType::COMPUTE:
				return RenderContext::active->getComputeQueue();
				break;
			case QueueType::TRANSFER:
				return RenderContext::active->getTransferQueue();
				break;
			default:
				Debug::runtimeError("Unknown Queue identifier");
			}
	}

	const vk::CommandPool& QueueTypeInfo::getCommandPool() const 
	{
		switch (type) {
		case QueueType::GRAPHICS:
			return RenderContext::active->getCurrentGraphicsPool();
			break;
		case QueueType::COMPUTE:
			return RenderContext::active->getComputePool();
			break;
		case QueueType::TRANSFER:
			return RenderContext::active->getTransferPool();
			break;
		default:
			Debug::runtimeError("Unknown Queue identifier");
		}
	}
}


