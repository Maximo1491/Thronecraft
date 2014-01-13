namespace octet
{

  class chunk
	{
	public:

		bool m_active;
		mesh chunk_mesh;
		mat4t modelToWorld;
    material mat;
    voxel*** voxels;
    voxel::VoxelType vt;
		int chunk_size;
		voxel v;
		
		chunk() { }

		~chunk() { }

		void init(mat4t mToW)
		{
			chunk_size = 16;

			voxels = new voxel**[chunk_size];

      for(int i = 0; i < chunk_size; i++)
      {
				voxels[i] = new voxel*[chunk_size];

        for(int j = 0; j < chunk_size; j++)
        {
					voxels[i][j] = new voxel[chunk_size];
					
					for(int k = 0; k < chunk_size; k++)
					{
						if(k % 2 > 0)
							v.init(voxel::VoxelType::voxelType_dirt);
						else
							v.init(voxel::VoxelType::voxelType_grass);

						voxels[i][j][k] = v;
					}
        }
      }

      voxels[0][8][8].setActive(false);
      voxels[1][8][8].setActive(false);
      voxels[8][0][8].setActive(false);
      voxels[8][8][0].setActive(false);
      voxels[chunk_size-1][8][8].setActive(false);
      voxels[8][chunk_size-1][8].setActive(false);
      voxels[8][8][chunk_size-1].setActive(false);

			chunk_mesh.make_chunk(voxels, chunk_size);

			//chunk_mesh = _mesh;
			modelToWorld = mToW;
			mat.make_color(vec4(1, 0, 0, 1), false, false);
		}

		void render(bump_shader &shader, const mat4t &worldToCamera, const mat4t &cameraToProjection, vec4 *light_uniforms, int num_light_uniforms, int num_of_lights)
		{
			//modelToWorld.rotateX(1.0f);
			//modelToWorld.rotateY(1.0f);
			//modelToWorld.rotateZ(1.0f);
			mat4t modelToCamera = modelToWorld * worldToCamera;
			mat4t modelToProjection = modelToCamera * cameraToProjection;
			mat.render(shader, modelToProjection, modelToCamera, light_uniforms, num_light_uniforms, num_of_lights);
      chunk_mesh.render();
			
		}

		bool isActive()
		{
			return m_active;
		}

		void setActive(bool _active)
		{
			m_active = _active;
		}

		mat4t getModelToWorld()
		{
			return modelToWorld;
		}
	};
	
	class voxel_app : public app
	{
	public:
		dynarray<voxel> voxels;
		vec3 numberOfVoxels;
		
		mat4t cameraToWorld;
		mat4t modelToWorld;

		bump_shader _bump_shader;
		//material voxel_mat;
		mesh voxel_mesh;
		material floor_mat;
		mesh floor_mesh;

    chunk chunks[1][1][1];
    int numChunks;

		voxel_app(int argc, char **argv) : app(argc, argv) { }

		void app_init()
		{
			//Enter the number of x, y, and z voxels you want rendered.
			numberOfVoxels = vec3(16, 16, 16);
			voxels.resize((int)(numberOfVoxels.x() * numberOfVoxels.y() * numberOfVoxels.z()));

			_bump_shader.init();

			//Change to change the size of the voxel.
//			voxel_mesh.make_cube(0.5f);
			//voxel_mat.make_color(vec4(1, 0, 0, 1), false, false);

			modelToWorld.loadIdentity();
			//modelToWorld.translate(0, 0, -100.0f);

			cameraToWorld.loadIdentity();
			//cameraToWorld.rotate(-45.0f, 0, 1, 0);
			cameraToWorld.translate(0, 0, -100.0f);


      numChunks = 1;

      for(int x = 0; x < numChunks; x++)
      {
        for(int y = 0; y < numChunks; y++)
        {
          for(int z = 0; z < numChunks; z++)
          {
            chunks[x][y][z].init(modelToWorld);
            modelToWorld.translate(0, 0, 32);
          }
          modelToWorld.translate(0, 32, -32*numChunks);
        }
        modelToWorld.translate(32, -32*numChunks, 0);
      }

			//Increasing this inreases space between voxels.
			float voxel_spacing = 1.0f;

			/*for (int i = 0; i < numberOfVoxels.z(); i++)
			{
				for (int j = 0; j < numberOfVoxels.y(); j++)
				{
					for (int k = 0; k < numberOfVoxels.x(); k++)
					{
						voxel::VoxelType vt;

						if ((int)((i * numberOfVoxels.y() * numberOfVoxels.x()) + (j * numberOfVoxels.x()) + k) % 2 > 0)
							vt = voxel::VoxelType::voxelType_dirt;
						else
							vt = voxel::VoxelType::voxelType_grass;
						
						//voxels[(int)((i * numberOfVoxels.y() * numberOfVoxels.x()) + (j * numberOfVoxels.x())) + k].init(&voxel_mesh, modelToWorld, vt);
						modelToWorld.translate(voxel_spacing, 0, 0);
					}

					modelToWorld.translate(-(voxel_spacing * numberOfVoxels.x()), voxel_spacing, 0);
				}

				modelToWorld.translate(0, -(voxel_spacing * numberOfVoxels.y()), voxel_spacing);
			}*/

			//modelToWorld.loadIdentity();
			//modelToWorld.translate(0, -10, 0);

			//floor_mesh.make_aa_box(4.0f, 1.0f, 1.0f);
			//floor_mat.make_color(vec4(0, 1, 0, 1), false, false);

			cameraToWorld.loadIdentity();
			cameraToWorld.translate(numberOfVoxels.x() * voxel_spacing / 2.0f - voxel_spacing / 2.0f, numberOfVoxels.y() * voxel_spacing / 2.0f - voxel_spacing / 2.0f, numberOfVoxels.z() * voxel_spacing * 1.5f);
		}

		void draw_world(int x, int y, int width, int height)
		{
			glViewport(x, y, width, height);

			glClearColor(0.5f, 0.5f, 0.5f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CW);

			if (is_key_down('W'))
				cameraToWorld.translate(0, 0, -1.0f);

			if (is_key_down('S'))
				cameraToWorld.translate(0, 0, 1.0f);

			if (is_key_down('Q'))
				cameraToWorld.rotate(-1.0f, 1, 0, 0);

			if (is_key_down('E'))
				cameraToWorld.rotate(1.0f, 1, 0, 0);

			if (is_key_down('A'))
				cameraToWorld.rotate(-1.0f, 0, 1, 0);

			if (is_key_down('D'))
				cameraToWorld.rotate(1.0f, 0, 1, 0);

      if (is_key_down(27))
        exit(1);

			mat4t worldToCamera;
			cameraToWorld.invertQuick(worldToCamera);

			mat4t cameraToProjection;
			cameraToProjection.loadIdentity();
			cameraToProjection.frustum(0.125f, -.125f, .125f, -.125, 0.125f, 256.0f);

			vec4 lights[5];
			memset(lights, 0, sizeof(lights));
			lights[0] = vec4(0.3f, 0.3f, 0.3f, 50);
			lights[2] = vec4(0.707f, 0.707f, 0.707f, 0) * cameraToWorld;//worldToCamera;
			lights[3] = vec4(1, 1, 1, 1);
			lights[4] = vec4(1, 1, 1, 1);

      for(int x = 0; x < numChunks; x++)
      {
        for(int y = 0; y < numChunks; y++)
        {
          for(int z = 0; z < numChunks; z++)
          {
				    chunks[x][y][z].render(_bump_shader, worldToCamera, cameraToProjection, lights, 5, 1);
          }
        }
      }

			floor_mesh.render();
		}
	};
}