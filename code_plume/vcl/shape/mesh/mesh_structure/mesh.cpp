#include "mesh.hpp"

#include "vcl/math/math.hpp"

namespace vcl
{

void mesh::push_back(const mesh& mesh_to_add)
{
    fill_empty_fields();
    mesh m = mesh_to_add;
    m.fill_empty_fields();

    const size_t N0 = position.size();
    const size_t N = m.position.size();
    for(size_t k=0; k<N; ++k )
    {
        position.push_back(m.position[k]);
        normal.push_back(m.normal[k]);
        color.push_back(m.color[k]);
        texture_uv.push_back(m.texture_uv[k]);
        tangent.push_back(m.tangent[k]);
        bitangent.push_back(m.bitangent[k]);
    }

    const size_t N_tri = m.connectivity.size();
    for(size_t k=0; k<N_tri; ++k)
    {
        uint3 index = m.connectivity[k];
        for(size_t kv=0; kv<3; ++kv)
            index[kv] += static_cast<unsigned int>(N0);

        connectivity.push_back(index);
    }

}

void mesh::fill_empty_fields()
{
    const size_t N = position.size();
    if(N==0)
        return ;

    assert_vcl( N>0, "Mesh doesn't have any vertex");
    assert_vcl( connectivity.size()>0, "Connectivity doesn't have any triangle" );

    if( normal.size()<N )
        normal = vcl::normal(position, connectivity);

    if( color.size()<N )
    {
        color.resize(N);
        for(size_t k=0; k<N; ++k)
            color[k] = vec4(1,1,1,1);
    }

    if( texture_uv.size()<N )
    {
        texture_uv.resize(N);
        for(size_t k=0; k<N; ++k)
            texture_uv[k] = vec2(0,0);
    }
    
    if( tangent.size()<N )
    {
        tangent.resize(N);
        for(size_t k=0; k<N; ++k)
            tangent[k] = vec3(0,0,0);
    }
    
    if( bitangent.size()<N )
    {
        bitangent.resize(N);
        for(size_t k=0; k<N; ++k)
            bitangent[k] = vec3(0,0,0);
    }

}

void normal(const buffer<vec3>& position, const buffer<uint3>& connectivity, buffer<vec3>& normals,bool invert)
{

    const size_t N = position.size();
    if(normals.size() != N)
        normals.resize(N);
    else
    {
        for(size_t k=0; k<N; ++k)
            normals[k] = vec3(0,0,0);
    }

    const size_t N_tri = connectivity.size();
    for(size_t k_tri=0; k_tri<N_tri; ++k_tri)
    {
        const uint3& f = connectivity[k_tri];
        assert_vcl_no_msg(f[0]<N);
        assert_vcl_no_msg(f[1]<N);
        assert_vcl_no_msg(f[2]<N);

        const vec3& p0 = position[f[0]];
        const vec3& p1 = position[f[1]];
        const vec3& p2 = position[f[2]];

        // Compute normal of the triangle
        const vec3& p10 = normalize(p1-p0);
        const vec3& p20 = normalize(p2-p0);
        const vec3& n = normalize( cross(p10,p20) );

        // Add normal direction to all vertices of this triangle
        for(size_t k=0; k<3; ++k)
            normals[f[k]] += n;
    }

    // Normalize all normals
    for(size_t k=0; k<N; ++k)
        normals[k] = normalize(normals[k]);

    if(invert)
        for(size_t k=0; k<N; ++k)
            normals[k] = -normals[k];

}

buffer<vec3> normal(const buffer<vec3>& position, const buffer<uint3>& connectivity)
{
    buffer<vec3> normals(position.size());
    normal(position,connectivity,normals);

    return normals;

}

void mesh::fill_color_uniform(const vec3& c)
{
    fill_color_uniform(vec4(c.x,c.y,c.z,1.0f));
}
void mesh::fill_color_uniform(const vec4& c)
{
    const size_t N = position.size();
    if(color.size()!=N)
        color.resize(N);

    for(size_t k=0; k<N; ++k)
        color[k] = c;
}


vec3 center_of_mass(const mesh& shape)
{
    return center_of_mass(shape.position, shape.connectivity);
}

vec3 center_of_mass(const buffer<vec3>& position, const buffer<uint3>& connectivity)
{
    const size_t N_vertex = position.size();
    assert( N_vertex>0 );

    vec3 COM = {0,0,0};
    float total_area = 0.0f;
    const size_t N_tri = connectivity.size();
    for(size_t k_tri=0; k_tri<N_tri; ++k_tri) {
        const uint3& t = connectivity[k_tri];

        assert(t[0]<N_vertex);
        assert(t[1]<N_vertex);
        assert(t[2]<N_vertex);

        const vec3& p0 = position[t[0]];
        const vec3& p1 = position[t[1]];
        const vec3& p2 = position[t[2]];

        const float area = 0.5f * norm(cross(p1-p0,p2-p0));
        COM += area * (p0+p1+p2)/3.0f;
        total_area += area;
    }

    if( total_area>1e-8f )
        COM /= total_area;
    else
        COM = position[0];

    return COM;
}

buffer<vec3> tangent(const buffer<vec3>& position, const buffer<vec2>& texture_uv, const buffer<uint3>& connectivity)
{
    buffer<vec3> tangents(position.size());
    tangent(position, texture_uv, connectivity, tangents);

    return tangents;
}

buffer<vec3> bitangent(const buffer<vec3>& position, const buffer<vec2>& texture_uv, const buffer<uint3>& connectivity)
{
    buffer<vec3> bitangents(position.size());
    bitangent(position, texture_uv, connectivity, bitangents);

    return bitangents;
}

void tangent(const buffer<vec3>& position, const buffer<vec2>& texture_uv, const buffer<uint3>& connectivity, buffer<vec3>& tangent)
{

    const size_t N = position.size();

    if (tangent.size() != N)
        tangent.resize(N);
    else
    {
        for (size_t k = 0; k < N; ++k)
            tangent[k] = vec3(0, 0, 0);
    }
    
    const size_t N_vertex = position.size();
    assert(N_vertex > 0);

    const size_t N_tri = connectivity.size();
    for (size_t i = 0; i < N_tri; ++i)
    {
        const uint3& t = connectivity[i];

        assert(t[0] < N_vertex);
        assert(t[1] < N_vertex);
        assert(t[2] < N_vertex);

        const vec3& p0 = position[t[0]];
        const vec3& p1 = position[t[1]];
        const vec3& p2 = position[t[2]];

        const vec2& uv0 = texture_uv[t[0]];
        const vec2& uv1 = texture_uv[t[1]];
        const vec2& uv2 = texture_uv[t[2]];

        const vec3 dp0 = p1 - p0;
        const vec3 dp1 = p2 - p0;
        
        const vec2 duv0 = uv1 - uv0;
        const vec2 duv1 = uv2 - uv0;

        const float r = 1.0f / (duv0.x * duv1.y - duv1.x * duv0.y);
        const vec3& tan = (dp0 * duv1.y - dp1 * duv0.y) * r;

        for (size_t k = 0; k < 3; ++k)
            tangent[t[k]] = tan;
    }
}

void bitangent(const buffer<vec3>& position, const buffer<vec2>& texture_uv, const buffer<uint3>& connectivity, buffer<vec3>& bitangent)
{

    const size_t N = position.size();

    if (bitangent.size() != N)
        bitangent.resize(N);
    else
    {
        for (size_t k = 0; k < N; ++k)
            bitangent[k] = vec3(0, 0, 0);
    }

    const size_t N_vertex = position.size();
    assert(N_vertex > 0);

    const size_t N_tri = connectivity.size();
    for (size_t i = 0; i < N_tri; ++i)
    {
        const uint3& t = connectivity[i];

        assert(t[0] < N_vertex);
        assert(t[1] < N_vertex);
        assert(t[2] < N_vertex);

        const vec3& p0 = position[t[0]];
        const vec3& p1 = position[t[1]];
        const vec3& p2 = position[t[2]];

        const vec2& uv0 = texture_uv[t[0]];
        const vec2& uv1 = texture_uv[t[1]];
        const vec2& uv2 = texture_uv[t[2]];

        const vec3 dp0 = p1 - p0;
        const vec3 dp1 = p2 - p0;
        
        const vec2 duv0 = uv1 - uv0;
        const vec2 duv1 = uv2 - uv0;

        const float r = 1.0f / (duv0.x * duv1.y - duv1.x * duv0.y);
        const vec3& bitan = (dp1 * duv0.x - dp0 * duv1.x) * r;

        for (size_t k = 0; k < 3; ++k)
            bitangent[t[k]] = bitan;
    }
}

}
