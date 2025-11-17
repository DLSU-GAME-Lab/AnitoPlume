#pragma once

#include "../../../math/vec/vec.hpp"
#include "vcl/base/base.hpp"
#include "vcl/containers/buffer/buffer.hpp"

#include <vector>
#include <array>

namespace vcl
{

    struct skybox
    {
        /** Per-vertex attributes */
        buffer<vec3> vertices;

        /** Triangle connectivity */
        buffer<uint3> connectivity;

        skybox();
    };

}