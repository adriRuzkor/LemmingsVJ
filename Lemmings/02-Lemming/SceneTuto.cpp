#include <iostream>
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "SceneTuto.h"
#include <gl/gl.h>	// standard OpenGL include
#include <gl/glu.h>	// OpenGL utilties
#include <GL/glut.h>

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

SceneTuto::SceneTuto()
{
	map = NULL;
}

SceneTuto::~SceneTuto()
{
	if(map != NULL)
		delete map;
}


void SceneTuto::init()
{
	displ = 120;
	glm::vec2 geom[2] = { glm::vec2(0.f, 0.f), glm::vec2(float(CAMERA_WIDTH), float(CAMERA_HEIGHT)) };
	glm::vec2 texCoords[2] = { glm::vec2(float(displ)/512.0f, 0.f), glm::vec2((float(displ)+ 320.0f) /512.0f, 160.f / 256.0f) };

	initShaders();

	map = MaskedTexturedQuad::createTexturedQuad(geom, texCoords, maskedTexProgram);
	colorTexture.loadFromFile("images/fun1_hell.png", TEXTURE_PIXEL_FORMAT_RGBA);
	colorTexture.setMinFilter(GL_NEAREST);
	colorTexture.setMagFilter(GL_NEAREST);
	colorTexturePintar.loadFromFile("images/texturadefons2.png", TEXTURE_PIXEL_FORMAT_RGBA);
	colorTexturePintar.setMinFilter(GL_NEAREST);
	colorTexturePintar.setMagFilter(GL_NEAREST);
	maskTexture.loadFromFile("images/fun1_mask.png", TEXTURE_PIXEL_FORMAT_L);
	maskTexture.setMinFilter(GL_NEAREST);
	maskTexture.setMagFilter(GL_NEAREST);

	projection = glm::ortho(0.f, float(CAMERA_WIDTH - 1), float(CAMERA_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
	red_door.init(glm::vec2(65, 10), simpleTexProgram);
	goal.init(glm::vec2(222, 105), simpleTexProgram, displ);
	lemmingsInitiated = 0;
	lemmings[lemmingsInitiated].init(glm::vec2(82, 26), simpleTexProgram, displ);
	livingLemmings = 10;
	victoriousLemmings = 0;
	lemmings[lemmingsInitiated].setMapMask(&maskTexture);
	++lemmingsInitiated;
	toolbar.init(glm::vec2(65, 10), simpleTexProgram);
	accel = false;
	pause = false;

	engine = irrklang::createIrrKlangDevice();
	engine->play2D("cowboys.wav", true);
	inicialited = true;
}

void SceneTuto::update(int deltaTime)
{
	if (pause) deltaTime = 0;
	else if (accel) deltaTime = deltaTime * 2;
	currentTime += deltaTime;
	if (lemmingsInitiated < 10 && lemmingsInitiated * 3000 >= currentTime) initiateNextLemming();
	for (int i = 0; i <10; ++i){
		if (currentTime > i * 3000) {
			if (lemmings[i].position().x == goal.position().x && lemmings[i].position().y == goal.position().y) {
				
				lemmings[i].give(7); //WINNER
			}
				
			lemmings[i].update(deltaTime);

			int j = lemmings[i].report();
			if (j == -1) livingLemmings -= 1;
			else if (j == 1) victoriousLemmings += 1;
		}
	}
	if(red_door.opened())red_door.update(deltaTime);
	goal.update(deltaTime);
	toolbar.update(simpleTexProgram);
}

void SceneTuto::initiateNextLemming() {
	lemmings[lemmingsInitiated].init(glm::vec2(82, 26), simpleTexProgram, displ);
	lemmings[lemmingsInitiated].setMapMask(&maskTexture);
	++lemmingsInitiated;
}

void SceneTuto::render()
{
	glm::mat4 modelview;

	maskedTexProgram.use();
	maskedTexProgram.setUniformMatrix4f("projection", projection);
	maskedTexProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	maskedTexProgram.setUniformMatrix4f("modelview", modelview);
	//map->render(maskedTexProgram, colorTexture, maskTexture);
	maskedTexProgram.setUniform1i("fons", 1);
	map->render(maskedTexProgram, colorTexturePintar, maskTexture);
	maskedTexProgram.setUniform1i("fons", 0);
	map->render(maskedTexProgram, colorTexture, maskTexture);
	simpleTexProgram.use();
	simpleTexProgram.setUniformMatrix4f("projection", projection);
	simpleTexProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	simpleTexProgram.setUniformMatrix4f("modelview", modelview);
	red_door.render();
	goal.render();
	for (int i = 0; i < 10; ++i){
		if (currentTime > i * 3000) lemmings[i].render();
	}
	toolbar.render();



}

void SceneTuto::mouseMoved(int mouseX, int mouseY, bool bLeftButton, bool bRightButton)
{
	if (mouseX > 940 && mouseX < 960 && displ < 200 && inicialited) {
		map->free();
		displ = displ + 10;
		glm::vec2 geom[2] = { glm::vec2(0.f, 0.f), glm::vec2(float(CAMERA_WIDTH), float(CAMERA_HEIGHT)) };
		glm::vec2 texCoords[2] = { glm::vec2(float(displ) / 512.0f, 0.f), glm::vec2((float(displ) + 320.0f) / 512.0f, 160.f / 256.0f) };
		map = MaskedTexturedQuad::createTexturedQuad(geom, texCoords, maskedTexProgram);
		red_door.displacement(10);
		goal.displacement(10);
		for (int i = 0; i < lemmingsInitiated; ++i) {
			lemmings[i].displacement(10);
		}

	}
	if (mouseX > 0 && mouseX < 20 && displ > 100 && inicialited) {
		map->free();
		displ = displ - 10;
		glm::vec2 geom[2] = { glm::vec2(0.f, 0.f), glm::vec2(float(CAMERA_WIDTH), float(CAMERA_HEIGHT)) };
		glm::vec2 texCoords[2] = { glm::vec2(float(displ) / 512.0f, 0.f), glm::vec2((float(displ) + 320.0f) / 512.0f, 160.f / 256.0f) };
		map = MaskedTexturedQuad::createTexturedQuad(geom, texCoords, maskedTexProgram);
		red_door.displacement(-10);
		goal.displacement(-10);
		for (int i = 0; i < lemmingsInitiated; ++i) {
			lemmings[i].displacement(-10);
		}

	}

	int posX, posY;
	if (bLeftButton) {
		eraseMask(mouseX, mouseY);
		for (int i = 0; i < 10; ++i) {
			posX = mouseX / 3 + displ;
			posY = mouseY / 3;
			if (lemmings[i].inTheBox(posX, posY))
				//lemmings[i].give(1); //DIGGER
				//lemmings[i].give(2); //CLIMBER
				//lemmings[i].give(3); //BASHER
				lemmings[i].give(4); //PARACHUTE
				//lemmings[i].give(5); //BLOCKER
				//lemmings[i].give(6); //BUILDER

		}
	}
	else if(bRightButton)
		applyMask(mouseX, mouseY);
}

void SceneTuto::eraseMask(int mouseX, int mouseY)
{
	int posX, posY;
	
	// Transform from mouse coordinates to map coordinates
	//   The map is enlarged 3 times and displaced 120 pixels
	posX = mouseX/3 + displ;
	posY = mouseY/3;

	for (int y = max(0, posY - 4); y <= min(maskTexture.height() - 1, posY + 4); y++){
		for (int x = max(0, posX - 4); x <= min(maskTexture.width() - 1, posX + 4); x++){
			if (sqrt(pow(x - posX, 2) + pow(y - posY, 2)) < 4) maskTexture.setPixel(x, y, 0);
		}
	}
}

void SceneTuto::applyMask(int mouseX, int mouseY)
{
	int posX, posY;
	
	// Transform from mouse coordinates to map coordinates
	//   The map is enlarged 3 times and displaced 120 pixels
	posX = mouseX/3 + displ;
	posY = mouseY/3;

	for (int y = max(0, posY - 4); y <= min(maskTexture.height() - 1, posY + 4); y++){
		for (int x = max(0, posX - 4); x <= min(maskTexture.width() - 1, posX + 4); x++){
			if (sqrt(pow(x - posX, 2) + pow(y - posY, 2)) < 4) maskTexture.setPixel(x, y, 255);
		}
	}
}

void SceneTuto::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	simpleTexProgram.init();
	simpleTexProgram.addShader(vShader);
	simpleTexProgram.addShader(fShader);
	simpleTexProgram.link();
	if(!simpleTexProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << simpleTexProgram.log() << endl << endl;
	}
	simpleTexProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();

	vShader.initFromFile(VERTEX_SHADER, "shaders/maskedTexture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/maskedTexture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	maskedTexProgram.init();
	maskedTexProgram.addShader(vShader);
	maskedTexProgram.addShader(fShader);
	maskedTexProgram.link();
	if(!maskedTexProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << maskedTexProgram.log() << endl << endl;
	}
	maskedTexProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



