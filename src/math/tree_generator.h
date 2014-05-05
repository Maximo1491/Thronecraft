namespace octet
{ 
  class tree_generator
	{
	private:
		//Create a structure to hold the information for
		//the branches in our tree
		struct TreeBox
		{
			int branchIndex;
			int parentBranchIndex;
			float branchAngleX;
			float branchAngleZ;
			float parentHeightAtBase;
			float branchHeight;
			float radiusOffset;
			mat4t modelToWorld;
			vec4 branchStartPoint;
		};

		//Declare all variables here
		int iterations, masterIterations, maxIterations, ignoreSize, numberOfBranches, branchSubSections;
		float height, width, depth, branchRadius, angle, leafWidth, scale;
		string originalAxiom, ignore;
		bool done;

		dynarray<char*> rules;
		dynarray<TreeBox*> formedTree;
		dynarray<GLfloat> vertices, normals, colors;
		dynarray<GLushort> texCoords;
		dynarray<char> formula, newFormula;

	public:

		//Enumeration to tell what kind of tree to make.
		enum
		{
			dead,
			dying,
			alive,
		};
		
		//Constructor
		tree_generator() 
		{
			srand((unsigned) time(0));

			//Set the size of the branches for our tree.
			height = 1.0f;
			width = 0.10f;
			depth = 0.10f;
			leafWidth = 10 * width;
			branchRadius = 0.05f;
			branchSubSections = 8;
			done = false;

			//Set the tree's scale percentage.
			scale = 0.25f;

			//Read the initial file.  This will trigger a domino
			//effect where the entire tree is made from this file.
			ReadFile("../../assets/thronecraft/Stochastic_Sample.txt");
		}

		//Deconstructor
		~tree_generator()
		{
			
		}

		//A command to generate another tree.
		void AddTree(int treeType, glm::vec3 origin)
		{
			glm::mat4 pos = glm::translate(glm::mat4(1.0), origin);
			FormFormula(treeType, pos);
		}

		//The ReadFile() function is used to read the outside
		//text file and translate the information into the
		//data variables used for building the L-System.
		void ReadFile(const char* fileName)
		{
			//Declare and initialize the variables needed to
			//read the file.
			originalAxiom = new char;
			char* buffer;
			long bufferSize;
			ignoreSize = 0;

			//Openthe file to be read.
			FILE* file = fopen(fileName, "r");

			//Check to see if the file opened successfully.
			//If not inform the user.
			if (file == NULL)
				printf("File does not exist!\n");

			else
			{
				//Find the end of the file, get the size of the
				//file, and reqind back to the beginning for
				//the reading process.
				fseek(file, 0, SEEK_END);
				bufferSize = ftell(file);
				rewind(file);

				//Set the buffer variable to the number of bufferSize
				//characters.  Read the entire file into the buffer.
				buffer = (char*)malloc(sizeof(char*) * bufferSize);
				fread(buffer, 1, bufferSize, file);

				//Find the end of the first line.
				strtok(buffer, "\n");

				do
				{
					//Check to see if the line refers to the Axiom.
					if (strcmp(buffer, "Axiom") == 0)
					{
						//Find the end of the next line.
						buffer = strtok(NULL, "\n");
						
						//Store the current line into the formula.  This
						//is the axiom of the formula.
						for (unsigned int i = 0; i < strlen(buffer); i++)
						{
							formula.push_back(buffer[i]);
						}

						//Store the axiom into the originalAxiom in case
						//we need to revert the formula back to the axiom.
						originalAxiom = buffer;

						//Read the next line.
						buffer = strtok(NULL, "\n");
					}

					//Check to see if the line refers to a rule.  If it
					//does push the line onto the rules stack.
					else if (strcmp(buffer, "Rule") == 0)
					{
						buffer = strtok(NULL, "\n");
						rules[rules.size()] = buffer;
						buffer = strtok(NULL, "\n");
					}

					//Check to see if the line refers to the angle of
					//the L-System.  If so, convert the line to a float
					//and save it in the angle variable.
					else if (strcmp(buffer, "Angle") == 0)
					{
						buffer = strtok(NULL, "\n");
						angle = (float)atof(buffer);
						buffer = strtok(NULL, "\n");
					}

					//Check to see if the line refers to the iterations of
					//the L-System.  If so convert the line to an integer
					//and save it in the iterations variable.
					else if (strcmp(buffer, "Iterations") == 0)
					{
						buffer = strtok(NULL, "\n");
						masterIterations = atoi(buffer);
						buffer = strtok(NULL, "\n");
					}

					//Check to see if the line refers to the maximum iterations
					//we want this L-System to go to.  If so change the line
					//an integer and store it in the maxIterations variable.
					else if (strcmp(buffer, "Max Iterations") == 0)
					{

						buffer = strtok(NULL, "\n");
						maxIterations = atoi(buffer);
						buffer = strtok(NULL, "\n");
					}

					//Check to see if the line refers to the ignore items.  If
					//so store the line in the ignore variable.
					else if (strcmp(buffer, "Ignore") == 0)
					{
						buffer = strtok(NULL, "\n");
						ignore = buffer;
						buffer = strtok(NULL, "\n");
					}
				}
				//Continue reading lines until the buffer is empty.
				while (buffer != NULL);

				//Delete the buffer to prevent a memory leak and
				//close the file.
				delete buffer;
				fclose(file);
			}
		}

		//This function will form the forumla based on the rules
		//and number of iterations that the L-System is currently
		//set to.
		void FormFormula(int treeType, glm::mat4 origin)
		{
			iterations = masterIterations + ((rand() % 2) - 1);

			//Go through each iteration to form the formula.
			for (int i = 0; i < iterations; i++)
			{
				//Go through each character in the formula checking
				//each to all the rules.
				for (unsigned int j = 0; j < formula.size(); j++)
				{
					//Get a random number for stochastic L-Systems.
					//Keep track of the accumulated percentage in the
					//stochastic L-System.
					//Declare a ruleApplied booleon to see if a
					//rule has been applied to this character in the
					//formula.
					bool ruleApplied = false;
					int random = rand() % 100;
					float accumulatedPercentage = 0.0f;

					//Go through each rule.  Check the character in the formula
					//to the rule.  Apply the rule if the lhs of the rule matches.
					for (unsigned int k = 0; k < rules.size(); k++)
					{
						//Get the lhs of the rule.
						int equalIndex;
						
						if (strrchr(rules[k], '=') != NULL)
							equalIndex = strrchr(rules[k], '=') - rules[k] + 1;

						//Check to see if the lhs of the rule is greater than 1
						//and if a rule has already been applied.
						if (strrchr(rules[k], '=') != NULL && !ruleApplied)
						{
							//Get the percent index, space index after the percentage, and the base
							//of the rule.
							char* base = strtok(rules[k], " ");

							//Check to see if the base matches the character being checked in the
							//formula.
							if (base[0] == formula[j])
							{
								//Convert the percentage in the rule to a float.  Store it
								//in the percentChance variable.
								//Add the percentChange to the accumulated percentage.
								float percentChance = (float)atof(strtok(rules[k], "%"));
								accumulatedPercentage += percentChance;

								//Check to see if the random percentage generated earlier is within the
								//accumulated percentage range.  If so, apply the stochastis rule.
								if ((float)random < accumulatedPercentage)
								{
									int ruleSize = strchr(rules[k], '0/') - rules[k] + 1;

									for (int l = equalIndex + 1; l < ruleSize; l++)
										newFormula.push_back(rules[k][l]);

									//Signal that a rule has been applied to this character.
									ruleApplied = true;
								}
							}
						}

						//No rules were applied.  The character does not change.
						//Push the character onto the formula being built.
						if (!ruleApplied && k == rules.size() - 1)
						{
							newFormula.push_back(formula[j]);
						}
					}
				}

				//Place the new formula into formula to prepare for the next
				//iteration.  Clear the new formula to build the next iteration
				//formula.
				formula = newFormula;
				
				for (unsigned int i = 0; i < newFormula.size(); i++)
					newFormula.pop_back();
			}

			//Execute MakeTree() to form the 3D objects for rendering.
			MakeTree(treeType, origin);
		}

		//This function will compute the vertices and indices of the tree for drawing.
		void MakeTree(int treeType, glm::mat4 origin)
		{
			TreeBox* treeBox;
			std::vector<mat4t> positionStack;
			std::vector<int> indexStack;

			GLfloat barkColor;
			GLfloat leafColor;

			treeBox = new TreeBox();
			treeBox->branchHeight = 0;
			treeBox->branchIndex = 0;
			treeBox->parentBranchIndex = -1;
			treeBox->parentHeightAtBase = 0;
			treeBox->branchAngleX = 0.0f;
			treeBox->branchAngleZ = 0.0f;

			treeBox->modelToWorld.loadIdentity();
			treeBox->branchStartPoint = treeBox->modelToWorld.row(3);

			mat4t modelToWorld = treeBox->modelToWorld;
			bool formCheck = false;
			int totalTreeHeight = 0;
			float baseAngle = max((float)(rand() % (int)(angle * 100)) / 100, angle / 2);

			//Cycle through the formula checking each character.
			for (unsigned int i = 0; i < formula.size(); i++)
			{
				//If the character is an 'F', we add 1 to the branch height
				//and move the modelToWorld up by one height.  This keeps
				//the modelToWorld halfway up the tree for rendering the
				//box.
				if (formula[i] == 'F')
				{
					vec4 branchVector = modelToWorld.row(3) - treeBox->branchStartPoint;

					treeBox->branchHeight++;
					treeBox->modelToWorld.translate(0, height, 0);
					totalTreeHeight++;

					modelToWorld.translate(0, height * 2.0f, 0);
				}

				//If the character is a '[', move the modelToWorld to the end
				//of the branch, save the modeltoWorld on the position stack,
				//and reset the model to world back to its original place.
				else if (formula[i] == '[')
				{
					indexStack.push_back(treeBox->branchIndex);
					positionStack.push_back(modelToWorld);
				}

				//If the character is a ']', check to see if the current
				//branch has a height.  If it does, push the branch onto the
				//formedTree stack.  Create a new branch and set the modelToWorld
				//to the position on the back of the position stack.
				//Pop the modelToWorld on the back of the position stack.
				else if (formula[i] == ']')
				{
					if (treeBox->branchIndex >= (int)formedTree.size())
					{
						treeBox->branchIndex = formedTree.size();
						formedTree.push_back(treeBox);
					}

					else
						formedTree[treeBox->branchIndex] = treeBox;


					treeBox = formedTree[indexStack.back()];
					modelToWorld = positionStack.back();

					indexStack.pop_back();
					positionStack.pop_back();
				}

				//If the character is '-' or '+', check to see if the current branch
				//has a height greater than 0.  If so push the branch onto the back
				//of the formedTree stack.  Move the modelToWorld to the end of the
				//current branch as well.  Create a new branch and rotate the
				//modelToWorld on the Z axis by the angle.
 				else if ((formCheck = formula[i] == '-') || formula[i] == '+')
				{
					if (treeBox->branchIndex >= (int)formedTree.size())
						formedTree.push_back(treeBox);
					else
						formedTree[treeBox->branchIndex] = treeBox;
					
					int parentIndex = treeBox->branchIndex;
					float parentHeight = treeBox->branchHeight;
					float parentAngleX = treeBox->branchAngleX;
					float parentAngleZ = treeBox->branchAngleZ;

					treeBox = new TreeBox();
					treeBox->branchHeight = 0;
					treeBox->branchIndex = formedTree.size();
					treeBox->parentBranchIndex = parentIndex;
					treeBox->parentHeightAtBase = parentHeight;
					
					float derivedAngle = baseAngle + (float)(rand() % 500) / 100;

					if (derivedAngle > angle)
						derivedAngle -= angle;
					
					treeBox->branchAngleX = baseAngle + (float)(rand() % 500) / 100;
					treeBox->branchAngleZ = (float)(rand() % 36000) / 100;
					
					if (formCheck)
					{
						modelToWorld.rotateY(-(treeBox->branchAngleZ));
						modelToWorld.rotateZ(-(treeBox->branchAngleX));
					}
					else
					{
						modelToWorld.rotateY(treeBox->branchAngleZ);
						modelToWorld.rotateZ(treeBox->branchAngleX);
					}

					mat4t swap = modelToWorld;
					treeBox->modelToWorld = swap;
					treeBox->branchStartPoint = treeBox->modelToWorld.row(3);
				}

				//If the character is '\' or '/', check to see if the current branch
				//has a height greater than 0.  If so push the branch onto the back
				//of the formedTree stack.  Move the modelToWorld to the end of the
				//current branch as well.  Create a new branch and rotate the
				//modelToWorld on the X axis by the angle.
				else if ((formCheck = formula[i] == '\\') || formula[i] == '/')
				{	
					if (treeBox->branchIndex >= (int)formedTree.size())
						formedTree.push_back(treeBox);
					else
						formedTree[treeBox->branchIndex] = treeBox;

					int parentIndex = treeBox->branchIndex;
					float parentHeight = treeBox->branchHeight;
					float parentAngleX = treeBox->branchAngleX;
					float parentAngleZ = treeBox->branchAngleZ;
					
					treeBox = new TreeBox();
					treeBox->branchHeight = 0;
					treeBox->branchIndex = formedTree.size();
					treeBox->parentBranchIndex = parentIndex;
					treeBox->parentHeightAtBase = parentHeight;

					treeBox->branchAngleX = abs(parentAngleX) + (((float)(rand() % 1000) / 1000.0f) * 33);
					treeBox->branchAngleZ = abs(parentAngleZ) + (((float)(rand() % 1000) / 1000.0f) * 33);
					
					if (formCheck)
					{
						modelToWorld.rotateZ(-(treeBox->branchAngleZ));
						modelToWorld.rotateX(-(treeBox->branchAngleX));
					}
					else
					{
						modelToWorld.rotateZ(treeBox->branchAngleZ);
						modelToWorld.rotateX(treeBox->branchAngleX);
					}

					mat4t swap = modelToWorld;
					treeBox->modelToWorld = swap;
					treeBox->branchStartPoint = treeBox->modelToWorld.row(3);
				}

				//Check to see if this is the last character in the formula.
				//If so and the branch height is greater than 0, push the branch
				//onto the formedTree stack.
				if (treeBox->branchHeight > 0 && i + 1 == formula.size())
				{
					if (treeBox->branchIndex >= (int)formedTree.size())
					{
						treeBox->branchIndex = formedTree.size();
						formedTree.push_back(treeBox);
					}

					else
						formedTree[treeBox->branchIndex] = treeBox;
				}
			}

			//Set the modelToWorld back to the origin.
			modelToWorld[0][0] = origin[0].x;
			modelToWorld[0][1] = origin[0].y;
			modelToWorld[0][2] = origin[0].z;
			modelToWorld[0][3] = origin[0].w;
			modelToWorld[1][0] = origin[1].x;
			modelToWorld[1][1] = origin[1].y;
			modelToWorld[1][2] = origin[1].z;
			modelToWorld[1][3] = origin[1].w;
			modelToWorld[2][0] = origin[2].x;
			modelToWorld[2][1] = origin[2].y;
			modelToWorld[2][2] = origin[2].z;
			modelToWorld[2][3] = origin[2].w;
			modelToWorld[3][0] = origin[3].x;
			modelToWorld[3][1] = origin[3].y;
			modelToWorld[3][2] = origin[3].z;
			modelToWorld[3][3] = origin[3].w;

			modelToWorld.transpose4x4();

			//Keep track of the number of branches that we have calculated.
			numberOfBranches = 0;

			//Keep track of the number of elements in each array.
			//int vertsSize = totalTreeHeight * branchSubSections * 24;
			//int texCsSize = totalTreeHeight * branchSubSections * 12;

			float angleOffset = 360.0f / branchSubSections;

			dynarray<float> leafHeight;
			dynarray<mat4t> leafMatrices;
			
			mat4t scaleMat;
			scaleMat.loadIdentity();
			scaleMat[0][0] = scale;
			scaleMat[1][1] = scale;
			scaleMat[2][2] = scale;

			if (treeType > dead)
			{
				leafColor = 1.0f;

				if (treeType == dying)
				{
					barkColor = 0.0f;
					leafColor = 1.0f;
				}
				else
				{
					barkColor = 0.0f;
					leafColor = 1.0f;
				}
			}

			else
			{
				barkColor = 0.0f;
				leafColor = 1.0f;
			}

			//Cycle through all the branches in the formedTree stack.
			//We will determine the radius of each branch section, find the vertices for all
			//the quads needed to make that section, and move the the next height section.
			for (dynarray<TreeBox*>::iterator iter = formedTree.begin(); iter != formedTree.end(); iter++)
			{
				float radiusOffset;
				float currentRadius;
				float nextRadius;

				//Determine the initial radius of this branch using the branch height.
				currentRadius = branchRadius * (*iter)->branchHeight;
				
				//Check to see if the radius of this branch is larger than the parent branch
				//radius where the current branch begins.  If it is larger, we set the radius
				//to the radius of the parent branch at that location and subtract the
				//radius offset to make it a bit smaller.
				if ((*iter)->parentBranchIndex > -1 && currentRadius > formedTree[(*iter)->parentBranchIndex]->radiusOffset * formedTree[(*iter)->parentBranchIndex]->branchHeight - (formedTree[(*iter)->parentBranchIndex]->radiusOffset * (*iter)->parentHeightAtBase))
					currentRadius = formedTree[(*iter)->parentBranchIndex]->radiusOffset * formedTree[(*iter)->parentBranchIndex]->branchHeight - (formedTree[(*iter)->parentBranchIndex]->radiusOffset * ((*iter)->parentHeightAtBase + 1));

				//Determine the radius offset of the branch.  Save that offset if any calculations
				//need to be made when rendering its children branches.
				radiusOffset = currentRadius / (*iter)->branchHeight;
				(*iter)->radiusOffset = radiusOffset;

				currentRadius += radiusOffset;
				nextRadius = currentRadius;

				//Determine if the tree is dying or alive.  If so save the branch
				//matrix and the height for rendering the leaves later.
				if (treeType > 0)
				{
					leafHeight.push_back((*iter)->branchHeight);
					leafMatrices.push_back((*iter)->modelToWorld);
				}

				//Cycle through all the height sections of the branch.
				for (int i = 0; i < (*iter)->branchHeight; i++)
				{
					//Set the current angle to draw the branch section to 0.
					//Determine the current section's radius by subtracting the radius offset.
					float currentAngle = 0.0f;
					currentRadius -= radiusOffset;
					nextRadius = currentRadius - radiusOffset;

					//If this is the base of the trunk of the tree increase the radius to make it
					//look like there are roots at the base.
					if ((*iter)->parentBranchIndex < 0 && i <= 0)
						currentRadius = currentRadius + (radiusOffset * 10);

					//Cycle through the subsections of each height section and determine the quad vertex locations,
					//the normals, the colors, and the texture coordinates.
					//We get the normals of the quads first before as we don't want the scale matrix and terrain location
					//matrix affecting the normals.
					//The vertices will have the scale matrix and terrain position location applied to the vertices.
					for (int j = 0; j < branchSubSections; j++)
					{	
						vec4 vert1 = vec4(currentRadius * cos(currentAngle * 0.0174532925f), ((float)i * (height * 2.0f)) - (*iter)->branchHeight, currentRadius * sin(currentAngle * 0.0174532925f), 1.0f) * (*iter)->modelToWorld * scaleMat * modelToWorld;
						vec4 vert2 = vec4(nextRadius * cos(currentAngle * 0.0174532925f), ((float)(i + 1) * (height * 2.0f)) - (*iter)->branchHeight, nextRadius * sin(currentAngle * 0.0174532925f), 1.0f) * (*iter)->modelToWorld * scaleMat * modelToWorld;
						vec4 vert3 = vec4(currentRadius * cos((currentAngle + angleOffset) * 0.0174532925f), ((float)i * (height * 2.0f)) - (*iter)->branchHeight, currentRadius * sin((currentAngle + angleOffset) * 0.0174532925f), 1.0f) * (*iter)->modelToWorld * scaleMat * modelToWorld;
						vec4 vert4 = vec4(currentRadius * cos((currentAngle + angleOffset) * 0.0174532925f), ((float)i * (height * 2.0f)) - (*iter)->branchHeight, currentRadius * sin((currentAngle + angleOffset) * 0.0174532925f), 1.0f) * (*iter)->modelToWorld * scaleMat * modelToWorld;
						vec4 vert5 = vec4(nextRadius * cos(currentAngle * 0.0174532925f), ((float)(i + 1) * (height * 2.0f)) - (*iter)->branchHeight, nextRadius * sin(currentAngle * 0.0174532925f), 1.0f) * (*iter)->modelToWorld * scaleMat * modelToWorld;
						vec4 vert6 = vec4(nextRadius * cos((currentAngle + angleOffset) * 0.0174532925f), ((float)(i + 1) * (height * 2.0f)) - (*iter)->branchHeight, nextRadius * sin((currentAngle + angleOffset) * 0.0174532925f), 1.0f) * (*iter)->modelToWorld * scaleMat * modelToWorld;

						vertices.push_back(vert1.x());
						vertices.push_back(vert1.y());
						vertices.push_back(vert1.z());
						vertices.push_back(barkColor);

						vertices.push_back(vert2.x());
						vertices.push_back(vert2.y());
						vertices.push_back(vert2.z());
						vertices.push_back(barkColor);

						vertices.push_back(vert3.x());
						vertices.push_back(vert3.y());
						vertices.push_back(vert3.z());
						vertices.push_back(barkColor);

						vertices.push_back(vert4.x());
						vertices.push_back(vert4.y());
						vertices.push_back(vert4.z());
						vertices.push_back(barkColor);

						vertices.push_back(vert5.x());
						vertices.push_back(vert5.y());
						vertices.push_back(vert5.z());
						vertices.push_back(barkColor);

						vertices.push_back(vert6.x());
						vertices.push_back(vert6.y());
						vertices.push_back(vert6.z());
						vertices.push_back(barkColor);

						//After determining the sections vertices, colors, tecture coordinates, and
						//normals we go to the next quad subsection.
						currentAngle += angleOffset;
					}

					//If this was the base height section of the trunk we need to adjust the radius
					//of the base back to the original radius before moving on.
					if ((*iter)->parentBranchIndex < 0 && i <= 0)
						currentRadius = currentRadius - (radiusOffset * 10);
				}
			}
			
			//Call the MakeLeaves function to draw the leaf bunches for the leaves.
			MakeLeaves(leafHeight, leafMatrices, modelToWorld, scaleMat, 6, leafColor);

			//Clear the formula and formed tree then load the original axiom to prepare for the next tree.
			ClearTreeForm(formedTree);
			ClearFormula();
			LoadOriginalAxiom();
		}

		//A function to make the leaves for a tree.  The height and start points are passed
		//to this function to determine where the leaf bunch will be rendered.
		void MakeLeaves(dynarray<float> &leafHeight, dynarray<mat4t> leafMat, mat4t &origin, mat4t &scaleMat, int numberOfLeafQuads, GLfloat &leafColor)
		{
			//Determine the number of quads that are needed and calculate the radius offset.
			//Keep track of the highest branch in the tree.
			float radiusOffset = 360.0f / (numberOfLeafQuads * 2);
			float highestBranch = 0.0f;

			//Loop through all the branches in the tree.
			for (unsigned int i = 0; i < leafHeight.size(); i++)
			{
				//Determine if this branch is the highest branch in the tee thus far.
				if (leafHeight[i] > highestBranch) highestBranch = leafHeight[i];
				
				//Determine the leaf radius in relation to the branch height.
				float leafRadius = 20 / leafHeight[i];

				//We don't want leaf bunches that are extremely small.  If the leaf radius
				//is less than 10 make it equal to 10.
				if (leafRadius < 10.0f) leafRadius = 10.0f;

				//Check if the branch height is greater than 1.  If it is draw leaves.
				if (leafHeight[i] > 1.0f)
				{
					//Move the branch starting position to the end of the branch.
					leafMat[i].translate(0.0f, leafHeight[i], 0.0f);
					mat4t branchPos;
					branchPos = leafMat[i];

					//Check to see if the branch height is greater than 125% of the highest branch.
					if (branchPos.row(3).y() > highestBranch * 1.25)
					{
						//Cycle through the number of leaf quads we want to generate and make
						//the leaf bunch.
						//The matrix will be moved to each location that each quad will be rendered.
						//The vertices, colors, texture coordinates, and normals will be saved in
						//the dynamic arrays.
						for (int j = 0; j < numberOfLeafQuads; j++)
						{
							leafMat[i].rotateY(radiusOffset);
							leafMat[i].translate(-leafRadius, leafRadius, 0.0f);

							vec4 leafVert = vec4(leafMat[i].row(3).x(), leafMat[i].row(3).y(), leafMat[i].row(3).z(), 1.0f) * scaleMat * origin;

							vertices.push_back(leafVert.x());
							vertices.push_back(leafVert.y());
							vertices.push_back(leafVert.z());
							vertices.push_back(leafColor);

							leafMat[i].translate(leafRadius * 2, 0.0f, 0.0f);

							leafVert = vec4(leafMat[i].row(3).x(), leafMat[i].row(3).y(), leafMat[i].row(3).z(), 1.0f) * scaleMat * origin;

							vertices.push_back(leafVert.x());
							vertices.push_back(leafVert.y());
							vertices.push_back(leafVert.z());
							vertices.push_back(leafColor);

							leafMat[i].translate(0.0f, -leafRadius * 2, 0.0f);

							leafVert = vec4(leafMat[i].row(3).x(), leafMat[i].row(3).y(), leafMat[i].row(3).z(), 1.0f) * scaleMat * origin;

							vertices.push_back(leafVert.x());
							vertices.push_back(leafVert.y());
							vertices.push_back(leafVert.z());
							vertices.push_back(leafColor);

							leafMat[i].translate(-leafRadius * 2, leafRadius * 2, 0.0f);

							leafVert = vec4(leafMat[i].row(3).x(), leafMat[i].row(3).y(), leafMat[i].row(3).z(), 1.0f) * scaleMat * origin;

							vertices.push_back(leafVert.x());
							vertices.push_back(leafVert.y());
							vertices.push_back(leafVert.z());
							vertices.push_back(leafColor);

							leafMat[i].translate(leafRadius * 2, -leafRadius * 2, 0.0f);

							leafVert = vec4(leafMat[i].row(3).x(), leafMat[i].row(3).y(), leafMat[i].row(3).z(), 1.0f) * scaleMat * origin;

							vertices.push_back(leafVert.x());
							vertices.push_back(leafVert.y());
							vertices.push_back(leafVert.z());
							vertices.push_back(leafColor);

							leafMat[i].translate(-leafRadius * 2, 0.0f, 0.0f);

							leafVert = vec4(leafMat[i].row(3).x(), leafMat[i].row(3).y(), leafMat[i].row(3).z(), 1.0f) * scaleMat * origin;

							vertices.push_back(leafVert.x());
							vertices.push_back(leafVert.y());
							vertices.push_back(leafVert.z());
							vertices.push_back(leafColor);

							leafMat[i].translate(leafRadius, leafRadius, 0.0f);
						}
					}
				}
			}
		}

		//A function to clear all branched from the formedTree stack.
		void ClearTreeForm(dynarray<TreeBox*> &arr)
		{
			while (arr.size() > 0)
				arr.pop_back();
		}

		//A function to load the original axiom of the current L-System back into
		//the formula variable.
		void LoadOriginalAxiom()
		{
			for (int i = 0; i < originalAxiom.size(); i++)
				formula.push_back(originalAxiom[i]);
		}

		//A function to clear the newFormula and formula usually in preparation to
		//form a new L-System.
		void ClearFormula()
		{
			for (unsigned int i = 0; i < formula.size(); i++)
				formula.pop_back();

			for (unsigned int i = 0; i < newFormula.size(); i++)
				newFormula.pop_back();
		}
	};
}