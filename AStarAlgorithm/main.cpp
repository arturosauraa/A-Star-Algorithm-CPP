#include <raylib.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <list>

using namespace std;

class PathFinding
{
public: 
 
    struct Node
    {
        bool obstacle = false;
        bool visited = false;
        float globalGoal;
        float localGoal;
        int x;
        int y;
        vector<Node*> neighbours;
        Node* parent;
    };

    Node *nodes = nullptr;
    int mapWidth = 16;
    int mapHeight = 16;

    Node *nodeStart = nullptr;
	Node *nodeEnd = nullptr;
    
    int nodesSize = 45;
    int nodeBorder = 10;


    PathFinding()
    {
            nodes = new Node[mapWidth * mapHeight];

            for(int x = 0; x <mapWidth;x++)
        {
            for( int y = 0; y<mapHeight;y++)
            {
                nodes[y * mapWidth + x].x = x;
                nodes[y * mapWidth + x].y = y;
                nodes[y * mapWidth + x].obstacle = false;
                nodes[y * mapWidth + x].visited = false;
                nodes[y * mapWidth + x].parent = nullptr;

            }
        } 

        for(int x = 0; x <mapWidth; x++)
        {
            for(int y = 0; y <mapHeight; y++)
            {
                    if(y>0)
                        nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x + 0)]);
                    if(y<mapHeight-1)
                        nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x + 0)]);
                    if (x>0)
                        nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 0) * mapWidth + (x - 1)]);
                    if(x<mapWidth-1)
                        nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 0) * mapWidth + (x + 1)]);
            }
        }

        nodeStart = &nodes[(mapWidth / 2) * mapWidth + 1];
        nodeEnd = &nodes[(mapWidth / 2) * mapWidth + mapWidth-2];
    }

    void Update()
    {
        int mouseX = GetMouseX();
        int mouseY = GetMouseY();

        int selectedNodeX = mouseX / nodesSize;
        int selectedNodeY = mouseY / nodesSize;

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if(selectedNodeX >= 0 && selectedNodeX < mapWidth)
            {
                if(selectedNodeY >= 0 && selectedNodeY < mapHeight)
                {
                    if(IsKeyDown(KEY_LEFT_SHIFT))
                        nodeStart = &nodes[selectedNodeY * mapWidth + selectedNodeX];
                    else if(IsKeyDown(KEY_LEFT_CONTROL))
                        nodeEnd = &nodes[selectedNodeY * mapWidth + selectedNodeX];
                    else
                        nodes[selectedNodeY * mapWidth + selectedNodeX].obstacle = !nodes[selectedNodeY * mapWidth + selectedNodeX].obstacle;
                    
                    A_Star();
                }
            }
        }
    }

    void Draw()
    {
        for (int x = 0; x < mapWidth; x++)
        {
            for (int y = 0; y < mapHeight; y++)
            {
                for (auto n : nodes[y*mapWidth + x].neighbours)
                {
                    DrawLine(x*nodesSize + (nodesSize + nodeBorder) / 2, y*nodesSize + (nodesSize + nodeBorder) / 2,
                        n->x*nodesSize + (nodesSize + nodeBorder) / 2, n->y*nodesSize + (nodesSize + nodeBorder) / 2, BLUE);
                }
            }
        }

        
        for(int x = 0; x < mapWidth; x++)
        {
            for(int y = 0; y < mapHeight; y++)
            {
                DrawRectangle(x*nodesSize + nodeBorder, y*nodesSize + nodeBorder, 
                nodesSize - nodeBorder, nodesSize - nodeBorder, 
                nodes[y * mapWidth + x].obstacle ? WHITE : BLUE);

                    if (nodes[y * mapWidth + x].visited)
					    DrawRectangle(x*nodesSize + nodeBorder, y*nodesSize + nodeBorder, nodesSize - nodeBorder, 
                        nodesSize - nodeBorder, DARKBLUE);


                if(&nodes[y * mapWidth + x] == nodeStart)
					DrawRectangle(x*nodesSize + nodeBorder, y*nodesSize + nodeBorder, nodesSize - nodeBorder, 
                    nodesSize - nodeBorder, GREEN);

				if(&nodes[y * mapWidth + x] == nodeEnd)
					DrawRectangle(x*nodesSize + nodeBorder, y*nodesSize + nodeBorder, nodesSize - nodeBorder, 
                    nodesSize - nodeBorder, RED);
            }
        }

        if (nodeEnd != nullptr)
		{
			Node *p = nodeEnd;
			while (p->parent != nullptr)
			{
				DrawLine(p->x*nodesSize + (nodesSize + nodeBorder) / 2, p->y*nodesSize + (nodesSize + nodeBorder) / 2,
					p->parent->x*nodesSize + (nodesSize + nodeBorder) / 2, p->parent->y*nodesSize + (nodesSize + nodeBorder) / 2, YELLOW);
				
				p = p->parent;
			}
		}
    }

    void A_Star()
    {
        for (int x = 0; x < mapWidth; x++)
			for (int y = 0; y < mapHeight; y++)
			{
				nodes[y*mapWidth + x].visited = false;
				nodes[y*mapWidth + x].globalGoal = __FLT_MAX__;
				nodes[y*mapWidth + x].localGoal = __FLT_MAX__;
				nodes[y*mapWidth + x].parent = nullptr;	
			}

		auto distance = [](Node* a, Node* b) 
		{
			return sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y));
		};

        Node *nodeCurrent = nodeStart;
		nodeStart->localGoal = 0.0f;
		nodeStart->globalGoal = distance(nodeStart, nodeEnd);

       	list<Node*> listNotTestedNodes;
		listNotTestedNodes.push_back(nodeStart);

        while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
		{

			listNotTestedNodes.sort([](const Node* lhs, const Node* rhs){ return lhs->globalGoal < rhs->globalGoal; } );
			

			while(!listNotTestedNodes.empty() && listNotTestedNodes.front()->visited)
				listNotTestedNodes.pop_front();


			if (listNotTestedNodes.empty())
				break;

			nodeCurrent = listNotTestedNodes.front();
			nodeCurrent->visited = true;

			for (auto nodeNeighbour : nodeCurrent->neighbours)
			{

				if (!nodeNeighbour->visited && nodeNeighbour->obstacle == 0)
					listNotTestedNodes.push_back(nodeNeighbour);

				float fPossiblyLowerGoal = nodeCurrent->localGoal + distance(nodeCurrent, nodeNeighbour);

				if (fPossiblyLowerGoal < nodeNeighbour->localGoal)
				{
					nodeNeighbour->parent = nodeCurrent;
					nodeNeighbour->localGoal = fPossiblyLowerGoal;
					nodeNeighbour->globalGoal = nodeNeighbour->localGoal + distance(nodeNeighbour, nodeEnd);
				}
			}	
		}

    }


};

int main()
{
    InitWindow(800, 800, "A Star Algorithm");
    SetTargetFPS(60);
    
    PathFinding pathFinding = PathFinding();

    while(WindowShouldClose() == false)
    {
        BeginDrawing();

        pathFinding.Update();

        ClearBackground(BLACK);

        pathFinding.Draw();

        EndDrawing();

    }
    CloseWindow();
    return 0;
} 