//
// Created by sebastian on 03.08.24.
//
#pragma once

#include <memory>
#include <vector>

class JarBackend {
	public:
	virtual ~JarBackend() = default;

	static std::shared_ptr<JarBackend> Create(const std::vector<const char*>& extensionNames);

	protected:
	JarBackend() = default;
};