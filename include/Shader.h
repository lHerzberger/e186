#pragma once
#include <unordered_set>
#include <set>
#include <array>

namespace e186
{
	
	class Shader
	{
		static GLuint Compile(const char* source, GLenum shaderType);
		void CheckErrorAndPrintInfoLog(const char* gl_error_location_hint, const char* info_log_description);
		void PrintInfoLog(const char* info_log_description);
		void DetermineTessData();
		void DetermineVertexAttribConfig();
		void DeterminePrimitivesMode();

		static std::string LoadFromFile(const std::string& path);

		void DetermineWhichAutoMatsToCalc();
		void PrepareAutoMatActionConfigs();
		void CreateAutoMatCalcers();

	public:
		Shader();
		Shader(const Shader& other) = delete;
		Shader(Shader&& other) noexcept;
		Shader& operator=(const Shader& other) = delete;
		Shader& operator=(Shader&& other) noexcept;
		~Shader();

		explicit operator GLuint() const;
		GLuint handle() const;

		Shader& AddVertexShaderSource(std::string shaderSource);
		Shader& AddTesselationControlShaderSource(std::string shaderSource);
		Shader& AddTesselationEvaluationShaderSource(std::string shaderSource);
		Shader& AddGeometryShaderSource(std::string shaderSource);
		Shader& AddFragmentShaderSource(std::string shaderSource, std::vector<std::tuple<GLuint, const GLchar*>> outputs);
		Shader& AddComputeShaderSource(std::string shaderSource);
		Shader& AddVertexShaderSourceFromFile(std::string path);
		Shader& AddTesselationControlShaderSourceFromFile(std::string path);
		Shader& AddTesselationEvaluationShaderSourceFromFile(std::string path);
		Shader& AddGeometryShaderSourceFromFile(std::string path);
		Shader& AddFragmentShaderSourceFromFile(std::string path, std::vector<std::tuple<GLuint, const GLchar*>> outputs);
		Shader& AddComputeShaderSourceFromFile(std::string path);
		Shader& SetTransformFeedbackVaryings(std::vector<const char*> varyings, GLenum buffer_mode);
		Shader& Build();
		Shader& QueryOptionalUniformLocations(const std::vector<std::string>& names);
		Shader& QueryUniformLocations(const std::vector<std::string>& names);
		Shader& QueryMandatoryUniformLocations(const std::vector<std::string>& names);
		Shader& QueryOptionalUniformLocation(const std::string& name);
		Shader& QueryUniformLocation(const std::string& name);
		Shader& QueryMandatoryUniformLocation(const std::string& name);
		Shader& DeclareAutoMatrix(std::string name, AutoMatrix properties);
		Shader& Destroy();
		bool HasUniform(const std::string& name) const;
		GLuint GetOptionalUniformLocation(const std::string& name);
		GLuint GetUniformLocation(const std::string& name);
		GLuint GetMandatoryUniformLocation(const std::string& name);
		bool HasTesselationShaders() const;
		GLint patch_vertices() const;
		bool HasGeometryShader() const;
		unsigned int vertex_attrib_config() const;
		GLenum kind_of_primitives() const;
		void set_kind_of_primitives(GLenum mode);
		void SetAutoMatrices(const glm::mat4& transformationMatrix, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

		void Use() const 
		{
			glUseProgram(m_prog_handle);
		}

		void SetUniform(GLuint location, const glm::vec4& value) const
		{
			glUniform4fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(value)));
		}

		void SetUniform(GLuint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const
		{
			glUniform4f(location, x, y, z, w);
		}

		void SetUniform(GLuint location, const glm::vec3& value) const
		{
			glUniform3fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(value)));
		}

		void SetUniform(GLuint location, GLfloat x, GLfloat y, GLfloat z) const
		{
			glUniform3f(location, x, y, z);
		}

		void SetUniform(GLuint location, GLfloat value) const
		{
			glUniform1f(location, value);
		}

		void SetUniform(GLuint location, GLint value) const
		{
			glUniform1i(location, value);
		}

		void SetUniform(GLuint location, const glm::vec2& value) const
		{
			glUniform2fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(value)));
		}

		void SetUniform(GLuint location, GLfloat x, GLfloat y) const
		{
			glUniform2f(location, x, y);
		}

		void SetUniform(GLuint location, const glm::mat3& value) const
		{
			glUniformMatrix3fv(location, 1, GL_FALSE, static_cast<const GLfloat*>(glm::value_ptr(value)));
		}

		void SetUniform(GLuint location, const glm::mat4& value) const
		{
			glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const GLfloat*>(glm::value_ptr(value)));
		}

		void SetSampler(GLuint location, GLenum tex_target, GLuint tex_handle, GLuint texture_unit = 0) const
		{
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + texture_unit));
			glBindTexture(tex_target, tex_handle);
			glUniform1i(location, static_cast<GLint>(texture_unit));
		}

		void SetSampler(GLuint location, const TexInfo& value, GLuint texture_unit = 0) const
		{
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + texture_unit));
			value.Bind();
			glUniform1i(location, static_cast<GLint>(texture_unit));
		}
			
		void SetImageTexture(GLuint location, const TexInfo& value, GLuint unit, GLint level, GLboolean layered, GLint layer, GLenum access)
		{
			glBindImageTexture(unit, value.handle(), level, layered, layer, access, value.internal_format());
			glUniform1i(location, static_cast<GLint>(unit));
		}
			

		template <typename... Args>
		void SetOptionalUniform(const std::string uniform_name, Args&&... args)
		{
			const auto loc = GetOptionalUniformLocation(uniform_name);
			if (-1 != loc) 
				SetUniform(loc, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void SetUniform(const std::string uniform_name, Args&&... args)
		{
			SetUniform(GetUniformLocation(uniform_name), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void SetMandatoryUniform(const std::string uniform_name, Args&&... args)
		{
			SetUniform(GetMandatoryUniformLocation(uniform_name), std::forward<Args>(args)...);
		}


		template <typename... Args>
		void SetOptionalSampler(const std::string uniform_name, Args&&... args)
		{
			const auto loc = GetOptionalUniformLocation(uniform_name);
			if (-1 != loc)
				SetSampler(loc, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void SetSampler(const std::string uniform_name, Args&&... args)
		{
			SetSampler(GetUniformLocation(uniform_name), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void SetMandatorySampler(const std::string uniform_name, Args&&... args)
		{
			SetSampler(GetMandatoryUniformLocation(uniform_name), std::forward<Args>(args)...);
		}


		template <typename... Args>
		void SetOptionalImageTexture(const std::string uniform_name, Args&&... args)
		{
			const auto loc = GetOptionalUniformLocation(uniform_name);
			if (-1 != loc)
				SetImageTexture(loc, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void SetImageTexture(const std::string uniform_name, Args&&... args)
		{
			SetImageTexture(GetUniformLocation(uniform_name), std::forward<Args>(args)...);
		}

		template <typename... Args>
		void SetMandatoryImageTexture(const std::string uniform_name, Args&&... args)
		{
			SetImageTexture(GetMandatoryUniformLocation(uniform_name), std::forward<Args>(args)...);
		}

	private:
		GLuint m_prog_handle;
		std::string m_vertex_shader_source;
		std::string m_tess_control_shader_source;
		std::string m_tess_eval_shader_source;
		std::string m_geometry_shader_source;
		std::string m_fragment_shader_source;
		std::string m_compute_shader_source;
		std::vector<std::tuple<GLuint, const GLchar*>> m_fragment_outputs;
		std::unordered_map<std::string, GLuint> m_uniform_locations;
		std::vector<const char*> m_transform_feedback_varyings;
		GLenum m_transform_feedback_buffer_mode;
		GLint m_patch_vertices;
		static const int kMaxShaders = 6;
		std::array<GLuint, kMaxShaders> m_shaderHandles;
		unsigned int m_vertex_attrib_config;
		GLenum m_kind_of_primitives;
		std::vector<std::tuple<std::string, AutoMatrix>> m_auto_matrices;
		std::vector<std::tuple<GLuint, AutoMatrix>> m_auto_mats_action_config;
		std::vector<std::tuple<GLuint, AutoMatrix>> m_auto_mats_action_config_nrm;
		std::array<bool, 16> m_auto_mat_do_calc;
		std::array<glm::mat4, 16> m_auto_mat_action_cache;
		std::vector<std::function<void()>> m_auto_mat_calcers;
	};

	void RenderVAO(const Shader& shader, VAOType vao, GLuint indices_len);
	void RenderMesh(const Shader& shader, Mesh& mesh);
	void RenderMeshes(const Shader& shader, const std::vector<std::tuple<MeshRef, VAOType>>& meshes_and_their_vaos);
	void RenderMeshesWithAlignedUniformSetters(const Shader& shader, const std::vector<std::tuple<MeshRef, VAOType>>& meshes_and_their_vaos, const std::vector<std::tuple<MeshRef, UniformSetter>>& uniform_setters);
	void UnbindVAO();
}
