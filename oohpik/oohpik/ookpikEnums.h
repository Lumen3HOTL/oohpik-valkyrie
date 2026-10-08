#pragma once
namespace ookpik {
	namespace mapTileIds {
		enum mapTileId {
			TILE_ERROR,
			EMPTY,
			TREE,
			SEED,
			OWL,
			EXIT,
			FLOOD
		};
	}
	namespace GenerationStages {
		enum GenerationStage {
			STAGE_ERROR,
			GENERATION_ERROR,
			BUILD_ERROR,
			READY,
			WAITING_TO_GENERATE,
			GENERATING,
			WAITING_FOR_THREAD_EXIT,
			GENERATION_DONE,
			WAITING_FOR_BUILD_START,
			BUILDING,
			BUILD_DONE,
			SENDING_EVENT,
			DONE,



		};
	}
}