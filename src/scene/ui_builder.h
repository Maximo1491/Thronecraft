namespace octet {
	//A class that allows users to build a UI
	class ui_builder {
		//Stores the window width and height
		float windowH;
		float windowW;

		//Stores the elements on the screen
		dynarray<ui_element> ui_elements;

	public:
		ui_builder()
		{
			windowH = 0;
			windowW = 0;
		}

		//Initialises the UI
		void init(float ww, float wh, int numElements)
		{
			windowH = wh;
			windowW = ww;

      ui_elements.resize(numElements);
		}

		//Adds an element to the screen but makes it invisible
		void addElement(int p, float x_, float y_, float w, float h)
		{
			ui_element e;
			e.init(x_, y_, w, h, windowW, windowH, glm::vec4(0,0,0,0));
			ui_elements[p] = e;
		}

		//Adds an element with a solid colour to the screen
    void addElement(int p, float x_, float y_, float w, float h, glm::vec4 c)
		{
			ui_element e;
			e.init(x_, y_, w, h, windowW, windowH, c);
			ui_elements[p] = e;
		}

		//Adds an element with a texture to the screen
		void addElement(int p, float x_, float y_, float w, float h, GLuint t)
		{
			ui_element e;
			e.init(x_, y_, w, h, windowW, windowH, t);
			ui_elements[p] = e;
		}

		//Changes the texture of an object
		void changeTexture(int e, string s)
		{
			ui_elements[e].changeTexture(s);
		}

		//Moves the object
    void setPos(int e, float x_, float y_)
    {
      ui_elements[e].setPos(x_, y_);
    }

		//Checks to see if the mouse is over the object
		bool hover(int e, int mouseX, int mouseY)
		{
			vec2 elePos;

			elePos = ui_elements[e].getPos();

			if (mouseX > elePos.x() && mouseX < elePos.x() + ui_elements[e].getWidth() &&
					mouseY > elePos.y() && mouseY < elePos.y() + ui_elements[e].getHeight())
			{
				return true;
			}

			return false;
		}

		//Renders the UI
		void render(ui_shader &shader)
		{
			for (unsigned int i = 1; i < ui_elements.size(); i++)
			{
				ui_elements[i].render(shader);
			}
		}
	};
}