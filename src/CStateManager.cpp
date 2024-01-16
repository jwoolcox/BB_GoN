/*
 * CStateManager.cpp
 *
 *  Created on: 2012-11-26
 *      Author: pwoolcox
 */

#include "CStateManager.h"

CStateManager::CStateManager(int rows, int cols) {
	m_StateRows = rows;
	m_StateCols = cols;
	m_CurrentState.resize(rows * cols);
}

CStateManager::~CStateManager() {
}

void CStateManager::resetState() {
	for (int i = 0; i < (int) m_CurrentState.size(); i++) {
		m_CurrentState[i] = m_StateDiff[i] = 0;
	}
}

void CStateManager::toggleTileState(int index) {
	m_CurrentState[index] = m_StateDiff[index] = !m_CurrentState[index];
}

int CStateManager::getRowFromIndex(int index) {
	return (index / m_StateCols);
}

int CStateManager::getColFromIndex(int index) {
	return index - (getRowFromIndex(index) * m_StateCols);
}

int CStateManager::getIndexFromRowCol(int row, int col) {
	return (row * m_StateCols) + col;
}

int CStateManager::isNeighbourAlive(int row, int col, NeighbourType type) {
	int targetRow = 0;
	int targetCol = 0;

	//solve row
	switch (type) {
	case NT_TOP_LEFT:
	case NT_TOP:
	case NT_TOP_RIGHT:
		if (0 == row)
			targetRow = m_StateRows - 1;
		else
			targetRow = row - 1;
		break;
	case NT_LEFT:
	case NT_RIGHT:
		targetRow = row;
		break;

	case NT_BOT_LEFT:
	case NT_BOT:
	case NT_BOT_RIGHT:
		if ((m_StateRows - 1) <= row)
			targetRow = 0;
		else
			targetRow = row + 1;
		break;
	}

	//solve col
	switch (type) {
	case NT_TOP_LEFT:
	case NT_LEFT:
	case NT_BOT_LEFT:
		if (0 == col)
			targetCol = m_StateCols - 1;
		else
			targetCol = col - 1;
		break;
	case NT_TOP:
	case NT_BOT:
		targetCol = col;
		break;
	case NT_TOP_RIGHT:
	case NT_RIGHT:
	case NT_BOT_RIGHT:
		if ((m_StateCols - 1) <= col)
			targetCol = 0;
		else
			targetCol = col + 1;
		break;
	}

	return isAlive(getIndexFromRowCol(targetRow, targetCol));
}

int CStateManager::countLivingNeighbours(int row, int col) {
	int result = 0;

	result += isNeighbourAlive(row, col, NT_TOP_LEFT);
	result += isNeighbourAlive(row, col, NT_TOP);
	result += isNeighbourAlive(row, col, NT_TOP_RIGHT);
	result += isNeighbourAlive(row, col, NT_LEFT);
	result += isNeighbourAlive(row, col, NT_RIGHT);
	result += isNeighbourAlive(row, col, NT_BOT_LEFT);
	result += isNeighbourAlive(row, col, NT_BOT);
	result += isNeighbourAlive(row, col, NT_BOT_RIGHT);

	return result;
}

int CStateManager::isAlive(int index) {
	return m_CurrentState[index];
}

void CStateManager::simulateIteration() {
	int l = m_StateCols * m_StateRows;
	std::vector<int> newState;

	newState.assign(m_CurrentState.begin(), m_CurrentState.end());

	//wipe the state diff
	m_StateDiff.clear();

	for (int i = 0; i < l; i++) {
		int count = countLivingNeighbours(getRowFromIndex(i),
				getColFromIndex(i));
		if (isAlive(i)) {
			//living node has 2 or 3 neighbours, stays alive
			if ((count < 2) || (count > 3)) {
				//less then 2 or more then 3 neighbours dies
				newState[i] = m_StateDiff[i] = 0;
			}
		} else {
			//dead node with exactly 3 neighbours lives
			if (count == 3) {
				newState[i] = m_StateDiff[i] = 1;
			}
		}
	}

	m_CurrentState.assign(newState.begin(), newState.end());
}

void CStateManager::getUpdatesMap(std::map<int, int> *inlist) {
	inlist->clear();

	std::map<int, int>::iterator iter;

	for (iter = m_StateDiff.begin(); iter != m_StateDiff.end(); iter++) {
		int one = (*iter).first;
		int two = (*iter).second;

		(*inlist)[one] = two;
	}

	m_StateDiff.clear();
}
