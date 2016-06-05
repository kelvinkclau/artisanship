#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "Game.h"

// constant value
#define CAMPUS_KPI 10
#define GO8_KPI 20
#define ARC_KPI 2
#define IP_KPI 10
#define MOST_ARC_KPI 10
#define MOST_PUB_KPI 10

#define NUM_ARC 78
#define NUM_VERTEX 54

#define A1 17
#define A2 38
#define B1 2
#define B2 48
#define C1 7
#define C2 53

#define RETRAIN_BPS1 14
#define RETRAIN_BPS2 15
#define RETRAIN_BQN1 50
#define RETRAIN_BQN2 51
#define RETRAIN_MJ1 45
#define RETRAIN_MJ2 46
#define RETRAIN_MTV1 7
#define RETRAIN_MTV2 12
#define RETRAIN_MMONEY1 28
#define RETRAIN_MMONEY2 38

#define HORIZONTAL 0
#define VERTICAL 1

// define new data type
typedef struct _student {
    int bQn;
    int bPs;
    int mMoney;
    int mJ;
    int mTV;
    int thD;
} student;

typedef struct _player {
    int ip;
    int publication;
} player;

typedef struct _game {
    int *discipline;
    int *dice;
    int *arcs;
    int *vertex;
    int currentTurn;
    int *mostArc;
    int *mostPublication;
    student *studentType;
    player *playerState;
} game;

typedef struct _position {
    double x;
    double y;
} position;

typedef struct _arc {
    position start;
    position end;
} arc;


// Game.c functions start here
static void clearArray (int *pointer, int size);
static int isSea (position coordinate);
static int coordinateToVertex (position coordinate);
static int coordinateToArc (arc coordinate);
static int arcState (arc coordinate);
static int isHexAdjacent (position hex, position coordinate);
static arc firstMove (path pathTo, Game g);
static arc finalPosition (path pathTo, Game g);
static position vertexToCoordinate (int vertex);
static position hexToCoordinate (int hex);
static int isHexAdjacent (position hex, position coordinate);
static int getNewStudent (int hex, int type);

// functions in Game.h
Game newGame (int discipline[], int dice[]) {
    Game g = malloc(sizeof(game));
    g->discipline = discipline;
    g->dice = dice;
    g->currentTurn = -1;
    
    g->arcs = malloc(NUM_ARC * sizeof(int));
    clearArray (g->arcs, NUM_ARC);
    
    g->vertex = malloc(NUM_VERTEX * sizeof(int));
    clearArray (g->vertex, NUM_VERTEX);
    g->vertex[A1] = CAMPUS_A;
    g->vertex[A2] = CAMPUS_A;
    g->vertex[B1] = CAMPUS_B;
    g->vertex[B2] = CAMPUS_B;
    g->vertex[C1] = CAMPUS_C;
    g->vertex[C2] = CAMPUS_C;
    
    g->mostArc = malloc(PATH_LIMIT * sizeof(int));
    clearArray(g->mostArc, PATH_LIMIT);
    g->mostPublication = malloc(PATH_LIMIT * sizeof(int));
    clearArray(g->mostPublication, PATH_LIMIT);

    g->studentType = malloc(NUM_UNIS * sizeof(student));
    g->playerState = malloc(NUM_UNIS * sizeof(player));
    
    int counter = UNI_A;
    while (counter <= NUM_UNIS) {
        g->studentType[counter].bQn = 3;
        g->studentType[counter].bPs = 3;
        g->studentType[counter].mMoney = 1;
        g->studentType[counter].mJ = 1;
        g->studentType[counter].mTV = 1;
        g->studentType[counter].thD = 0;
        g->playerState[counter].ip = 0;
        g->playerState[counter].publication = 0;
        counter++;
    }
    
    return g;
}


void disposeGame (Game g) {
    free(g->vertex);
    free(g->studentType);
    free(g->playerState);
    free(g);
}


void makeAction (Game g, action a) {
    int uni = getWhoseTurn(g);
    arc finalPos = finalPosition(a.destination, g);
    int index = 0;
    
    if (a.actionCode == BUILD_CAMPUS) {
        g->studentType[uni].bPs--;
        g->studentType[uni].bQn--;
        g->studentType[uni].mJ--;
        g->studentType[uni].mTV--;
        index = coordinateToVertex(finalPos.end);
        g->vertex[index] = uni;
    } else if (a.actionCode == BUILD_GO8) {
        g->studentType[uni].mJ -= 2;
        g->studentType[uni].mMoney -= 3;
        index = coordinateToVertex(finalPos.end);
        g->vertex[index] = uni + NUM_UNIS;
    } else if (a.actionCode == OBTAIN_ARC) {
        g->studentType[uni].bPs--;
        g->studentType[uni].bQn--;
        index = coordinateToArc(finalPos);
        g->arcs[index] = uni;
    } else if (a.actionCode == OBTAIN_PUBLICATION) {
        g->studentType[uni].mJ--;
        g->studentType[uni].mTV--;
        g->studentType[uni].mMoney--;
        g->playerState[uni].publication++;
    } else if (a.actionCode == OBTAIN_IP_PATENT) {
        g->studentType[uni].mJ--;
        g->studentType[uni].mTV--;
        g->studentType[uni].mMoney--;
        g->playerState[uni].ip++;
    } else if (a.actionCode == RETRAIN_STUDENTS) {
        if (a.disciplineFrom == STUDENT_BPS) {
            g->studentType[uni].bPs -= getExchangeRate(g, uni, a.disciplineFrom, a.disciplineTo);
        } else if (a.disciplineFrom == STUDENT_BQN) {
            g->studentType[uni].bQn -= getExchangeRate(g, uni, a.disciplineFrom, a.disciplineTo);
        } else if (a.disciplineFrom == STUDENT_MJ) {
            g->studentType[uni].mJ -= getExchangeRate(g, uni, a.disciplineFrom, a.disciplineTo);
        } else if (a.disciplineFrom == STUDENT_MTV) {
            g->studentType[uni].mTV -= getExchangeRate(g, uni, a.disciplineFrom, a.disciplineTo);
        } else if (a.disciplineFrom == STUDENT_MMONEY) {
            g->studentType[uni].bQn -= getExchangeRate(g, uni, a.disciplineFrom, a.disciplineTo);
        }
        if (a.disciplineTo == STUDENT_THD) {
            g->studentType[uni].thD++; // uni recieves retrained student
        } else if (a.disciplineTo == STUDENT_BPS) {
            g->studentType[uni].bPs++;
        } else if (a.disciplineTo == STUDENT_BQN) {
            g->studentType[uni].bQn++;
        } else if (a.disciplineTo == STUDENT_MJ) {
            g->studentType[uni].mJ++;
        } else if (a.disciplineTo == STUDENT_MTV) {
            g->studentType[uni].mTV++;
        } else if (a.disciplineTo == STUDENT_MMONEY) {
            g->studentType[uni].mMoney++;
        }
    }
}


void throwDice (Game g, int diceScore) {
    int counter = 0;
    int player = UNI_A;
    
    while (counter < NUM_REGIONS) {
        if (g->discipline[counter] == diceScore) {
            while (player <= NUM_UNIS) {
                if (diceScore == STUDENT_BPS) {
                    g->studentType[player].bPs += getNewStudent(counter, STUDENT_BPS);
                } else if (g->discipline[counter] == STUDENT_BQN) {
                    g->studentType[player].bQn += getNewStudent(counter, STUDENT_BQN);
                } else if (g->discipline[counter] == STUDENT_MJ ) {
                    g->studentType[player].mJ += getNewStudent(counter, STUDENT_MJ);
                } else if (g->discipline[counter] == STUDENT_MTV) {
                    g->studentType[player].mTV += getNewStudent(counter, STUDENT_MTV);
                } else if (g->discipline[counter] == STUDENT_MMONEY) {
                    g->studentType[player].mMoney += getNewStudent(counter, STUDENT_MMONEY);
                } else if (g->discipline[counter] == STUDENT_THD) {
                    g->studentType[player].thD += getNewStudent(counter, STUDENT_THD);
                }
                player++;
            }
        }
        counter++;
    }
    
    g->currentTurn++;
}


int getDiscipline (Game g, int regionID) {
    return g->discipline[regionID];
}


int getDiceValue (Game g, int regionID) {
    return g->dice[regionID];
}


int getMostARCs (Game g) {
    int maxARC = NO_ONE;
    int uni = UNI_A;
    
    while (uni <= NUM_UNIS) {
        if (getARCs(g, uni) > maxARC) {
            maxARC = uni;
            g->mostArc[g->currentTurn] = uni;
        }
        uni++;
    }
    
    return maxARC;
}


int getMostPublications (Game g) {
    int maxPublications = NO_ONE;
    int uni = UNI_A;
    
    while (uni <= NUM_UNIS) {
        if (getPublications(g, uni) > maxPublications) {
            maxPublications = uni;
            g->mostPublication[g->currentTurn] = uni;
        }
        uni++;
    }

    return maxPublications;
}


int getTurnNumber (Game g) {
    return g->currentTurn;
}


int getWhoseTurn (Game g) {
    int whoseTurn;
    
    if (g->currentTurn < 0) {
        whoseTurn = NO_ONE;
    } else {
        whoseTurn = (g->currentTurn + 1) % NUM_UNIS;
        if (whoseTurn == 0) {
            whoseTurn = UNI_C;
        }
    }
    
    return whoseTurn;
}


int getCampus(Game g, path pathToVertex) {
    int index = 0;
    arc finalPos;
    int result = 0;
    
    finalPos = finalPosition(pathToVertex, g);
    index = coordinateToVertex(finalPos.end);
    result = g->vertex[index];
    
    return result;
}


int getARC(Game g, path pathToEdge) {
    int index = 0;
    arc finalPos;
    int result = 0;
    
    finalPos = finalPosition(pathToEdge, g);
    index = coordinateToVertex(finalPos.end);
    result = g->vertex[index];
    
    return result;
}


int isLegalAction (Game g, action a) {
    int isLegal = TRUE;
    int counter = 0;
    int player = getWhoseTurn(g);
    
    if (g->currentTurn < 0) {
        return FALSE;
    }
    
    while (a.destination[counter] != 0) {
        arc pos = finalPosition(a.destination, g);
        if (a.destination[counter] != 'L' || a.destination[counter] != 'R' || a.destination[counter] != 'B') {
            isLegal = FALSE;
        } else if (isSea(pos.end) == TRUE) {
            isLegal = FALSE;
        } else if (getARC(g, a.destination) != VACANT_ARC) {
            isLegal = FALSE;
        } else if (a.actionCode < PASS || a.actionCode > RETRAIN_STUDENTS) {
            isLegal = FALSE;
        } else if (a.actionCode == BUILD_CAMPUS) {
            if (g->studentType[player].bPs < 1 || g->studentType[player].bQn < 1 || g->studentType[player].mJ < 1 || g->studentType[player].mTV < 1) {
                isLegal = FALSE;
            } else if (getCampus(g, a.destination) != VACANT_VERTEX) {
                isLegal = FALSE;
            }
        } else if (a.actionCode == BUILD_GO8) {
            if (g->studentType[player].mJ < 2 || g->studentType[player].mMoney < 3) {
                isLegal = FALSE;
            } else if (getCampus(g, a.destination) != VACANT_VERTEX) {
                isLegal = FALSE;
            }
        } else if (a.actionCode == OBTAIN_ARC) {
            if (g->studentType[player].bPs < 1 || g->studentType[player].bQn < 1) {
                isLegal = FALSE;
            }
        } else if (a.actionCode == START_SPINOFF) {
            if (g->studentType[player].mJ < 1 || g->studentType[player].mTV < 1 || g->studentType[player].mMoney < 1) {
                isLegal = FALSE;
            }
        } else if (a.actionCode == RETRAIN_STUDENTS) {
            if ((a.disciplineFrom <= STUDENT_THD || a.disciplineFrom > STUDENT_MMONEY) || (a.disciplineTo <= STUDENT_THD && a.disciplineTo > STUDENT_MMONEY)) {
                isLegal = FALSE;
            } else if (a.disciplineFrom == STUDENT_BPS) {
                if (g->studentType[player].bPs < getExchangeRate(g, player, a.disciplineFrom, a.disciplineTo)) {
                    isLegal = FALSE;
                }
            } else if (a.disciplineFrom == STUDENT_BQN) {
                if (g->studentType[player].bQn < getExchangeRate(g, player, a.disciplineFrom, a.disciplineTo)) {
                    isLegal = FALSE;
                }
            } else if (a.disciplineFrom == STUDENT_MJ) {
                if (g->studentType[player].mJ < getExchangeRate(g, player, a.disciplineFrom, a.disciplineTo)) {
                    isLegal = FALSE;
                }
            } else if (a.disciplineFrom == STUDENT_MTV) {
                if (g->studentType[player].mTV >= getExchangeRate(g, player, a.disciplineFrom, a.disciplineTo)) {
                    isLegal = FALSE;
                }
            } else if (a.disciplineFrom == STUDENT_MMONEY) {
                if (g->studentType[player].mMoney >= getExchangeRate(g, player, a.disciplineFrom, a.disciplineTo)) {
                    isLegal = FALSE;
                }
            }
        }
        counter++;
    }

    return isLegal;
}


int getKPIpoints (Game g, int player) {
    int total = 0;
    
    total += ARC_KPI * getARCs(g, player);
    total += CAMPUS_KPI * getCampuses(g, player);
    total += GO8_KPI * getGO8s(g, player);
    total += IP_KPI * getIPs(g, player);
    
    if (g->mostArc[g->currentTurn] == player && g->mostArc[g->currentTurn - 1] != player) {
        total += MOST_ARC_KPI;
    } else if (g->mostArc[g->currentTurn] != player && g->mostArc[g->currentTurn - 1] == player) {
        total -= MOST_ARC_KPI;
    }
    
    if (g->mostPublication[g->currentTurn] == player && g->mostPublication[g->currentTurn - 1] != player) {
        total += MOST_PUB_KPI;
    } else if (g->mostPublication[g->currentTurn] != player && g->mostPublication[g->currentTurn - 1] == player) {
        total -= MOST_PUB_KPI;
    }
    
    return total;
}


int getARCs (Game g, int player) {
    int counter = 0;
    int arcNum = 0;
    
    while (counter < NUM_ARC) {
        if (g->arcs[counter] == player) {
            arcNum++;
        }
        counter++;
    }
    
    return arcNum;
}


int getGO8s (Game g, int player) {
    int counter = 0;
    int g08Num = 0;
    
    while (counter < NUM_VERTEX) {
        if (g->vertex[counter] == player + NUM_UNIS) {
            g08Num++;
        }
        counter++;
    }
    
    return g08Num;
}


int getCampuses (Game g, int player) {
    int counter = 0;
    int campusNum = 0;
    
    while (counter < NUM_ARC) {
        if (g->vertex[counter] == player) {
            campusNum++;
        }
        counter++;
    }
    
    return campusNum;
}


int getIPs (Game g, int player) {
    return g->playerState[player].ip;
}


int getPublications (Game g, int player) {
    return g->playerState[player].publication;
}


int getStudents (Game g, int player, int discipline) {
    int numStudents = 0;
    
    if (discipline == STUDENT_THD) {
        numStudents = g->studentType[player].thD;
    } else if (discipline == STUDENT_BPS) {
        numStudents = g->studentType[player].bPs;
    } else if (discipline == STUDENT_BQN) {
        numStudents = g->studentType[player].bQn;
    } else if (discipline == STUDENT_MJ) {
        numStudents = g->studentType[player].mJ;
    } else if (discipline == STUDENT_MTV) {
        numStudents = g->studentType[player].mTV;
    } else if (discipline == STUDENT_MMONEY) {
        numStudents = g->studentType[player].mMoney;
    }
    
    return numStudents;
}


int getExchangeRate (Game g, int player, int disciplineFrom, int disciplineTo) {
    int numStudents = 0;
    
    if ((g->vertex[RETRAIN_BPS1] == player || g->vertex[RETRAIN_BPS2] == player) && (disciplineFrom == STUDENT_BPS && disciplineTo == STUDENT_BPS)) {
        numStudents = 2;
    } else if ((g->vertex[RETRAIN_BQN1] == player || g->vertex[RETRAIN_BQN2] == player) && (disciplineFrom == STUDENT_BQN && disciplineTo == STUDENT_BQN)) {
        numStudents = 2;
    } else if ((g->vertex[RETRAIN_MJ1] == player || g->vertex[RETRAIN_MJ2] == player) && (disciplineFrom == STUDENT_MJ && disciplineTo == STUDENT_MJ)) {
        numStudents = 2;
    } else if ((g->vertex[RETRAIN_MTV1] == player || g->vertex[RETRAIN_MTV2] == player) && (disciplineFrom == STUDENT_MTV && disciplineTo == STUDENT_MTV)) {
        numStudents = 2;
    } else if ((g->vertex[RETRAIN_MMONEY1] == player || g->vertex[RETRAIN_MMONEY2] == player) && (disciplineFrom == STUDENT_MMONEY && disciplineTo == STUDENT_MMONEY)) {
        numStudents = 2;
    } else {
        numStudents = 3;
    }
        
    return numStudents;
}


// help functions
static void clearArray (int *pointer, int size) {
    int counter = 0;
    while (counter < size) {
        pointer[counter] = 0;
        counter++;
    }
}


static int isSea (position coordinate) {
    int result;
    
    if (coordinate.x < 1) {
        if (coordinate.y < 2 || coordinate.y > 8) {
            result = TRUE;
        } else {
            result = FALSE;
        }
    } else if (coordinate.x < 2) {
        if (coordinate.y < 1 || coordinate.y > 9) {
            result = TRUE;
        } else {
            result = FALSE;
        }
    } else if (coordinate.x > 3) {
        if (coordinate.y < 1 || coordinate.y > 9) {
            result = TRUE;
        } else {
            result = FALSE;
        }
    } else if (coordinate.x > 4) {
        if (coordinate.y < 2 || coordinate.y > 8) {
            result = TRUE;
        } else {
            result = FALSE;
        }
    } else {
        result = FALSE;
    }
    
    return result;
}


static int coordinateToVertex (position coordinate) {
    int vertex = 0;
    
    if (coordinate.x == 0) {
        vertex = coordinate.y - 2;
    } else if (coordinate.x == 1) {
        vertex = coordinate.y + 6;
    } else if (coordinate.x == 2) {
        vertex = coordinate.y + 16;
    } else if (coordinate.x == 3) {
        vertex = coordinate.y + 27;
    } else if (coordinate.x == 4) {
        vertex = coordinate.y + 38;
    } else if (coordinate.x == 5) {
        vertex = coordinate.y + 47;
    }
    
    return vertex;
}


static position vertexToCoordinate (int vertex) {
    position coordinate;
    
    if (vertex <= 6) {
        coordinate.x = 0;
        coordinate.y = vertex - 2;
    } else if (vertex <= 15) {
        coordinate.x = 1;
        coordinate.y = vertex - 6;
    } else if (vertex <= 26) {
        coordinate.x = 2;
        coordinate.y = vertex - 16;
    } else if (vertex <= 35) {
        coordinate.x = 3;
        coordinate.y = vertex - 27;
    } else if (vertex <= 46) {
        coordinate.x = 4;
        coordinate.y = vertex - 37;
    } else if (vertex <= 53) {
        coordinate.x = 5;
        coordinate.y = - 45;
    }
    
    return coordinate;
}


static int coordinateToArc (arc coordinate) {
    int arcs = 0;
    
    if (coordinate.start.x == coordinate.end.x) {
        if (coordinate.start.x == 0) {
            arcs = coordinate.start.y - 2;
        } else if (coordinate.start.x == 1) {
            arcs = coordinate.start.y + 6;
        } else if (coordinate.start.x == 2) {
            arcs = coordinate.start.y + 16;
        } else if (coordinate.start.x == 3) {
            arcs = coordinate.start.y + 27;
        } else if (coordinate.start.x == 4) {
            arcs = coordinate.start.y + 38;
        } else if (coordinate.start.x == 5) {
            arcs = coordinate.start.y + 47;
        }
    } else if (coordinate.start.y == coordinate.end.y) {
        if (coordinate.start.x == 0) {
            arcs = coordinate.start.y / 2 + NUM_VERTEX - 1;
        } else if (coordinate.start.x == 1) {
            arcs = (coordinate.start.y + 1) / 2 + NUM_VERTEX + 3;
        } else if (coordinate.start.x == 2) {
            arcs = (coordinate.start.y + 2) / 2 + NUM_VERTEX + 8;
        } else if (coordinate.start.x == 3) {
            arcs = (coordinate.start.y - 1) / 2 + NUM_VERTEX + 14;
        } else if (coordinate.start.x == 4) {
            arcs = coordinate.start.y / 2 + NUM_VERTEX + 19;
        }
    }
    
    return arcs;
}

static int arcState (arc coordinate) {
    int result = 0;
    
    if (coordinate.start.x == coordinate.end.x) {
        result = VERTICAL;
    } else if (coordinate.start.y == coordinate.end.y) {
        result = HORIZONTAL;
    }
    
    return result;
}


static arc firstMove (path pathTo, Game g) {
    arc move;
    position coordinate;
    int player = NO_ONE;
    int vertex = 0;
    
    player = getWhoseTurn(g);
    if (player == UNI_A) {
        vertex = A1;
        coordinate = vertexToCoordinate(vertex);
        move.start.x = coordinate.x;
        move.start.y = coordinate.y;
        if (pathTo[0] == 'L') {
            move.end.x = coordinate.x + 1;
            move.end.y = coordinate.y;
        } else if (pathTo[0] == 'R') {
            move.end.x = coordinate.x;
            move.end.y = coordinate.y - 1;
        }
        if (isSea(coordinate) == TRUE) {
            vertex = A2;
            coordinate = vertexToCoordinate(vertex);
            move.start.x = coordinate.x;
            move.start.y = coordinate.y;
            if (pathTo[0] == 'L') {
                move.end.x = coordinate.x - 1;
                move.end.y = coordinate.y;
            } else if (pathTo[0] == 'R') {
                move.end.x = coordinate.x;
                move.end.y = coordinate.y + 1;
            }
        }
    } else if (player == UNI_B) {
        vertex = B1;
        coordinate = vertexToCoordinate(vertex);
        move.start.x = coordinate.x;
        move.start.y = coordinate.y;
        if (pathTo[0] == 'L') {
            move.end.x = coordinate.x;
            move.end.y = coordinate.y - 1;
        } else if (pathTo[0] == 'R') {
            move.end.x = coordinate.x;
            move.end.y = coordinate.y + 1;
        }
        if (isSea(coordinate) == TRUE) {
            vertex = B2;
            coordinate = vertexToCoordinate(vertex);
            move.start.x = coordinate.x;
            move.start.y = coordinate.y;
            if (pathTo[0] == 'L') {
                move.end.x = coordinate.x;
                move.end.y = coordinate.y + 1;
            } else if (pathTo[0] == 'R') {
                move.end.x = coordinate.x;
                move.end.y = coordinate.y - 1;
            }
        }
    } else if (player == UNI_C) {
        vertex = C1;
        coordinate = vertexToCoordinate(vertex);
        move.start.x = coordinate.x;
        move.start.y = coordinate.y;
        if (pathTo[0] == 'L') {
            move.end.x = coordinate.x;
            move.end.y = coordinate.y - 1;
        } else if (pathTo[0] == 'R') {
            move.end.x = coordinate.x + 1;
            move.end.y = coordinate.y;
        }
        if (isSea(coordinate) == TRUE) {
            vertex = C2;
            coordinate = vertexToCoordinate(vertex);
            move.start.x = coordinate.x;
            move.start.y = coordinate.y;
            if (pathTo[0] == 'L') {
                move.end.x = coordinate.x;
                move.end.y = coordinate.y + 1;
            } else if (pathTo[0] == 'R') {
                move.end.x = coordinate.x - 1;
                move.end.y = coordinate.y;
            }
        }
    }
    
    return move;
}

static arc finalPosition (path pathTo, Game g) {
    int counter = 1;
    position coordinate;
    arc move = firstMove(pathTo, g);
    int lastDirection = arcState(move);
    
    coordinate.x = move.end.x;
    coordinate.y = move.end.y;
    
    while (pathTo[counter] == 'L' || pathTo[counter] == 'R' || pathTo[counter] == 'B') {
        if (pathTo[counter] == 'L') {
            if (lastDirection == VERTICAL) {
                if (move.start.y > move.end.y) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.x++;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                } else if (move.start.y < move.end.y) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.x--;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                }
            } else if (lastDirection == HORIZONTAL) {
                if (move.start.x > move.end.x) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.y++;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                } else if (move.start.x < move.end.x) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.y--;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                }
            }
        } else if (pathTo[counter] == 'R') {
            if (lastDirection == VERTICAL) {
                if (move.start.y > move.end.y) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.x--;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                } else if (move.start.y < move.end.y) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.x++;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                }
            } else if (lastDirection == HORIZONTAL) {
                if (move.start.x > move.end.x) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.y--;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                } else if (move.start.x < move.end.x) {
                    // new start point
                    move.start.x = coordinate.x;
                    move.start.y = coordinate.y;
                    // changing
                    coordinate.y++;
                    // new end point
                    move.end.x = coordinate.x;
                    move.end.y = coordinate.y;
                }
            }
        } else if (pathTo[counter] == 'B') {
            if (lastDirection == VERTICAL) {
                coordinate.y = move.start.y;
                move.end.y = move.start.y;
                move.end.x = move.start.x;
            } else if (lastDirection == HORIZONTAL) {
                coordinate.x = move.start.x;
                move.end.x = move.start.x;
                move.end.y = move.start.y;
            }
        }
        counter++;
    }
    
    return move;
}


static position hexToCoordinate (int hex) {
    position coordinate;
    
    if (hex == 0) {
        coordinate.x = 0.5;
        coordinate.y = 3;
    } else if (hex == 1) {
        coordinate.x = 0.5;
        coordinate.y = 5;
    } else if (hex == 2) {
        coordinate.x = 0.5;
        coordinate.y = 7;
    } else if (hex == 3) {
        coordinate.x = 1.5;
        coordinate.y = 2;
    } else if (hex == 4) {
        coordinate.x = 1.5;
        coordinate.y = 4;
    } else if (hex == 5) {
        coordinate.x = 1.5;
        coordinate.y = 6;
    } else if (hex == 6) {
        coordinate.x = 1.5;
        coordinate.y = 8;
    } else if (hex == 7) {
        coordinate.x = 2.5;
        coordinate.y = 1;
    } else if (hex == 8) {
        coordinate.x = 2.5;
        coordinate.y = 3;
    } else if (hex == 9) {
        coordinate.x = 2.5;
        coordinate.y = 5;
    } else if (hex == 10) {
        coordinate.x = 2.5;
        coordinate.y = 7;
    } else if (hex == 11) {
        coordinate.x = 2.5;
        coordinate.y = 9;
    } else if (hex == 12) {
        coordinate.x = 3.5;
        coordinate.y = 2;
    } else if (hex == 13) {
        coordinate.x = 3.5;
        coordinate.y = 4;
    } else if (hex == 14) {
        coordinate.x = 3.5;
        coordinate.y = 6;
    } else if (hex == 15) {
        coordinate.x = 3.5;
        coordinate.y = 8;
    } else if (hex == 16) {
        coordinate.x = 4.5;
        coordinate.y = 3;
    } else if (hex == 17) {
        coordinate.x = 4.5;
        coordinate.y = 5;
    } else if (hex == 18) {
        coordinate.x = 4.5;
        coordinate.y = 7;
    }

    return coordinate;
}


static int isHexAdjacent (position hex, position coordinate) {
    int isAdjacent = 0;
    
    if (fabs(hex.x - coordinate.x) <= 0.5 && fabs(hex.y - coordinate.y) <= 1) {
        isAdjacent = TRUE;
    } else {
        isAdjacent = FALSE;
    }
    
    return isAdjacent;
}


static int getNewStudent (int hex, int type) {
    int numStudent = 0;
    int counter = 0;
    position vertexCoor = vertexToCoordinate(counter);
    position hexCoor = hexToCoordinate(hex);
    
    while (counter < NUM_VERTEX) {
        if (isHexAdjacent(hexCoor, vertexCoor) == TRUE) {
            numStudent++;
        }
        counter++;
    }
    
    return numStudent;
}