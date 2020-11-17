/**
 * A library for a generic Red Black Tree.
 */
#define NDEBUG

#include <stdlib.h>
#include "RBTree.h"
#include <stdbool.h>
#include <assert.h>
#include <string.h>
/**
 * def smaller than 0 for first smaller in compare functions.
 */
#define FIRST_LESS(x) (x < 0)
/**
 * def greater than 0 for first greater in compare functions.
 */
#define FIRST_GREATER(x) (x > 0)
/**
 * def 0 for both equal in compare functions.
 */
#define BOTH_EQUAL(x) (x == 0)
/**
 * 0 as failure in functions other than main
 */
#define FAILURE 0
/**
 *  1 as success in functions other than main
 */
#define SUCCESS 1

/**
 *
 * @param compFunc
 * @param freeFunc
 * @return
 */
RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
	if (compFunc == NULL || freeFunc == NULL)
	{
		return NULL;
	}
	RBTree *tree = (RBTree *) malloc(sizeof(RBTree));
	if (tree == NULL)
	{
		return NULL;
	}
	RBTree newTree = {NULL, compFunc, freeFunc, 0};
	*tree = newTree;
	return tree;
}

/**
 * Find am element in the tree and return a pointer to the node containing.
 * @param tree: A pointer to the tree structure.
 * @param data: A generic void pointer to the data.
 * @param closestParent: A pointer to a pointer in which the closest parent will be
 * stored in case the data is not in the tree for inserstion.
 * @return: A pointer to the node if  found, NULL otherwise.
 */
static Node *find(const RBTree *tree, const void *data, Node **closestParent)
{
	Node *currNode = tree->root;
	if (currNode == NULL)
	{
		return NULL;
	}
	int cmpRes = tree->compFunc(data, currNode->data);
	while ((currNode->right && FIRST_GREATER(cmpRes)) || (currNode->left && FIRST_LESS(cmpRes)))
	{
		if (currNode->left && FIRST_LESS(cmpRes))
		{
			currNode = currNode->left;

		}
		else if (currNode->right && FIRST_GREATER(cmpRes))
		{
			currNode = currNode->right;

		}
		cmpRes = tree->compFunc(data, currNode->data);
		if (BOTH_EQUAL(cmpRes))
		{

			return currNode;
		}
	}
	if (BOTH_EQUAL(cmpRes))
	{
		return currNode;
	}
	if (closestParent != NULL)
	{
		*closestParent = currNode;
	}
	return NULL;
}

/**
 * Check if a node is a right child of is parent.
 * @param node
 * @return 1
 if node is a right child, 0 otherwise.
 */
static int isRightChild(const Node *node)
{
	if (node->parent == NULL)
	{
		return false;
	}
	return node->parent->right == node;
}

/**
 * Find the node with the minimal value in a subtree.
 * @param subTreeRoot: pointer root of the subtree.
 * @return: A pointer to the Node containing the min value.
 */
static Node *subTreeMin(Node *subTreeRoot)
{
	Node *currNode = subTreeRoot;
	while (currNode && currNode->left != NULL)
	{
		currNode = currNode->left;
	}
	return currNode;
}

/**
 * Perform a right-rotation at a given node.
 * @param node: a none NULL pointer to a node to make the rotation at.
 */
static void rotateRight(Node *node)
{
	//Change the node to hold the information of its left node, and its left node wil hold
	// the node info.
	assert(node != NULL);
	Node *rChild = node->right, *lChild = node->left;
	if (lChild == NULL)
	{
		return;
	}
	void *data = node->data;
	Color color = node->color;
	node->right = node->left;
	node->left = lChild->left;
	if (node->left != NULL)
	{
		node->left->parent = node;
	}

	node->data = lChild->data;
	node->color = lChild->color;
	node->right->data = data;
	node->right->color = color;
	node->right->left = node->right->right;
	node->right->right = rChild;
	if (node->right->right)
	{
		node->right->right->parent = node->right;
	}

	if (node->right->right != NULL)
	{
		node->right->right->parent = node->right;
	}
	assert(node->right->right == NULL || node->right->right->parent == node->right);
}

/**
 * Perform a left-rotation at a  given node.
 * @param node: a pointer to a node to make the rotation at.
 */
static void rotateLeft(Node *node)
{
	Node *rChild = node->right, *lChild = node->left;
	void *data = node->data;
	Color color = node->color;
	node->left = node->right;
	node->right = rChild->right;
	if (node->right != NULL)
	{
		node->right->parent = node;
	}

	node->data = rChild->data;
	node->color = rChild->color;
	node->left->data = data;
	node->left->color = color;
	node->left->right = node->left->left;
	if (node->left->right != NULL)
	{
		node->left->right->parent = node->left;
	}
	assert(node->left->right == NULL || node->left->right->parent == node->left);
	node->left->left = lChild;
	if (node->left->left != NULL)
	{
		node->left->left->parent = node->left;
	}
	assert(node->left->left == NULL || node->left->left->parent == node->left);
	assert(node->right == NULL || node->right->parent == node);
}

/**
 * correct a RB tree in which a node be red illegally (a red parent).
 * @param node: A pointer to the node which might violate the property.
 */
static void fixTwoRed(Node *node)
{
	assert(node != NULL);
	Node *parent = node->parent;
	if (parent == NULL)
	{
		node->color = BLACK;
		return;
	}
	if (parent->color == BLACK)
	{
		return;
	}
	Node *grandParent = parent->parent;
	assert(parent->parent != NULL);
	Node *uncle = (isRightChild(parent) ? grandParent->left : grandParent->right);
	if (uncle != NULL && uncle->color == RED)
	{
		parent->color = BLACK, uncle->color = BLACK;
		grandParent->color = RED;
		fixTwoRed(grandParent);
		return;
	}
	if (isRightChild(node) && !isRightChild(parent))
	{
		rotateLeft(parent);
	}
	if (!isRightChild(node) && isRightChild(parent))
	{
		rotateRight(parent);
	}
	if (!isRightChild(node) && !isRightChild(parent))
	{
		rotateRight(grandParent);
	}
	if (isRightChild(node) && isRightChild(parent))
	{
		rotateLeft(grandParent);
	}
	grandParent->color = BLACK;
	parent->color = RED;
}

/**
 * Choose and call the appropriate rotation s.t after the rotation, the node will be in the parent
 * position.
 * @param node: A node with a none NULL parent to rotate from.
 */
static void rotateFromChild(Node *node)
{
	if (isRightChild(node))
	{
		rotateLeft(node->parent);
	}

	else
	{
		rotateRight(node->parent);
	}
}

/**
 * fix a situation when a node is violating the tree property after deleting a node/
 * @param node: the node which might have the problem
 */
static void fixDoubleBlack(Node *node)
{
	if (node->color == RED)
	{
		node->color = BLACK;
		return;
	}
	Node *parent = node->parent;
	if (parent == NULL)
	{
		return;
	}
	Node *sibling = node == parent->right ? parent->left : parent->right;
	assert(sibling != NULL);
	Color sibRChildColor = sibling->right ? sibling->right->color : BLACK;
	Color sibLChildColor = sibling->left ? sibling->left->color : BLACK;
	if (sibling->color == BLACK)
	{
		if (sibLChildColor == BLACK && sibRChildColor == BLACK)
		{
			sibling->color = RED;
			fixDoubleBlack(parent);
			return;
		}
		Node *closeNephew = isRightChild(node) ? sibling->right : sibling->left;
		Color closeNephewColor = closeNephew ? closeNephew->color : BLACK;
		if (closeNephewColor == RED)
		{
			closeNephew->color = BLACK;
			sibling->color = RED;
			rotateFromChild(closeNephew);
		}
		Node *farNephew = isRightChild(node) ? sibling->left : sibling->right;
		farNephew->color = BLACK;
		sibling->color = parent->color;
		parent->color = BLACK;
		rotateFromChild(sibling);
		sibling->color = BLACK;
		return;
	}
	if (sibling->color == RED)
	{
		sibling->color = BLACK;
		parent->color = RED;
		rotateFromChild(sibling);
		fixDoubleBlack(node);
		return;
	}
}

/**
 * delete a specific node from a tree.
 * @param tree:
 * @param node: The node to delete.
 * @return 1 upon success, 0 otherwise
 */
static int deleteNode(RBTree *tree, Node *node)
{
	if (node->color == RED)
	{
		Node **ptrFromParent = isRightChild(node) ? &(node->parent->right) : &(node->parent->left);
		*ptrFromParent = NULL;
		tree->freeFunc(node->data);
		free(node);
		return SUCCESS;
	}
	Node *child = node->right ? node->right : node->left;
	Color childColor = child ? child->color : BLACK;
	if (childColor == RED)
	{
		//Swap with child.
		void *data = node->data;
		Node *parent = node->parent;
		child->parent = parent;
		child->color = BLACK;

		*node = *child;
		tree->freeFunc(data);
		free(child);
		return SUCCESS;
	}
	fixDoubleBlack(node);
	if (node->parent)
	{
		Node **ptrFromParent = isRightChild(node) ? &(node->parent->right) : &(node->parent->left);
		*ptrFromParent = NULL;
	}
	else
	{
		tree->root = NULL;
	}
	tree->freeFunc(node->data);
	free(node);
	return SUCCESS;
}

/**
 *
 * @param tree
 * @param data
 * @return
 */
int insertToRBTree(RBTree *tree, void *data)
{
	if (!tree || !data)
	{
		return FAILURE;
	}
	Node *closestParent = NULL;
	Node *dataNode = find(tree, data, &closestParent);
	if (dataNode != NULL)
	{
		return FAILURE;
	}
	Node newNode = {closestParent, NULL, NULL, RED, data};
	Node *node = (Node *) malloc(sizeof(Node));
	if (node == NULL)
	{
		return FAILURE;
	}
	*node = newNode;
	(tree->size)++;
	if (tree->root == NULL)
	{
		node->color = BLACK;
		tree->root = node;
		return SUCCESS;
	}
	int cmpRes = tree->compFunc(data, closestParent->data);
	Node **newNodePlace = FIRST_LESS(cmpRes) ? &(closestParent->left) : &(closestParent->right);
	*newNodePlace = node;
	fixTwoRed(node);
	return SUCCESS;
}

/**
 *
 * @param tree
 * @param data
 * @return
 */
int RBTreeContains(const RBTree *tree, const void *data)
{
	if (!tree || !data)
	{
		return FAILURE;
	}
	Node *closestParent = NULL;
	Node *dataNode = find(tree, data, &closestParent);
	return dataNode != NULL;
}

/**
 *
 * @param tree
 * @param data
 * @return
 */
int deleteFromRBTree(RBTree *tree, void *data)
{
	if (!tree || !data)
	{
		return FAILURE;
	}
	Node *dataNode = find(tree, data, NULL);
	if (dataNode == NULL)
	{
		return FAILURE;
	}
	Node *toDelete = NULL;
	if (dataNode->right != NULL && dataNode->left != NULL)
	{
		toDelete = subTreeMin(dataNode->right);
		void *dataToDelete = dataNode->data;
		dataNode->data = toDelete->data;
		toDelete->data = dataToDelete;
	}
	else
	{
		toDelete = dataNode;
	}
	tree->size--;
	return deleteNode(tree, toDelete);
}

/**
 * find in order successor (by cmp func) of node/
 * @param node
 * @return: The successor node
 */
static Node *successor(Node const *node)
{
	if (node->right)
	{
		return subTreeMin(node->right);
	}
	const Node *currNode = node;
	while (currNode && isRightChild(currNode))
	{
		currNode = currNode->parent;
	}
	Node *res = currNode ? currNode->parent : NULL;
	return res;
}

/**
 *
 * @param tree
 * @param func
 * @param args
 * @return
 */
int forEachRBTree(const RBTree *tree, forEachFunc func, void *args)
{
	if (tree == NULL)
	{
		return FAILURE;
	}
	Node *currNode = subTreeMin(tree->root);
	while (currNode)
	{
		int res = func(currNode->data, args);
		if (!res)
		{
			return FAILURE;
		}
		currNode = successor(currNode);

	}
	return SUCCESS;
}

/**
 * free the sub tree whose root is node
 * @param tree: the parent tree
 * @param node
 */
static void freeSubTree(RBTree *tree, Node *node)
{
	if (node->right)
	{
		freeSubTree(tree, node->right);
	}
	if (node->left)
	{
		freeSubTree(tree, node->left);
	}
	tree->freeFunc(node->data);
	free(node);
}

/**
 *
 * @param tree
 */
void freeRBTree(RBTree **tree)
{
	if ((*tree)->root)
	{
		freeSubTree(*tree, (*tree)->root);
	}
	free(*tree);
	*tree = NULL;
}
