#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>

using namespace std;

// ============================================================================
// CUSTOM DATA STRUCTURES (No STL)
// ============================================================================

// Forward declarations
struct RouteEdge;
struct Port;
struct Ship;
struct QueueNode;
struct ListNode;

// Port (Vertex) structure
struct Port {
    char* name;
    int x;
    int y;
    int portCharge;
    RouteEdge* firstEdge;
    Port* next;
    bool isActive;  // For subgraph filtering
    
    Port() : name(nullptr), x(0), y(0), portCharge(0), firstEdge(nullptr), 
             next(nullptr), isActive(true) {}
};

// Route (Edge) structure
struct RouteEdge {
    Port* destination;
    char* shippingCompany;
    int voyageCost;
    char* date;
    char* departureTime;
    char* arrivalTime;
    RouteEdge* next;
    bool isActive;  // For filtering
    
    RouteEdge() : destination(nullptr), shippingCompany(nullptr), 
                  voyageCost(0), date(nullptr), departureTime(nullptr), 
                  arrivalTime(nullptr), next(nullptr), isActive(true) {}
};

// Ship structure for queue
struct Ship {
    char* shipName;
    char* shippingCompany;
    char* arrivalTime;
    char* departureTime;
    int cargoType;
    
    Ship() : shipName(nullptr), shippingCompany(nullptr), 
            arrivalTime(nullptr), departureTime(nullptr), cargoType(0) {}
};

// Queue Node for layover management
struct QueueNode {
    Ship* ship;
    QueueNode* next;
    
    QueueNode() : ship(nullptr), next(nullptr) {}
};

// Linked List Node for multi-leg routes
struct ListNode {
    Port* port;
    RouteEdge* route;
    ListNode* next;
    
    ListNode() : port(nullptr), route(nullptr), next(nullptr) {}
};

// ============================================================================
// QUEUE IMPLEMENTATION
// ============================================================================
class Queue {
private:
    QueueNode* front;
    QueueNode* rear;
    int size;
    
public:
    Queue() : front(nullptr), rear(nullptr), size(0) {}
    
    ~Queue() {
        clear();
    }
    
    void enqueue(Ship* ship) {
        QueueNode* newNode = new QueueNode();
        newNode->ship = ship;
        newNode->next = nullptr;
        
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }
    
    Ship* dequeue() {
        if (front == nullptr) return nullptr;
        
        QueueNode* temp = front;
        Ship* ship = temp->ship;
        front = front->next;
        
        if (front == nullptr) {
            rear = nullptr;
        }
        
        delete temp;
        size--;
        return ship;
    }
    
    Ship* peek() {
        return (front != nullptr) ? front->ship : nullptr;
    }
    
    bool isEmpty() {
        return front == nullptr;
    }
    
    int getSize() {
        return size;
    }
    
    void clear() {
        while (!isEmpty()) {
            Ship* ship = dequeue();
            if (ship) {
                delete[] ship->shipName;
                delete[] ship->shippingCompany;
                delete[] ship->arrivalTime;
                delete[] ship->departureTime;
                delete ship;
            }
        }
    }
};

// ============================================================================
// LINKED LIST IMPLEMENTATION (for multi-leg routes)
// ============================================================================
class LinkedList {
private:
    ListNode* head;
    int size;
    
public:
    LinkedList() : head(nullptr), size(0) {}
    
    ~LinkedList() {
        clear();
    }
    
    void addNode(Port* port, RouteEdge* route) {
        ListNode* newNode = new ListNode();
        newNode->port = port;
        newNode->route = route;
        newNode->next = head;
        head = newNode;
        size++;
    }
    
    void removeNode(int index) {
        if (index < 0 || index >= size) return;
        
        if (index == 0) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
            size--;
            return;
        }
        
        ListNode* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        
        ListNode* temp = current->next;
        current->next = temp->next;
        delete temp;
        size--;
    }
    
    ListNode* getNode(int index) {
        if (index < 0 || index >= size) return nullptr;
        
        ListNode* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current;
    }
    
    int getSize() {
        return size;
    }
    
    void clear() {
        while (head != nullptr) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
        }
        size = 0;
    }
};

// ============================================================================
// GRAPH IMPLEMENTATION
// ============================================================================
class Graph {
private:
    Port* headPort;
    int portCount;
    
    Port* findPort(const char* portName) {
        Port* current = headPort;
        while (current != nullptr) {
            if (strcmp(current->name, portName) == 0) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    
public:
    void addPort(const char* name, int x, int y, int charge) {
        Port* existing = findPort(name);
        if (existing != nullptr) {
            existing->x = x;
            existing->y = y;
            existing->portCharge = charge;
            return;
        }
        
        Port* newPort = new Port();
        int nameLen = strlen(name);
        newPort->name = new char[nameLen + 1];
        strcpy(newPort->name, name);
        newPort->x = x;
        newPort->y = y;
        newPort->portCharge = charge;
        newPort->next = headPort;
        headPort = newPort;
        portCount++;
    }
    Graph() : headPort(nullptr), portCount(0) {}
    
    ~Graph() {
        clear();
    }
    
    void addRoute(const char* origin, const char* dest, const char* date,
                  const char* depTime, const char* arrTime, int cost, 
                  const char* company) {
        Port* originPort = findPort(origin);
        Port* destPort = findPort(dest);
        
        if (originPort == nullptr || destPort == nullptr) {
            return;
        }
        
        RouteEdge* newEdge = new RouteEdge();
        int len;
        
        len = strlen(company);
        newEdge->shippingCompany = new char[len + 1];
        strcpy(newEdge->shippingCompany, company);
        
        len = strlen(date);
        newEdge->date = new char[len + 1];
        strcpy(newEdge->date, date);
        
        len = strlen(depTime);
        newEdge->departureTime = new char[len + 1];
        strcpy(newEdge->departureTime, depTime);
        
        len = strlen(arrTime);
        newEdge->arrivalTime = new char[len + 1];
        strcpy(newEdge->arrivalTime, arrTime);
        
        newEdge->voyageCost = cost;
        newEdge->destination = destPort;
        newEdge->next = originPort->firstEdge;
        originPort->firstEdge = newEdge;
    }
    
    Port* getPort(const char* portName) {
        return findPort(portName);
    }
    
    Port* getFirstPort() {
        return headPort;
    }
    
    int getPortCount() {
        return portCount;
    }
    
    void clear() {
        Port* current = headPort;
        while (current != nullptr) {
            RouteEdge* edge = current->firstEdge;
            while (edge != nullptr) {
                RouteEdge* nextEdge = edge->next;
                delete[] edge->shippingCompany;
                delete[] edge->date;
                delete[] edge->departureTime;
                delete[] edge->arrivalTime;
                delete edge;
                edge = nextEdge;
            }
            
            Port* nextPort = current->next;
            delete[] current->name;
            delete current;
            current = nextPort;
        }
        headPort = nullptr;
        portCount = 0;
    }
};

// ============================================================================
// PATHFINDING ALGORITHMS
// ============================================================================

// Priority Queue Node for Dijkstra/A*
struct PQNode {
    Port* port;
    int cost;
    int time;
    PQNode* next;
    
    PQNode() : port(nullptr), cost(0), time(0), next(nullptr) {}
};

class PriorityQueue {
private:
    PQNode* head;
    
public:
    PriorityQueue() : head(nullptr) {}
    
    ~PriorityQueue() {
        while (head != nullptr) {
            PQNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    void push(Port* port, int cost, int time, bool useCost) {
        PQNode* newNode = new PQNode();
        newNode->port = port;
        newNode->cost = cost;
        newNode->time = time;
        
        if (head == nullptr || (useCost ? cost < head->cost : time < head->time)) {
            newNode->next = head;
            head = newNode;
            return;
        }
        
        PQNode* current = head;
        while (current->next != nullptr) {
            int nextValue = useCost ? current->next->cost : current->next->time;
            int newValue = useCost ? cost : time;
            if (newValue < nextValue) {
                break;
            }
            current = current->next;
        }
        
        newNode->next = current->next;
        current->next = newNode;
    }
    
    PQNode* pop() {
        if (head == nullptr) return nullptr;
        
        PQNode* temp = head;
        head = head->next;
        return temp;
    }
    
    bool isEmpty() {
        return head == nullptr;
    }
};

// ============================================================================
// HELPER FUNCTIONS FOR TIME/DATE CALCULATIONS
// ============================================================================

// Convert time string (HH:MM) to minutes since midnight
int timeToMinutes(const char* timeStr) {
    int hours, minutes;
    sscanf(timeStr, "%d:%d", &hours, &minutes);
    return hours * 60 + minutes;
}

// Parse date string (DD/MM/YYYY) to days since epoch (simplified)
int dateToDays(const char* dateStr) {
    int day, month, year;
    sscanf(dateStr, "%d/%d/%d", &day, &month, &year);
    // Simplified: assume all dates are in December 2024
    return day;  // For simplicity, just return day
}

// Calculate time difference in minutes
int timeDifference(const char* time1, const char* time2) {
    int t1 = timeToMinutes(time1);
    int t2 = timeToMinutes(time2);
    if (t2 < t1) {
        // Next day
        return (24 * 60 - t1) + t2;
    }
    return t2 - t1;
}

// Check if connecting route is feasible (arrival time < departure time)
bool isFeasibleConnection(RouteEdge* arriving, RouteEdge* departing) {
    if (!arriving || !departing) return false;
    
    int arrTime = timeToMinutes(arriving->arrivalTime);
    int depTime = timeToMinutes(departing->departureTime);
    
    // Check if dates are same or next day
    int arrDay = dateToDays(arriving->date);
    int depDay = dateToDays(departing->date);
    
    if (depDay == arrDay) {
        return depTime >= arrTime;  // Same day, departure after arrival
    } else if (depDay == arrDay + 1) {
        return true;  // Next day, always feasible
    }
    return false;
}

// Calculate layover time in hours
int calculateLayover(RouteEdge* arriving, RouteEdge* departing) {
    if (!arriving || !departing) return 0;
    
    int arrTime = timeToMinutes(arriving->arrivalTime);
    int depTime = timeToMinutes(departing->departureTime);
    int arrDay = dateToDays(arriving->date);
    int depDay = dateToDays(departing->date);
    
    if (depDay == arrDay) {
        return (depTime - arrTime) / 60;
    } else {
        return ((24 * 60 - arrTime) + depTime) / 60;
    }
}

// ============================================================================
// DIJKSTRA'S ALGORITHM IMPLEMENTATION
// ============================================================================

struct DijkstraNode {
    Port* port;
    int distance;
    Port* previous;
    RouteEdge* edgeToPrevious;
    bool visited;
    
    DijkstraNode() : port(nullptr), distance(999999), previous(nullptr), 
                     edgeToPrevious(nullptr), visited(false) {}
};

// Find Dijkstra node in array
DijkstraNode* findDijkstraNode(DijkstraNode* nodes, int count, Port* port) {
    for (int i = 0; i < count; i++) {
        if (nodes[i].port == port) {
            return &nodes[i];
        }
    }
    return nullptr;
}

// Dijkstra's algorithm for shortest/cheapest path
LinkedList* dijkstra(Graph* graph, Port* start, Port* end, bool useCost, const char* targetDate = nullptr) {
    if (!start || !end || start == end) {
        return new LinkedList();
    }
    
    // Create array of Dijkstra nodes for all ports
    int portCount = graph->getPortCount();
    DijkstraNode* nodes = new DijkstraNode[portCount];
    
    // Initialize nodes
    int index = 0;
    Port* current = graph->getFirstPort();
    while (current != nullptr) {
        nodes[index].port = current;
        if (current == start) {
            nodes[index].distance = 0;
        }
        index++;
        current = current->next;
    }
    
    // Main Dijkstra loop
    for (int i = 0; i < portCount; i++) {
        // Find unvisited node with minimum distance
        DijkstraNode* minNode = nullptr;
        int minDist = 999999;
        
        for (int j = 0; j < portCount; j++) {
            if (!nodes[j].visited && nodes[j].distance < minDist) {
                minDist = nodes[j].distance;
                minNode = &nodes[j];
            }
        }
        
        if (minNode == nullptr || minNode->port == end) {
            break;
        }
        
        minNode->visited = true;
        
        // Explore neighbors
        RouteEdge* edge = minNode->port->firstEdge;
        while (edge != nullptr) {
            if (!edge->isActive) {
                edge = edge->next;
                continue;
            }
            
            // Filter by date if provided
            if (targetDate && strcmp(edge->date, targetDate) != 0) {
                edge = edge->next;
                continue;
            }
            
            DijkstraNode* neighbor = findDijkstraNode(nodes, portCount, edge->destination);
            if (neighbor && !neighbor->visited) {
                int edgeWeight = useCost ? edge->voyageCost : timeDifference(edge->departureTime, edge->arrivalTime);
                int newDist = minNode->distance + edgeWeight;
                
                if (newDist < neighbor->distance) {
                    neighbor->distance = newDist;
                    neighbor->previous = minNode->port;
                    neighbor->edgeToPrevious = edge;
                }
            }
            edge = edge->next;
        }
    }
    
    // Reconstruct path
    LinkedList* path = new LinkedList();
    DijkstraNode* endNode = findDijkstraNode(nodes, portCount, end);
    
    if (endNode && endNode->distance < 999999) {
        // Build path backwards
        Port* pathPorts[100];
        RouteEdge* pathEdges[100];
        int pathLength = 0;
        
        DijkstraNode* node = endNode;
        while (node != nullptr && node->previous != nullptr) {
            pathPorts[pathLength] = node->port;
            pathEdges[pathLength] = node->edgeToPrevious;
            pathLength++;
            
            node = findDijkstraNode(nodes, portCount, node->previous);
        }
        
        // Add start port
        pathPorts[pathLength] = start;
        pathLength++;
        
        // Add to linked list (reverse order)
        for (int i = pathLength - 1; i >= 0; i--) {
            RouteEdge* edge = (i < pathLength - 1) ? pathEdges[pathLength - 2 - i] : nullptr;
            path->addNode(pathPorts[i], edge);
        }
    }
    
    delete[] nodes;
    return path;
}

// ============================================================================
// DATA PARSING FUNCTIONS
// ============================================================================

void parsePorts(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        cerr << "Error opening ports.txt" << endl;
        return;
    }
    
    char portName[100];
    int x;
    
    // Map port names to approximate world map coordinates
    // These coordinates should match your world_map.jpeg dimensions
    // Format: port name -> (x, y) on world map
    struct PortCoord {
        const char* name;
        int x, y;
    };
    
    // Approximate geographic coordinates for major ports on a world map
    // Adjust these based on your actual world_map.jpeg dimensions
    PortCoord portCoords[] = {
        {"HongKong", 1200, 400}, {"Durban", 1100, 750}, {"Oslo", 950, 200},
        {"Genoa", 980, 350}, {"Osaka", 1300, 380}, {"Lisbon", 800, 380},
        {"Hamburg", 970, 280}, {"Rotterdam", 960, 280}, {"Helsinki", 1000, 180},
        {"Mumbai", 1100, 450}, {"Jakarta", 1200, 550}, {"Tokyo", 1350, 380},
        {"Athens", 1050, 400}, {"CapeTown", 1000, 800}, {"Stockholm", 1000, 200},
        {"PortLouis", 1150, 650}, {"Dubai", 1120, 450}, {"Shanghai", 1250, 400},
        {"NewYork", 600, 350}, {"Vancouver", 400, 300}, {"LosAngeles", 300, 380},
        {"Antwerp", 960, 280}, {"Copenhagen", 980, 240}, {"London", 920, 280},
        {"AbuDhabi", 1130, 450}, {"Chittagong", 1180, 450}, {"Karachi", 1100, 420},
        {"Dublin", 850, 280}, {"Marseille", 980, 370}, {"Jeddah", 1080, 450},
        {"Colombo", 1150, 500}, {"Sydney", 1300, 700}, {"Melbourne", 1300, 720},
        {"Montreal", 550, 300}, {"Doha", 1120, 440}, {"Manila", 1250, 480},
        {"Alexandria", 1050, 420}, {"Istanbul", 1040, 380}, {"Singapore", 1200, 520},
        {"Busan", 1320, 380}
    };
    
    while (fscanf(file, "%s %d", portName, &x) == 2) {
        // Try to find port in coordinate map
        int y = 400;  // Default Y
        bool found = false;
        for (int i = 0; i < 40; i++) {
            if (strcmp(portName, portCoords[i].name) == 0) {
                x = portCoords[i].x;
                y = portCoords[i].y;
                found = true;
                break;
            }
        }
        
        // If not found, use X from file and calculate Y (fallback)
        if (!found) {
            y = 400 + (x % 300);
        }
        
        graph->addPort(portName, x, y, 0);
    }
    
    fclose(file);
}

void parsePortCharges(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        cerr << "Error opening PortCharges.txt" << endl;
        return;
    }
    
    char portName[100];
    int charge;
    
    while (fscanf(file, "%s %d", portName, &charge) == 2) {
        Port* port = graph->getPort(portName);
        if (port != nullptr) {
            port->portCharge = charge;
        }
    }
    
    fclose(file);
}

void parseRoutes(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        cerr << "Error opening Routes.txt" << endl;
        return;
    }
    
    char origin[100], dest[100], date[20], depTime[10], arrTime[10], company[50];
    int cost;
    
    while (fscanf(file, "%s %s %s %s %s %d %s", 
                  origin, dest, date, depTime, arrTime, &cost, company) == 7) {
        graph->addRoute(origin, dest, date, depTime, arrTime, cost, company);
    }
    
    fclose(file);
}

// ============================================================================
// ROUTE BOOKING AND PREFERENCES
// ============================================================================

class RouteBooking {
private:
    Graph* graph;
    char* preferredCompany;
    char** avoidedPorts;
    int avoidedPortCount;
    int maxVoyageTime;  // Maximum voyage time in hours
    
public:
    RouteBooking(Graph* g) : graph(g), preferredCompany(nullptr), 
                             avoidedPorts(nullptr), avoidedPortCount(0), maxVoyageTime(0) {}
    
    ~RouteBooking() {
        if (preferredCompany) delete[] preferredCompany;
        if (avoidedPorts) {
            for (int i = 0; i < avoidedPortCount; i++) {
                delete[] avoidedPorts[i];
            }
            delete[] avoidedPorts;
        }
    }
    
    void setPreferredCompany(const char* company) {
        if (preferredCompany) delete[] preferredCompany;
        int len = strlen(company);
        preferredCompany = new char[len + 1];
        strcpy(preferredCompany, company);
    }
    
    void addAvoidedPort(const char* portName) {
        char** newAvoided = new char*[avoidedPortCount + 1];
        for (int i = 0; i < avoidedPortCount; i++) {
            newAvoided[i] = avoidedPorts[i];
        }
        int len = strlen(portName);
        newAvoided[avoidedPortCount] = new char[len + 1];
        strcpy(newAvoided[avoidedPortCount], portName);
        if (avoidedPorts) delete[] avoidedPorts;
        avoidedPorts = newAvoided;
        avoidedPortCount++;
    }
    
    void setMaxVoyageTime(int hours) {
        maxVoyageTime = hours;
    }
    
    bool isRouteAllowed(RouteEdge* edge) {
        if (!edge || !edge->isActive) return false;
        
        // Check preferred company
        if (preferredCompany && strcmp(edge->shippingCompany, preferredCompany) != 0) {
            return false;
        }
        
        // Check avoided ports
        if (avoidedPorts) {
            for (int i = 0; i < avoidedPortCount; i++) {
                if (edge->destination && strcmp(edge->destination->name, avoidedPorts[i]) == 0) {
                    return false;
                }
            }
        }
        
        // Check max voyage time
        if (maxVoyageTime > 0) {
            int voyageTime = timeDifference(edge->departureTime, edge->arrivalTime) / 60;
            if (voyageTime > maxVoyageTime) {
                return false;
            }
        }
        
        return true;
    }
    
    // Find all direct and connecting routes from origin to destination
    LinkedList** findRoutes(Port* origin, Port* dest, const char* date, int& routeCount) {
        routeCount = 0;
        LinkedList** routes = new LinkedList*[100];  // Max 100 routes
        
        // Find direct routes
        RouteEdge* edge = origin->firstEdge;
        while (edge != nullptr) {
            if (edge->destination == dest && 
                (!date || strcmp(edge->date, date) == 0) &&
                isRouteAllowed(edge)) {
                LinkedList* path = new LinkedList();
                path->addNode(origin, nullptr);
                path->addNode(dest, edge);
                routes[routeCount++] = path;
            }
            edge = edge->next;
        }
        
        // Find connecting routes (1 stop)
        edge = origin->firstEdge;
        while (edge != nullptr && routeCount < 100) {
            if (!date || strcmp(edge->date, date) == 0) {
                if (isRouteAllowed(edge)) {
                    RouteEdge* edge2 = edge->destination->firstEdge;
                    while (edge2 != nullptr) {
                        if (edge2->destination == dest &&
                            isFeasibleConnection(edge, edge2) &&
                            isRouteAllowed(edge2)) {
                            LinkedList* path = new LinkedList();
                            path->addNode(origin, nullptr);
                            path->addNode(edge->destination, edge);
                            path->addNode(dest, edge2);
                            routes[routeCount++] = path;
                        }
                        edge2 = edge2->next;
                    }
                }
            }
            edge = edge->next;
        }
        
        return routes;
    }
};

// ============================================================================
// LAYOVER MANAGEMENT
// ============================================================================

class LayoverManager {
private:
    Graph* graph;
    Queue** portQueues;
    int portCount;
    
    int getPortIndex(Port* port) {
        if (!port || !graph) return -1;
        int index = 0;
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            if (current == port) return index;
            current = current->next;
            index++;
        }
        return -1;
    }
    
public:
    LayoverManager(Graph* g) : graph(g), portCount(g->getPortCount()) {
        portQueues = new Queue*[portCount];
        for (int i = 0; i < portCount; i++) {
            portQueues[i] = new Queue();
        }
    }
    
    ~LayoverManager() {
        for (int i = 0; i < portCount; i++) {
            delete portQueues[i];
        }
        delete[] portQueues;
    }
    
    void addShipToQueue(Port* port, Ship* ship) {
        int index = getPortIndex(port);
        if (index >= 0) {
            portQueues[index]->enqueue(ship);
        }
    }
    
    Ship* processShip(Port* port) {
        int index = getPortIndex(port);
        if (index >= 0) {
            return portQueues[index]->dequeue();
        }
        return nullptr;
    }
    
    int getQueueSize(Port* port) {
        int index = getPortIndex(port);
        if (index >= 0) {
            return portQueues[index]->getSize();
        }
        return 0;
    }
    
    // Calculate layover time including queue wait
    int calculateTotalLayover(Port* port, RouteEdge* arriving, RouteEdge* departing) {
        int baseLayover = calculateLayover(arriving, departing);
        int queueWait = getQueueSize(port) * 8;  // 8 hours per ship in queue
        return baseLayover + queueWait;
    }
};

// ============================================================================
// SUBGRAPH GENERATION
// ============================================================================

class SubgraphGenerator {
private:
    Graph* graph;
    
public:
    SubgraphGenerator(Graph* g) : graph(g) {}
    
    // Generate subgraph based on shipping company
    void filterByCompany(const char* company) {
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            current->isActive = false;
            RouteEdge* edge = current->firstEdge;
            while (edge != nullptr) {
                edge->isActive = false;
                if (company == nullptr || strcmp(edge->shippingCompany, company) == 0) {
                    edge->isActive = true;
                    current->isActive = true;
                    if (edge->destination) {
                        edge->destination->isActive = true;
                    }
                }
                edge = edge->next;
            }
            current = current->next;
        }
    }
    
    // Generate subgraph based on date
    void filterByDate(const char* date) {
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            current->isActive = false;
            RouteEdge* edge = current->firstEdge;
            while (edge != nullptr) {
                edge->isActive = false;
                if (date == nullptr || strcmp(edge->date, date) == 0) {
                    edge->isActive = true;
                    current->isActive = true;
                    if (edge->destination) {
                        edge->destination->isActive = true;
                    }
                }
                edge = edge->next;
            }
            current = current->next;
        }
    }
    
    // Reset all ports and routes to active
    void reset() {
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            current->isActive = true;
            RouteEdge* edge = current->firstEdge;
            while (edge != nullptr) {
                edge->isActive = true;
                edge = edge->next;
            }
            current = current->next;
        }
    }
    
    // Filter by port region (simplified - by port name prefix or list)
    void filterByPorts(const char** portNames, int count) {
        reset();
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            bool portInList = false;
            for (int i = 0; i < count; i++) {
                if (strcmp(current->name, portNames[i]) == 0) {
                    portInList = true;
                    break;
                }
            }
            if (!portInList) {
                current->isActive = false;
            }
            current = current->next;
        }
    }
};

// ============================================================================
// SFML GRAPHICS IMPLEMENTATION
// ============================================================================

class OceanRouteNav {
private:
    sf::RenderWindow* window;
    Graph* graph;
    sf::Texture worldMapTexture;
    sf::Texture anchorTexture;
    sf::Texture oceanCurrentsTexture;
    sf::Sprite worldMapSprite;
    sf::Sprite anchorSprite;
    sf::Sprite oceanCurrentsSprite;
    
    sf::Font font;
    Port* selectedPort;
    Port* destinationPort;
    Port* hoveredPort;  // Port currently under mouse
    RouteEdge* hoveredRoute;  // Route currently under mouse
    LinkedList* currentPath;
    RouteBooking* routeBooking;
    LayoverManager* layoverManager;
    SubgraphGenerator* subgraphGen;
    
    // Multi-leg route (linked list)
    LinkedList* multiLegRoute;
    
    // Mouse position for hover detection
    int mouseX, mouseY;
    
    // UI State
    bool showStartScreen;
    bool showMainMap;
    sf::Clock operationClock;  // Clock to track operation time
    char* currentOperation;  // Current operation text
    float sidebarWidth;
    
    bool loadTextures() {
        if (!worldMapTexture.loadFromFile("world_map.jpeg")) {
            cerr << "Error loading world_map.jpeg" << endl;
            return false;
        }
        
        if (!anchorTexture.loadFromFile("anchor.jpeg")) {
            cerr << "Error loading anchor.jpeg" << endl;
            return false;
        }
        
        if (!oceanCurrentsTexture.loadFromFile("ocean_currents.jpeg")) {
            cerr << "Error loading ocean_currents.jpeg" << endl;
            return false;
        }
        
        worldMapSprite.setTexture(worldMapTexture);
        oceanCurrentsSprite.setTexture(oceanCurrentsTexture);
        anchorSprite.setTexture(anchorTexture);
        
        // Scale textures to fit window
        float scaleX = 1920.0f / worldMapTexture.getSize().x;
        float scaleY = 1080.0f / worldMapTexture.getSize().y;
        worldMapSprite.setScale(scaleX, scaleY);
        
        // Scale ocean currents to match world map
        oceanCurrentsSprite.setScale(scaleX, scaleY);
        
        return true;
    }
    
    // Draw button with visual indicator (no text needed)
    void drawButtonIndicator(float x, float y, float width, float height, int buttonType) {
        // Draw colored indicator bar on left side of button
        sf::RectangleShape indicator(sf::Vector2f(5, height - 4));
        indicator.setPosition(x + 2, y + 2);
        
        // Different colors for different buttons
        switch(buttonType) {
            case 1: indicator.setFillColor(sf::Color::Green); break;  // Cheapest - Green
            case 2: indicator.setFillColor(sf::Color::Blue); break;  // Fastest - Blue
            case 3: indicator.setFillColor(sf::Color::Magenta); break; // Filter - Magenta
            case 4: indicator.setFillColor(sf::Color::Red); break;    // Reset - Red
            default: indicator.setFillColor(sf::Color::White); break;
        }
        window->draw(indicator);
        
        // Draw simple icon pattern in center
        float centerX = x + width / 2;
        float centerY = y + height / 2;
        
        // Draw different patterns for each button type
        if (buttonType == 1) {
            // Dollar sign pattern for cheapest
            sf::CircleShape dot(3);
            dot.setPosition(centerX - 8, centerY - 3);
            dot.setFillColor(sf::Color::White);
            window->draw(dot);
            dot.setPosition(centerX + 2, centerY - 3);
            window->draw(dot);
            sf::RectangleShape line(sf::Vector2f(12, 2));
            line.setPosition(centerX - 6, centerY);
            line.setFillColor(sf::Color::White);
            window->draw(line);
        } else if (buttonType == 2) {
            // Arrow pattern for fastest
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setPoint(0, sf::Vector2f(centerX - 5, centerY));
            arrow.setPoint(1, sf::Vector2f(centerX + 5, centerY - 5));
            arrow.setPoint(2, sf::Vector2f(centerX + 5, centerY + 5));
            arrow.setFillColor(sf::Color::White);
            window->draw(arrow);
        } else if (buttonType == 3) {
            // Filter icon (funnel shape)
            sf::ConvexShape funnel;
            funnel.setPointCount(4);
            funnel.setPoint(0, sf::Vector2f(centerX - 6, centerY - 5));
            funnel.setPoint(1, sf::Vector2f(centerX + 6, centerY - 5));
            funnel.setPoint(2, sf::Vector2f(centerX + 2, centerY + 5));
            funnel.setPoint(3, sf::Vector2f(centerX - 2, centerY + 5));
            funnel.setFillColor(sf::Color::White);
            window->draw(funnel);
        } else if (buttonType == 4) {
            // Reset icon (circular arrow)
            sf::CircleShape circle(6);
            circle.setPosition(centerX - 6, centerY - 6);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(2);
            window->draw(circle);
            sf::RectangleShape arrow(sf::Vector2f(4, 2));
            arrow.setPosition(centerX - 2, centerY - 8);
            arrow.setFillColor(sf::Color::White);
            window->draw(arrow);
        }
    }
    
    void drawPorts() {
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            if (!current->isActive) {
                current = current->next;
                continue;
            }
            
            // Determine port state for coloring
            bool isSelected = (current == selectedPort || current == destinationPort);
            bool isHovered = (current == hoveredPort);
            bool hasQueue = layoverManager && layoverManager->getQueueSize(current) > 0;
            
            // Draw port background circle (larger for better visibility)
            // Adjust position for sidebar offset
            sf::CircleShape portCircle(8);
            portCircle.setPosition(current->x + sidebarWidth - 8, current->y - 8);
            
            if (isSelected) {
                portCircle.setFillColor(sf::Color::Yellow);
                portCircle.setOutlineColor(sf::Color::White);
                portCircle.setOutlineThickness(2);
            } else if (isHovered) {
                portCircle.setFillColor(sf::Color::Cyan);
                portCircle.setOutlineColor(sf::Color::White);
                portCircle.setOutlineThickness(1);
            } else if (hasQueue) {
                portCircle.setFillColor(sf::Color::Red);
                portCircle.setOutlineColor(sf::Color::White);
                portCircle.setOutlineThickness(1);
            } else {
                portCircle.setFillColor(sf::Color::White);
                portCircle.setOutlineColor(sf::Color::Blue);
                portCircle.setOutlineThickness(1);
            }
            window->draw(portCircle);
            
            // Draw anchor icon at port location (smaller, centered)
            // Adjust position for sidebar offset
            anchorSprite.setPosition(current->x + sidebarWidth - 6, current->y - 6);
            anchorSprite.setScale(0.05f, 0.05f);
            window->draw(anchorSprite);
            
            // Draw port name label
            sf::Text text;
            if (font.getInfo().family != "") {
                text.setFont(font);
            }
            text.setString(current->name);
            text.setCharacterSize(10);
            
            if (isSelected) {
                text.setFillColor(sf::Color::Yellow);
                text.setStyle(sf::Text::Bold);
            } else if (isHovered) {
                text.setFillColor(sf::Color::Cyan);
            } else {
                text.setFillColor(sf::Color::White);
            }
            
            // Position text below port (adjusted for sidebar)
            text.setPosition(current->x + sidebarWidth - 30, current->y + 12);
            window->draw(text);
            
            // Draw queue indicator if port has ships waiting
            if (hasQueue) {
                int queueSize = layoverManager->getQueueSize(current);
                sf::CircleShape queueDot(3);
                queueDot.setPosition(current->x + sidebarWidth + 8, current->y - 8);
                queueDot.setFillColor(sf::Color::Red);
                window->draw(queueDot);
                
                // Show queue count (uncomment when font is loaded)
                /*
                sf::Text queueText;
                queueText.setFont(font);
                char queueStr[10];
                sprintf(queueStr, "%d", queueSize);
                queueText.setString(queueStr);
                queueText.setCharacterSize(8);
                queueText.setFillColor(sf::Color::Red);
                queueText.setPosition(current->x + 12, current->y - 10);
                window->draw(queueText);
                */
            }
            
            current = current->next;
        }
    }
    
    void drawRoutes() {
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            if (!current->isActive) {
                current = current->next;
                continue;
            }
            
            RouteEdge* edge = current->firstEdge;
            while (edge != nullptr) {
                if (!edge->isActive || !edge->destination || edge->destination->isActive == false) {
                    edge = edge->next;
                    continue;
                }
                
                bool isHovered = (edge == hoveredRoute);
                
                // Calculate line properties (adjusted for sidebar offset)
                float x1 = current->x + sidebarWidth;
                float y1 = current->y;
                float x2 = edge->destination->x + sidebarWidth;
                float y2 = edge->destination->y;
                
                // Draw thicker route line using rectangle
                float dx = x2 - x1;
                float dy = y2 - y1;
                float length = sqrt(dx * dx + dy * dy);
                float angle = atan2(dy, dx) * 180.0f / 3.14159f;
                
                // Line thickness based on cost (cheaper = thicker, more visible)
                float thickness = isHovered ? 4.0f : 2.0f;
                
                // Color based on cost (blue gradient: lighter = cheaper, darker = more expensive)
                int cost = edge->voyageCost;
                int blueIntensity = 255;
                int greenIntensity = 200 - (cost / 100);  // More expensive = less green
                if (greenIntensity < 50) greenIntensity = 50;
                int redIntensity = 100 + (cost / 200);  // More expensive = more red
                if (redIntensity > 255) redIntensity = 255;
                
                sf::Color routeColor;
                if (isHovered) {
                    routeColor = sf::Color::Yellow;  // Highlight hovered route
                } else {
                    routeColor = sf::Color(redIntensity, greenIntensity, blueIntensity);
                }
                
                // Draw route line as rectangle (thicker line)
                sf::RectangleShape routeLine(sf::Vector2f(length, thickness));
                routeLine.setPosition(x1, y1 - thickness / 2);
                routeLine.setRotation(angle);
                routeLine.setFillColor(routeColor);
                window->draw(routeLine);
                
                // Draw arrow at destination (small triangle)
                float arrowSize = 6.0f;
                float arrowAngle = angle + 180.0f;  // Point towards destination
                
                sf::ConvexShape arrow;
                arrow.setPointCount(3);
                arrow.setPoint(0, sf::Vector2f(0, 0));
                arrow.setPoint(1, sf::Vector2f(-arrowSize, -arrowSize / 2));
                arrow.setPoint(2, sf::Vector2f(-arrowSize, arrowSize / 2));
                arrow.setFillColor(routeColor);
                arrow.setPosition(x2, y2);
                arrow.setRotation(arrowAngle);
                window->draw(arrow);
                
                edge = edge->next;
            }
            
            current = current->next;
        }
    }
    
    void drawPath(LinkedList* path) {
        if (path == nullptr || path->getSize() < 2) return;
        
            for (int i = 0; i < path->getSize() - 1; i++) {
            ListNode* node1 = path->getNode(i);
            ListNode* node2 = path->getNode(i + 1);
            
            if (node1 == nullptr || node2 == nullptr) continue;
            
            sf::Vertex line[2];
            // Adjust for sidebar offset
            line[0].position = sf::Vector2f(node1->port->x + sidebarWidth, node1->port->y);
            line[1].position = sf::Vector2f(node2->port->x + sidebarWidth, node2->port->y);
            line[0].color = sf::Color::Yellow;
            line[1].color = sf::Color::Yellow;
            
            window->draw(line, 2, sf::Lines);
        }
    }
    
    void drawLayovers() {
        if (!layoverManager) return;
        
        Port* current = graph->getFirstPort();
        while (current != nullptr) {
            if (!current->isActive) {
                current = current->next;
                continue;
            }
            int queueSize = layoverManager->getQueueSize(current);
            if (queueSize > 0) {
                // Draw dashed circle around port to indicate queue
                // Simplified: draw multiple small circles around port (adjusted for sidebar)
                for (int i = 0; i < 8; i++) {
                    float angle = i * 45.0f * 3.14159f / 180.0f;
                    sf::CircleShape dash(3);
                    dash.setPosition(current->x + sidebarWidth + 25 * cos(angle) - 3,
                                     current->y + 25 * sin(angle) - 3);
                    dash.setFillColor(sf::Color::Red);
                    window->draw(dash);
                }
            }
            current = current->next;
        }
    }
    
public:
    OceanRouteNav() : window(nullptr), graph(nullptr), selectedPort(nullptr), 
                      destinationPort(nullptr), hoveredPort(nullptr), 
                      hoveredRoute(nullptr), currentPath(nullptr), 
                      routeBooking(nullptr), layoverManager(nullptr), 
                      subgraphGen(nullptr), multiLegRoute(nullptr), 
                      mouseX(0), mouseY(0), showStartScreen(true), 
                      showMainMap(false), currentOperation(nullptr), 
                      sidebarWidth(250.0f) {
        int len = strlen("Ready");
        currentOperation = new char[len + 1];
        strcpy(currentOperation, "Ready");
    }
    
    ~OceanRouteNav() {
        if (graph) delete graph;
        if (currentPath) delete currentPath;
        if (multiLegRoute) delete multiLegRoute;
        if (routeBooking) delete routeBooking;
        if (layoverManager) delete layoverManager;
        if (subgraphGen) delete subgraphGen;
        if (currentOperation) delete[] currentOperation;
        if (window) delete window;
    }
    
    void setCurrentOperation(const char* operation) {
        if (currentOperation) delete[] currentOperation;
        int len = strlen(operation);
        currentOperation = new char[len + 1];
        strcpy(currentOperation, operation);
        operationClock.restart();  // Reset clock when new operation starts
    }
    
    // Public methods for route operations
    void findShortestPath(Port* start, Port* end, bool useCost, const char* date = nullptr) {
        if (currentPath) {
            delete currentPath;
            currentPath = nullptr;
        }
        currentPath = dijkstra(graph, start, end, useCost, date);
    }
    
    void findCheapestPath(Port* start, Port* end, const char* date = nullptr) {
        findShortestPath(start, end, true, date);
    }
    
    void findFastestPath(Port* start, Port* end, const char* date = nullptr) {
        findShortestPath(start, end, false, date);
    }
    
    void bookRoute(Port* origin, Port* dest, const char* date) {
        if (!routeBooking) return;
        
        int routeCount = 0;
        LinkedList** routes = routeBooking->findRoutes(origin, dest, date, routeCount);
        
        if (routeCount > 0 && currentPath) {
            delete currentPath;
        }
        if (routeCount > 0) {
            currentPath = routes[0];  // Use first available route
        }
        
        // Clean up other routes (simplified - in full implementation would show all)
        for (int i = 1; i < routeCount; i++) {
            delete routes[i];
        }
        delete[] routes;
    }
    
    void setPreferredCompany(const char* company) {
        if (routeBooking) {
            routeBooking->setPreferredCompany(company);
        }
    }
    
    void addAvoidedPort(const char* portName) {
        if (routeBooking) {
            routeBooking->addAvoidedPort(portName);
        }
    }
    
    void setMaxVoyageTime(int hours) {
        if (routeBooking) {
            routeBooking->setMaxVoyageTime(hours);
        }
    }
    
    void filterByCompany(const char* company) {
        if (subgraphGen) {
            subgraphGen->filterByCompany(company);
        }
    }
    
    void filterByDate(const char* date) {
        if (subgraphGen) {
            subgraphGen->filterByDate(date);
        }
    }
    
    void resetFilters() {
        if (subgraphGen) {
            subgraphGen->reset();
        }
    }
    
    // Add port to multi-leg route
    void addToMultiLegRoute(Port* port, RouteEdge* route) {
        if (!multiLegRoute) {
            multiLegRoute = new LinkedList();
        }
        multiLegRoute->addNode(port, route);
    }
    
    // Remove port from multi-leg route
    void removeFromMultiLegRoute(int index) {
        if (multiLegRoute) {
            multiLegRoute->removeNode(index);
        }
    }
    
    LinkedList* getMultiLegRoute() {
        return multiLegRoute;
    }
    
    // Get layover information for a port
    int getLayoverInfo(Port* port) {
        if (layoverManager) {
            return layoverManager->getQueueSize(port);
        }
        return 0;
    }
    
    bool initialize() {
        // Create window
        window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "OceanRoute Nav - Maritime Navigation Optimizer");
        
        // Try to load font (optional - SFML will use default if not found)
        // You can add a font file if needed: font.loadFromFile("arial.ttf");
        
        // Load textures
        if (!loadTextures()) {
            return false;
        }
        
        // Initialize graph
        graph = new Graph();
        
        // Parse data files
        parsePorts(graph, "ports.txt");
        parsePortCharges(graph, "PortCharges.txt");
        parseRoutes(graph, "Routes.txt");
        
        // Initialize feature managers
        routeBooking = new RouteBooking(graph);
        layoverManager = new LayoverManager(graph);
        subgraphGen = new SubgraphGenerator(graph);
        
        cout << "Loaded " << graph->getPortCount() << " ports" << endl;
        cout << "All features initialized: Route Booking, Layover Management, Subgraph Generation" << endl;
        
        return true;
    }
    
    void drawStartScreen() {
        // Draw ocean currents as cover image
        oceanCurrentsSprite.setColor(sf::Color::White);
        window->draw(oceanCurrentsSprite);
        
        // Draw title
        // Note: Font rendering commented out - uncomment when font is available
        /*
        sf::Text title;
        title.setFont(font);
        title.setString("OceanRoute Nav");
        title.setCharacterSize(72);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        title.setPosition(1920.0f / 2 - 200, 200);
        window->draw(title);
        
        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("Maritime Navigation Optimizer");
        subtitle.setCharacterSize(36);
        subtitle.setFillColor(sf::Color::Cyan);
        subtitle.setPosition(1920.0f / 2 - 250, 300);
        window->draw(subtitle);
        */
        
        // Draw start button
        sf::RectangleShape startButton(sf::Vector2f(300, 80));
        startButton.setPosition(1920.0f / 2 - 150, 500);
        startButton.setFillColor(sf::Color(0, 150, 255, 200));
        startButton.setOutlineColor(sf::Color::White);
        startButton.setOutlineThickness(3);
        window->draw(startButton);
        
        // Draw button text (uncomment when font available)
        /*
        sf::Text buttonText;
        buttonText.setFont(font);
        buttonText.setString("START");
        buttonText.setCharacterSize(40);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setStyle(sf::Text::Bold);
        buttonText.setPosition(1920.0f / 2 - 60, 515);
        window->draw(buttonText);
        */
        
        // Draw instructions
        /*
        sf::Text instructions;
        instructions.setFont(font);
        instructions.setString("Click START to begin navigation");
        instructions.setCharacterSize(24);
        instructions.setFillColor(sf::Color::Yellow);
        instructions.setPosition(1920.0f / 2 - 180, 600);
        window->draw(instructions);
        */
    }
    
    void drawSidebar() {
        // Draw sidebar background
        sf::RectangleShape sidebar(sf::Vector2f(sidebarWidth, 1080));
        sidebar.setPosition(0, 0);
        sidebar.setFillColor(sf::Color(30, 30, 50, 230));
        sidebar.setOutlineColor(sf::Color::White);
        sidebar.setOutlineThickness(2);
        window->draw(sidebar);
        
        // Draw menu title
        // Note: Font rendering commented - uncomment when font available
        /*
        sf::Text menuTitle;
        menuTitle.setFont(font);
        menuTitle.setString("MENU");
        menuTitle.setCharacterSize(28);
        menuTitle.setFillColor(sf::Color::Yellow);
        menuTitle.setStyle(sf::Text::Bold);
        menuTitle.setPosition(20, 20);
        window->draw(menuTitle);
        */
        
        // Draw operation status section
        sf::RectangleShape statusBox(sf::Vector2f(sidebarWidth - 20, 150));
        statusBox.setPosition(10, 900);
        statusBox.setFillColor(sf::Color(20, 20, 40, 255));
        statusBox.setOutlineColor(sf::Color::Cyan);
        statusBox.setOutlineThickness(2);
        window->draw(statusBox);
        
         // Draw operation status indicator
         // Draw status label area
         sf::RectangleShape statusLabel(sf::Vector2f(sidebarWidth - 40, 20));
         statusLabel.setPosition(20, 910);
         statusLabel.setFillColor(sf::Color(40, 40, 60));
         statusLabel.setOutlineColor(sf::Color::Cyan);
         statusLabel.setOutlineThickness(1);
         window->draw(statusLabel);
         
         // Draw operation indicator bar
         if (currentOperation) {
             float opY = 935;
             float maxWidth = sidebarWidth - 40;
             
             // Draw operation progress bar
             sf::RectangleShape opBar(sf::Vector2f(maxWidth, 8));
             opBar.setPosition(20, opY);
             opBar.setFillColor(sf::Color(20, 20, 40));
             opBar.setOutlineColor(sf::Color::Cyan);
             opBar.setOutlineThickness(1);
             window->draw(opBar);
             
             // Draw filled portion (animated based on time)
             float elapsed = operationClock.getElapsedTime().asSeconds();
             float fillRatio = fmin(elapsed / 5.0f, 1.0f);  // Fill over 5 seconds
             sf::RectangleShape fillBar(sf::Vector2f(maxWidth * fillRatio, 6));
             fillBar.setPosition(22, opY + 1);
             fillBar.setFillColor(sf::Color::Cyan);
             window->draw(fillBar);
         }
         
         // Draw time indicator
         float elapsed = operationClock.getElapsedTime().asSeconds();
         float timeY = 960;
         
         // Draw time bar
         sf::RectangleShape timeBar(sf::Vector2f(sidebarWidth - 40, 15));
         timeBar.setPosition(20, timeY);
         timeBar.setFillColor(sf::Color(40, 40, 60));
         timeBar.setOutlineColor(sf::Color::Yellow);
         timeBar.setOutlineThickness(1);
         window->draw(timeBar);
         
         // Draw time indicator dots
         int dotCount = (int)(elapsed * 2) % 10;  // Animated dots
         for (int i = 0; i < dotCount && i < 8; i++) {
             sf::CircleShape dot(2);
             dot.setPosition(25 + i * 25, timeY + 5);
             dot.setFillColor(sf::Color::Yellow);
             window->draw(dot);
         }
        
        // Draw menu buttons
        float buttonY = 80;
        float buttonHeight = 50;
        float buttonSpacing = 10;
        
        // Button 1: Find Cheapest Route (Green indicator)
        sf::RectangleShape btn1(sf::Vector2f(sidebarWidth - 20, buttonHeight));
        btn1.setPosition(10, buttonY);
        btn1.setFillColor(sf::Color(50, 50, 100, 200));
        btn1.setOutlineColor(sf::Color::White);
        btn1.setOutlineThickness(1);
        window->draw(btn1);
        drawButtonIndicator(10, buttonY, sidebarWidth - 20, buttonHeight, 1);
        
        // Button 2: Find Fastest Route (Blue indicator)
        buttonY += buttonHeight + buttonSpacing;
        sf::RectangleShape btn2(sf::Vector2f(sidebarWidth - 20, buttonHeight));
        btn2.setPosition(10, buttonY);
        btn2.setFillColor(sf::Color(50, 50, 100, 200));
        btn2.setOutlineColor(sf::Color::White);
        btn2.setOutlineThickness(1);
        window->draw(btn2);
        drawButtonIndicator(10, buttonY, sidebarWidth - 20, buttonHeight, 2);
        
        // Button 3: Filter by Company (Magenta indicator)
        buttonY += buttonHeight + buttonSpacing;
        sf::RectangleShape btn3(sf::Vector2f(sidebarWidth - 20, buttonHeight));
        btn3.setPosition(10, buttonY);
        btn3.setFillColor(sf::Color(50, 50, 100, 200));
        btn3.setOutlineColor(sf::Color::White);
        btn3.setOutlineThickness(1);
        window->draw(btn3);
        drawButtonIndicator(10, buttonY, sidebarWidth - 20, buttonHeight, 3);
        
        // Button 4: Reset Filters (Red indicator)
        buttonY += buttonHeight + buttonSpacing;
        sf::RectangleShape btn4(sf::Vector2f(sidebarWidth - 20, buttonHeight));
        btn4.setPosition(10, buttonY);
        btn4.setFillColor(sf::Color(50, 50, 100, 200));
        btn4.setOutlineColor(sf::Color::White);
        btn4.setOutlineThickness(1);
        window->draw(btn4);
        drawButtonIndicator(10, buttonY, sidebarWidth - 20, buttonHeight, 4);
    }
    
    void drawMainMap() {
        // Draw world map background only (shifted right to make room for sidebar)
        worldMapSprite.setPosition(sidebarWidth, 0);
        window->draw(worldMapSprite);
        
        // Ocean currents overlay removed - only show clean map
    }
    
    void run() {
        if (!window) return;
        
        while (window->isOpen()) {
            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window->close();
                }
                
                // Handle start screen click
                if (showStartScreen && event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        int mx = event.mouseButton.x;
                        int my = event.mouseButton.y;
                        // Check if clicked on start button (center of screen)
                        if (mx >= 1920.0f / 2 - 150 && mx <= 1920.0f / 2 + 150 &&
                            my >= 500 && my <= 580) {
                            showStartScreen = false;
                            showMainMap = true;
                            setCurrentOperation("Map Loaded");
                        }
                    }
                }
                
                // Handle mouse movement for hover detection (only in main map)
                if (showMainMap && event.type == sf::Event::MouseMoved) {
                    mouseX = event.mouseMove.x;
                    mouseY = event.mouseMove.y;
                    
                    // Adjust for sidebar offset
                    float adjustedX = mouseX - sidebarWidth;
                    
                    // Check if hovering over sidebar buttons
                    if (adjustedX < 0) {
                        // Mouse is in sidebar area
                        hoveredPort = nullptr;
                        hoveredRoute = nullptr;
                    } else {
                        // Check if hovering over a port
                        hoveredPort = nullptr;
                        Port* current = graph->getFirstPort();
                        while (current != nullptr) {
                            if (current->isActive) {
                                int dx = adjustedX - current->x;
                                int dy = mouseY - current->y;
                                if (dx * dx + dy * dy < 400) {  // Within 20 pixels
                                    hoveredPort = current;
                                    break;
                                }
                            }
                            current = current->next;
                        }
                        
                        // Check if hovering over a route
                        hoveredRoute = nullptr;
                        if (!hoveredPort) {
                            current = graph->getFirstPort();
                            while (current != nullptr) {
                                if (current->isActive) {
                                    RouteEdge* edge = current->firstEdge;
                                    while (edge != nullptr) {
                                        if (edge->isActive && edge->destination && edge->destination->isActive) {
                                            // Check if mouse is near the route line
                                            float x1 = current->x;
                                            float y1 = current->y;
                                            float x2 = edge->destination->x;
                                            float y2 = edge->destination->y;
                                            
                                            // Distance from point to line segment
                                            float A = adjustedX - x1;
                                            float B = mouseY - y1;
                                            float C = x2 - x1;
                                            float D = y2 - y1;
                                            float dot = A * C + B * D;
                                            float lenSq = C * C + D * D;
                                            float param = (lenSq != 0) ? dot / lenSq : -1;
                                            
                                            float xx, yy;
                                            if (param < 0) {
                                                xx = x1;
                                                yy = y1;
                                            } else if (param > 1) {
                                                xx = x2;
                                                yy = y2;
                                            } else {
                                                xx = x1 + param * C;
                                                yy = y1 + param * D;
                                            }
                                            
                                            float dx = adjustedX - xx;
                                            float dy = mouseY - yy;
                                            float dist = sqrt(dx * dx + dy * dy);
                                            
                                            if (dist < 5) {  // Within 5 pixels of route
                                                hoveredRoute = edge;
                                                break;
                                            }
                                        }
                                        edge = edge->next;
                                    }
                                    if (hoveredRoute) break;
                                }
                                current = current->next;
                            }
                        }
                    }
                }
                
                // Handle mouse clicks for port selection (only in main map)
                if (showMainMap && event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        mouseX = event.mouseButton.x;
                        mouseY = event.mouseButton.y;
                        float adjustedX = mouseX - sidebarWidth;
                        
                        // Check if clicked on sidebar buttons
                        if (adjustedX < 0) {
                            float buttonY = 80;
                            float buttonHeight = 50;
                            float buttonSpacing = 10;
                            
                            // Button 1: Find Cheapest Route
                            if (mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
                                if (selectedPort && destinationPort) {
                                    setCurrentOperation("Finding Cheapest Route...");
                                    findCheapestPath(selectedPort, destinationPort);
                                    setCurrentOperation("Cheapest Route Found");
                                } else {
                                    setCurrentOperation("Please select origin and destination ports");
                                }
                            }
                            // Button 2: Find Fastest Route
                            buttonY += buttonHeight + buttonSpacing;
                            if (mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
                                if (selectedPort && destinationPort) {
                                    setCurrentOperation("Finding Fastest Route...");
                                    findFastestPath(selectedPort, destinationPort);
                                    setCurrentOperation("Fastest Route Found");
                                } else {
                                    setCurrentOperation("Please select origin and destination ports");
                                }
                            }
                            // Button 3: Filter by Company (placeholder)
                            buttonY += buttonHeight + buttonSpacing;
                            if (mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
                                setCurrentOperation("Filter: MaerskLine");
                                filterByCompany("MaerskLine");
                            }
                            // Button 4: Reset Filters
                            buttonY += buttonHeight + buttonSpacing;
                            if (mouseY >= buttonY && mouseY <= buttonY + buttonHeight) {
                                setCurrentOperation("Resetting Filters...");
                                resetFilters();
                                setCurrentOperation("Filters Reset");
                            }
                        } else {
                            // Find clicked port
                            Port* current = graph->getFirstPort();
                            while (current != nullptr) {
                                if (current->isActive) {
                                    int dx = adjustedX - current->x;
                                    int dy = mouseY - current->y;
                                    if (dx * dx + dy * dy < 400) {  // Within 20 pixels
                                        if (selectedPort == nullptr) {
                                            selectedPort = current;
                                            setCurrentOperation("Origin Selected");
                                            cout << "Selected origin port: " << current->name << endl;
                                        } else if (selectedPort != current) {
                                            destinationPort = current;
                                            setCurrentOperation("Destination Selected");
                                            cout << "Selected destination port: " << current->name << endl;
                                            // Auto-find shortest path
                                            setCurrentOperation("Finding Route...");
                                            findShortestPath(selectedPort, destinationPort, false);
                                            setCurrentOperation("Route Found");
                                        }
                                        break;
                                    }
                                }
                                current = current->next;
                            }
                        }
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        // Right click to clear selection
                        selectedPort = nullptr;
                        destinationPort = nullptr;
                        if (currentPath) {
                            delete currentPath;
                            currentPath = nullptr;
                        }
                        setCurrentOperation("Selection Cleared");
                    }
                }
                
                // Handle keyboard input for features (only in main map)
                if (showMainMap && event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::C) {
                        // Find cheapest path
                        if (selectedPort && destinationPort) {
                            setCurrentOperation("Finding Cheapest Route...");
                            findCheapestPath(selectedPort, destinationPort);
                            setCurrentOperation("Cheapest Route Found");
                        }
                    } else if (event.key.code == sf::Keyboard::F) {
                        // Find fastest path
                        if (selectedPort && destinationPort) {
                            setCurrentOperation("Finding Fastest Route...");
                            findFastestPath(selectedPort, destinationPort);
                            setCurrentOperation("Fastest Route Found");
                        }
                    } else if (event.key.code == sf::Keyboard::R) {
                        // Reset filters
                        setCurrentOperation("Resetting Filters...");
                        resetFilters();
                        setCurrentOperation("Filters Reset");
                    }
                }
            }
            
            // Clear window
            window->clear(sf::Color::Black);
            
            if (showStartScreen) {
                // Draw start screen
                drawStartScreen();
            } else if (showMainMap) {
                // Draw main map view
                drawMainMap();
                
                // Draw sidebar
                drawSidebar();
                
                // Draw routes (adjusted for sidebar)
                drawRoutes();
                
                // Draw layovers
                drawLayovers();
                
                // Draw path if exists
                if (currentPath != nullptr) {
                    drawPath(currentPath);
                }
                
                // Draw multi-leg route if exists
                if (multiLegRoute != nullptr && multiLegRoute->getSize() > 1) {
                    drawPath(multiLegRoute);
                }
                
                // Draw ports (adjusted for sidebar)
                drawPorts();
                
                // Highlight selected ports
                if (selectedPort != nullptr) {
                    sf::CircleShape highlight(25);
                    highlight.setPosition(selectedPort->x + sidebarWidth - 25, selectedPort->y - 25);
                    highlight.setFillColor(sf::Color::Transparent);
                    highlight.setOutlineColor(sf::Color::Yellow);
                    highlight.setOutlineThickness(3);
                    window->draw(highlight);
                }
                if (destinationPort != nullptr) {
                    sf::CircleShape highlight(25);
                    highlight.setPosition(destinationPort->x + sidebarWidth - 25, destinationPort->y - 25);
                    highlight.setFillColor(sf::Color::Transparent);
                    highlight.setOutlineColor(sf::Color::Green);
                    highlight.setOutlineThickness(3);
                    window->draw(highlight);
                }
            }
            
            // Display
            window->display();
        }
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    OceanRouteNav app;
    
    if (!app.initialize()) {
        cerr << "Failed to initialize application" << endl;
        return -1;
    }
    
    app.run();
    
    return 0;
}


