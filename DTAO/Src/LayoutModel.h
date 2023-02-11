#pragma once

#include "lve_model.hpp"

namespace lve {

	class LayoutModel : public LveModel
	{
	public:
		LayoutModel(LveDevice& device, MODEL_TYPE model_type, const std::string& data_file);
		~LayoutModel();

		LayoutModel() = delete;
		LayoutModel(const LayoutModel&) = delete;
		LayoutModel& operator=(const LayoutModel&) = delete;

	private:
		LayoutDataManager layout_data;

	public:
		virtual void makeRenderingData(const std::string& file_path = "");
		virtual void loadData(const std::string& file_path = "");
		virtual void makeVertices();
		virtual void makeIndices();

		void makeCubeVertices();
		void makeIndicesForFace();
		void makeIndicesForEdge();

		LayoutDataManager* getLayoutDataManager() {return &this->layout_data;}
			
	};

}//namespace lve


