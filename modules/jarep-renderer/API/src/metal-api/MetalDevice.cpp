//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalDevice.hpp"

namespace Graphics::Metal {

	MetalDevice::~MetalDevice() = default;

	void MetalDevice::Initialize() {
		_device = std::make_optional(MTL::CreateSystemDefaultDevice());
	}

	std::optional<MTL::Device*> MetalDevice::getDevice() const {
		return _device;
	}

	void MetalDevice::Release() {
		if (!_device.has_value()) return;
		_device.value()->release();
	}

	uint32_t MetalDevice::GetMaxUsableSampleCount() {
		MTL::Device* device = _device.value();
		std::vector<uint32_t> possibleSampleCounts = {64, 32, 16, 8, 4, 2};
		for (const auto sampleCount: possibleSampleCounts) {
			if (device->supportsTextureSampleCount(sampleCount))
				return sampleCount;
		}
		return 1;
	}

	bool MetalDevice::IsFormatSupported(Graphics::PixelFormat format) {
		MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::alloc()->init();
		descriptor->setTextureType(MTL::TextureType2D);
		descriptor->setPixelFormat(pixelFormatMap[format]);
		descriptor->setWidth(1);  // Minimum size
		descriptor->setHeight(1);
		descriptor->setDepth(1);
		descriptor->setStorageMode(MTL::StorageModePrivate);
		descriptor->setUsage(MTL::TextureUsageRenderTarget);

		MTL::Texture* testTexture = _device.value()->newTexture(descriptor);
		bool isSupported = (testTexture != nullptr);
		if (testTexture) {
			testTexture->release();  // Clean up
		}
		descriptor->release();
		return isSupported;
	}
}