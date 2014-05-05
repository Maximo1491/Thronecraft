namespace octet
{
	class Volumetric : public app
	{
	public:

		flat_shader flat_shader_;
		color_shader color_shader_;
		ui_shader ui_shader_;
		sky_shader sky_shader_;

		int windowWidth, windowHeight;
		superChunk* c;

		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		glm::vec3 position;
		glm::vec2 angles;

		int numOfLights;
		glm::vec4 light_information[8];
		glm::vec4 light_ambient;
		glm::vec4 light_diffuse[8];
		float light_angle;
		sky* sky_;
		float skyTimer;
		clock_t oldTime;

		uint8_t selectedBlock;
		float current_block_position;
		bool gravity;

		int chunksX, chunksY, chunksZ;

		int mouseX, mouseY;

		enum {
			num_sound_sources = 8,
		};

		ALuint sources[num_sound_sources];
		unsigned cur_source;
		ALuint whoosh;
		ALuint bang;

		int intScan;
		FILE* file;

		enum{
			empty,
			grass,
			dirt,
			stone,
			wood,
			leaves,
			snow,
			sand,
			brick,

			numTypes,
		};

		glm::vec4 blockColors[numTypes];

		enum{
			ui_empty,
			ui_grass_block,
			ui_dirt_block,
			ui_stone_block,
			ui_wood_block,
			ui_leaves_block,
			ui_snow_block,
			ui_sand_block,
			ui_brick_block,
			ui_current_block_background,
			ui_hotbar_background,
			ui_crosshair_left,
			ui_crosshair_right,
			ui_crosshair_top,
			ui_crosshair_bottom,

			num_ui_elements,
		};

		ui_builder ui;

		enum{
			ss_empty,
			ss_background,
			ss_exit,
			ss_load,
			ss_new,

			num_ss_elements,
		};

		ui_builder startScreen;

		enum{
			ls_empty,
			ls_background,

			num_ls_elements,
		};

		ui_builder loadScreen;

		enum{
			ps_empty,
			ps_background,
			ps_notice,
			ps_quit,
			ps_saveandquit,
			ps_cancel,
			ps_menu,
			num_ps_elements,
		};

		ui_builder pauseScreen;

		enum{
			start,
			cleanLoading,
			readingFile,
			saveLoading,
			playing,
			paused,
		};

		int playState;

		noise terrainNoise;

		Volumetric(int argc, char **argv) : app(argc, argv){ }

		ALuint get_sound_source()
		{
			return sources[cur_source++ % num_sound_sources];
		}

		void app_init()
		{
			//Initialise our shaders
			flat_shader_.init();
			color_shader_.init();
			ui_shader_.init();
			sky_shader_.init();

			//Initialize sound
			whoosh = resource_dict::get_sound_handle (AL_FORMAT_MONO16, "assets/invaderers/whoosh.wav");
			cur_source = 0;
			alGenSources (num_sound_sources, sources);

			//Initialize the timer.
			oldTime = clock();
			skyTimer = 0.0f;

			//Get window width and height for UI elements
			get_viewport_size(windowWidth, windowHeight);

			//windowWidth = glutGet(GLUT_WINDOW_WIDTH);
			//windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

			blockColors[grass] = glm::vec4(0.160, 0.570, 0.168, 1);
			blockColors[dirt] = glm::vec4(0.6, 0.3, 0, 1);
			blockColors[stone] = glm::vec4(0.5, 0.5, 0.5, 1);
			blockColors[wood] = glm::vec4(0.4, 0.2, 0, 1);
			blockColors[leaves] = glm::vec4(0.06, 0.25, 0.06, 1);
			blockColors[snow] = glm::vec4(0.75, 0.75, 0.75, 1);
			blockColors[sand] = glm::vec4(1.0, 0.95, 0.21, 1);
			blockColors[brick] = glm::vec4(0.87, 0.16, 0.04, 1);

			//ui[loading_screen].init(0, 0, windowWidth, windowHeight, windowWidth, windowHeight, glm::vec4(1, 0, 0, 1));
			playState = start;

			startScreen.init((float)windowWidth, (float)windowHeight, num_ss_elements);

			GLuint texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startScreen.gif");
			startScreen.addElement(ss_background, 0.0f, 0.0f, (float)windowWidth, (float)windowHeight, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startNewButton.gif");
			startScreen.addElement(ss_new, (windowWidth*0.5f) -362.5f, (windowHeight*0.5f) - 83.0f - 50.0f, 725.0f, 100.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startLoadButton.gif");
			startScreen.addElement(ss_load, (windowWidth*0.5f) - 362.5f, (windowHeight*0.5f), 725.0f, 100.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startExitButton.gif");
			startScreen.addElement(ss_exit, (windowWidth*0.5f) - 362.5f, (windowHeight*0.5f) + 83.0f + 50.0f, 725.0f, 100.0f, texture);

			loadScreen.init((float)windowWidth, (float)windowHeight, num_ls_elements);

			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/load/loadScreen.gif");
			loadScreen.addElement(ls_background, 0.0f, 0.0f, (float)windowWidth, (float)windowHeight, texture);

			pauseScreen.init((float)windowWidth, (float)windowHeight, num_ps_elements);

			pauseScreen.addElement(ps_background, (windowWidth*0.5f) - 400.0f, (windowHeight*0.5f) - 300.0f, 800.0f, (windowHeight*0.5f) + 300.0f - 41.0f, glm::vec4(0, 0, 0, 0.75f));
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/pause/PauseNotice2.gif");
			pauseScreen.addElement(ps_notice, (windowWidth*0.5f) - 175.0f, (windowHeight*0.5f) - 285.0f, 350.0f, 75.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/pause/PauseQuitButton2.gif");
			pauseScreen.addElement(ps_quit, (windowWidth*0.5f) - 100.0f, (windowHeight*0.5f) - 140.0f -40.0f, 200.0f, 60.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/pause/PauseSaveButton2.gif");
			pauseScreen.addElement(ps_saveandquit, (windowWidth*0.5f) - 100.0f, (windowHeight*0.5f) - 80.0f, 200.0f, 120.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/pause/PauseMenuButton2.gif");
			pauseScreen.addElement(ps_menu, (windowWidth*0.5f) - 100.0f, (windowHeight*0.5f) + 80.0f, 200.0f, 60.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/pause/PauseCancelButton2.gif");
			pauseScreen.addElement(ps_cancel, (windowWidth*0.5f) - 100.0f, (windowHeight*0.5f) + 160.0f + 20.0f, 200.0f, 60.0f, texture);

			ui.init((float)windowWidth, (float)windowHeight, num_ui_elements);

			//CrossHair
			ui.addElement(ui_crosshair_left, (windowWidth*0.5f) - 6, (windowHeight*0.5f) - 2, 5, 4, glm::vec4(0, 0, 0, 1)); //Left
			ui.addElement(ui_crosshair_right, (windowWidth*0.5f) + 1, (windowHeight*0.5f) - 2, 5, 4, glm::vec4(0, 0, 0, 1)); //Right
			ui.addElement(ui_crosshair_top, (windowWidth*0.5f) - 2, (windowHeight*0.5f) - 6, 4, 5, glm::vec4(0, 0, 0, 1)); //Top
			ui.addElement(ui_crosshair_bottom, (windowWidth*0.5f) - 2, (windowHeight*0.5f) + 1, 4, 5, glm::vec4(0, 0, 0, 1)); //Bottom

			//UI bar
			float hotbar_width = 60 * 8 + 10;
			current_block_position = (windowWidth*0.5f) - (hotbar_width*0.5f) + 5;
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/barTexture.gif");
			ui.addElement(ui_hotbar_background, (windowWidth * 0.5f) - (hotbar_width * 0.5f), (float)windowHeight - 80.0f, hotbar_width, 70.0f, texture);
			//Currently selected block
			ui.addElement(ui_current_block_background, current_block_position, (float)windowHeight - 75.0f, 60.0f, 60.0f, glm::vec4(1, 0, 0.6, 1));

			//Fill the UI with avaliable blocks
			float inventoryX = ((windowWidth*0.5f) - (hotbar_width*0.5f)) + 10;
			for (int i = ui_grass_block; i <= ui_brick_block; i++)
			{
				ui.addElement(i, inventoryX, (float)windowHeight - 70.0f, 50.0f, 50.0f, blockColors[i]);
				inventoryX += 60;
			}

			//Number of Lights
			numOfLights = 3;

			//Ambient Light
			light_ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);

			//Light 1
			light_information[0] = glm::vec4(1.0f, 2.0f, 3.0f, 0.0f);
			light_diffuse[0] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

			light_angle = 0.0f;

			//Light 2
			light_information[1] = glm::vec4(-2.0f, -1.0f, -4.0f, 0.0f);
			light_diffuse[1] = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);

			//Light 3
			light_information[2] = glm::vec4(1.0f, 2.0f, 3.0f, 0.0f);
			light_diffuse[2] = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

			projection = glm::perspective(45.0f, 1.0f*windowWidth / windowHeight, 0.1f, 256.0f);

			c = new superChunk();

			//Start by selecting a grass block
			selectedBlock = grass;

			//Create our sky sphere for rendering.
			sky_ = new sky();
		}

		void SaveChunks()
		{
			FILE* file = fopen("../../assets/volumetric/saveFile.txt", "w");

			fprintf(file, "%i %i %i\n", chunksX, chunksY, chunksZ);

			for (int x = 0; x < CX * chunksX; x++)
			{
				for (int y = 0; y < CY * chunksY; y++)
				{
					for (int z = 0; z < CZ * chunksZ; z++)
					{
						fprintf(file, "%i ", c->get(x, y, z));
					}

					fprintf(file, "\n");
				}
			}

			fclose(file);
		}

		void generateTerrain(int xPos, int zPos, int width, int depth)
		{

			for (int z = zPos; z < zPos + depth; z++)
			{
				for (int x = xPos; x < xPos + width; x++)
				{
					float total = terrainNoise.perlinNoise((float)x, (float)z);

					int y = (int)((total * 64.0f) + 64.0f); //Creates a height between 0-128

					for(int i = 0; i <= y; i++)
					{
						if(i == y && y > (CY * SCY) * 0.7)
							c->set(x, i, z, snow);
						else if( i >= y - 2)
							c->set(x, i, z, grass);
						else if( i >= y - 6)
							c->set(x, i, z, dirt);
						else
							c->set(x, i, z, stone);
					}
				}
			}
		}

		//mouse movement
#ifdef __APPLE__

		void motion(int x, int y){
			static bool wrap = false;

			if(!wrap) {
				int ww = glutGet(GLUT_WINDOW_WIDTH);
				int wh = glutGet(GLUT_WINDOW_HEIGHT);

				int dx = x - ww / 2;
				int dy = y - wh / 2;

				const float mouseSpeed = -0.001f;

				angles.x += dx * mouseSpeed;
				angles.y += dy * mouseSpeed;

				if(angles.x < -3.14)
					angles.x += 3.14*2;
				else if (angles.x > 3.14)
					angles.x -= 3.14*2;

				if(angles.y < -3.14 / 2)
					angles.y = -3.14 / 2;
				else if(angles.y > 3.14 / 2)
					angles.y = 3.14 / 2;

				// move mouse pointer back to the center of the window
				wrap = true;
				glutWarpPointer(ww / 2, wh / 2);
			} else {
				wrap = false;
			}
		}

#elif defined(_WIN32) || defined(_WIN64)

		void motion(int x, int y, HWND* w)
		{
			if (playState == paused)
			{

			}
			else if (playState == playing)
			{
				static bool wrap = false;

				if (!wrap)
				{
					int ww, wh;
					get_viewport_size(ww, wh);

					int dx = x - ww / 2;
					int dy = y - wh / 2;

					const float mouseSpeed = -0.001f;

					angles.x += dx * mouseSpeed;
					angles.y += dy * mouseSpeed;

					if (angles.x < -3.14)
						angles.x += 3.14 * 2;
					else if (angles.x > 3.14)
						angles.x -= 3.14 * 2;

					if (angles.y < -3.14 / 2)
						angles.y = -3.14 / 2;
					else if (angles.y > 3.14 / 2)
						angles.y = 3.14 / 2;

					wrap = true;

					tagPOINT p;
					p.x = ww / 2;
					p.y = wh / 2;
					ClientToScreen(*w, &p);
					SetCursorPos(p.x, p.y);
				}
				else {
					wrap = false;
				}
			}
		}

#endif

		float dti(float val) {
			return fabsf(val - glm::round(val));
		}


		//mouse controls
		void mouse_controls(int width, int height)
		{
			if (is_key_down(key_rmb))
			{
				set_key(key_rmb, false);

				GLfloat depth;

				glReadPixels((int)(width * 0.5f), (int)(height * 0.5f), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
				glm::vec4 viewport = glm::vec4(0, 0, width, height);
				glm::vec3 wincoord = glm::vec3(width * 0.5, height * 0.5, depth);
				glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);
				if (objcoord.x < 0)
					objcoord.x = 0;
				if (objcoord.y < 0)
					objcoord.y = 0;
				if (objcoord.z < 0)
					objcoord.z = 0;

				if (dti(objcoord.x) < 0.01f && dti(objcoord.x) > -0.01f)
					objcoord.x = glm::round(objcoord.x);
				if (dti(objcoord.y) < 0.01f && dti(objcoord.y) > -0.01f)
					objcoord.y = glm::round(objcoord.y);
				if (dti(objcoord.z) < 0.01f && dti(objcoord.z) > -0.01f)
					objcoord.z = glm::round(objcoord.z);

				float nx = objcoord.x;
				float ny = objcoord.y;
				float nz = objcoord.z;

				if (dti(objcoord.x) < dti(objcoord.y))
				{
					if (dti(objcoord.x) < dti(objcoord.z))
					{
						if (position.x > objcoord.x)
							nx -= 0.1;
						else
							nx += 0.1;
					}
					else
					{
						if (position.z > objcoord.z)
							nz -= 0.1;
						else
							nz += 0.1;
					}
				}
				else
				{
					if (dti(objcoord.y) < dti(objcoord.z))
					{
						if (position.y > objcoord.y)
							ny -= 0.1;
						else
							ny += 0.1;
					}
					else
					{
						if (position.z > objcoord.z)
							nz -= 0.1;
						else
							nz += 0.1;
					}
				}

				int x = (int)glm::floor(nx);
				int y = (int)glm::floor(ny);
				int z = (int)glm::floor(nz);

				glm::vec3 end = glm::vec3(x, y, z);

				if (c->get((int)end.x, (int)end.y, (int)end.z) != 0)
				{
					c->set((int)end.x, (int)end.y, (int)end.z, 0);
				}
			}

			if (is_key_down(key_lmb))
			{
				set_key(key_lmb, false);

				GLfloat depth;

				glReadPixels((int)(width * 0.5f), (int)(height * 0.5f), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
				glm::vec4 viewport = glm::vec4(0, 0, width, height);
				glm::vec3 wincoord = glm::vec3(width * 0.5, height * 0.5, depth);
				glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);
				if (objcoord.x < 0)
					objcoord.x = 0;
				if (objcoord.y < 0)
					objcoord.y = 0;
				if (objcoord.z < 0)
					objcoord.z = 0;

				if (dti(objcoord.x) < 0.01f && dti(objcoord.x) > -0.01f)
					objcoord.x = glm::round(objcoord.x);
				if (dti(objcoord.y) < 0.01f && dti(objcoord.y) > -0.01f)
					objcoord.y = glm::round(objcoord.y);
				if (dti(objcoord.z) < 0.01f && dti(objcoord.z) > -0.01f)
					objcoord.z = glm::round(objcoord.z);

				float nx = objcoord.x;
				float ny = objcoord.y;
				float nz = objcoord.z;

				if (dti(objcoord.x) < dti(objcoord.y))
				{
					if (dti(objcoord.x) < dti(objcoord.z))
					{
						if (position.x > objcoord.x)
							nx += 0.1;
						else
							nx -= 0.1;
					}
					else
					{
						if (position.z > objcoord.z)
							nz += 0.1;
						else
							nz -= 0.1;
					}
				}
				else
				{
					if (dti(objcoord.y) < dti(objcoord.z))
					{
						if (position.y > objcoord.y)
							ny += 0.1;
						else
							ny -= 0.1;
					}
					else
					{
						if (position.z > objcoord.z)
							nz += 0.1;
						else
							nz -= 0.1;
					}
				}

				int x = (int)glm::floor(nx);
				int y = (int)glm::floor(ny);
				int z = (int)glm::floor(nz);

				glm::vec3 end = glm::vec3(x, y, z);

				if (x > -1 && x < CX * SCX && y > -1 && y < CY * SCY && z > -1 && z < CZ * SCZ && c->get((int)end.x, (int)end.y, (int)end.z) == 0)
				{
					c->set((int)end.x, (int)end.y, (int)end.z, selectedBlock);
				}
			}
		}

		void playing_keyboard_controls()
		{
			const float movespeed = 0.5f;

			glm::vec3 forward_dir = glm::vec3(sinf(angles.x), 0, cosf(angles.x));
			glm::vec3 right_dir = glm::vec3(-forward_dir.z, 0, forward_dir.x);
			glm::vec3 temp_w;
			glm::vec3 temp_a;
			glm::vec3 temp_s;
			glm::vec3 temp_d;

			if (is_key_down('A'))
				//temp_a = position - (right_dir * (movespeed * 3));
			//if (c->get((int)glm::floor(temp_a.x), (int)glm::floor(temp_a.y), (int)glm::floor(temp_a.z)) == 0)
			{
				position -= right_dir * movespeed;
			}
			if (is_key_down('D'))
				//temp_d = position + (right_dir * (movespeed * 3));
			//if (c->get((int)glm::floor(temp_d.x), (int)glm::floor(temp_d.y), (int)glm::floor(temp_d.z)) == 0)
			{
				position += right_dir * movespeed;
			}
			if (is_key_down('W'))
				//temp_w = position + (forward_dir * (movespeed * 3));
			//if (c->get((int)glm::floor(temp_w.x), (int)glm::floor(temp_w.y), (int)glm::floor(temp_w.z)) == 0)
			{
				position += forward_dir * movespeed;
			}
			if (is_key_down('S'))
				//temp_s = position - (forward_dir * (movespeed * 3));
			//if (c->get((int)glm::floor(temp_s.x), (int)glm::floor(temp_s.y), (int)glm::floor(temp_s.z)) == 0)
			{
				position -= forward_dir * movespeed;
			}
			if (is_key_down('Q'))
				position.y += movespeed;
			if (is_key_down('E'))
				position.y -= movespeed;
			if (is_key_down('O'))
			{
				set_key('O', false);
				if (c->get(0, 0, 0) == 3)
				{
					for (int i = 0; i < CZ; i++)
						c->set(0, 0, i, 0);
				}

				else
				{
					for (int i = 0; i < CZ; i++)
						c->set(0, 0, i, 3);
				}
			}

			if (get_mouse_wheel() != 0)
			{
				if (get_mouse_wheel() < 0)
				{
					selectedBlock--;

					if (selectedBlock < 1)
						selectedBlock = 1;
				}
				else if (get_mouse_wheel() > 0)
				{
					selectedBlock++;

					if (selectedBlock > 8)
						selectedBlock = 8;
				}

				ui.setPos(ui_current_block_background, current_block_position + ((float)(selectedBlock - 1) * 60.0f), (float)windowHeight - 75.0f);

				set_mouse_wheel(0);
			}

			if (is_key_down('1')) // grass
			{
				set_key('1', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(grass - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = grass;
			}
			if (is_key_down('2')) // dirt
			{
				set_key('2', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(dirt - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = dirt;
			}
			if (is_key_down('3')) // stone
			{
				set_key('3', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(stone - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = stone;
			}
			if (is_key_down('4')) // wood
			{
				set_key('4', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(wood - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = wood;
			}
			if (is_key_down('5')) // leaves
			{
				set_key('5', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(leaves - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = leaves;
			}
			if (is_key_down('6')) // snow
			{
				set_key('6', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(snow - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = snow;
			}
			if (is_key_down('7')) // sand
			{
				set_key('7', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(sand - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = sand;
			}
			if (is_key_down('8')) // brick
			{
				set_key('8', false);
				ui.setPos(ui_current_block_background, current_block_position + ((float)(brick - 1) * 60.0f), (float)windowHeight - 75.0f);
				selectedBlock = brick;
			}
			if (is_key_down(key_backspace)){
				gravity = !gravity;
				set_key(key_backspace, false);
			}

			if (is_key_down(key_space))
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (is_key_down(key_esc))
			{
				playState = paused;
				ShowCursor(true);

				set_key(key_esc, false);
			}

			glm::vec3 lookat;
			lookat.x = sinf(angles.x) * cosf(angles.y);
			lookat.y = sinf(angles.y);
			lookat.z = cosf(angles.x) * cosf(angles.y);

			view = glm::lookAt(position, position + lookat, glm::vec3(0, 1, 0));
		}

		void start_keyboard_controls()
		{
			if (is_key_down(key_esc))
			{
				exit(1);
			}

			if (is_key_down('1'))
			{
				playState = cleanLoading;
			}

			if(is_key_down('2'))
			{
				playState = readingFile;
			}

		}

		void start_mouse_controls()
		{
			get_mouse_pos(mouseX, mouseY);

			if (startScreen.hover(ss_new, mouseX, mouseY))
			{
				startScreen.changeTexture(ss_new,"assets/Volumetric/start/startNewButtonHover.gif");
				if(is_key_down(key_lmb)){ 
				set_key(key_lmb, false);
				playState = cleanLoading;
				loadScreen.render(ui_shader_);
				}
			}
			else if (startScreen.hover(ss_load, mouseX, mouseY))
			{
				startScreen.changeTexture(ss_load,"assets/Volumetric/start/startLoadButtonHover.gif");
				if(is_key_down(key_lmb)){ 
				set_key(key_lmb, false);
				playState = readingFile;
				loadScreen.render(ui_shader_);
				}
			}
			else if (startScreen.hover(ss_exit, mouseX, mouseY))
			{
				startScreen.changeTexture(ss_exit,"assets/Volumetric/start/startExitButtonHover.gif");
				if(is_key_down(key_lmb)){ 
				set_key(key_lmb, false);
				exit(1);
				}
			}
			else{
				startScreen.changeTexture(ss_exit,"assets/Volumetric/start/startExitButton.gif");
				startScreen.changeTexture(ss_load,"assets/Volumetric/start/startLoadButton.gif");
				startScreen.changeTexture(ss_new,"assets/Volumetric/start/startNewButton.gif");
			}
		}

		void paused_keyboard_controls()
		{
			if (is_key_down(key_esc))
			{
				playState = playing;
				ShowCursor(false);

				set_key(key_esc, false);
			}

			if (is_key_down('1'))
			{
				exit(1);
			}

			if (is_key_down('2'))
			{
				SaveChunks();
				exit(1);
			}

			if (is_key_down('3'))
			{
				playState = start;
			}
		}

		void paused_mouse_controls()
		{
			get_mouse_pos(mouseX, mouseY);

			if (pauseScreen.hover(ps_quit, mouseX, mouseY))
			{
				pauseScreen.changeTexture(ps_quit,"assets/Volumetric/pause/PauseQuitButton2Hover.gif");
				if(is_key_down(key_lmb)) exit(1);
			}
			else if (pauseScreen.hover(ps_saveandquit, mouseX, mouseY))
			{
				pauseScreen.changeTexture(ps_saveandquit,"assets/Volumetric/pause/PauseSaveButton2Hover.gif"); 
				if(is_key_down(key_lmb)){ 
				SaveChunks();
				exit(1);
				}
			}
			else if (pauseScreen.hover(ps_menu, mouseX, mouseY))
			{
				pauseScreen.changeTexture(ps_menu,"assets/Volumetric/pause/PauseMenuButton2Hover.gif");
				if(is_key_down(key_lmb)){ 
				set_key(key_lmb, false);
				playState = start;
				}
			}
			else if (pauseScreen.hover(ps_cancel, mouseX, mouseY))
			{
				pauseScreen.changeTexture(ps_cancel,"assets/Volumetric/pause/PauseCancelButton2Hover.gif"); 
				if(is_key_down(key_lmb)){ 
				set_key(key_lmb, false);
				playState = playing;
				ShowCursor(false);
				}
			}
			else{
				pauseScreen.changeTexture(ps_quit,"assets/Volumetric/pause/PauseQuitButton2.gif");
				pauseScreen.changeTexture(ps_saveandquit,"assets/Volumetric/pause/PauseSaveButton2.gif"); 
				pauseScreen.changeTexture(ps_menu,"assets/Volumetric/pause/PauseMenuButton2.gif"); 
				pauseScreen.changeTexture(ps_cancel,"assets/Volumetric/pause/PauseCancelButton2.gif"); 
			}
		}

		void update_player()
		{
			if (position.x > 0 && position.x < CX * SCX && position.z > 0 && position.z < CZ * SCZ)
				if (c->get((int)(glm::floor(position.x)), (int)(glm::floor(position.y - 1.0f)), (int)(glm::floor(position.z))) == 0 && gravity == true)
					position.y -= 0.6f;
		}

		void draw_world(int x, int y, int width, int height)
		{
			//Sets our viewpoint
			glViewport(x, y, width, height);
			glClearColor(0.25f, 0.95f, 1.0f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			if (playState == start){
				//ShowCursor(false);
				glDepthMask(GL_FALSE);
				startScreen.render(ui_shader_);
				glDepthMask(GL_TRUE);
				start_keyboard_controls();
				start_mouse_controls();

				//player position & gravity
				position = glm::vec3(176, 100, 176);
				gravity = false;

			}
			else if (playState == cleanLoading)
			{
				ShowCursor(false);
				loadScreen.render(ui_shader_);

				chunksX = SCX;
				chunksY = SCY;
				chunksZ = SCZ;

				for (int x = 0; x < CX * chunksX; x++)
				{
					for (int y = 0; y < CY * chunksY; y++)
					{
						for (int z = 0; z < CZ * chunksZ; z++)
						{
							c->set(x, y, z, 0);
						}
					}
				}

				terrainNoise.setRandomSeed();
				generateTerrain(0, 0, 256, 256);

				playState = playing;
			}
			else if (playState == readingFile)
			{
				ShowCursor(false);
				loadScreen.render(ui_shader_);

				file = fopen("../../assets/volumetric/saveFile.txt", "r");

				if (file == NULL)
				{
					chunksX = SCX;
					chunksY = SCY;
					chunksZ = SCZ;
					terrainNoise.setRandomSeed();
					generateTerrain(0, 0, 256, 256);
				}
				else
				{
					fscanf(file, "%i", &chunksX);
					fscanf(file, "%i", &chunksY);
					fscanf(file, "%i", &chunksZ);

					intScan;

					for (int x = 0; x < CX * chunksX; x++)
					{
						for (int y = 0; y < CY * chunksY; y++)
						{
							for (int z = 0; z < CZ * chunksZ; z++)
							{
								fscanf(file, "%i", &intScan);
								c->set(x, y, z, intScan);
							}
						}
					}
				}
				playState = playing;
			}
			else if (playState == playing)
			{
				clock_t newTime = clock();

				skyTimer = (float)((double)newTime - (double)oldTime) / CLOCKS_PER_SEC;
				light_angle += skyTimer * 3.0f;

				oldTime = newTime;

				if (light_angle > 180.0f)
					light_angle -= 360.0f;

				light_information[0] = glm::vec4(cos(light_angle * 0.0174532925f) * 0.75f, sin(light_angle * 0.0174532925f), cos(light_angle * 0.0174532925f) * 0.25f, 0.0f);

				glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
				glm::mat4 lightView = glm::lookAt(glm::vec3(light_information[0].x * 250.0f, light_information[0].y * 250.0f, light_information[0].z * 250.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				glm::mat4 lightModel = glm::mat4(1.0f);

				glm::mat4 lightMVP = lightProjection * lightView * lightModel;

				//Draws our super chunk
				c->render(model, view, projection, numOfLights, light_information, light_ambient, light_diffuse, flat_shader_, color_shader_);

				//Draw the sky.
				sky_->render(projection * view * model, position, glm::vec3(light_information[0].x, light_information[0].y, light_information[0].z), sky_shader_);

				//Draw the UI
				ui.render(ui_shader_);

				playing_keyboard_controls();
				mouse_controls(width, height);
				update_player();
			}
			else if (playState == paused)
			{
				clock_t newTime = clock();
				oldTime = newTime;

				light_information[0] = glm::vec4(cos(light_angle * 0.0174532925f) * 0.75f, sin(light_angle * 0.0174532925f), cos(light_angle * 0.0174532925f) * 0.25f, 0.0f);

				glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
				glm::mat4 lightView = glm::lookAt(glm::vec3(light_information[0].x * 250.0f, light_information[0].y * 250.0f, light_information[0].z * 250.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				glm::mat4 lightModel = glm::mat4(1.0f);

				glm::mat4 lightMVP = lightProjection * lightView * lightModel;

				//Draws our super chunk
				c->render(model, view, projection, numOfLights, light_information, light_ambient, light_diffuse, flat_shader_, color_shader_);

				//Draw the sky.
				sky_->render(projection * view * model, position, glm::vec3(light_information[0].x, light_information[0].y, light_information[0].z), sky_shader_);

				glDepthMask(GL_FALSE);
				pauseScreen.render(ui_shader_);
				glDepthMask(GL_TRUE);

				paused_keyboard_controls();
				paused_mouse_controls();
			}
		}
	};
}