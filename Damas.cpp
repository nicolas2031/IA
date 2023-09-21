#include <iostream>
#include <vector>
#include <list>
#include <tuple>
#include <limits>
#include <stdio.h>

using namespace std;

#define INF 12345

int maxDepth;

int tablero[8][8] = {
    {0, -1, 0, -1, 0, -1, 0, -1},
    {-1, 0, -1, 0, -1, 0, -1, 0},
    {0, -1, 0, -1, 0, -1, 0, -1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0}
};

bool revisarGanador(int jugador) {
    bool mov = false;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (tablero[i][j] == jugador) {
                if (i - 1 >= 0 && i - 1 < 8 && j - 1 >= 0 && j - 1 < 8 && tablero[i - 1][j - 1] == 0)
                    mov = true;
                if (i - 1 >= 0 && i - 1 < 8 && j + 1 >= 0 && j + 1 < 8 && tablero[i - 1][j + 1] == 0)
                    mov = true;
                if (i - 2 >= 0 && i - 2 < 8 && j - 2 >= 0 && j - 2 < 8 && tablero[i - 2][j - 2] == 0)
                    mov = true;
                if (i - 2 >= 0 && i - 2 < 8 && j + 2 >= 0 && j + 2 < 8 && tablero[i - 2][j + 2] == 0)
                    mov = true;
            }
        }
    }
    return mov;
}

void copiar(int t1[8][8], int t2[8][8]) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            t1[i][j] = t2[i][j];
        }
    }
}

// Función para imprimir el tablero en un formato más estético
void printChevere(int tablero[8][8]) {
    int rr, cc, pp;

    cout << "\n\n";
    cout << "   0   1   2   3   4   5   6   7\n";
    cout << " +---.---.---.---.---.---.---.---+\n";

    for (rr = 0; rr < 8; ++rr) {
        cout << rr << "|";
        for (cc = 0; cc < 8; ++cc) {
            if (tablero[rr][cc] == 1)
                cout << " # |";
            else if (tablero[rr][cc] == -1)
                cout << " $ |";
            else
                cout << "   |";
        }
        cout << rr << "\n";
        cout << " +---.---.---.---.---.---.---.---+\n";
    }

    cout << "   0   1   2   3   4   5   6   7\n";
    cout << '\n';
}

struct Nodo {
    int Board[8][8];
    int eval;
    list<Nodo*> children;

    Nodo(int c[8][8]) {
        copiar(Board, c);
    }
};

struct GameTree {
    Nodo* root = 0;
    int depth;
    bool maximize;
    int player;

    GameTree(int board[8][8], int _depth, int _player, bool _maximize) {
        root = new Nodo(board);
        this->depth = _depth;
        this->maximize = _maximize;
        this->player = _player;

        // Genera el árbol de posibilidades
        makeGameTree(root, depth, player);
    }

    // Verifica si un nodo es terminal
    bool isTerminalNode(Nodo* node) {
        int AI = 0, Human = 0;
        int moves = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                AI += (node->Board[i][j] == -1);
                Human += (node->Board[i][j] == 1);

                if (node->Board[i][j] != 0) {
                    vector<tuple<bool, int, int>> possibleMoves;
                    checkPossibilities(node->Board[i][j], i, j, possibleMoves, node->Board);
                    moves += possibleMoves.size();
                }
            }
        }

        // Caso de empate
        if (!moves) return true;

        // Caso de victoria
        if (AI * Human == 0) return true;

        return  false;
    }

    bool isValidPosition(int i, int j) {
        return i >= 0 && i <= 7 && j >= 0 && j <= 7;
    }

    void checkPossibilities(int player, int ii, int jj, vector<tuple<bool, int, int>>& moves, int board[8][8]) {
        // Verifica 2 movimientos posibles dependiendo del jugador
        // AI = -1 ; Human = 1
        int i = player * -1;
        for (int j = -1; j <= 1; j += 2) {

            if (isValidPosition(ii + i, jj + j)) {

                int _i = ii + i;
                int _j = jj + j;

                if (board[_i][_j] == 0) {
                    // Mueve a una celda vacía
                    moves.push_back(make_tuple(false, _i, _j));
                }
                else if (board[_i][_j] == player * -1 && isValidPosition(ii + 2 * i, jj + 2 * j) && !board[ii + 2 * i][jj + 2 * j]) {
                    // Come al oponente
                    moves.push_back(make_tuple(true, ii + 2 * i, jj + 2 * j));
                }
            }
        }
    }

    void makeGameTree(Nodo* _node, int _depth, int player) {
        // Si no existe el nodo, retorna
        if (!_node) return;

        if (_depth == 0 || isTerminalNode(_node)) {
            return;
        }

        // Verifica las posibilidades para cada pieza
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                // Encuentra una pieza
                if (_node->Board[i][j] == player) {
                    vector<tuple<bool, int, int>> possibleMoves;
                    checkPossibilities(player, i, j, possibleMoves, _node->Board);

                    // Crea nodos hijos
                    for (int k = 0; k < possibleMoves.size(); k++) {
                        int newBoard[8][8];
                        copiar(newBoard, _node->Board);

                        bool eat = get<0>(possibleMoves[k]);
                        int new_i = get<1>(possibleMoves[k]);
                        int new_j = get<2>(possibleMoves[k]);

                        // Mueve
                        newBoard[i][j] = 0;
                        newBoard[new_i][new_j] = player;

                        if (eat) {
                            newBoard[i + ((new_i - i) / 2)][j + ((new_j - j) / 2)] = 0;
                        }

                        // Crea un hijo
                        Nodo* child = new Nodo(newBoard);
                        _node->children.push_back(child);

                        makeGameTree(child, _depth - 1, player * -1);
                    }
                }
            }
        }
    }

    int evaluation(Nodo* node) {
        int AI = 0, Human = 0;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                AI += (node->Board[i][j] == -1);
                Human += (node->Board[i][j] == 1);
            }
        }

        if (!Human) return 99999;

        return AI - Human;
    }

    Nodo* miniMax(Nodo* _node, int _depth, bool maximize) {
        if (!_node) {
            return 0;
        }

        if (_depth == 0 || isTerminalNode(_node)) {
            _node->eval = evaluation(_node);
            return _node;
        }

        if (maximize) {
            int maxEva = numeric_limits<int>::min(); // -infinito
            for (auto it = _node->children.begin(); it != _node->children.end(); it++) {
                int eva = miniMax(*it, _depth - 1, false)->eval;
                _node->eval = maxEva = max(maxEva, eva);
            }
            return _node;
        }
        else {
            int minEva = numeric_limits<int>::max(); // +infinito
            for (auto it = _node->children.begin(); it != _node->children.end(); it++) {
                int eva = miniMax(*it, _depth - 1, true)->eval;
                _node->eval = minEva = min(minEva, eva);
            }
            return _node;
        }
    }
};

void getOptimalMovement(int board[8][8], int newBoard[8][8], int depth, int player, bool maximize) {
    GameTree t(board, depth, player, maximize);
    Nodo* n = t.miniMax(t.root, depth, maximize);

    for (auto it = t.root->children.begin(); it != t.root->children.end(); it++) {
        if ((*it)->eval == t.root->eval) {
            copiar(newBoard, (*it)->Board);
            break;
        }
    }
}

bool isValidPosition(int i, int j) {
    return i >= 0 && i <= 7 && j >= 0 && j <= 7;
}

int main() {
    int Player;
    bool End = false;

    cout << "¿Quién empieza el juego? \n -1 = computadora,  1 = Jugador: ";
    cin >> Player;
    cout << endl << "Ingrese Profundidad del árbol: ";
    cin >> maxDepth;

    printChevere(tablero);

    while (!End) {
        if (Player) {
            // Movimiento del Jugador
            int i_1, i_2, j_1, j_2;
            int board[8][8];

            cout << "\n ** Turno de la Computadora ... ***\n";
            getOptimalMovement(tablero, board, maxDepth, Player, true);
            printChevere(board);

        Human:
            cout << "\n ** Turno del Jugador ... ***\n";
            cout << "Ingrese Origen (fila, columna) > ";
            cin >> i_1 >> j_1;
            cout << "Ingrese Destino (fila, columna) > ";
            cin >> i_2 >> j_2;

            if (isValidPosition(i_2, j_2) && board[i_2][j_2] == 0) {
                board[i_1][j_1] = 0;
                board[i_2][j_2] = 1;

                if ((i_2 - i_1) % 2 == 0 && (j_2 - j_1 % 2) == 0) {
                    // Come al oponente
                    int i_AI = i_1 + (i_2 - i_1) / 2;
                    int j_AI = j_1 + (j_2 - j_1) / 2;

                    if (isValidPosition(i_AI, j_AI) && board[i_AI][j_AI] == -1) {
                        board[i_AI][j_AI] = 0;
                    }
                }

                copiar(tablero, board);

                cout << "\n ** Turno del Jugador ... ***\n";
                printChevere(tablero);
            }
            else {
                cout << "\nIngrese una coordenada válida ...";
                goto Human;
                End = true;
            }
        }
    }

    return 0;
}
