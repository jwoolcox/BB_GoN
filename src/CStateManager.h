/*
 * CStateManager.h
 *
 *  Created on: 2012-11-26
 *      Author: pwoolcox
 */

#ifndef CSTATEMANAGER_H_
#define CSTATEMANAGER_H_

#include <vector>
#include <map>

class CStateManager {
public:
	enum NeighbourType {
		NT_TOP_LEFT,
		NT_TOP,
		NT_TOP_RIGHT,
		NT_LEFT,
		NT_RIGHT,
		NT_BOT_LEFT,
		NT_BOT,
		NT_BOT_RIGHT
	};

	CStateManager(int rows, int cols);
	virtual ~CStateManager();
	void resetState();
	void toggleTileState(int index);
	void getUpdatesMap(std::map<int, int> *inlist);
	void simulateIteration();
	int isNeighbourAlive(int row, int col, NeighbourType type);
	int countLivingNeighbours(int row, int col);
	int getIndexFromRowCol(int row, int col);
	int getRowFromIndex(int index);
	int getColFromIndex(int index);
	int isAlive(int index);
private:

	std::vector<int> m_CurrentState;
	std::map<int, int> m_StateDiff;
	int m_StateRows;
	int m_StateCols;
};

#endif /* CSTATEMANAGER_H_ */
