#pragma once

#include "lve_model.hpp"


namespace lve {
    class PEXResistorModel : public LveModel
    {
    public:

        PEXResistorModel(LveDevice& device, MODEL_TYPE model_type, const std::string& data_file = "");
        ~PEXResistorModel();

        PEXResistorModel() = delete;
        PEXResistorModel(const PEXResistorModel&) = delete;
        PEXResistorModel& operator=(const PEXResistorModel&) = delete;

    private:
        PEXDataManager pex_data;
        double res_cube_thickness;
        

    public:
        virtual void makeRenderingData(const std::string& file_path = "");
        virtual void loadData(const std::string& file_path = "");
        virtual void makeVertices();
        virtual void makeIndices();

        void makeCubeVertices();
        void makeCubes();
        void makeCube(const PEXResistor& in_res, cube_info& out_cube, PEXResDirection direction);
        void setResCubeThickness(double value) { this->res_cube_thickness = value; }

    private:

    };
}


