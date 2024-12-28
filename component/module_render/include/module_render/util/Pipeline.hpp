#pragma once

#include <dep_nvrhi/nvrhi.hpp>

namespace lib::render {

template <typename Pipeline_Id, size_t Num_Ids>
requires std::is_scoped_enum_v<Pipeline_Id>
class Pipeline {
public:
    explicit Pipeline(
        nvrhi::IDevice* device
    )
        : _device(device) {

    }

    nvrhi::IGraphicsPipeline* operator[](Pipeline_Id id) const {
        return _pipeline.at(static_cast<size_t>(id));
    }

    void back_buffer_resized(std::function<void(std::array<nvrhi::GraphicsPipelineHandle, Num_Ids>&)> cb) {
        cb(_pipeline);
    }

protected:
    nvrhi::IDevice* _device                                         = nullptr;
    std::array<nvrhi::GraphicsPipelineHandle, Num_Ids> _pipeline    = {};

};

}