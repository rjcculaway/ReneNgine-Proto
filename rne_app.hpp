#pragma once

#include "rne_window.hpp"
#include "rne_pipeline.hpp"

namespace rne {
	class RneApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();
	private:
		RneWindow rneWindow{ WIDTH, HEIGHT, "ReneNgine Proto" };
		RnePipeline rnePipeline { "./vertex.vert.spv", "./fragment.frag.spv" };
	};
}