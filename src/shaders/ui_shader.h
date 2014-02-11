namespace octet {
	class ui_shader : public shader
	{
		GLuint matrixIndex_, colorIndex_, samplerIndex_;

	public:
		void init()
		{
			const char vertex_shader[] = SHADER_STR(

			attribute vec2 pos;
			attribute vec2 uv;
			uniform mat4 matrix;
			uniform vec4 uicolor;

			varying vec4 f_color;
			varying vec2 uv_;

			void main() {
				gl_Position = matrix * vec4(pos, 0.0, 1.0);
				uv_ = uv;
				f_color = vec4(uicolor);
			}
			);

			const char fragment_shader[] = SHADER_STR(
				varying vec2 uv_;
				varying vec4 f_color;

				uniform sampler2D sampler;

			void main()
			{
				gl_FragColor = texture2D(sampler, uv_);

				if (f_color.w != 0.0)
					gl_FragColor = f_color;
					
			}
			);

			shader::init(vertex_shader, fragment_shader);
			matrixIndex_ = glGetUniformLocation(program(), "matrix");
			colorIndex_ = glGetUniformLocation(program(), "uicolor");
			samplerIndex_ = glGetUniformLocation(program(), "sampler");
		}

		void render(glm::mat4 &view, glm::vec4 &color, int texture)
		{
			shader::render();
			glUniformMatrix4fv(matrixIndex_, 1, GL_FALSE, glm::value_ptr(view));
			glUniform4f(colorIndex_, color.x, color.y, color.z, color.w);
			glUniform1i(samplerIndex_, texture);
		}
	};
}
