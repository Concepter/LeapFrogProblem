// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
// Nicolas Tsagarides
// 29/11/2014
// The jumping frogs puzzle
//
// Operations:
// LeftFrog jumps one spot
// LeftFrog jumps two spots
// RightFrog jumps one spot
// RightFrog jumps two spots
// Simplified into one operation:
// Frog on place x jumps to the empty spot
// The priority is from frog x = 0 to frog x = 6

#include "stdafx.h"
#include <iostream>
#include <stack>
#include <string>

using namespace std;

enum Field { EMPTY, LeftFrog, RightFrog };
enum validation { INVALID, TWOLEFT, ONELEFT, TWORIGHT, ONERIGHT };

class State
{
public:
	Field GameField[7];
	State* parent;
	int derivedFromOperation = -1; // -1 means no operation was performed
	Field DirectionOfParent;

	State(Field[], State*);
	State(Field[], State*, int);
	State(Field[], State*, int, Field);
	State(State*);
	static void childOf(State*, State*);
};

validation valid(Field[], int);
bool GameFieldsEqual(Field[], Field[]);
bool nodeAlreadyExpanded(Field[], stack <State>);

int main(int argc, char const *argv[])
{
	Field startingState[] = { LeftFrog, LeftFrog, LeftFrog, EMPTY, RightFrog, RightFrog, RightFrog }; // This is the starting state
	Field acceptedState[] = { RightFrog, RightFrog, RightFrog, EMPTY, LeftFrog, LeftFrog, LeftFrog }; // This is the goal

	stack <State> open; // OPEN stack
	stack <State> close; // CLOSE stack
	stack <State> expandedStates; // temporary stack to put the expanded states before pushing them in the open stack

	open.push(State(startingState, NULL, -1, EMPTY)); // push the starting state to the OPEN stack

	State* ParentNode;
	State* ThisNode;

	Field currentState[7];
	Field currentDirectionOfParent = EMPTY;

	int currentOperation;

	while (!open.empty())
	{
		copy(begin(open.top().GameField), end(open.top().GameField), begin(currentState)); // get the current state
		ParentNode = open.top().parent; // saving the parent of the state to be pushed into the CLOSE stack later
		ThisNode = new State(&open.top()); // saving this node to be used as a parent on the expanded nodes
		currentOperation = open.top().derivedFromOperation;
		currentDirectionOfParent = open.top().DirectionOfParent;

		open.pop(); // pop the current state from the OPEN stack

		if (GameFieldsEqual(currentState, acceptedState))
		{
			stack <State*> solutionPath;
			solutionPath.push(ThisNode);

			while (solutionPath.top()->parent != NULL) // filling the solutionPath stack with the nodes
			{
				solutionPath.push(ThisNode->parent);
				ThisNode = ThisNode->parent;
			}

			cout << "Jumps as follows:\n";
			
			solutionPath.pop(); // discard the first node from the stack since its the root node

			while (!solutionPath.empty())
			{
				string color;
				if (solutionPath.top()->DirectionOfParent == LeftFrog)
				{
					color = "LeftFrog";
				}
				else if (solutionPath.top()->DirectionOfParent == RightFrog)
				{
					color = "RightFrog";
				}
				cout << color << " frog on place " << solutionPath.top()->derivedFromOperation + 1 << endl;
				solutionPath.pop();
			}

			break;
		}
		else
		{
			close.push(State(currentState, ParentNode, currentOperation, currentDirectionOfParent));
			for (int i = 0; i<7; i++)
			{
				validation action = valid(currentState, i); // checking which action is valid if any

				Field newState[7];

				for (int j = 0; j<7; j++)
				{
					newState[j] = currentState[j];
				}

				switch (action) // performs the valid action if any
				{
				case TWOLEFT:
					newState[i] = EMPTY; // frog jumps
					newState[i - 2] = RightFrog; // frog lands 2 spots to the left
					if (!nodeAlreadyExpanded(newState, open) && !nodeAlreadyExpanded(newState, close)) // checking if current state is already in the open or close stack
					{
						expandedStates.push(State(newState, ThisNode, i, RightFrog)); // add the current state to the OPEN stack
					}
					break;
				case ONELEFT:
					newState[i] = EMPTY;
					newState[i - 1] = RightFrog;
					if (!nodeAlreadyExpanded(newState, open) && !nodeAlreadyExpanded(newState, close))
					{
						expandedStates.push(State(newState, ThisNode, i, RightFrog));
					}
					break;
				case TWORIGHT:
					newState[i] = EMPTY;
					newState[i + 2] = LeftFrog;
					if (!nodeAlreadyExpanded(newState, open) && !nodeAlreadyExpanded(newState, close))
					{
						expandedStates.push(State(newState, ThisNode, i, LeftFrog));
					}
					break;
				case ONERIGHT:
					newState[i] = EMPTY;
					newState[i + 1] = LeftFrog;
					if (!nodeAlreadyExpanded(newState, open) && !nodeAlreadyExpanded(newState, close))
					{
						expandedStates.push(State(newState, ThisNode, i, LeftFrog));
					}
					break;
				default:
					break;
				}
			}
			while (!expandedStates.empty()) // emptying the expanded states in the open stack
			{
				open.push(expandedStates.top());
				expandedStates.pop();
			}
		}
	}

	return 0;
}

State::State(Field la[], State* p) // creating a state with a specified GameField layout and a specified parent
{
	State::childOf(this, p);
	for (int i = 0; i<7; i++)
	{
		GameField[i] = la[i];
	}
}

State::State(Field la[], State* p, int op) : State(la, p) // also setting the operation that this node was generated from
{
	derivedFromOperation = op;
}

State::State(Field la[], State* p, int op, Field c) : State(la, p, op) // also setting the color of the frog from the parent node
{
	DirectionOfParent = c;
}

State::State(State* s)
{
	for (int i = 0; i<7; i++)
	{
		GameField[i] = s->GameField[i];
	}

	derivedFromOperation = s->derivedFromOperation;

	parent = s->parent;

	DirectionOfParent = s->DirectionOfParent;
}

void State::childOf(State* c, State* p) // setting the parent of the state
{
	c->parent = p;
}

validation valid(Field GameField[], int i)
{
	if (GameField[i] == LeftFrog) // check in the frog on i is LeftFrog
	{
		if (i<6) // check if the frog goes out of bounds
		{
			if ((GameField[i + 1]) == EMPTY) // check if there is an empty spot
			{
				return (ONERIGHT);
			}
		}
		if (i<5)
		{
			if ((GameField[i + 2]) == EMPTY)
			{
				return (TWORIGHT);
			}
		}
	}
	else if (GameField[i] == RightFrog)
	{
		if (i>0)
		{
			if (GameField[i - 1] == EMPTY)
			{
				return (ONELEFT);
			}
		}
		if (i>1)
		{
			if (GameField[i - 2] == EMPTY)
			{
				return (TWOLEFT);
			}
		}
	}

	return (INVALID);
}

bool GameFieldsEqual(Field a[], Field b[]) // check if GameFields are equal
{
	bool equality = true;
	for (int i = 0; i<6; i++)
	{
		if (a[i] != b[i])
		{
			equality = false;
		}
	}

	return (equality);
}

bool nodeAlreadyExpanded(Field la[], stack <State> st) // check if the state already exist in the stack
{
	while (!st.empty())
	{
		if (GameFieldsEqual(la, st.top().GameField))
		{
			return true;
		}
		st.pop();
	}
	return false;
}

