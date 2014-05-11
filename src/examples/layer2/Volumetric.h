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
		bool jumping;
		float jumpStr;

		int chunksX, chunksY, chunksZ;

		int mouseX, mouseY;

		//Sound
		Sound soundEngine;
		bool musicPlaying, menuMusicPlaying;
		bool dayTime;
		//Sound End

		int intScan;
		FILE* file;

		//Sound
		enum{
			placing,
			breaking,
		};
		//End Sound
		
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
			water,

			numTypes,
		};
		//The number of different block types in the game
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
		//All the UI elements for the main game
		ui_builder ui;

		enum{
			ss_empty,
			ss_background,
			ss_exit,
			ss_load,
			ss_new,

			num_ss_elements,
		};
		//All the start screen elements
		ui_builder startScreen;

		enum{
			ls_empty,
			ls_background,

			num_ls_elements,
		};
		//The load screen elements
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
		//The pause screen elements
		ui_builder pauseScreen;

		enum{
			start,
			cleanLoading,
			readingFile,
			saveLoading,
			playing,
			paused,
		};
		//The game states
		int playState;

		//Used to generate the terrain
		noise terrainNoise;

		Volumetric(int argc, char **argv) : app(argc, argv){ }

		void app_init()
		{
			//Initialise our shaders
			flat_shader_.init();
			color_shader_.init();
			ui_shader_.init();
			sky_shader_.init();

			//Initialize the timer.
			oldTime = clock();
			skyTimer = 0.0f;

			//Sound Items
			dayTime = true;
			musicPlaying = false;
			menuMusicPlaying = false;
			//Sound

			//Get window width and height for UI elements
			get_viewport_size(windowWidth, windowHeight);

			//Used in the Mac version (currently doesnt work)
			//windowWidth = glutGet(GLUT_WINDOW_WIDTH);
			//windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

			//Initialises the colour values for the blocks (used in the UI)
			blockColors[grass] = glm::vec4(0.160, 0.570, 0.168, 1);
			blockColors[dirt] = glm::vec4(0.6, 0.3, 0, 1);
			blockColors[stone] = glm::vec4(0.5, 0.5, 0.5, 1);
			blockColors[wood] = glm::vec4(0.4, 0.2, 0, 1);
			blockColors[leaves] = glm::vec4(0.06, 0.25, 0.06, 1);
			blockColors[snow] = glm::vec4(0.75, 0.75, 0.75, 1);
			blockColors[sand] = glm::vec4(1.0, 0.95, 0.21, 1);
			blockColors[brick] = glm::vec4(0.87, 0.16, 0.04, 1);
			blockColors[water] = glm::vec4(0.137, 0.812, 0.933, 1);

			//Sets the playstate
			playState = start;

			//Initialises the start screen
			startScreen.init((float)windowWidth, (float)windowHeight, num_ss_elements);

			//Adds all the start screen elements
			GLuint texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startScreen.gif");
			startScreen.addElement(ss_background, 0.0f, 0.0f, (float)windowWidth, (float)windowHeight, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startNewButton.gif");
			startScreen.addElement(ss_new, (windowWidth*0.5f) -362.5f, (windowHeight*0.5f) - 83.0f - 50.0f, 725.0f, 100.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startLoadButton.gif");
			startScreen.addElement(ss_load, (windowWidth*0.5f) - 362.5f, (windowHeight*0.5f), 725.0f, 100.0f, texture);
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/start/startExitButton.gif");
			startScreen.addElement(ss_exit, (windowWidth*0.5f) - 362.5f, (windowHeight*0.5f) + 83.0f + 50.0f, 725.0f, 100.0f, texture);

			//Initialises the loading screen
			loadScreen.init((float)windowWidth, (float)windowHeight, num_ls_elements);

			//Adds the loading screen elements
			texture = resource_dict::get_texture_handle(GL_RGBA, "assets/Volumetric/load/loadScreen.gif");
			loadScreen.addElement(ls_background, 0.0f, 0.0f, (float)windowWidth, (float)windowHeight, texture);

			//Initialises the pause screen
			pauseScreen.init((float)windowWidth, (float)windowHeight, num_ps_elements);

			//Adds the pause screen elements
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

			//Initialises the game UI
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
			//Open a file and prepare it for writing.  If the file exists this will overwrite the file.
			FILE* file = fopen("../../assets/volumetric/saveFile.txt", "w");

			//Save the dimensions of the superchunk within the save file.
			fprintf(file, "%i %i %i\n", chunksX, chunksY, chunksZ);

			//Sort through each of the chunks and save the voxel identification in the save file.
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

			//We are done saving the game information.  Close the file.
			fclose(file);
		}

		//Creates a tree at a specified position
		void createTree1(int x_, int y_, int z_)
		{
			c->set(x_, y_ + 1, z_, wood);
			c->set(x_, y_ + 2, z_, wood);
			c->set(x_, y_ + 3, z_, wood);
			c->set(x_, y_ + 4, z_, wood);
			c->set(x_ + 1, y_ + 4, z_, leaves);
			c->set(x_ - 1, y_ + 4, z_, leaves);
			c->set(x_, y_ + 4, z_ + 1, leaves);
			c->set(x_, y_ + 4, z_ - 1, leaves);
			c->set(x_, y_ + 5, z_, leaves);
		}

		//Creates a tree at a specified position
		void createTree2(int x_, int y_, int z_)
		{
			c->set(x_, y_ + 1, z_, wood);
			c->set(x_, y_ + 2, z_, wood);
			c->set(x_, y_ + 3, z_, wood);
			c->set(x_, y_ + 4, z_, wood);
			c->set(x_, y_ + 5, z_, wood);
			c->set(x_ + 1, y_ + 5, z_, leaves);
			c->set(x_ + 1, y_ + 5, z_ + 1, leaves);
			c->set(x_ + 1, y_ + 5, z_ - 1, leaves);
			c->set(x_ - 1, y_ + 5, z_, leaves);
			c->set(x_ - 1, y_ + 5, z_ + 1, leaves);
			c->set(x_ - 1, y_ + 5, z_ - 1, leaves);
			c->set(x_, y_ + 5, z_ + 1, leaves);
			c->set(x_, y_ + 5, z_ - 1, leaves);
			c->set(x_, y_ + 6, z_, leaves);
		}

		//Creates a tree at a specified position
		void createTree3(int x_, int y_, int z_)
		{
			c->set(x_, y_ + 1, z_, wood);
			c->set(x_, y_ + 2, z_, wood);
			c->set(x_, y_ + 3, z_, wood);
			c->set(x_, y_ + 4, z_, wood);
			c->set(x_, y_ + 5, z_, wood);
			c->set(x_ + 1, y_ + 3, z_, leaves);
			c->set(x_ - 1, y_ + 3, z_, leaves);
			c->set(x_, y_ + 3, z_ + 1, leaves);
			c->set(x_, y_ + 3, z_ - 1, leaves);
			c->set(x_ + 2, y_ + 4, z_, leaves);
			c->set(x_ + 1, y_ + 4, z_, leaves);
			c->set(x_ + 1, y_ + 4, z_ + 1, leaves);
			c->set(x_ + 1, y_ + 4, z_ - 1, leaves);
			c->set(x_ - 2, y_ + 4, z_, leaves);
			c->set(x_ - 1, y_ + 4, z_, leaves);
			c->set(x_ - 1, y_ + 4, z_ + 1, leaves);
			c->set(x_ - 1, y_ + 4, z_ - 1, leaves);
			c->set(x_, y_ + 4, z_ + 2, leaves);
			c->set(x_, y_ + 4, z_ + 1, leaves);
			c->set(x_, y_ + 4, z_ - 2, leaves);
			c->set(x_, y_ + 4, z_ - 1, leaves);
			c->set(x_ + 1, y_ + 5, z_, leaves);
			c->set(x_ - 1, y_ + 5, z_, leaves);
			c->set(x_, y_ + 5, z_ + 1, leaves);
			c->set(x_, y_ + 5, z_ - 1, leaves);
			c->set(x_, y_ + 6, z_, leaves);
		}

		//Creates a house at a specified position
		void createBrickHouse(int x_, int y_, int z_)
		{
			c->set(x_ - 1, y_ + 1, z_ - 1, brick);
			c->set(x_ - 2, y_ + 1, z_ - 1, brick);
			c->set(x_ - 3, y_ + 1, z_ - 1, brick);
			c->set(x_ - 4, y_ + 1, z_ - 1, brick);

			c->set(x_ - 1, y_ + 2, z_ - 1, brick);
			c->set(x_ - 2, y_ + 2, z_ - 1, brick);
			c->set(x_ - 3, y_ + 2, z_ - 1, brick);
			c->set(x_ - 4, y_ + 2, z_ - 1, brick);

			c->set(x_ - 1, y_ + 3, z_ - 1, brick);
			c->set(x_ - 2, y_ + 3, z_ - 1, brick);
			c->set(x_ - 3, y_ + 3, z_ - 1, brick);
			c->set(x_ - 4, y_ + 3, z_ - 1, brick);

			c->set(x_ - 1, y_ + 4, z_ - 1, brick);
			c->set(x_ - 2, y_ + 4, z_ - 1, brick);
			c->set(x_ - 3, y_ + 4, z_ - 1, brick);
			c->set(x_ - 4, y_ + 4, z_ - 1, brick);

			c->set(x_ - 1, y_ + 1, z_ - 2, brick);
			c->set(x_ - 4, y_ + 1, z_ - 2, brick);

			c->set(x_ - 1, y_ + 2, z_ - 2, brick);
			c->set(x_ - 4, y_ + 2, z_ - 2, brick);

			c->set(x_ - 1, y_ + 4, z_ - 2, brick);
			c->set(x_ - 2, y_ + 4, z_ - 2, brick);
			c->set(x_ - 3, y_ + 4, z_ - 2, brick);
			c->set(x_ - 4, y_ + 4, z_ - 2, brick);

			c->set(x_ - 2, y_ + 5, z_ - 2, brick);
			c->set(x_ - 3, y_ + 5, z_ - 2, brick);

			c->set(x_ - 1, y_ + 1, z_ - 3, brick);
			c->set(x_ - 4, y_ + 1, z_ - 3, brick);

			c->set(x_ - 1, y_ + 2, z_ - 3, brick);
			c->set(x_ - 4, y_ + 2, z_ - 3, brick);

			c->set(x_ - 1, y_ + 3, z_ - 3, brick);
			c->set(x_ - 4, y_ + 3, z_ - 3, brick);

			c->set(x_ - 1, y_ + 4, z_ - 3, brick);
			c->set(x_ - 2, y_ + 4, z_ - 3, brick);
			c->set(x_ - 3, y_ + 4, z_ - 3, brick);
			c->set(x_ - 4, y_ + 4, z_ - 3, brick);

			c->set(x_ - 2, y_ + 5, z_ - 3, brick);
			c->set(x_ - 3, y_ + 5, z_ - 3, brick);

			c->set(x_ - 1, y_ + 1, z_ - 4, brick);
			c->set(x_ - 4, y_ + 1, z_ - 4, brick);

			c->set(x_ - 1, y_ + 2, z_ - 4, brick);
			c->set(x_ - 4, y_ + 2, z_ - 4, brick);

			c->set(x_ - 1, y_ + 3, z_ - 4, brick);
			c->set(x_ - 4, y_ + 3, z_ - 4, brick);

			c->set(x_ - 1, y_ + 4, z_ - 4, brick);
			c->set(x_ - 2, y_ + 4, z_ - 4, brick);
			c->set(x_ - 3, y_ + 4, z_ - 4, brick);
			c->set(x_ - 4, y_ + 4, z_ - 4, brick);

			c->set(x_ - 2, y_ + 5, z_ - 4, brick);
			c->set(x_ - 3, y_ + 5, z_ - 4, brick);

			c->set(x_ - 1, y_ + 1, z_ - 5, brick);
			c->set(x_ - 4, y_ + 1, z_ - 5, brick);

			c->set(x_ - 1, y_ + 2, z_ - 5, brick);
			c->set(x_ - 4, y_ + 2, z_ - 5, brick);

			c->set(x_ - 1, y_ + 4, z_ - 5, brick);
			c->set(x_ - 2, y_ + 4, z_ - 5, brick);
			c->set(x_ - 3, y_ + 4, z_ - 5, brick);
			c->set(x_ - 4, y_ + 4, z_ - 5, brick);

			c->set(x_ - 2, y_ + 5, z_ - 5, brick);
			c->set(x_ - 3, y_ + 5, z_ - 5, brick);

			c->set(x_ - 1, y_ + 1, z_ - 6, brick);
			c->set(x_ - 4, y_ + 1, z_ - 6, brick);

			c->set(x_ - 1, y_ + 2, z_ - 6, brick);
			c->set(x_ - 4, y_ + 2, z_ - 6, brick);

			c->set(x_ - 1, y_ + 3, z_ - 6, brick);
			c->set(x_ - 4, y_ + 3, z_ - 6, brick);

			c->set(x_ - 1, y_ + 4, z_ - 6, brick);
			c->set(x_ - 2, y_ + 4, z_ - 6, brick);
			c->set(x_ - 3, y_ + 4, z_ - 6, brick);
			c->set(x_ - 4, y_ + 4, z_ - 6, brick);
		}

		//Creates a house at a specified position
		void createWoodHouse(int x_, int y_, int z_)
		{
			c->set(x_ - 1, y_ + 1, z_ - 1, wood);
			c->set(x_ - 2, y_ + 1, z_ - 1, wood);
			c->set(x_ - 3, y_ + 1, z_ - 1, wood);
			c->set(x_ - 4, y_ + 1, z_ - 1, wood);

			c->set(x_ - 1, y_ + 2, z_ - 1, wood);
			c->set(x_ - 2, y_ + 2, z_ - 1, wood);
			c->set(x_ - 3, y_ + 2, z_ - 1, wood);
			c->set(x_ - 4, y_ + 2, z_ - 1, wood);

			c->set(x_ - 1, y_ + 3, z_ - 1, wood);
			c->set(x_ - 2, y_ + 3, z_ - 1, wood);
			c->set(x_ - 3, y_ + 3, z_ - 1, wood);
			c->set(x_ - 4, y_ + 3, z_ - 1, wood);

			c->set(x_ - 1, y_ + 4, z_ - 1, wood);
			c->set(x_ - 2, y_ + 4, z_ - 1, wood);
			c->set(x_ - 3, y_ + 4, z_ - 1, wood);
			c->set(x_ - 4, y_ + 4, z_ - 1, wood);

			c->set(x_ - 1, y_ + 1, z_ - 2, wood);
			c->set(x_ - 4, y_ + 1, z_ - 2, wood);

			c->set(x_ - 1, y_ + 2, z_ - 2, wood);
			c->set(x_ - 4, y_ + 2, z_ - 2, wood);

			c->set(x_ - 1, y_ + 4, z_ - 2, wood);
			c->set(x_ - 2, y_ + 4, z_ - 2, wood);
			c->set(x_ - 3, y_ + 4, z_ - 2, wood);
			c->set(x_ - 4, y_ + 4, z_ - 2, wood);

			c->set(x_ - 2, y_ + 5, z_ - 2, wood);
			c->set(x_ - 3, y_ + 5, z_ - 2, wood);

			c->set(x_ - 1, y_ + 1, z_ - 3, wood);
			c->set(x_ - 4, y_ + 1, z_ - 3, wood);

			c->set(x_ - 1, y_ + 2, z_ - 3, wood);
			c->set(x_ - 4, y_ + 2, z_ - 3, wood);

			c->set(x_ - 1, y_ + 3, z_ - 3, wood);
			c->set(x_ - 4, y_ + 3, z_ - 3, wood);

			c->set(x_ - 1, y_ + 4, z_ - 3, wood);
			c->set(x_ - 2, y_ + 4, z_ - 3, wood);
			c->set(x_ - 3, y_ + 4, z_ - 3, wood);
			c->set(x_ - 4, y_ + 4, z_ - 3, wood);

			c->set(x_ - 2, y_ + 5, z_ - 3, wood);
			c->set(x_ - 3, y_ + 5, z_ - 3, wood);

			c->set(x_ - 1, y_ + 1, z_ - 4, wood);
			c->set(x_ - 4, y_ + 1, z_ - 4, wood);

			c->set(x_ - 1, y_ + 2, z_ - 4, wood);
			c->set(x_ - 4, y_ + 2, z_ - 4, wood);

			c->set(x_ - 1, y_ + 3, z_ - 4, wood);
			c->set(x_ - 4, y_ + 3, z_ - 4, wood);

			c->set(x_ - 1, y_ + 4, z_ - 4, wood);
			c->set(x_ - 2, y_ + 4, z_ - 4, wood);
			c->set(x_ - 3, y_ + 4, z_ - 4, wood);
			c->set(x_ - 4, y_ + 4, z_ - 4, wood);

			c->set(x_ - 2, y_ + 5, z_ - 4, wood);
			c->set(x_ - 3, y_ + 5, z_ - 4, wood);

			c->set(x_ - 1, y_ + 1, z_ - 5, wood);
			c->set(x_ - 4, y_ + 1, z_ - 5, wood);

			c->set(x_ - 1, y_ + 2, z_ - 5, wood);
			c->set(x_ - 4, y_ + 2, z_ - 5, wood);

			c->set(x_ - 1, y_ + 4, z_ - 5, wood);
			c->set(x_ - 2, y_ + 4, z_ - 5, wood);
			c->set(x_ - 3, y_ + 4, z_ - 5, wood);
			c->set(x_ - 4, y_ + 4, z_ - 5, wood);

			c->set(x_ - 2, y_ + 5, z_ - 5, wood);
			c->set(x_ - 3, y_ + 5, z_ - 5, wood);

			c->set(x_ - 1, y_ + 1, z_ - 6, wood);
			c->set(x_ - 4, y_ + 1, z_ - 6, wood);

			c->set(x_ - 1, y_ + 2, z_ - 6, wood);
			c->set(x_ - 4, y_ + 2, z_ - 6, wood);

			c->set(x_ - 1, y_ + 3, z_ - 6, wood);
			c->set(x_ - 4, y_ + 3, z_ - 6, wood);

			c->set(x_ - 1, y_ + 4, z_ - 6, wood);
			c->set(x_ - 2, y_ + 4, z_ - 6, wood);
			c->set(x_ - 3, y_ + 4, z_ - 6, wood);
			c->set(x_ - 4, y_ + 4, z_ - 6, wood);
		}

		//Creates a border around the free space found (used for debugging)
		void createBuildingBorder(int x, int y, int z)
		{
			c->set(x - 5, y + 1, z - 7, brick);
			c->set(x - 4, y + 1, z - 7, brick);
			c->set(x - 3, y + 1, z - 7, brick);
			c->set(x - 2, y + 1, z - 7, brick);
			c->set(x - 1, y + 1, z - 7, brick);
			c->set(x - 5, y + 1, z, brick);
			c->set(x - 4, y + 1, z, brick);
			c->set(x - 3, y + 1, z, brick);
			c->set(x - 2, y + 1, z, brick);
			c->set(x - 1, y + 1, z, brick);
			c->set(x, y + 1, z - 7, brick);
			c->set(x, y + 1, z - 6, brick);
			c->set(x, y + 1, z - 5, brick);
			c->set(x, y + 1, z - 4, brick);
			c->set(x, y + 1, z - 3, brick);
			c->set(x, y + 1, z - 2, brick);
			c->set(x, y + 1, z - 1, brick);
			c->set(x, y + 1, z, brick);
			c->set(x - 5, y + 1, z - 7, brick);
			c->set(x - 5, y + 1, z - 6, brick);
			c->set(x - 5, y + 1, z - 5, brick);
			c->set(x - 5, y + 1, z - 4, brick);
			c->set(x - 5, y + 1, z - 3, brick);
			c->set(x - 5, y + 1, z - 2, brick);
			c->set(x - 5, y + 1, z - 1, brick);
			c->set(x - 5, y + 1, z, brick);
		}

		void generateTerrain(int xPos, int zPos, int width, int depth)
		{
			//Goes along the x and z axis
			for (int z = zPos; z < zPos + depth; z++)
			{
				for (int x = xPos; x < xPos + width; x++)
				{
					//Returns a value between -1 and 1 from the noise class
					float total = terrainNoise.perlinNoise((float)x, (float)z);

					//Sets the height of the current position
					int y = (int)((total * 64.0f) + 64.0f); //Creates a height between 0-128

					//For every voxel at the current location from the floor too the height create a voxel
					for (int i = 0; i <= y; i++)
					{
						//Depending on how high the loop is currently decides what block to place
						if (i == y && y >(CY * SCY) * 0.7)
							c->set(x, i, z, snow);
						else if (i > (CY * SCY) * 0.65)
							c->set(x, i, z, stone);
						else if (i > (CY * SCY) * 0.48)
							c->set(x, i, z, grass);
						else if (i > (CY * SCY) * 0.47)
							c->set(x, i, z, dirt);
						else if (i > (CY * SCY) * 0.45)
							c->set(x, i, z, sand);
						else if (i > (CY * SCY) * 0.4)
							c->set(x, i, z, dirt);
						else
							c->set(x, i, z, stone);
					}

					//If the top of the voxel "tower" for the current position is grass then have a 1 in 300 chance of placing a tree at this position
					if (y < (CY * SCY) * 0.65 && y >(CY * SCY) * 0.47 && rand() % 300 == 1)
					{
						//Checks to make sure the tree would not be placed outside of the super chunk
						if (x > xPos + 2 && z > zPos + 2 && x < xPos + width - 2 && z < zPos + depth - 2)
						{
							//Randomly chooses which tree to make
							switch (rand() % 3)
							{
							case 0:
								createTree1(x, y, z);
								break;
							case 1:
								createTree2(x, y, z);
								break;
							case 2:
								createTree3(x, y, z);
								break;
							}
						}
					}

					//Tries to find a flat area
					bool flat = true;

					//If the current block is above water
					if (x > xPos && z > zPos && x < xPos + width && z < zPos + depth && y >(CY * SCY) * 0.45)
					{
						//Checks to see if there is a flat space 8x6 wide before the current block
						for (int i = x - 5; i <= x; i++)
						{
							for (int j = z - 7; j <= z; j++)
							{
								if (c->get(i, y + 1, j) != 0)
								{
									flat = false;
								}
								if (c->get(i, y, j) == 0)
								{
									flat = false;
								}
							}
						}
					}
					else
					{
						flat = false;
					}

					//If there is a flat space then place a house in that space
					if (flat == true)
					{
						if (y > (CY * SCY) * 0.45 && y <= (CY * SCY) * 0.47)
						{
							if (rand() % 20 == 1)
								createWoodHouse(x, y, z);
						}
						else if (y > (CY * SCY) * 0.47 && y <= (CY * SCY) * 0.65)
						{
							if (rand() % 100 == 1)
								createBrickHouse(x, y, z);
						}
						else
						{
							if (rand() % 40 == 1)
								createWoodHouse(x, y, z);
						}
					}

					//If the height of the voxel "tower" for the current position is below 45% of the height then make every block after that upto 45%
					//a water block and put sand below the water
					if (y < (CY * SCY) * 0.45)
					{
						c->set(x, y - 1, z, sand);
						for (int i = y; i <= (CY * SCY) * 0.45; i++)
						{
							c->set(x, i, z, water);
						}
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

		//Mouse movement if the player is playing
		void motion(int x, int y, HWND* w)
		{
			if (playState == paused)
			{

			}
			//Keeps the mouse in the center of the screen.
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
				//Unprojects the pixel in the middle of the screen and removes the block that is in that position
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

				//Rounds the pixel so that it is in its correct position
				if (dti(objcoord.x) < 0.01f && dti(objcoord.x) > -0.01f)
					objcoord.x = glm::round(objcoord.x);
				if (dti(objcoord.y) < 0.01f && dti(objcoord.y) > -0.01f)
					objcoord.y = glm::round(objcoord.y);
				if (dti(objcoord.z) < 0.01f && dti(objcoord.z) > -0.01f)
					objcoord.z = glm::round(objcoord.z);

				float nx = objcoord.x;
				float ny = objcoord.y;
				float nz = objcoord.z;

				//Moves the current pixel position slightly inside the block that was clicked so we know what block we need to remove
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

				//Removes the block
				if (c->get((int)end.x, (int)end.y, (int)end.z) != 0)
				{
					c->set((int)end.x, (int)end.y, (int)end.z, 0);

					//Sound
					soundEngine.PlaySound (breaking);
					//Sound End
				}
			}

			if (is_key_down(key_lmb))
			{
				set_key(key_lmb, false);

				GLfloat depth;
				//Same as above except we are adding a block
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

				//Rounds the pixel so that it is in its correct position
				if (dti(objcoord.x) < 0.01f && dti(objcoord.x) > -0.01f)
					objcoord.x = glm::round(objcoord.x);
				if (dti(objcoord.y) < 0.01f && dti(objcoord.y) > -0.01f)
					objcoord.y = glm::round(objcoord.y);
				if (dti(objcoord.z) < 0.01f && dti(objcoord.z) > -0.01f)
					objcoord.z = glm::round(objcoord.z);

				float nx = objcoord.x;
				float ny = objcoord.y;
				float nz = objcoord.z;

				//Moves the current pixel position slightly away from the clicked block so we know where to add the new block
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

				//Adds the block as long as it is in range of the superchunk
				if (x > -1 && x < CX * SCX && y > -1 && y < CY * SCY && z > -1 && z < CZ * SCZ && c->get((int)end.x, (int)end.y, (int)end.z) == 0)
				{
					c->set((int)end.x, (int)end.y, (int)end.z, selectedBlock);

					//Sound
					soundEngine.PlaySound(placing);
				}
			}
		}

		void playing_keyboard_controls()
		{
			const float movespeed = 0.25f;
			float jumpspeed = 1.1f;

			glm::vec3 forward_dir = glm::vec3(sinf(angles.x), 0, cosf(angles.x));
			glm::vec3 right_dir = glm::vec3(-forward_dir.z, 0, forward_dir.x);
			glm::vec3 temp_w;
			glm::vec3 temp_a;
			glm::vec3 temp_s;
			glm::vec3 temp_d;

			//Strafes left in the direction of the camera
			if (is_key_down('A')){
				if (gravity == true)
				{
					temp_a = position - (right_dir * (movespeed * 3));
					if (c->get((int)glm::floor(temp_a.x), (int)glm::floor(temp_a.y - 2.0), (int)glm::floor(temp_a.z)) == 0)
					{
						position -= right_dir * movespeed;
					}
				}
				if (gravity == false)
				{
					position -= right_dir * (movespeed * 2.0f);
				}
			}

			//Strafes right in the direction of the camera
			if (is_key_down('D')){
				
				if (gravity == true)
				{
					temp_d = position + (right_dir * (movespeed * 3));
					if (c->get((int)glm::floor(temp_d.x), (int)glm::floor(temp_d.y - 2.0), (int)glm::floor(temp_d.z)) == 0)
					{
						position += right_dir * movespeed;
					}
				}
				if (gravity == false)
				{
					position += right_dir * (movespeed * 2.0f);
				}
			}

			//Moves forward in the direction of the camera
			if (is_key_down('W')){
				
				if (gravity == true)
				{
					temp_w = position + (forward_dir * (movespeed * 3));
					if (c->get((int)glm::floor(temp_w.x), (int)glm::floor(temp_w.y - 2.0), (int)glm::floor(temp_w.z)) == 0)
					{
						position += forward_dir * movespeed;
					}
				}
				if (gravity == false)
				{
					position += forward_dir * (movespeed * 2.0f) ;
				}
			}

			//Moves backwards along the oposite direction of the camera
			if (is_key_down('S')){
				
				if (gravity == true)
				{
					temp_s = position - (forward_dir * (movespeed * 3));
					if (c->get((int)glm::floor(temp_s.x), (int)glm::floor(temp_s.y - 2.0f), (int)glm::floor(temp_s.z)) == 0)
					{
						position -= forward_dir * movespeed;
					}
				}
				if (gravity == false)
				{
					position -= forward_dir * (movespeed * 2.0f);
				}
			}

			//Moves the camera up if in fly mode or jumps in player mode
			if (is_key_down('Q'))
			{
				if (gravity == false){
					position.y += movespeed;
				}
				if (gravity == true && jumping == false){
						position.y += jumpspeed;
						jumpStr += 5.0f;
				}
			}

			//Move the camera down
			if (is_key_down('E')){
				if (gravity == false){
					position.y -= movespeed;
				}
			}

			//Updates the currently selected block on the hotbar by using the mouse wheel
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

			//Used for updating the hot bar on the UI
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

			//Turns gravity on and off to go between a fly through camera and our actual game player
			if (is_key_down(key_backspace)){
				gravity = !gravity;
				set_key(key_backspace, false);
			}

			//Turns wireframe on if space is held down
			if (is_key_down(key_space))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//Sets the state to paused
			if (is_key_down(key_esc))
			{
				playState = paused;
				ShowCursor(true);

				set_key(key_esc, false);
			}

			//Updates the camera position
			glm::vec3 lookat;
			lookat.x = sinf(angles.x) * cosf(angles.y);
			lookat.y = sinf(angles.y);
			lookat.z = cosf(angles.x) * cosf(angles.y);

			view = glm::lookAt(position, position + lookat, glm::vec3(0, 1, 0));
		}

		//Keyboard controls for the start screen
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

				//Sound
				soundEngine.AdjustBackground (0.0f);
				soundEngine.PlayMenuMusic();
				//Sound End
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
			{
				if (c->get((int)(glm::floor(position.x)), (int)(glm::floor(position.y - 3.0f)), (int)(glm::floor(position.z))) == 0 && gravity == true)
				{
					position.y -= 0.8f;
				}
				else if (c->get((int)(glm::floor(position.x)), (int)(glm::floor(position.y - 3.0f)), (int)(glm::floor(position.z))) != 0 && gravity == true)
				{
					jumping = false;
					jumpStr = 0.0f;
				}
			}

			if (jumpStr >= 100.0f) //how high the player can jump
			{
				jumping = true;
			}
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
				jumping = true;
				jumpStr = 0.0f;

				//Sound
				if (!menuMusicPlaying)
				{
					soundEngine.PlayMenuMusic();
					menuMusicPlaying = true;
				}
				//Sound End
			}
			else if (playState == cleanLoading)
			{
				soundEngine.StopMenuMusic();
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
				//Get the current time.
				clock_t newTime = clock();

				//If the music for the game is not playing, start the music.
				if (!musicPlaying)
				{
					musicPlaying = true;
					soundEngine.PlayMusic();
				}

				//Update the sky timer with the delta time.  Use the sky timer to update the light_angle.
				//Multiply the sky timer to speed up or slow down time.
				skyTimer = (float)((double)newTime - (double)oldTime) / CLOCKS_PER_SEC;
				light_angle += skyTimer * 3.0f;

				//Set the past new time as the old time.
				oldTime = newTime;

				//If the lgiht angle is beyond 180 degrees, reset the angle by subtracting 360 degrees.
				if (light_angle > 180.0f)
					light_angle -= 360.0f;

				//Set the max volume of the sound effects.
				//Get the absolute angle of the light source.
				float maxVolume = 0.5f;
				float abs_light_angle = abs (light_angle);

				//Check to see if the light is currently at sunrise or sunset.  If so adjust the appropriate music level
				//to fade in or out the day time music.
				if ((light_angle < 10.0f && light_angle > 0.0f) || (light_angle > 170.0f && light_angle < 180.0f))
				{
					//Check to see if the time is during the night.  If so adjust the music gain to 0
					//for the night music.  Set the day time check in the sound class to true and play the day music.
					if (!dayTime)
					{
						soundEngine.AdjustBackground (0.0f);
						dayTime = true;
						soundEngine.setDayPlaying (dayTime);
					}

					if (abs_light_angle > 170.0f)
						abs_light_angle = 180.0f - abs_light_angle;

					//Use the absolute light angle divided by the number of degrees used to fade in/out
					//the day music.  That percentage will be used to adjust the volume of the music.
					float diff = (abs_light_angle / 10.0f) * maxVolume;
					soundEngine.AdjustBackground (diff);
				}

				//Check to see if the light is currently at sunrise or sunset.  If so adjust the appropriate music level
				//to face in or out during the night time music.
				else if ((light_angle > -10.0f && light_angle < 0.0f)  || (light_angle < -170.0f && light_angle > -180.0f))
				{
					//Check to see if the time is during the day.  If so adjust the music gain to 0
					//for the day music.  Set the day time check in the sound class to true and play the day music.
					if (dayTime)
					{
						soundEngine.AdjustBackground (0.0f);
						dayTime = false;
						soundEngine.setDayPlaying (dayTime);
					}

					//Get light angle between 0 and 10 no matter if it is sunrise or sunset.
					if (abs_light_angle > 170.0f)
						abs_light_angle = 180.0f - abs_light_angle;

					//Use the absolute light angle divided by the number of degrees used to fade in/out
					//the night music.  That percentage will be used to adjust the volume of the music.
					float diff = (abs_light_angle / 10.0f) * maxVolume;
					soundEngine.AdjustBackground (diff);
				}

				//It isn't sunrise or sunset.  Set the music to full volume.
				else
					soundEngine.AdjustBackground (maxVolume);

				//Adjust the sun's angle using the light angle calculated above.
				light_information[0] = glm::vec4(cos(light_angle * 0.0174532925f) * 0.75f, sin(light_angle * 0.0174532925f), cos(light_angle * 0.0174532925f) * 0.25f, 0.0f);

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
				//Get the current time and swap the old time for the new time.  This will pause the time in the game.
				clock_t newTime = clock();
				oldTime = newTime;

				//Adjust the sun's light angle in order to keep track of the sun angle.  With the time paused this should remain constant.
				light_information[0] = glm::vec4(cos(light_angle * 0.0174532925f) * 0.75f, sin(light_angle * 0.0174532925f), cos(light_angle * 0.0174532925f) * 0.25f, 0.0f);

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
