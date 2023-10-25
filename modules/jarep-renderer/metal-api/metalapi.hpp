//
// Created by Sebastian Borsch on 24.10.23.
//

#ifndef JAREP_METALAPI_HPP
#define JAREP_METALAPI_HPP

namespace Graphics::Metal {

	/// Pimpl of the metalAPI class to avoid objective-C++ code in a C++ class
	struct MetalImpl;

	class MetalAPI {
		public:
			MetalAPI();

			~MetalAPI();

			void CreateDevice();
			void CreateCommandQueue();
			void CreateLibrary();

		private:
			MetalImpl* impl;
	};

}
#endif //JAREP_METALAPI_HPP
