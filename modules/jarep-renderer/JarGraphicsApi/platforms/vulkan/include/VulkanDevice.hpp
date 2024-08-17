//
// Created by sebastian on 15.08.24.
//

#pragma once

#include "../../../include/JarDevice.hpp"

class VulkanDevice : public JarDevice {

	public:
	VulkanDevice(std::shared_ptr<JarSurface> surface) {};
	~VulkanDevice() override = default;

	void Release() override;

	uint32_t GetMaxUsableSampleCount() override;

	bool IsPixelFormatSupported() override;

	private:

};
