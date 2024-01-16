#ifndef gonAppMain_H_
#define gonAppMain_H_

#include "CStateManager.h"
#include "gameplay.h"
#include "Gesture.h"
#include "audiosource.h"
#include <control.h>
#include <sstream>
#include <string>
#include "TimeListener.h"

using namespace gameplay;

/**
 * Main game class.
 */
class gonAppMain: public Game, public Control::Listener, public TimeListener {
public:

	/**
	 * Constructor.
	 */
	gonAppMain();

	/**
	 * @see Game::keyEvent
	 */
	void keyEvent(Keyboard::KeyEvent evt, int key);

	void gestureSwipeEvent(int x, int y, int direction);

	/**
	 * @see Game::touchEvent
	 */
	void touchEvent(Touch::TouchEvent evt, int x, int y,
			unsigned int contactIndex);

	bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);

protected:

	/**
	 * @see Game::initialize
	 */
	void initialize();

	/**
	 * @see Game::finalize
	 */
	void finalize();

	/**
	 * @see Game::update
	 */
	void update(float elapsedTime);

	/**
	 * @see Game::render
	 */
	void render(float elapsedTime);

private:
	bool m_DrawDebug;
	bool m_DrawWireframe;
	bool m_Simulating;
	bool m_RunGeneration;
	bool m_NoAudio;

	AudioSource *m_Theme1;
	unsigned int m_UseIterations;
	std::string m_CameraDebugText;
	CStateManager *m_GridStateManager;
	Form *m_UIForm;
	Font *m_Font;
	Scene *m_Scene;
	RenderState::StateBlock *m_RenderState;
	bool m_ScreenDragActive;
	Vector2 xyDelta;

	Material* m_MatBlack;
	Material* m_MatWhite;

	Texture *m_LiveTexture;
	Texture *m_DeadTexture;

	void drawSplash(void* param);

	void controlEvent(Control* control, Control::Listener::EventType evt);

	void initializeMaterial(Model *model, bool lighting, bool specular);

	void printAt(const char* text, int x, int y);

	bool drawScene(Node* node);

	void playAudio();

	void prepareTileAnimation(Node *node);

	void prepareGridLayout();

	void prepareMaterials();

	Node *getPickNode(int x, int y);

	void runIterations(int iterationCount);

	void stopSimulation();

	void timeEvent(long timeDiff, void* cookie);

	void drawFrameRate(Font* font, const Vector4& color, unsigned int x,
			unsigned int y, unsigned int fps);
};

#endif
