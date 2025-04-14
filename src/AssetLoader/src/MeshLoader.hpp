//
// Created by Godri on 4/13/2025.
//

#pragma once
#include <tiny_obj_loader.h>
#include <stdexcept>
#include <iostream>
#include "AssetLoader/IMeshLoader.hpp"


namespace JAREP::AssetLoader {
	class MeshLoader : public IMeshLoader {
		public:
			MeshLoader();

			~MeshLoader() override;

			Core::Types::Mesh LoadMeshAsObj(std::string&meshName) override;
	};
}
