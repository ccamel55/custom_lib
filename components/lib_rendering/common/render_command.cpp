#include <lib_rendering/common/render_command.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace lib::rendering;

void render_command::start_new(const lib::point2Di& screen_size)
{
	// generate new projection matrix, view and model matrices are identity by default, so leave alone for now.
	// note: vulkan has y co-oridnates flipped from opengl
#if defined(DEF_LIB_RENDERING_gl3)
	ubo.projection_matrix = glm::ortho(
		0.f,
		static_cast<float>(screen_size.x),
		static_cast<float>(screen_size.y),
		0.f);
#elif defined(DEF_LIB_RENDERING_vulkan)
	ubo.projection_matrix = glm::ortho(
		0.f,
		static_cast<float>(screen_size.x),
		0.f,
		static_cast<float>(screen_size.y));
#else
#error "render_command: did not specify projection matrix for render api"
#endif

	// reset iterators to start of array, we will simply write over existing values
	vertex_count = 0;
	index_count = 0;
	batch_count = 0;
}

uint32_t render_command::prepare_batch(const lib::point4Di& clipped_area, shader_type shader_type)
{
	// only update batch when batch count is empty or if we update clipped area
	if (batch_count != 0 && current_batch->clipped_area == clipped_area && shader_type == current_batch->shader)
	{
		return vertex_count;
	}

	// update iterator position (aka add new iterator) then increment internal counter
	current_batch = batches.begin() + batch_count;
	batch_count += 1;

	// reset all members to a known state
	current_batch->offset = index_count;
	current_batch->count = 0;
	current_batch->clipped_area = clipped_area;
	current_batch->shader = shader_type;

	return vertex_count;
}

std::array<vertex_t, MAX_VERTICES>::iterator render_command::insert_vertices(uint32_t size)
{
	const auto ret = vertices.begin() + vertex_count;
	vertex_count += size;

	return ret;
}

std::array<uint32_t, MAX_INDICES>::iterator render_command::insert_indices(uint32_t size)
{
	const auto ret = indices.begin() + index_count;

	index_count += size;
	current_batch->count += size;

	return ret;
}