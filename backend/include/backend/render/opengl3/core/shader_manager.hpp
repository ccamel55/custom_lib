#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace lib::backend::gl3
{
	class shader_manager
	{
	public:
		shader_manager();
		shader_manager(const std::string& vertex_shader, const std::string& fragment_shader);

		~shader_manager();

		//! Bind shader to render state.
		void bind() const;

		//! Restore render state shader.
		void unbind() const;

		//! Bind a uniform \a matrix to the shader.
		void bind_uniform_mat4(const std::string& uniformName, const glm::mat4& matrix) const;

		//! Bind a uniform \a v0 the shader.
		void bind_uniform(const std::string& uniform_name, int v0) const;

	private:
		static uint32_t gen_shader(const std::string& shader, uint32_t type);

	private:
		uint32_t _shader_id = 0;
	};
}