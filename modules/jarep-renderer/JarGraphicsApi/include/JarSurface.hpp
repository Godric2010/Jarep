//
// Created by sebastian on 15.08.24.
//

#pragma once
#include "JarBackend.hpp"
#include "JarExtent2D.hpp"
#include "NativeWindowHandleProvider.hpp"
#include <memory>

class JarSurface {
	public:
	static std::shared_ptr<JarSurface> Create(std::shared_ptr<JarBackend>& backend, NativeWindowHandleProvider* windowHandle);

	virtual ~JarSurface() = default;
	virtual void RecreateSurface(JarExtent2D extent) = 0;
	virtual void Release() = 0;
	virtual uint32_t GetSwapchainImageAmount() = 0;
	virtual JarExtent2D GetSurfaceExtent() = 0;
};
