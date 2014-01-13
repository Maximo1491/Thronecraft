namespace octet
{
	class voxel_app : public app
	{
	public:

		color_shader color_shader_;

		superChunk* c;

		mat4t modelToWorld;
		mat4t cameraToWorld;

		voxel_app(int argc, char **argv) : app(argc, argv) { }

		void app_init()
		{
      //Initialise our colour shader
			color_shader_.init();

      //Reset our modelToWorld and cameraToWorld then move the camera out 40
			modelToWorld.loadIdentity();
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(0, 0, 40);
			
			c = new superChunk();

      //Loops through 1 chunk and creates either a type 1 or type 2,
      //uncomment the SCX stuff to fill the whole superChunk
      for(int x = 0; x < CX/* *SCX */; x++){
				for(int y = 0; y < CY/* *SCY */; y++){
					for(int z = 0; z < CZ/* *SCZ */; z++){
			      if(z % 2 > 0) 
              c->set( x, y, z, 1);
            else
              c->set( x, y, z, 2);
          }
        }
      }
		}

		void draw_world(int x, int y, int width, int height)
		{
      //Sets our viewpoint
			glViewport(x, y, width, height);
			glClearColor(0.5f, 0.5f, 0.5f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //Camera controls
			if (is_key_down('W'))
				cameraToWorld.translate(0, 0, -1.0f);

			if (is_key_down('S'))
				cameraToWorld.translate(0, 0, 1.0f);

			if (is_key_down('Q'))
				cameraToWorld.rotate(-1.0f, 1, 0, 0);

			if (is_key_down('E'))
				cameraToWorld.rotate(1.0f, 1, 0, 0);

			if (is_key_down('A'))
				cameraToWorld.rotate(1.0f, 0, 1, 0);

			if (is_key_down('D'))
				cameraToWorld.rotate(-1.0f, 0, 1, 0);

      if (is_key_down(27))
        exit(1);

      //Uncomment this line for wireframe
      //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

      //Draws our super chunk
			c->render(modelToWorld, cameraToWorld, color_shader_);
		}
	};
}