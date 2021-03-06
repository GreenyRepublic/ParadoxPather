#include "stdafx.h"
#include <vector>
#include <queue>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <tchar.h>

#define INT_MAX 2147483647

struct gridCompare {
	const bool operator()(const int& a,
		const int& b, const std::vector<int>& weights)
	{
		return (weights[a] > weights[b]);
	}
};

//Add neighbours to frontier
void updateNeighbours(std::vector<int>& gridWeights, std::vector<int>& gridParents, std::vector<int>& frontier, int index, int rowSize)
{
	int weight = gridWeights[index];
	int neighbours[4] = {index - 1, index + 1, index - rowSize, index + rowSize}; /*| Left | Right | Down | Up |*/

	for (auto i : neighbours)
	{
		if (i < 0 || i >= gridWeights.size() || 
			i % rowSize == 0 || (i - 1) % rowSize == (rowSize - 1))	continue;

		if (gridWeights[i] != -1 && gridWeights[i] > weight + 1)
		{
			gridWeights[i] = weight + 1;
			gridParents[i] = index;
			frontier.push_back(index);
		}
	}
}

//Implements Dijkstra's algorithm with implicit path/neighbours, and frontier/restricted search optimisations.
int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	//Numbers (row-major) of the start and end grid squares
	const unsigned int startSquare = (nMapWidth * nStartY) + nStartX,
						endSquare = (nMapWidth * nTargetY) + nTargetX;	
	int length = -1;

	//gridWeights uses -1 to represent an intraversible or a visited square, and the int limit (large weight) for anything unvisited
	//parents records the squares on the shortest path
	//frontier is a list of all adjacent nodes to those already visited, if empty we're out of luck.
	std::vector<int> gridWeights;
	std::vector<int> parents;
	std::priority_queue<std::pair<int, int>, std::vector<int>, gridCompare> frontier;

	for (int i = 0; i < nMapHeight * nMapWidth; i++)
	{
		const int value = ((unsigned int)*pMap == 1)? INT_MAX : -1;
		gridWeights.push_back(value);
		parents.push_back(-1);
		pMap++;
	}
	gridWeights[startSquare] = 0;
	frontier.push(startSquare);
	//Loop terminates if every square has been visited (implicit if smallest() returns -1), we reach the destination square, or hit the path size limit
	while (true)
	{
		//Get the index of the square with the smallest weight
		int minIndex = frontier.top();
		if (minIndex == -1) return -1; //No traversible squares available
		//updateNeighbours(gridWeights, parents, minIndex, nMapWidth); //Add neighbours (if not yet visited) to frontier and update their weights.

		//Mark current square as visited
		gridWeights[minIndex] = -1;

		//Found the end square!
		if (minIndex == endSquare)
		{
			//Walk backwards through parent array to construct path.
			std::vector<int> path;
			unsigned int node = endSquare;
			do {
				path.push_back(node);
				node = parents[node];
			} while (node != startSquare);

			if (path.size() > nOutBufferSize) return -1; //Too long

			for (auto i = path.crbegin(); i != path.crend(); i++)
			{
				*pOutBuffer = *i;
				pOutBuffer++;
			}
			return path.size();
		}
	}
}

int main()
{
	
	unsigned char pMap[256];
	std::fill_n(pMap, 256, 1);

	int pOutBuffer[32];
	int test = FindPath(0, 0, 8, 8, pMap, 16, 16, pOutBuffer, 32);
	std::cout << "Path length: " << test << std::endl;
	std::cout << "Path: ";
	for (auto i : pOutBuffer)
	{
		std::cout << i << " > ";
	}
	std::cout << std::endl;
	
    return 0;
}

