#pragma once

#include "rne_descriptors.hpp"
#include "rne_device.hpp"
#include "rne_game_object.hpp"
#include "rne_renderer.hpp"
#include "rne_window.hpp"

#include <memory>
#include <vector>

namespace rne {
	class RneApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		RneApp();
		~RneApp();

		RneApp(const RneApp&) = delete;
		RneApp& operator=(const RneApp&) = delete;

		void run();
	private:
		void loadGameObjects();

		RneWindow rneWindow{ WIDTH, HEIGHT, "ReneNgine Proto" };
		RneDevice rneDevice{ rneWindow };
		RneRenderer rneRenderer{ rneWindow, rneDevice };

		// order of declarations matters
		std::unique_ptr<RneDescriptorPool> globalPool{};

		std::vector<RneGameObject> gameObjects;
		//RnePipeline rnePipeline { rneDevice, "./vertex.vert.spv", "./fragment.frag.spv", RnePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};
}