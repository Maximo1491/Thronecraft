namespace octet
{
	class sky_shader : public shader 
	{
		GLuint uniform_projection, uniform_playerPos, uniform_sunDirection;

	public:
		void init() 
			{
			const char vertex_shader[] = SHADER_STR(
				attribute vec3 pos;
				uniform vec3 playerPos;
				uniform mat4 projection;
				varying vec3 vertex;

				void main()
				{
					gl_Position = projection * vec4(playerPos + pos, 1.0);
					vertex = pos;
				}
			);

			const char fragment_shader[] = SHADER_STR(
				varying vec3 vertex;
				uniform vec3 sunDirection;

				void main()
				{
					vec3 daySky = vec3(0.5294117647, 0.8078431373, 0.9803921569);
					vec3 nightSky = vec3(0.1, 0.1, 0.43);
					vec3 sunsetColor = vec3(0.98, 0.7, 0.4);
					vec3 sunDayColor = vec3(1, 1, 1);
					vec3 moonColor = vec3(1, 1, 1);
					float glowRadius = 0.9;
					float sunRadius = 0.9975;
					float moonRadius = 0.9975;
					float moonGlowRadius = 0.99;
					float sunsetStart = 0.4;
	
					float Ksun = dot(normalize(vertex), normalize(sunDirection));
					float Kmoon = dot(normalize(-vertex), normalize(sunDirection));
					float Ksky;

					if (Ksun > sunRadius)
						Ksun = 1.0;
					else if (Ksun > glowRadius)
						Ksun = (Ksun - glowRadius) / (1.0 - glowRadius);
					else if (Ksun <= glowRadius)
						Ksun = 0.0;

					if (Kmoon > moonRadius)
						Kmoon = 1.0;
					//else if (Kmoon > moonGlowRadius + ((moonRadius - moonGlowRadius) * 0.999))
						//Kmoon = 0.0;
					else if (Kmoon > moonGlowRadius)
						Kmoon = (Kmoon - moonGlowRadius) / (1.0 - moonGlowRadius);
					else
						Kmoon = 0.0;
	
					if (sunDirection.y > -sunsetStart)
						Ksky = (sunDirection.y + sunsetStart) / (1.0 + sunsetStart);
					else
						Ksky = 0.0;
	
					vec3 sunColor = (Ksky * (Ksun * sunDayColor)) + ((1.0 - Ksky) * (Ksun * sunsetColor));
					vec3 skyColor = (Ksky * daySky) + nightSky;
	
					if (Ksun > sunRadius)
						gl_FragColor = vec4(max(Ksky * daySky, sunColor), 1.0);
					//else if (Kmoon > moonGlowRadius)
						//gl_FragColor = vec4(mix(moonColor,  skyColor, 0.35 * Kmoon), 1.0);
					else if (Kmoon > 0.0)
						gl_FragColor = vec4(mix(skyColor, moonColor, 0.7 * Kmoon), 1.0);
					else
						gl_FragColor = vec4(max(skyColor, sunColor), 1.0);
				}
			);

			shader::init(vertex_shader, fragment_shader);

			uniform_projection = glGetUniformLocation(program(), "projection");
			uniform_playerPos = glGetUniformLocation(program(), "playerPos");
			uniform_sunDirection = glGetUniformLocation(program(), "sunDirection");
		}

		void render(glm::mat4 projection, glm::vec3 playerPos, glm::vec3 sunDirection)
		{
			shader::render();

			glUniformMatrix4fv( uniform_projection, 1, GL_FALSE, &projection[0][0] );
			glUniform3f( uniform_playerPos, playerPos.x, playerPos.y, playerPos.z );
			glUniform3f( uniform_sunDirection, sunDirection.x, sunDirection.y, sunDirection.z );
		}
	};
}
