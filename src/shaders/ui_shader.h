namespace octet {
	class ui_shader : public shader
	{
		GLuint vec2_vp, vec2_vt;

	public:
		void init()
		{
			const char vertex_shader[] = SHADER_STR(

				attribute vec2 position;
				attribute vec2 texcoord;
				uniform mat4 matrix;
				varying vec2 st;

				void main() {
					st = texcoord;
					gl_Position = matrix * vec4(position, 0.0, 1.0);
				}
			);

			const char fragment_shader[] = SHADER_STR(

				void main()
				{
					gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);
				}
			);

			shader::init(vertex_shader, fragment_shader);
			/*vec2_vp = glGetUniformLocation(program(), "vp");
			vec2_vt = glGetUniformLocation(program(), "vt");*/
		}

		void render()
		{
			shader::render();
		}
	};
}
