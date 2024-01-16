#include "gonAppMain.h"
#define ROWS 15
#define COLS 28
#define ITERATION_SCHEDULE_DELAY 375.0f
#define BASE_SCALE_X 1.0f
#define BASE_SCALE_Y 1.0f
#define BASE_SPACING_X 2.065f
#define BASE_SPACING_Y 2.065f
#define BASE_CAMERA_TRANS_X 27.9f
#define BASE_CAMERA_TRANS_Y 16.29f
#define BASE_CAMERA_TRANS_Z 43.0f


// Declare our game instance
gonAppMain game;

gonAppMain::gonAppMain()
{
}

void gonAppMain::drawSplash(void* param) {
	clear(CLEAR_COLOR_DEPTH, Vector4(0, 0, 0, 1), 1.0f, 0);
	SpriteBatch* splash = SpriteBatch::create(
			"res/textures/logo_powered_white.png");

	splash->start();
	splash->draw(this->getWidth() * 0.5f, this->getHeight() * 0.5f, 0.0f,
			512.0f, 512.0f, 0.0f, 1.0f, 1.0f, 0.0f, Vector4::one(), true);
	splash->finish();
	SAFE_DELETE(splash);
}

void gonAppMain::initialize() {
	m_NoAudio = false;
	m_DrawDebug = false;
	m_DrawWireframe = false;
	m_Scene = NULL;

	if (isGestureSupported(Gesture::GESTURE_SWIPE)) {
		registerGesture(Gesture::GESTURE_SWIPE);
	}

	if (isMultiTouch()) {
		setMultiTouch(true);
	}

	// Display the gameplay splash screen for at least 1 second.
	displayScreen(this, &gonAppMain::drawSplash, NULL, 650L);

	m_RenderState = RenderState::StateBlock::create();

	m_ScreenDragActive = false;

	m_RenderState->setCullFace(true);
	m_RenderState->setDepthTest(true);
	m_RenderState->setBlend(true);
	m_RenderState->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
	m_RenderState->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);

	//disable vsync, unneeded
	setVsync(false);

	// Load scene
	Bundle* bundle = Bundle::create("res/gpb/lifetile.gpb");
	m_Scene = bundle->loadScene();
	SAFE_RELEASE(bundle);

	m_GridStateManager = new CStateManager(ROWS, COLS);
	prepareGridLayout();

	Camera* activeCamera = m_Scene->findNode("Camera")->getCamera();
	activeCamera->setNearPlane(10.0f);
	activeCamera->setFarPlane(1000.0f);
	activeCamera->setAspectRatio((float) getWidth() / (float) getHeight());
	activeCamera->setFieldOfView(45.0f);

	activeCamera->getNode()->setTranslationX(BASE_CAMERA_TRANS_X);
	activeCamera->getNode()->setTranslationY(BASE_CAMERA_TRANS_Y);
	activeCamera->getNode()->setTranslationZ(BASE_CAMERA_TRANS_Z);

	m_Scene->setActiveCamera(activeCamera);

	//load awesome font
	m_Font = Font::create("res/gpb/MonospaceTypewriter.gpb");

	m_UIForm = Form::create("res/ui/gonui.form");

	Slider* sldIterations = static_cast<Slider*>(m_UIForm->getControl("sldIterations"));
	sldIterations->addListener(this, Control::Listener::VALUE_CHANGED);
	m_UseIterations = sldIterations->getValue() / 1;

	Button* btnStartStop = static_cast<Button*>(m_UIForm->getControl(
			"btnStartStop"));	

	btnStartStop->addListener(this,
			Control::Listener::PRESS | Control::Listener::RELEASE);
	btnStartStop->setSkinRegion(Rectangle(238, 81, 73, 73));
	Button* btnReset = static_cast<Button*>(m_UIForm->getControl("btnReset"));
	btnReset->addListener(this,
			Control::Listener::PRESS | Control::Listener::RELEASE);

	Button* btnAudio = static_cast<CheckBox*>(m_UIForm->getControl("btnAudio"));
	btnAudio->addListener(this,
		Control::Listener::PRESS | Control::Listener::RELEASE );

	m_Theme1 = AudioSource::create("res/wav/gontheme1.wav");
	m_Theme1->setLooped(true);

	m_Theme1->play();
}

void gonAppMain::finalize() {
	SAFE_DELETE(m_GridStateManager);
	SAFE_RELEASE(m_Theme1);
	SAFE_RELEASE(m_UIForm);
	SAFE_RELEASE(m_Font);
	SAFE_RELEASE(m_Scene);
}

void gonAppMain::prepareTileAnimation(Node *node) {
	unsigned int keyTimes[] = { 0L, 118L };

		float keyValsRight[] = {
			0.0f, 0.0f, 0.0f, 1.0f,
			0.70710677f, 0.0f, 0.0f, 0.70710677f
		};

		float keyValsLeft[] = {
			0.70710677f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int keyCount = 2;

		node->createAnimation("rotateRight", Transform::ANIMATE_ROTATE, keyCount, keyTimes, keyValsRight, Curve::LINEAR);
		node->createAnimation("rotateLeft", Transform::ANIMATE_ROTATE, keyCount, keyTimes, keyValsLeft, Curve::LINEAR);
}

void gonAppMain::prepareGridLayout() {
	//make grid to scale and fit
	Node* node = m_Scene->findNode("Cube000");
	Node* refNode = node;

	node->setId("0");

	node->setCollisionObject(PhysicsCollisionObject::GHOST_OBJECT);

	prepareTileAnimation(node);

	node->setScaleX(BASE_SCALE_X);
	node->setScaleY(BASE_SCALE_Y);
	
	node->setTranslation(0.0f,
			(((ROWS * COLS) - 1) / COLS) * BASE_SPACING_Y, 0.0f);
	initializeMaterial(node->getModel(), true, false);

	for (int i = 1; i < (COLS * ROWS); i++) {
		node = refNode->clone();
		node->translateX((i - ((i / COLS) * COLS)) * BASE_SPACING_X);
		node->translateY(-1 * (i / COLS) * BASE_SPACING_Y);
		m_Scene->addNode(node);
		char sId[10];

		itoa(i, sId, 10);

		node->setId(sId);

		node->setCollisionObject(PhysicsCollisionObject::GHOST_OBJECT);
		
		prepareTileAnimation(node);

		//setup the model texture
		initializeMaterial(node->getModel(), true, false);

		SAFE_RELEASE(node);
	}
}

void gonAppMain::initializeMaterial(Model *model, bool lighting,
		bool specular) {
	Material *material = Material::create("res/mtl/lifetile.material");

	// Set the common render state block for the material
	material->setStateBlock(m_RenderState);

	if (lighting) {
		material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix",
				RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);

		Node* lightNode = m_Scene->findNode("Lamp");
		Vector3 lightDirection = lightNode->getForwardVector();
		lightDirection.normalize();

		material->getParameter("u_lightDirection")->setValue(lightDirection);
		material->getParameter("u_lightColor")->setValue(lightNode->getLight()->getColor());
	}

	model->setMaterial(material);
}

void gonAppMain::printAt(const char* text, int x, int y) {
	m_Font->start();
	m_Font->drawText(text, x, y, Vector4(0.0f, 0.5f, 1.0f, 0.5f),
			m_Font->getSize());
	m_Font->finish();
}

void gonAppMain::render(float elapsedTime) {
	// Clear the color and depth buffers
	clear(CLEAR_COLOR_DEPTH, Vector4(0.322f, 0.394f, 0.424f, 1.0f), 100.0f, 0);

	// Visit all the nodes in the scene for drawing
	m_Scene->visit(this, &gonAppMain::drawScene);

	if (m_DrawDebug) {
		printAt(m_CameraDebugText.c_str(), 150, 10);
		drawFrameRate(m_Font, Vector4(0, 0.5f, 1, 1), 181, 1, getFrameRate());
		getPhysicsController()->drawDebug(m_Scene->getActiveCamera()->getViewProjectionMatrix());
	}
	
	m_UIForm->draw();
}

void gonAppMain::update(float elapsedTime) {
	m_UIForm->update(elapsedTime);
	std::vector<bool> gridNodes;
	std::map<int, int> updateNodes;
	std::map<int, int>::iterator iter;

	m_GridStateManager->getUpdatesMap(&updateNodes);

	for (iter = updateNodes.begin(); iter != updateNodes.end(); iter++) {
		int idx = (*iter).first;
		bool isAlive = (*iter).second;

		char sId[10];
		itoa(idx, sId, 10);
		Node *node = m_Scene->findNode(sId);
		Model *model = node->getModel();

		Quaternion quat = node->getRotation();
		Animation *rightAnimation = node->getAnimation("rotateRight");
		Animation *leftAnimation = node->getAnimation("rotateLeft");

		if (isAlive) {
			if (rightAnimation)
				rightAnimation->play();
		} else {
			if (leftAnimation)
				leftAnimation->play();
		}
	}
}

bool gonAppMain::drawScene(Node* node) {
	Model* model = node->getModel();

	if (model) {
		model->draw(m_DrawWireframe);
	}

	return true;
}

Node* gonAppMain::getPickNode(int x, int y) {
	Vector3 pt;
	Ray pickRay;
	PhysicsController::HitResult hitResult;
	Node *result = NULL;

	//get a calculated pick ray for tile interaction
	m_Scene->getActiveCamera()->pickRay(Game::getInstance()->getViewport(), x,
			y, &pickRay);

	if (getPhysicsController()->rayTest(pickRay, 100.0f, &hitResult)) {
		if (hitResult.object) {
			result = hitResult.object->getNode();
		}
	}
	return result;
}

void gonAppMain::stopSimulation() {
	m_Simulating = false;
	Button* btnRun = static_cast<Button*>(m_UIForm->getControl("btnStartStop"));
	btnRun->setSkinRegion(Rectangle(238, 81, 73, 73));

	Label* lblStatus = static_cast<Label*>(m_UIForm->getControl("lblStatus"));
	lblStatus->setText("Idle");
}

void gonAppMain::timeEvent(long timeDiff, void* cookie) {
	if (m_Simulating) {
		int count = *(int*) cookie;

		Label* lblStatus = static_cast<Label*>(m_UIForm->getControl("lblStatus"));

		char buff[200];
		sprintf(buff, "Simulating [ %u/%u ]", count, m_UseIterations);
		lblStatus->setText(buff);

		m_GridStateManager->simulateIteration();
		
		delete (int*) cookie;

		if (count > 1) {
			schedule(ITERATION_SCHEDULE_DELAY, this,
					(void*) new int(count - 1));
		} else {
			stopSimulation();
		}
	}
}

void gonAppMain::runIterations(int iterationCount) {
	m_Simulating = true;

	Button* btnRun = static_cast<Button*>(m_UIForm->getControl("btnStartStop"));
	btnRun->setSkinRegion(Rectangle(238, 2, 73, 73));

	Label* lblStatus = static_cast<Label*>(m_UIForm->getControl("lblStatus"));
	lblStatus->setText("Simulating");

	schedule(0001.0f, this, (void*) new int(iterationCount));
}

void gonAppMain::keyEvent(Keyboard::KeyEvent evt, int key) {
	if (evt == Keyboard::KEY_PRESS) {
		switch (key) {
		case Keyboard::KEY_ESCAPE:
			exit();
			break;
		}
	}
}

void gonAppMain::controlEvent(Control* control,
		Control::Listener::EventType evt) {
	switch (evt) {
	case Control::Listener::PRESS:
		if (strcmp(control->getId(), "btnStartStop") == 0 && !m_Simulating) {
			runIterations(m_UseIterations);
		} else if (strcmp(control->getId(), "btnStartStop") == 0 && m_Simulating) {
			stopSimulation();
		} else if (strcmp(control->getId(), "btnReset") == 0) {
			if (m_Simulating)
				stopSimulation();

			m_GridStateManager->resetState();
		} else if (strcmp(control->getId(), "btnAudio") == 0) {
			Button* btnAudio = static_cast<Button*>(control);
			m_NoAudio = m_NoAudio ? false : true;

			if (!m_NoAudio) {
				btnAudio->setSkinRegion(Rectangle(91, 81, 51, 51));
				if (m_Theme1->getState() != AudioSource::PLAYING)
					m_Theme1->play();
			} else {
				btnAudio->setSkinRegion(Rectangle(91, 24, 51, 51));
				if (m_Theme1->getState() == AudioSource::PLAYING)
					m_Theme1->stop();
			}
		}
		break;
	case Control::Listener::RELEASE:
		break;
	case Control::Listener::VALUE_CHANGED:
		if ((strcmp(control->getId(), "sldIterations") == 0)) {
			Slider *slider = static_cast<Slider*>(control);
			m_UseIterations = slider->getValue() / 1;

			char sIterations[10];
			itoa(m_UseIterations, sIterations, 10);

			slider->setText(sIterations);
		}
		break;
	}
}

void gonAppMain::gestureSwipeEvent(int x, int y, int direction) {

}

void gonAppMain::touchEvent(Touch::TouchEvent evt, int x, int y,
		unsigned int contactIndex) {
	if (!m_Simulating) {
		Node *node = getPickNode(x, y);
		switch (evt) {
		case Touch::TOUCH_MOVE:
			break;
		case Touch::TOUCH_RELEASE:
			break;
		case Touch::TOUCH_PRESS:
			if (NULL != node) {
				m_GridStateManager->toggleTileState(
					atoi(node->getId()));
			}
			break;
		};
	}
}

bool gonAppMain::mouseEvent(Mouse::MouseEvent evt, int x, int y,
		int wheelDelta) {

	Node* cameraNode = m_Scene->getActiveCamera()->getNode();
	Node* node = NULL;
	float incr = 0.0f;

	float curTransX = cameraNode->getTranslationX();
	float curTransY = cameraNode->getTranslationY();
	float curTransZ = cameraNode->getTranslationZ();

	switch (evt) {
	case Mouse::MOUSE_WHEEL:
		if ((curTransZ + wheelDelta > 30) && (curTransZ + wheelDelta < 59)) {
			cameraNode->translateZ(wheelDelta);
		}
		break;
	case Mouse::MOUSE_MOVE:
		if (m_ScreenDragActive) {
			float xdelta = 0.0f;
			float ydelta = 0.0f;

			if (x < xyDelta.x) {
				xyDelta.x = -(xyDelta.x - x);
			} else {
				xyDelta.x = (x - xyDelta.x);
			}

			if (y < xyDelta.y) {
				xyDelta.y = (xyDelta.y - y);
			} else {
				xyDelta.y = -(y - xyDelta.y);
			}

			cameraNode->translate(xyDelta.x / 10.0f, xyDelta.y / 10.0f, 0.0f);

			xyDelta = Vector2(x, y);
		}

		break;
	case Mouse::MOUSE_PRESS_MIDDLE_BUTTON:
		m_ScreenDragActive = true;
		xyDelta.x = x;
		xyDelta.y = y;
		break;
	case Mouse::MOUSE_RELEASE_MIDDLE_BUTTON:
		m_ScreenDragActive = false;
		xyDelta.x = 0;
		xyDelta.y = 0;
		break;
	case Mouse::MOUSE_PRESS_RIGHT_BUTTON:
		node = getPickNode(x, y);

		if (NULL != node) {
			int index = atoi(node->getId());
			bool idxalive = m_GridStateManager->isAlive(index);
		} else {

		}

		break;
	}
	return false;
}

void gonAppMain::drawFrameRate(Font* font, const Vector4& color, unsigned int x, unsigned int y, unsigned int fps)
{
    char buffer[50];
    sprintf(buffer, "Node Count:%u FPS: %u", m_Scene->getNodeCount(), fps);
    font->start();
    font->drawText(buffer, x, y, color, font->getSize());
    font->finish();
}
