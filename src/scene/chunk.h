namespace octet
{
  class chunk
	{
	public:
    //Creates a 3D array which stores all the types of "voxels"
		uint8_t block[CX][CY][CZ];
    //Used to point to the Vertex buffer object
		GLuint vbo, nbo, ebo;
    //The amount of vertices we have
		int elements, edgeElements;
    //If the chunk has been changed then redraw it
		bool changed;
    //Used to store the vertices, color, and normals
		typedef glm::detail::tvec4<GLbyte> byte4;
		typedef glm::detail::tvec3<GLbyte> byte3;
		
		chunk()
		{
      //Set enough memory for the chunk
			memset(block, 0, sizeof block);
      //Reset the amount of vertices to 0
			elements = 0;
			edgeElements = 0;

      //Set its changed to true, to rebuild the chunk
			changed = true;
      //Generate a buffer for the VBO
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &nbo);
			glGenBuffers(1, &ebo);
	  }

    //When the chunk is deleted, delete the VBO
		~chunk()
		{
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &nbo);
			glDeleteBuffers(1, &ebo);
		}

    //Gets the voxel at position xyz
		uint8_t get(int x, int y, int z)
		{
			return block[x][y][z];
		}

    //Sets the voxel to a type and then says that the block has been changed so needs to be rebuilt
		void set(int x, int y, int z, uint8_t type)
		{
      //Sets the block to the type sent ie(1 == grass, 2 == stone etc)
			block[x][y][z] = type;
			changed = true;
		}

		void setChanged(bool _changed)
		{
			changed = _changed;
		}

		void update(chunk **c)
		{
			//Sets changed to false so we know the chunk has already been rebuilt
			changed = false;

			//Create a vertex array with a maximum size of the chunk vertices
			byte4 vertex[CX * CY * CZ * 6 * 6];
			byte3 normal[CX * CY * CZ * 6 * 6];
			byte4 edges[CX * CY * CZ * 6 * 4 * 2];

			//Used to keep track of what vertex we are current on
			int i = 0;

			//Used to keep track of the edge count we are on.
			int j = 0;

			int vertSaveXPositive[6];
			int vertSaveXNegative[6];
			int vertSaveYPositive[CZ][6];
			int vertSaveYNegative[CZ][6];
			int vertSaveZPositive[CY][6];
			int vertSaveZNegative[CY][6];

			//Loops through all the positions
			for(int x = 0; x < CX; x++)
			{
				for(int y = 0; y < CY; y++)
				{

					for(int z = 0; z < CZ; z++)
					{

						//If the block is empty then go to the next position in the loop
						if(!block[x][y][z])
							continue;

						if ((x > 0 && !block[x - 1][y][z]) || (x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))))
						{
							if (z > 0 && block[x][y][z] == block[x][y][z - 1] && vertSaveXNegative[0] > -1)
							{
								vertex[vertSaveXNegative[1]] = byte4(vertex[vertSaveXNegative[1]].x, vertex[vertSaveXNegative[1]].y, vertex[vertSaveXNegative[1]].z + 1, block[x][y][z]);
								vertex[vertSaveXNegative[4]] = byte4(vertex[vertSaveXNegative[4]].x, vertex[vertSaveXNegative[4]].y, vertex[vertSaveXNegative[4]].z + 1, block[x][y][z]);
								vertex[vertSaveXNegative[5]] = byte4(vertex[vertSaveXNegative[5]].x, vertex[vertSaveXNegative[5]].y, vertex[vertSaveXNegative[5]].z + 1, block[x][y][z]);

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}
								
								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x, y + 1, z, 1);
									edges[j++] = byte4(x, y, z, 1);
								}
							}

							//Adds a face to the negative x side of the voxel if there is no voxel to its -x side
							else
							{
								vertSaveXNegative[0] = i;
								normal[i] = byte3(-1, 0, 0);
								vertex[i++] = byte4(x,     y,     z,     block[x][y][z]);

								vertSaveXNegative[1] = i;
								normal[i] = byte3(-1, 0, 0);
								vertex[i++] = byte4(x,     y,     z + 1, block[x][y][z]);

								vertSaveXNegative[2] = i;
								normal[i] = byte3(-1, 0, 0);
								vertex[i++] = byte4(x,     y + 1, z,     block[x][y][z]);
								
								vertSaveXNegative[3] = i;
								normal[i] = byte3(-1, 0, 0);
								vertex[i++] = byte4(x,     y + 1, z,     block[x][y][z]);

								vertSaveXNegative[4] = i;
								normal[i] = byte3(-1, 0, 0);
								vertex[i++] = byte4(x,     y,     z + 1, block[x][y][z]);

								vertSaveXNegative[5] = i;
								normal[i] = byte3(-1, 0, 0);
								vertex[i++] = byte4(x,     y + 1, z + 1, block[x][y][z]);
								
								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}
							}
						}

						else
						{
							for (int k = 0; k < 6; k++)
								vertSaveXNegative[k] = -1;
						}

						if ((x < CX - 1 && !block[x + 1][y][z]) || (x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))))
						{
							if (z > 0 && block[x][y][z] == block[x][y][z - 1] && vertSaveXPositive[0] > -1)
							{
								vertex[vertSaveXPositive[2]] = byte4(vertex[vertSaveXPositive[2]].x, vertex[vertSaveXPositive[2]].y, vertex[vertSaveXPositive[2]].z + 1, block[x][y][z]);
								vertex[vertSaveXPositive[4]] = byte4(vertex[vertSaveXPositive[4]].x, vertex[vertSaveXPositive[4]].y, vertex[vertSaveXPositive[4]].z + 1, block[x][y][z]);
								vertex[vertSaveXPositive[5]] = byte4(vertex[vertSaveXPositive[5]].x, vertex[vertSaveXPositive[5]].y, vertex[vertSaveXPositive[5]].z + 1, block[x][y][z]);

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x + 1, y, z, 1);
									edges[j++] = byte4(x + 1, y, z + 1, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y + 1, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y, z + 1, 1);
								}

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x + 1, y + 1, z, 1);
									edges[j++] = byte4(x + 1, y, z, 1);
								}
							}

							//Adds a face to the positive x side of the voxel if there is no voxel to its +x side
							else
							{
								vertSaveXPositive[0] = i;
								normal[i] = byte3(1, 0, 0);
								vertex[i++] = byte4(x + 1, y,     z,     block[x][y][z]);

								vertSaveXPositive[1] = i;
								normal[i] = byte3(1, 0, 0);
								vertex[i++] = byte4(x + 1, y + 1, z,     block[x][y][z]);

								vertSaveXPositive[2] = i;
								normal[i] = byte3(1, 0, 0);
								vertex[i++] = byte4(x + 1, y,     z + 1, block[x][y][z]);

								vertSaveXPositive[3] = i;
								normal[i] = byte3(1, 0, 0);
								vertex[i++] = byte4(x + 1, y + 1, z,     block[x][y][z]);

								vertSaveXPositive[4] = i;
								normal[i] = byte3(1, 0, 0);
								vertex[i++] = byte4(x + 1, y + 1, z + 1, block[x][y][z]);

								vertSaveXPositive[5] = i;
								normal[i] = byte3(1, 0, 0);
								vertex[i++] = byte4(x + 1, y,     z + 1, block[x][y][z]);

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x + 1, y, z, 1);
									edges[j++] = byte4(x + 1 , y + 1, z, 1);
								}

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x + 1, y, z, 1);
									edges[j++] = byte4(x + 1, y, z + 1, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y + 1, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y, z + 1, 1);
								}
							}
						}

						else
						{
							for (int k = 0; k < 6; k++)
								vertSaveXPositive[k] = -1;
						}

						//Does the same here for the -y
						if ((y > 0 && !block[x][y - 1][z]) || (y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))))
						{
							if (z > 0 && block[x][y][z] == block[x][y][z - 1] && vertSaveYNegative[y][0] > -1) 
							{
								vertex[vertSaveYNegative[y][1]] = byte4(vertex[vertSaveYNegative[y][1]].x, vertex[vertSaveYNegative[y][1]].y, vertex[vertSaveYNegative[y][1]].z + 1, block[x][y][z]);
								vertex[vertSaveYNegative[y][2]] = byte4(vertex[vertSaveYNegative[y][2]].x, vertex[vertSaveYNegative[y][2]].y, vertex[vertSaveYNegative[y][2]].z + 1, block[x][y][z]);
								vertex[vertSaveYNegative[y][5]] = byte4(vertex[vertSaveYNegative[y][5]].x, vertex[vertSaveYNegative[y][5]].y, vertex[vertSaveYNegative[y][5]].z + 1, block[x][y][z]);

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y, z + 1, 1);
									edges[j++] = byte4(x + 1, y, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x + 1, y, z + 1, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z < 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x + 1, y, z, 1);
									edges[j++] = byte4(x, y, z, 1);
								}
							}

							else
							{
								vertSaveYNegative[y][0] = i;
								normal[i] = byte3(0, -1, 0);
								vertex[i++] = byte4(x,     y,     z,     block[x][y][z]);

								vertSaveYNegative[y][1] = i;
								normal[i] = byte3(0, -1, 0);
								vertex[i++] = byte4(x + 1, y,     z + 1, block[x][y][z]);

								vertSaveYNegative[y][2] = i;
								normal[i] = byte3(0, -1, 0);
								vertex[i++] = byte4(x,     y,     z + 1, block[x][y][z]);

								vertSaveYNegative[y][3] = i;
								normal[i] = byte3(0, -1, 0);
								vertex[i++] = byte4(x,     y,     z,     block[x][y][z]);

								vertSaveYNegative[y][4] = i;
								normal[i] = byte3(0, -1, 0);
								vertex[i++] = byte4(x + 1, y,     z,     block[x][y][z]);

								vertSaveYNegative[y][5] = i;
								normal[i] = byte3(0, -1, 0);
								vertex[i++] = byte4(x + 1, y,     z + 1, block[x][y][z]);

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x + 1 , y, z, 1);
								}

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y, z + 1, 1);
									edges[j++] = byte4(x + 1, y, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x + 1, y, z + 1, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}
							}
						}

						else
						{
							for (int k = 0; k < 6; k++)
								vertSaveYNegative[y][k] = -1;
						}

						//Does the same here for the +y
						if ((y < CY - 1 && !block[x][y + 1][z]) || (y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))))
						{
							if (z > 0 && block[x][y][z] == block[x][y][z - 1] && vertSaveYPositive[y][0] > -1)
							{
								vertex[vertSaveYPositive[y][1]] = byte4(vertex[vertSaveYPositive[y][1]].x, vertex[vertSaveYPositive[y][1]].y, vertex[vertSaveYPositive[y][1]].z + 1, block[x][y][z]);
								vertex[vertSaveYPositive[y][2]] = byte4(vertex[vertSaveYPositive[y][2]].x, vertex[vertSaveYPositive[y][2]].y, vertex[vertSaveYPositive[y][2]].z + 1, block[x][y][z]);
								vertex[vertSaveYPositive[y][4]] = byte4(vertex[vertSaveYPositive[y][4]].x, vertex[vertSaveYPositive[y][4]].y, vertex[vertSaveYPositive[y][4]].z + 1, block[x][y][z]);

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y + 1, z, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y + 1, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x + 1, y + 1, z, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}
							}

							else
							{
								vertSaveYPositive[y][0] = i;
								normal[i] = byte3(0, 1, 0);
								vertex[i++] = byte4(x,     y + 1, z,     block[x][y][z]);

								vertSaveYPositive[y][1] = i;
								normal[i] = byte3(0, 1, 0);
								vertex[i++] = byte4(x,     y + 1, z + 1, block[x][y][z]);

								vertSaveYPositive[y][2] = i;
								normal[i] = byte3(0, 1, 0);
								vertex[i++] = byte4(x + 1, y + 1, z + 1, block[x][y][z]);

								vertSaveYPositive[y][3] = i;
								normal[i] = byte3(0, 1, 0);
								vertex[i++] = byte4(x,     y + 1, z,     block[x][y][z]);

								vertSaveYPositive[y][4] = i;
								normal[i] = byte3(0, 1, 0);
								vertex[i++] = byte4(x + 1, y + 1, z + 1, block[x][y][z]);

								vertSaveYPositive[y][5] = i;
								normal[i] = byte3(0, 1, 0);
								vertex[i++] = byte4(x + 1, y + 1, z,     block[x][y][z]);

								if ((z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))) || (z > 0 && !get(x, y, z - 1)))
								{
									edges[j++] = byte4(x, y + 1, z, 1);
									edges[j++] = byte4(x + 1 , y + 1, z, 1);
								}

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y + 1, z, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y + 1, z, 1);
								}

								if ((z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))) || (z < CZ - 1 && !get(x, y, z + 1)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}
							}
						}

						else
						{
							for (int k = 0; k < 6; k++)
								vertSaveYPositive[y][k] = -1;
						}

						//Does the same here for the -z
						if ((z > 0 && !block[x][y][z - 1]) || (z == 0 && (c[4] == NULL || !c[4]->get(x, y, CZ - 1))))
						{
							if (y > 0 && block[x][y][z] == block[x][y - 1][z] && vertSaveZNegative[z][0] > -1)
							{
								vertex[vertSaveZNegative[z][1]] = byte4(vertex[vertSaveZNegative[z][1]].x, vertex[vertSaveZNegative[z][1]].y + 1, vertex[vertSaveZNegative[z][1]].z, block[x][y][z]);
								vertex[vertSaveZNegative[z][2]] = byte4(vertex[vertSaveZNegative[z][2]].x, vertex[vertSaveZNegative[z][2]].y + 1, vertex[vertSaveZNegative[z][2]].z, block[x][y][z]);
								vertex[vertSaveZNegative[z][4]] = byte4(vertex[vertSaveZNegative[z][4]].x, vertex[vertSaveZNegative[z][4]].y + 1, vertex[vertSaveZNegative[z][4]].z, block[x][y][z]);

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z, 1);
									edges[j++] = byte4(x + 1, y, z, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x + 1, y, z, 1);
									edges[j++] = byte4(x, y, z, 1);
								}
							}

							else
							{
								vertSaveZNegative[z][0] = i;
								normal[i] = byte3(0, 0, -1);
								vertex[i++] = byte4(x,     y,     z,     block[x][y][z]);

								vertSaveZNegative[z][1] = i;
								normal[i] = byte3(0, 0, -1);
								vertex[i++] = byte4(x,     y + 1, z,     block[x][y][z]);

								vertSaveZNegative[z][2] = i;
								normal[i] = byte3(0, 0, -1);
								vertex[i++] = byte4(x + 1, y + 1, z,     block[x][y][z]);

								vertSaveZNegative[z][3] = i;
								normal[i] = byte3(0, 0, -1);
								vertex[i++] = byte4(x,     y,     z,     block[x][y][z]);

								vertSaveZNegative[z][4] = i;
								normal[i] = byte3(0, 0, -1);
								vertex[i++] = byte4(x + 1, y + 1, z,     block[x][y][z]);

								vertSaveZNegative[z][5] = i;
								normal[i] = byte3(0, 0, -1);
								vertex[i++] = byte4(x + 1, y,     z,     block[x][y][z]);

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x + 1 , y, z, 1);
								}

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y, z, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z, 1);
									edges[j++] = byte4(x + 1, y, z, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z, 1);
									edges[j++] = byte4(x, y + 1, z, 1);
								}
							}
						}

						else
						{
							for (int k = 0; k < 6; k++)
								vertSaveZNegative[z][k] = -1;
						}

						//Does the same here for the +z
						if ((z < CZ - 1 && !block[x][y][z + 1]) || (z == CZ - 1 && (c[5] == NULL || !c[5]->get(x, y, 0))))
						{
							if (y > 0 && block[x][y][z] == block[x][y - 1][z] && vertSaveZPositive[z][0] > -1)
							{
								vertex[vertSaveZPositive[z][2]] = byte4(vertex[vertSaveZPositive[z][2]].x, vertex[vertSaveZPositive[z][2]].y + 1, vertex[vertSaveZPositive[z][2]].z, block[x][y][z]);
								vertex[vertSaveZPositive[z][4]] = byte4(vertex[vertSaveZPositive[z][4]].x, vertex[vertSaveZPositive[z][4]].y + 1, vertex[vertSaveZPositive[z][4]].z, block[x][y][z]);
								vertex[vertSaveZPositive[z][5]] = byte4(vertex[vertSaveZPositive[z][5]].x, vertex[vertSaveZPositive[z][5]].y + 1, vertex[vertSaveZPositive[z][5]].z, block[x][y][z]);

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}

								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y, z + 1, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x + 1, y, z + 1, 1);
									edges[j++] = byte4(x, y, z + 1, 1);
								}
							}

							else
							{
								vertSaveZPositive[z][0] = i;
								normal[i] = byte3(0, 0, 1);
								vertex[i++] = byte4(x,     y,     z + 1, block[x][y][z]);

								vertSaveZPositive[z][1] = i;
								normal[i] = byte3(0, 0, 1);
								vertex[i++] = byte4(x + 1, y,     z + 1, block[x][y][z]);

								vertSaveZPositive[z][2] = i;
								normal[i] = byte3(0, 0, 1);
								vertex[i++] = byte4(x + 1, y + 1, z + 1, block[x][y][z]);

								vertSaveZPositive[z][3] = i;
								normal[i] = byte3(0, 0, 1);
								vertex[i++] = byte4(x,     y,     z + 1, block[x][y][z]);

								vertSaveZPositive[z][4] = i;
								normal[i] = byte3(0, 0, 1);
								vertex[i++] = byte4(x + 1, y + 1, z + 1, block[x][y][z]);

								vertSaveZPositive[z][5] = i;
								normal[i] = byte3(0, 0, 1);
								vertex[i++] = byte4(x,     y + 1, z + 1, block[x][y][z]);

								if ((y == 0 && (c[2] == NULL || !c[2]->get(x, CY - 1, z))) || (y > 0 && !get(x, y - 1, z)))
								{
									edges[j++] = byte4(x, y, z + 1, 1);
									edges[j++] = byte4(x + 1 , y, z + 1, 1);
								}

								if ((x == 0 && (c[0] == NULL || !c[0]->get(CX - 1, y, z))) || (x > 0 && !get(x - 1, y, z)))
								{
									edges[j++] = byte4(x, y, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}
								
								if ((x == CX - 1 && (c[1] == NULL || !c[1]->get(0, y, z))) || (x < CX - 1 && !get(x + 1, y, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x + 1, y, z + 1, 1);
								}

								if ((y == CY - 1 && (c[3] == NULL || !c[3]->get(x, 0, z))) || (y < CY - 1 && !get(x, y + 1, z)))
								{
									edges[j++] = byte4(x + 1, y + 1, z + 1, 1);
									edges[j++] = byte4(x, y + 1, z + 1, 1);
								}
							}
						}

						else
						{
							for (int k = 0; k < 6; k++)
								vertSaveZPositive[z][k] = -1;
						}
					}
				}
			}

			//Set the amount of vertices to i
			elements = i;

			edgeElements = j;

			//Bind the vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			//Set the data of the buffer
			glBufferData(GL_ARRAY_BUFFER, elements * sizeof *vertex, vertex, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, nbo);
			glBufferData(GL_ARRAY_BUFFER, elements * sizeof *normal, normal, GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, ebo);
			glBufferData(GL_ARRAY_BUFFER, edgeElements * sizeof * edges, edges, GL_STATIC_DRAW);
		}

		void render(chunk **c, glm::mat4 mvp, int numOfLights, glm::vec4 *light_information, glm::vec4 light_ambient, glm::vec4 *light_diffuse, flat_shader flat_shader_, color_shader color_shader_)
		{
			//If the block has changed since the last time it was built, then update it
			if(changed)
				update(c);

      //If there are no voxels in the chunk then skip it
			if(!elements)
				return;

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			flat_shader_.render(mvp, numOfLights, light_information, light_ambient, light_diffuse);

      //Binds the buffer to the current VBO
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
      //Gets each vertex position and sends it to the shader
			glEnableVertexAttribArray(attribute_pos);
			glVertexAttribPointer(attribute_pos, 4, GL_BYTE, GL_FALSE, 0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, nbo);
			glEnableVertexAttribArray(attribute_normal);
			glVertexAttribPointer(attribute_normal, 3, GL_BYTE, GL_FALSE, 0, 0);
      //Draws the chunk
			glDrawArrays(GL_TRIANGLES, 0, elements);

			glDisableVertexAttribArray(attribute_normal);
			glDisableVertexAttribArray(attribute_pos);

			mat4t modelToProjection;

			modelToProjection[0][0] = mvp[0].x;
			modelToProjection[0][1] = mvp[0].y;
			modelToProjection[0][2] = mvp[0].z;
			modelToProjection[0][3] = mvp[0].w;
			modelToProjection[1][0] = mvp[1].x;
			modelToProjection[1][1] = mvp[1].y;
			modelToProjection[1][2] = mvp[1].z;
			modelToProjection[1][3] = mvp[1].w;
			modelToProjection[2][0] = mvp[2].x;
			modelToProjection[2][1] = mvp[2].y;
			modelToProjection[2][2] = mvp[2].z;
			modelToProjection[2][3] = mvp[2].w;
			modelToProjection[3][0] = mvp[3].x;
			modelToProjection[3][1] = mvp[3].y;
			modelToProjection[3][2] = mvp[3].z;
			modelToProjection[3][3] = mvp[3].w;

			modelToProjection.transpose4x4();

			color_shader_.render(modelToProjection, vec4(0.0f, 0.0f, 0.0f, 1.0f));

			glEnableVertexAttribArray(attribute_pos);
			
			glBindBuffer(GL_ARRAY_BUFFER, ebo);
			glVertexAttribPointer(attribute_pos, 4, GL_BYTE, GL_FALSE, 0, 0);

			glLineWidth(2.0f);
			glDrawArrays(GL_LINES, 0, edgeElements);
			glLineWidth(1.0f);

			glDisableVertexAttribArray(attribute_pos);
		}
	};
}