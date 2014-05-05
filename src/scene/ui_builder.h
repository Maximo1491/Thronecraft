namespace octet {
	class ui_builder {
		float windowH;
		float windowW;

		dynarray<ui_element> ui_elements;

	public:
		ui_builder()
		{
			windowH = 0;
			windowW = 0;
		}

		void init(float ww, float wh, int numElements)
		{
			windowH = wh;
			windowW = ww;

      ui_elements.resize(numElements);
		}

		void addElement(int p, float x_, float y_, float w, float h)
		{
			ui_element e;
			e.init(x_, y_, w, h, windowW, windowH, glm::vec4(0,0,0,0));
			ui_elements[p] = e;
		}

    void addElement(int p, float x_, float y_, float w, float h, glm::vec4 c)
		{
			ui_element e;
			e.init(x_, y_, w, h, windowW, windowH, c);
			ui_elements[p] = e;
		}

		void addElement(int p, float x_, float y_, float w, float h, GLuint t)
		{
			ui_element e;
			e.init(x_, y_, w, h, windowW, windowH, t);
			ui_elements[p] = e;
		}

		void changeTexture(int e, string s)
		{
			ui_elements[e].changeTexture(s);
		}

    void setPos(int e, float x_, float y_)
    {
      ui_elements[e].setPos(x_, y_);
    }

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

		void render(ui_shader &shader)
		{
			for (unsigned int i = 1; i < ui_elements.size(); i++)
			{
				ui_elements[i].render(shader);
			}
		}
	};
}