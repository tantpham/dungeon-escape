
#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream ifs(fileName);
    if(!ifs.is_open()) {
        cout << "Error: File unable to open: " << fileName << endl;
        return nullptr;
    }
    ifs >> maxRow >> maxCol;
    ifs >> player.row >> player.col;

    char** diffMap = createMap(maxRow,maxCol);

    if(diffMap == nullptr) {
        cout << "Error: Map unable to open." << endl;
        return nullptr;
    }

    for(int i = 0; i < maxRow; i++) {
        for(int j = 0; j < maxCol; j++) {
            ifs >> diffMap[i][j];
            if((i == player.row) && (j == player.col)) {
                diffMap[i][j] = TILE_PLAYER;
            }
        }
    }

    return diffMap;
}

/**
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {

    switch(input) {
        case MOVE_RIGHT:
            nextCol++;
            break;
        case MOVE_LEFT:
            nextCol--;
            break;
        case MOVE_UP:
            nextRow--;
            break;
        case MOVE_DOWN:
            nextRow++;
            break;
    }
    
}

/**
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    char** diffMap = new char*[maxRow];

    for(int i = 0; i < maxRow; i++){
        diffMap[i] = new char[maxCol];
    }

    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            diffMap[i][j] = TILE_OPEN;
        }
    }

    return diffMap;
}

/**
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {

    for(int j = 0; j < maxRow; j++) {
		delete[] map[j];
	}

	delete[] map;
}

/**
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int tempRow = 2*maxRow;
    int tempCol = 2*maxCol;
    char **resize = new char*[tempRow];

    for(int i = 0; i < tempRow; i++) {
        char* temp = new char[tempCol];
        resize[i] = temp;
    }

    int curRow = 0;
    int curCol = 0;

    for(int i = 0; i < maxRow; i++) {
        for(int j = 0; j < maxCol; j++) {
            if(map[i][j] == 'o') {
                curRow = i;
                curCol = j;
            }
        }
    }
    map[curRow][curCol] = TILE_OPEN;

    for(int i = 0; i < tempRow; i++) {
        for(int j = 0; j < tempCol; j++) {
            resize[i][j] = map[i%maxRow][j%maxCol];
        }
    }
    resize[curRow][curCol] = 'o';

    deleteMap(map, maxRow);

    maxCol = tempCol;
    maxRow = tempRow;
    
	return resize;
}

/**
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {

    if((nextRow < 0) || (nextRow >= maxRow)) {
		nextRow = player.row;
		nextCol = player.col;
		
		return STATUS_STAY;
	}

	if((nextCol < 0) || (nextCol >= maxCol)) {
		nextCol = player.col;
		nextRow = player.row;

		return STATUS_STAY;

	}
	
	// movement around obstacles and monsters
	if((map[nextRow][nextCol] == TILE_PILLAR) || map[nextRow][nextCol] == TILE_MONSTER) {
		nextRow = player.row;
		nextCol = player.col;

		return STATUS_STAY;

	// movemenet for treasure
	} else if(map[nextRow][nextCol] == TILE_TREASURE) {
		player.treasure++;
		map[player.row][player.col] = TILE_OPEN;
		map[nextRow][nextCol] = TILE_PLAYER;

		player.row = nextRow;
		player.col = nextCol;

		return STATUS_TREASURE;

	// movement for amulet
	} else if(map[nextRow][nextCol] == TILE_AMULET) {
		map[player.row][player.col] = TILE_OPEN;
		map[nextRow][nextCol] = TILE_PLAYER;

		player.row = nextRow;
		player.col = nextCol;

		return STATUS_AMULET;
	} else if(map[nextRow][nextCol] == TILE_DOOR) {
		map[player.row][player.col] = TILE_OPEN;
		map[nextRow][nextCol] = TILE_PLAYER;

		player.row = nextRow;
		player.col = nextCol;

		return STATUS_LEAVE;
	} else if(map[nextRow][nextCol] == TILE_EXIT) {
		if(player.treasure >= 1) {
			map[player.row][player.col] = TILE_OPEN;
			map[nextRow][nextCol] = TILE_PLAYER;

			player.row = nextRow;
			player.col = nextCol;

			return STATUS_ESCAPE;
		}
		nextRow = player.row;
		nextCol = player.col;

		return STATUS_STAY;
	}

	map[player.row][player.col] = TILE_OPEN;
	map[nextRow][nextCol] = TILE_PLAYER;

	player.row = nextRow;
	player.col = nextCol;

	return STATUS_MOVE;
}

/**
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

    // CHECKS THE TILE ABOVE
    for(int i = player.col - 1; i >= 0; --i){
        if(map[player.row][i] == TILE_PILLAR){
            break;
        } 
        if(map[player.row][i] == TILE_MONSTER){
            char stay = map[player.row][i + 1];
            
            if(map[player.row][i + 1] == TILE_PLAYER){
                stay = TILE_OPEN;
            }
            
            map[player.row][i + 1] = map[player.row][i];
            map[player.row][i] = stay;
        }
    }
    // CHECKS THE TILE BELOW
    for(int i = player.col + 1; i < maxCol; ++i){
        if(map[player.row][i] == TILE_PILLAR){
            break;
        } 
        if(map[player.row][i] == TILE_MONSTER){
            char stay = map[player.row][i - 1];
            
            if(map[player.row][i - 1] == TILE_PLAYER){
                stay = TILE_OPEN;
            }
            
            map[player.row][i - 1] = map[player.row][i];
            map[player.row][i] = stay;
        }
    }
    // CHECKS THE TILE TO THE LEFT
    for(int i = player.row - 1; i >= 0; --i){
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }   
        if(map[i][player.col] == TILE_MONSTER){
            char stay = map[i + 1][player.col];
            
            if(map[i + 1][player.col] == TILE_PLAYER){
                stay = TILE_OPEN;
            }
            
            map[i + 1][player.col] = map[i][player.col];
            map[i][player.col] = stay;
        }
    }
    // CHECKS THE TILE TO THE RIGHT
    for(int i = player.row + 1; i < maxRow; i++){
        if(map[i][player.col] == TILE_PILLAR){
            break;
        } 
        if(map[i][player.col] == TILE_MONSTER){
            char stay = map[i - 1][player.col];
            
            if(map[i - 1][player.col] == TILE_PLAYER){
                stay = TILE_OPEN;
            }
            
            map[i - 1][player.col] = map[i][player.col];
            map[i][player.col] = stay;
        }
    }
    // CHECKS IF THE PLAYER IS ON MONSTER TILE
    if(map[player.row][player.col] == TILE_MONSTER){
        return true;
    } else {
        return false;
    }
}
