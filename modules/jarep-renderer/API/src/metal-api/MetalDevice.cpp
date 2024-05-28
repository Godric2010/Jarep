//
// Created by Sebastian Borsch on 26.05.24.
//

#include "MetalDevice.hpp"

namespace Graphics::Metal {

	MetalDevice::~MetalDevice() = default;

	void MetalDevice::Initialize() {
		m_device = MTL::CreateSystemDefaultDevice();
	}

	MTL::Device* MetalDevice::getDevice() const {
		return m_device;
	}

	void MetalDevice::Release() {
		m_device->release();
	}

	uint32_t MetalDevice::GetMaxUsableSampleCount() {
		std::vector<uint32_t> possibleSampleCounts = {64, 32, 16, 8, 4, 2};
		for (const auto sampleCount: possibleSampleCounts) {
			if (m_device->supportsTextureSampleCount(sampleCount))
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

		MTL::Texture* testTexture = m_device->newTexture(descriptor);
		bool isSupported = (testTexture != nullptr);
		if (testTexture) {
			testTexture->release();  // Clean up
		}
		descriptor->release();
		return isSupported;
	}
}