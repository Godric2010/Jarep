//
// Created by sebastian on 15.08.24.
//

#pragma once

#include "JarSurface.hpp"
#include <memory>

class JarDevice {
	public:
	static std::shared_ptr<JarDevice> Create(std::shared_ptr<JarSurface> surface);

	virtual ~JarDevice() = default;

	virtual void Release() = 0;

	virtual uint32_t GetMaxUsableSampleCount() = 0;

	virtual bool IsPixelFormatSupported() = 0;
};
