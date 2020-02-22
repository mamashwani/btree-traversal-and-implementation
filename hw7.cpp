#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "ArgumentManager.h"

using namespace std;

struct bTreeNode
{
	int bTreeOrder;
	int recCount;
	int *list;
	bTreeNode **children;

	bTreeNode(int btreeO = 0)
	{
		bTreeOrder = btreeO;
		recCount = 0;
		list = new int[bTreeOrder - 1];
		children = new bTreeNode *[bTreeOrder];
	}
};

class bTree
{
public:

	void searchNode(bTreeNode *current, const int& item, bool& found, int& location)
	{
		location = 0;
		while (location < current->recCount && item > current->list[location])
		{
			location++;
		}
		if (location < current->recCount && item == current->list[location])
		{
			found = true;
		}
		else
		{
			found = false;
		}
	} //end searchNode

	//Function to determine if searchItem is in the B-tree.
	int search_level(const int& searchItem)
	{
		int i = 1;
		bool found = false;
		int location;
		bTreeNode *current;
		current = root;
		while (current != nullptr && !found)
		{

			searchNode(current, searchItem, found, location);
			if (!found)
			{
				current = current->children[location];
				i++;
			}
		}
		return i;
	} //end search_level

	bool search(const int& searchItem)
	{
		int i = 0;
		bool found = false;
		int location;
		bTreeNode *current;
		current = root;
		while (current != nullptr && !found)
		{
			
			searchNode(current, searchItem, found, location);
			if (!found)
			{
				current = current->children[location];
				
			}
		}
		return found;
	} //end search

	void insertNode(bTreeNode *current, const int& insertItem, bTreeNode* &rightChild, int insertPosition)
	{
		int index;
		for (index = current->recCount; index > insertPosition; index--)
		{
			current->list[index] = current->list[index - 1];
			current->children[index + 1] = current->children[index];
		}
		current->list[index] = insertItem;
		current->children[index + 1] = rightChild;
		current->recCount++;
	} //end insertNode

	void splitNode(bTreeNode *current, const int& insertItem, bTreeNode* rightChild, int insertPosition, bTreeNode* &rightNode, int &median)
	{
		rightNode = new bTreeNode(bTreeDegree);
		int mid = (bTreeDegree - 1) / 2;
		if (insertPosition <= mid) //new item goes in the first half of the node
		{
			int index = 0;
			int i = mid;
			while (i < bTreeDegree - 1)
			{
				rightNode->list[index] = current->list[i];
				rightNode->children[index + 1] = current->children[i + 1];
				index++;
				i++;
			}
			current->recCount = mid;
			insertNode(current, insertItem, rightChild, insertPosition);
			(current->recCount)--;
			median = current->list[current->recCount];
			rightNode->recCount = index;
			rightNode->children[0] = current->children[current->recCount + 1];

		}
		else //new item goes in the second half of the node
		{
			int i = mid + 1;
			int index = 0;
			while (i < bTreeDegree - 1)
			{
				rightNode->list[index] = current->list[i];
				rightNode->children[index + 1] = current->children[i + 1];
				index++;
				i++;
			}
			current->recCount = mid;
			rightNode->recCount = index;
			median = current->list[mid];
			insertNode(rightNode, insertItem, rightChild, insertPosition - mid - 1);
			rightNode->children[0] = current->children[current->recCount + 1];
		}
	} //splitNode

	void insertBTree(bTreeNode *current, const int& insertItem, int& median, bTreeNode *&rightChild, bool& isTaller)
	{
		int position;
		isTaller = false;

		if (current == nullptr)
		{
			median = insertItem;
			rightChild = nullptr;
			isTaller = true;
		}
		else
		{
			bool found;
			searchNode(current, insertItem, found, position);
			if (found)
			{
				return;
			}
			else
			{
				int newMedian;
				bTreeNode *newChild;
				insertBTree(current->children[position], insertItem, newMedian, newChild, isTaller);

				if (isTaller)
				{
					if (current->recCount < (bTreeDegree - 1))
					{
						isTaller = false;
						insertNode(current, newMedian, newChild, position); // made change to this part.... median and child 
					}
					else
					{
						splitNode(current, newMedian, newChild, position, rightChild, median); // made change to this..... rightChild and rightNode 
					}
				}
			}
		}
	}

	//Function to insert insertItem in the B-tree.
	void insert(const int& insertItem)
	{
		bool isTaller = false;
		int median;
		bTreeNode *rightChild = nullptr; // changed this part 
		insertBTree(root, insertItem, median, rightChild, isTaller);

		if (isTaller) //the tree is initially empty or the root was split by the function insertBTree
		{
			heightCounter++;
			bTreeNode *tempRoot;
			tempRoot = new bTreeNode(bTreeDegree);
			tempRoot->recCount = 1;
			tempRoot->list[0] = median;
			tempRoot->children[0] = root;
			tempRoot->children[1] = rightChild;
			root = tempRoot;

		}

	} // end insert

	void recInorderLevel(bTreeNode *current, int levelExpect, ofstream &ofs)
	{
		int levelExpec = levelExpect;
		if (current != nullptr)
		{
			int level = search_level(current->list[0]);
			recInorderLevel(current->children[0], levelExpec, ofs);
			for (int i = 0; i < current->recCount; i++)
			{
				if (level == levelExpec)
				{
					ofs << current->list[i] << " ";
				}
				recInorderLevel(current->children[i + 1], levelExpec, ofs);
			}
			
		}
	} //end recInorder

	void inOrderLevel(int level, ofstream &ofs)
	{
		recInorderLevel(root, level, ofs);
	} // end inOrder

	void recInorder(bTreeNode *current,ofstream &ofs)
	{
		if (current != nullptr)
		{
			recInorder(current->children[0],ofs);
			for (int i = 0; i < current->recCount; i++)
			{
				ofs << current->list[i] << " ";
				recInorder(current->children[i + 1],ofs);
			}
		}
	} //end recInorder

	void inOrder(ofstream &ofs)
	{
		recInorder(root, ofs);
	} // end inOrder

	bTree()
	{
		root = nullptr;
	}

	bTree(int order)
	{
		root = nullptr;
		bTreeDegree = order;
		heightCounter = 0;
	}

	void setDegree(int degree) 
	{
		bTreeDegree = degree;
	}

	int getHeightCounter() 
	{
		return heightCounter;
	}
	
protected:
	bTreeNode *root;
	int bTreeDegree;
	int heightCounter;
};

int main(int argc, char* argv[])
{
	bTree t;
	ArgumentManager am(argc, argv);
	string input = am.get("input");
	string output = am.get("output");
	string command = am.get("command");
	fstream cmdfile(command);
	fstream infile(input);
	ofstream outfile(output);
	int feeder;
	int degree;
	while (getline(cmdfile, command))
	{
		if (!command.empty())
		{
			if (command[0] == 'D') // Degree read from the command file (this only happens once)
			{
				degree = (int)(command[command.length() - 1]) - 48;
				t.setDegree(degree);
				while (getline(infile, input))
				{
					if (!input.empty())
					{
						istringstream iss(input);
						while (iss >> feeder)
						{
							t.insert(feeder);
						}
					}
				}
			}
			else if (command[0] == 'I') // command to print traversal 
			{
				t.inOrder(outfile);
				outfile << endl;
			}
			else if (command[0] == 'L') // printing specific Level of the tree with root at level 1 
			{
				istringstream iss(command);
				iss >> input; // stores garbage string 
				iss >> feeder; // stores level that needs to be printed
				
				if (feeder > t.getHeightCounter() || feeder < 0)
				{
					outfile << "empty" << endl;
				}
				else
				{
					t.inOrderLevel(feeder, outfile);
					outfile << endl;
				}
			}
		}
	}

	return 0;
}
