//
// Created by sebastian on 16.10.23.
//

#ifndef JAREP_IRENDERER_HPP
#define JAREP_IRENDERER_HPP

namespace Graphics {

	class IRenderer {
		public:
			virtual ~IRenderer() = default;

			virtual int Initialize() = 0;

			virtual void Render() = 0;

			virtual void Shutdown() = 0;
	};

}


#endif //JAREP_IRENDERER_HPP
