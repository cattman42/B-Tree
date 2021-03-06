// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>



// TODO: reference additional headers your program requires here
template <typename T, int M>
class BTree;

template <typename T, int M>
class BTreeNode
{
	friend class BTree<T, M>;
public:
	BTreeNode()
	{
		keyTally = 0;
	};
	BTreeNode(const T& el)
	{
		keyTally = 1;
		keys[0] = el;
	};
	void snapShot(std::ostream&, out);		// Why would this error be produced?
private:
	bool leaf;
	int keyTally;
	T keys[M - 1];
	BTreeNode* pointers[M];
};
template <typename T, int M>
class BTree
{
public:
	BTree() { root = new BTreeNode<T, M>(); }
	void snapShot(std::ostream& out);
	void insert(const T& key);
	void BTreeDelete(const T& key) { BTreeDelete(root, key); }
	void BTreeMerge(const T& key) { BTreeMerge(root, key); }
	void traversal() { traversal(root); }
	BTreeNode<T, M>* toParent(const T& key) { return toParent(root, nullptr, key); }
private:
	BTreeNode<T, M>* root;
	void BTreeDelete(BTreeNode<T, M>* &mover, const T& key);
	void BTreeMerge(BTreeNode<T, M>* &node, int x);
	void traversal(BTreeNode<T, M>* &node);
	BTreeNode<T, M>* toParent(BTreeNode<T, M>* node, BTreeNode<T, M>* parent, const T& key);
};

template <typename T, int M>
BTreeNode<T, M>* BTree<T, M>::toParent(BTreeNode<T, M>* node, BTreeNode<T, M>* parent, const T& key)
{
	int m = 0;
	while (m < node->keyTally && key > node->keys[m])
	{
		m++;
	}
	if (m < node->keyTally && node->keys[m] == key)
	{
		return parent;
	}
	return toParent(node->pointers[m], node, key);
}

template <typename T, int M>
void BTreeNode<T, M>::snapShot(std::ostream& out)
{
	out << '\"' << keys[0];
	for (int n = 1; n < keyTally; n++)
	{
		out << ':' << keys[n];
	}
	out << '\"';
	if (!leaf)
	{
		out << '[';
		pointers[0]->snapShot(out);
		for (int n = 1; n <= keyTally; n++)
		{
			out << ',';
			pointers[n]->snapShot(out);
		}
		out << ']';
	}
}
template <typename T, int M>
void BTree<T, M>::snapShot(std::ostream& out)
{
	out << "TreeForm[";
	root->snapShot(out);
	out << ']' << std::endl;
}

template <typename T, int M>
void BTree<T, M>::insert(const T& key)
{
	BTreeNode<T, M>* selector = root;
	BTreeNode<T, M>* parent = nullptr;
	while (selector->keyTally == M - 1 || !selector->leaf)
	{
		if (selector->keyTally == M - 1)
		{
			if (selector == root)
			{
				parent = new BTreeNode<T, M>();
				parent->leaf = false;
				parent->pointers[0] = selector;
				root = parent;
			}
			BTreeNode<T, M>* sibling = new BTreeNode<T, M>();
			int middle = ceil(((double)M) / 2 - 1);
			int j;
			for (j = parent->keyTally; j > 0; j--)
			{
				if (parent->keys[j - 1] > selector->keys[middle])
				{
					parent->keys[j] = parent->keys[j - 1];
					parent->pointers[j + 1] = parent->pointers[j];
				}
				else
					break;
			}
			parent->keys[j] = selector->keys[middle];
			parent->keyTally++;
			parent->pointers[j + 1] = sibling;
			int k;
			for (k = middle + 1; k < selector->keyTally; k++)
			{
				sibling->keys[sibling->keyTally] = selector->keys[k];
				sibling->pointers[sibling->keyTally] = selector->pointers[k];
				sibling->keyTally++;
			}
			sibling->pointers[sibling->keyTally] = selector->pointers[k];
			selector->keyTally = middle;
			if (selector->leaf)
			{
				sibling->leaf = true;
				selector = parent;
				int m = 0;
				while (key > selector->keys[m])
				{
					if (m == selector->keyTally)
					{
						break;
					}
					m++;
				}
				if (key == selector->keys[m])
				{
					return;
				}
				selector = selector->pointers[m];
				break;
			}
		}
		else
		{
			int m = 0;
			while (key > selector->keys[m])
			{
				if (m == selector->keyTally)
				{
					break;
				}
				m++;
			}
			if (key == selector->keys[m])
			{
				return;
			}
			parent = selector;
			selector = selector->pointers[m];
		}
	}

	int i;
	for (i = selector->keyTally; i > 0; i--)
	{
		if (selector->keys[i - 1] > key)
		{
			selector->keys[i] = selector->keys[i - 1];
		}
		else
		{
			break;
		}
	}
	selector->keys[i] = key;
	selector->keyTally++;
}

template <typename T, int M>
void BTree<T, M>::BTreeMerge(BTreeNode<T, M>* &node, int index)
{
	BTreeNode<T, M>* leftChild = node->pointers[index];
	BTreeNode<T, M>* rightChild = node->pointers[index + 1];
	leftChild->keys[leftChild->keyTally] = node->keys[index];
	leftChild->keyTally++;
	for (int i = 0; i < rightChild->keyTally; i++)
	{
		leftChild->keys[leftChild->keyTally] = rightChild->keys[i];
		leftChild->pointers[leftChild->keyTally] = rightChild->pointers[i];
		leftChild->keyTally++;
	}
	leftChild->pointers[leftChild->keyTally] = rightChild->pointers[rightChild->keyTally];
	rightChild = nullptr;
	if (node == root && node->keyTally == 1)
	{
		root = leftChild;
		node = root;
		return;
	}
	for (int i = index + 1; i < node->keyTally; i++)
	{
		node->keys[i - 1] = node->keys[i];
		node->pointers[i] = node->pointers[i + 1];
	}
	node->pointers[node->keyTally] = nullptr;
	node->keyTally--;
}

template <typename T, int M>
void BTree<T, M>::BTreeDelete(BTreeNode<T, M>* &selector, const T& key)
{
	int i = 0;
	while (key > selector->keys[i] && i < selector->keyTally)
	{
		i++;
	}
	int a = i;
	if (selector->leaf)
	{
		if (selector->keys[i] != key)
		{
			return;
		}
		else
		{
			for (int j = i; j < selector->keyTally - 1; j++)
			{
				selector->keys[j] = selector->keys[j + 1];
			}
			selector->keyTally--;
			selector->keys[selector->keyTally] = 0;
			return;
		}
	}
	else
	{
		if (selector->keys[i] != key)
		{
			BTreeDelete(selector->pointers[i], key);
		}
		else
		{
			BTreeNode<T, M>* newMover = selector->pointers[i];
			BTreeNode<T, M>* start = newMover;
			while (!newMover->leaf)
			{
				newMover = newMover->pointers[newMover->keyTally];
			}
			T largestKey = newMover->keys[newMover->keyTally - 1];
			selector->keys[i] = largestKey;
			BTreeDelete(newMover, largestKey);
			newMover = start;
			while (newMover->keyTally <= ceil(((double)M) / 2) && newMover != selector && !newMover->leaf)
			{
				i = newMover->keyTally;
				BTreeNode<T, M>* leftChild = newMover->pointers[i];
				BTreeNode<T, M>* rightChild = newMover->pointers[i + 1];
				if (i > 0 && (leftChild->keyTally + newMover->pointers[i - 1]->keyTally + 1 <= M - 1) && newMover->keyTally > 1)
				{
					BTreeMerge(newMover, i - 1);
				}
				if (i < newMover->keyTally && (leftChild->keyTally + rightChild->keyTally + 1 <= M - 1) && newMover->keyTally > 1)
				{
					BTreeMerge(newMover, i);
				}
				if (i > 0)
				{
					while (leftChild->keyTally < ceil(((double)M) / 2 - 1))
					{
						leftChild->keys[leftChild->keyTally] = leftChild->keys[leftChild->keyTally - 1];
						leftChild->keys[leftChild->keyTally - 1] = newMover->keys[i - 1];
						leftChild->keyTally++;
						for (int j = leftChild->keyTally; j > 0; j--)
						{
							leftChild->pointers[j] = leftChild->pointers[j - 1];
						}
						newMover->keys[i - 1] = newMover->pointers[i - 1]->keys[newMover->pointers[i - 1]->keyTally - 1];
						leftChild->pointers[0] = newMover->pointers[i - 1]->pointers[newMover->pointers[i - 1]->keyTally];
						newMover->pointers[i - 1]->pointers[newMover->pointers[i - 1]->keyTally] = nullptr;
						newMover->pointers[i - 1]->keyTally--;
						newMover->keys[newMover->keyTally] = 0;
					}
				}
				if (i < newMover->keyTally)
				{
					while (leftChild->keyTally < ceil(((double)M) / 2 - 1))
					{
						leftChild->keys[leftChild->keyTally] = newMover->keys[i];
						leftChild->pointers[leftChild->keyTally + 1] = rightChild->pointers[0];
						leftChild->keyTally++;
						newMover->keys[i] = rightChild->keys[0];
						for (int j = 0; j < rightChild->keyTally - 1; j++)
						{
							rightChild->keys[j] = rightChild->keys[j + 1];
							rightChild->pointers[j] = rightChild->pointers[j + 1];
						}
						rightChild->pointers[rightChild->keyTally - 1] = rightChild->pointers[rightChild->keyTally];
						rightChild->keyTally--;
					}
				}
				newMover = toParent(newMover->keys[0]);
			}
		}

	}
	i = a;
	BTreeNode<T, M>* leftChild = selector->pointers[i];
	BTreeNode<T, M>* rightChild = selector->pointers[i + 1];
	if (selector->pointers[i]->keyTally >= ceil(((double)M) / 2 - 1))
	{
		return;
	}
	if (i > 0 && (leftChild->keyTally + selector->pointers[i - 1]->keyTally + 1 <= M - 1) && selector->keyTally > 1)
	{
		BTreeMerge(selector, i - 1);
		return;
	}
	if (i < selector->keyTally && (leftChild->keyTally + rightChild->keyTally + 1 <= M - 1) && selector->keyTally > 1)
	{
		BTreeMerge(selector, i);
		return;
	}
	if (i > 0)
	{
		while (leftChild->keyTally < ceil(((double)M) / 2 - 1))
		{
			leftChild->keys[leftChild->keyTally] = leftChild->keys[leftChild->keyTally - 1];
			leftChild->keys[leftChild->keyTally - 1] = selector->keys[i - 1];
			leftChild->keyTally++;
			for (int j = leftChild->keyTally; j > 0; j--)
			{
				leftChild->pointers[j] = leftChild->pointers[j - 1];
			}
			selector->keys[i - 1] = selector->pointers[i - 1]->keys[selector->pointers[i - 1]->keyTally - 1];
			leftChild->pointers[0] = selector->pointers[i - 1]->pointers[selector->pointers[i - 1]->keyTally];
			selector->pointers[i - 1]->pointers[selector->pointers[i - 1]->keyTally] = nullptr;
			selector->pointers[i - 1]->keyTally--;
			selector->keys[selector->keyTally] = 0;
		}
	}
	if (i < selector->keyTally)
	{
		while (leftChild->keyTally < ceil(((double)M) / 2 - 1))
		{
			leftChild->keys[leftChild->keyTally] = selector->keys[i];
			leftChild->pointers[leftChild->keyTally + 1] = rightChild->pointers[0];
			leftChild->keyTally++;
			selector->keys[i] = rightChild->keys[0];
			for (int j = 0; j < rightChild->keyTally - 1; j++)
			{
				rightChild->keys[j] = rightChild->keys[j + 1];
				rightChild->pointers[j] = rightChild->pointers[j + 1];
			}
			rightChild->pointers[rightChild->keyTally - 1] = rightChild->pointers[rightChild->keyTally];
			rightChild->keyTally--;
		}
	}
	selector = toParent(selector->keys[0]);
}

template <typename T, int M>
void BTree<T, M>::traversal(BTreeNode<T, M>* &node) {
	for (int i = 0; i < node->keyTally; i++) {
		if (!node->leaf) {
			traversal(node->pointers[i]);
		}
		std::cout << node->keys[i] << std::endl;
	}
	if (!node->leaf) {
		traversal(node->pointers[node->keyTally]);
	}
}














