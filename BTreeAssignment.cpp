// BTreeAssignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cmath>


int main(int argc, const char * argv[])
{
	BTree<int, 5> tree;

	std::ifstream in("add.txt");
	std::ifstream in1("remove.txt");

	int x;
	while (in >> x) {
		tree.insert(x);
		tree.snapShot(out);// erroring out because of my other error
	}
	tree.traversal();
	std::cout << std::endl;
	std::cout << std::endl;

	int y;
	while (in1 >> y) {
		tree.BTreeDelete(y);
		tree.snapShot(out1);
	}
	tree.traversal();

	return 0;
}

