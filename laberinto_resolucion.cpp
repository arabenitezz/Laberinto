#include <iostream>    // Biblioteca estándar para la entrada/salida en consola
#include <vector>      // Biblioteca estándar para el uso de vectores
#include <cstdlib>     // Biblioteca estándar para funciones generales (como rand() y srand())
#include <ctime>       // Biblioteca estándar para funciones de fecha y hora (para srand())
#include <stack>       // Biblioteca estándar para el uso de pilas

// Estructura que representa un nodo del laberinto
struct Node {
    int x, y;                // Coordenadas del nodo en la cuadrícula
    bool visited;            // Indica si el nodo ha sido visitado
    bool topWall, bottomWall, leftWall, rightWall; // Indican si hay paredes en cada dirección
    bool onPath;             // Indica si el nodo está en el camino recorrido

    // Constructor que inicializa las coordenadas y establece todas las paredes y visited a true/false
    Node(int x, int y) : x(x), y(y), visited(false), topWall(true), bottomWall(true), leftWall(true), rightWall(true), onPath(false) {}
};

// Clase que representa el laberinto
class Grid {
public:
    int rows, cols;                        // Dimensiones de la cuadrícula
    std::vector<std::vector<Node>> nodes;  // Matriz de nodos que representa la cuadrícula del laberinto

    // Método para obtener los vecinos no visitados de un nodo dado
    std::vector<Node*> getNeighbors(Node* node) {
        std::vector<Node*> neighbors; // Se crea un vector vacío neighbors que almacenará los punteros a los nodos vecinos no visitados.
        int x = node->x;
        int y = node->y;
        // Verifica y añade vecinos no visitados a la lista
        if (x > 0 && !nodes[x - 1][y].visited) neighbors.push_back(&nodes[x - 1][y]);
        if (x < rows - 1 && !nodes[x + 1][y].visited) neighbors.push_back(&nodes[x + 1][y]);
        if (y > 0 && !nodes[x][y - 1].visited) neighbors.push_back(&nodes[x][y - 1]);
        if (y < cols - 1 && !nodes[x][y + 1].visited) neighbors.push_back(&nodes[x][y + 1]);
        return neighbors;
    }

public:
    // Constructor que inicializa la cuadrícula y crea los nodos
    Grid(int r, int c) : rows(r), cols(c), nodes(rows, std::vector<Node>(cols, Node(0, 0))) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                nodes[i][j] = Node(i, j);  // Inicializa cada nodo con sus coordenadas
            }
        }
    }

    // Método para remover la pared entre dos nodos
    void removeWall(Node* current, Node* neighbor) {
        if (current->x == neighbor->x) {  // Si están en la misma fila
            if (current->y > neighbor->y) {
                current->leftWall = false;
                neighbor->rightWall = false;
            } else {
                current->rightWall = false;
                neighbor->leftWall = false;
            }
        } else {  // Si están en la misma columna
            if (current->x > neighbor->x) {
                current->topWall = false;
                neighbor->bottomWall = false;
            } else {
                current->bottomWall = false;
                neighbor->topWall = false;
            }
        }
    }

    // Método para generar el laberinto usando el algoritmo de búsqueda en profundidad (DFS)
    void generateMaze() {
        srand(time(0));  // Inicializa la semilla para la generación de números aleatorios
        std::stack<Node*> nodeStack;
        Node* current = &nodes[0][0];  // Empieza desde el nodo superior izquierdo
        current->visited = true;
        nodeStack.push(current);

        while (!nodeStack.empty()) {
            Node* current = nodeStack.top();
            std::vector<Node*> neighbors = getNeighbors(current);

            if (!neighbors.empty()) {  // Si hay vecinos no visitados
                Node* neighbor = neighbors[rand() % neighbors.size()];  // Selecciona un vecino aleatorio
                removeWall(current, neighbor);  // Remueve la pared entre el nodo actual y el vecino
                neighbor->visited = true;
                nodeStack.push(neighbor);  // Añade el vecino a la pila
            } else {
                nodeStack.pop();  // Si no hay vecinos no visitados, retrocede
            }
        }
    }

void printGrid(int entryX, int entryY, int exitX, int exitY) {
    for (int i = 0; i < rows; ++i) {
        // Imprimir paredes superiores
        for (int j = 0; j < cols; ++j) {
            std::cout << "+";
            if (nodes[i][j].topWall) {
                std::cout << "---";
            } else {
                std::cout << "   ";
            }
        }
        std::cout << "+" << std::endl;

        // Imprimir paredes laterales y espacios interiores
        for (int j = 0; j < cols; ++j) {
            if (nodes[i][j].leftWall) {
                std::cout << "|";
            } else {
                std::cout << " ";
            }
            if (i == entryX && j == entryY) {
                std::cout << " E ";
            } else if (i == exitX && j == exitY) {
                std::cout << " S ";
            } else if (nodes[i][j].onPath) {
                // Aplicar un gradiente de color al asterisco (*)
                double ratio = 1.0 * j / cols;  // Ratio para calcular el color
                int color = ratio * 255;  // Escala de 0 a 255 para el color (en este caso, verde a rojo)

                // Secuencias de escape ANSI para el color
                std::cout << "\033[38;2;" << color << ";255;0m * \033[0m";
            } else {
                std::cout << "   ";
            }
        }
        std::cout << "|" << std::endl;
    }

    // Imprimir paredes inferiores
    for (int j = 0; j < cols; ++j) {
        std::cout << "+---";
    }
    std::cout << "+" << std::endl;
}


    // Método para resolver el laberinto usando la regla de la mano derecha
    void solveMaze(int entryX, int entryY, int exitX, int exitY) {
        enum Direction { UP, RIGHT, DOWN, LEFT };  // Definición de direcciones
        Direction dir = RIGHT;  // Inicialmente nos movemos a la derecha

        int x = entryX, y = entryY;
        nodes[x][y].onPath = true;

        while (x != exitX || y != exitY) {
            printPosition(x, y);  // Imprimir posición actual
            printGrid(entryX, entryY, exitX, exitY); // Imprimir el laberinto con el camino recorrido

            if (canMoveRight(x, y, dir)) {
                dir = static_cast<Direction>((dir + 1) % 4);  // Girar a la derecha
                moveForward(x, y, dir);
            } else if (canMoveForward(x, y, dir)) {
                moveForward(x, y, dir);
            } else {
                dir = static_cast<Direction>((dir + 3) % 4);  // Girar a la izquierda
            }
            nodes[x][y].onPath = true;
        }
        std::cout << "Salida encontrada en (" << x << ", " << y << ")" << std::endl;
    }

public:
    // Método para verificar si se puede avanzar en la dirección actual
    bool canMoveForward(int x, int y, int dir) {
        switch (dir) {
        case 0: return !nodes[x][y].topWall;  // ARRIBA
        case 1: return !nodes[x][y].rightWall;  // DERECHA
        case 2: return !nodes[x][y].bottomWall;  // ABAJO
        case 3: return !nodes[x][y].leftWall;  // IZQUIERDA
        }
        return false;
    }

    // Método para verificar si se puede girar a la derecha
    bool canMoveRight(int x, int y, int dir) {
        switch (dir) {
        case 0: return !nodes[x][y].rightWall;  // ARRIBA -> DERECHA
        case 1: return !nodes[x][y].bottomWall;  // DERECHA -> ABAJO
        case 2: return !nodes[x][y].leftWall;  // ABAJO -> IZQUIERDA
        case 3: return !nodes[x][y].topWall;  // IZQUIERDA -> ARRIBA
        }
        return false;
    }

    // Método para avanzar en la dirección actual
    void moveForward(int& x, int& y, int dir) {
        switch (dir) {
        case 0: x--; break;  // ARRIBA
        case 1: y++; break;  // DERECHA
        case 2: x++; break;  // ABAJO
        case 3: y--; break;  // IZQUIERDA
        }
    }

    // Método para imprimir la posición actual
    void printPosition(int x, int y) {
        std::cout << "Posicion actual: (" << x << ", " << y << ")" << std::endl;
    }
};

int main() {
    int rows, cols;
    std::cout << "Ingrese el numero de filas y columnas del laberinto: ";
    std::cin >> rows >> cols;  // Solicita las dimensiones del laberinto al usuario

    Grid grid(rows, cols);  // Crea una instancia de Grid con las dimensiones dadas

    grid.generateMaze();
    grid.printGrid(0, 0, rows - 1, cols - 1); // Generar y mostrar el laberinto

    grid.solveMaze(0, 0, rows - 1, cols - 1); // Resolver el laberinto desde (0, 0) hasta (rows-1, cols-1)

    return 0;
}

