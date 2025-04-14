//
// Created by Godri on 4/13/2025.
//

#pragma once

#include <string>
#include "EngineCore/Types/Mesh.hpp"

namespace JAREP::AssetLoader {
	class IMeshLoader {
		public:
			virtual ~IMeshLoader() = default;

			virtual Core::Types::Mesh LoadMeshAsObj(std::string&meshName) =0;
	};
}
