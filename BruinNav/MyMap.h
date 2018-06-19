#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap() {
		m_size = 0;
		root = nullptr;
	}

	~MyMap() {
		clear();
	}

	void clear() {
		deleteTree(root);
	}

	int size() const {
		return m_size;
	}

	void associate(const KeyType& key, const ValueType& value) {
		if (root == nullptr) {										// First Node
			root = new Node;
			root->n_key = key;
			root->n_value = value;
			root->right = nullptr;
			root->left = nullptr;
			m_size++;
			return;
		}
		Node* NodePtr = root;										// iterator to iterate through Nodes

		if (find(key) != nullptr) {									// if there already exists this key
			*find(key) = value;
			return;
		}

		for (;;)													// add the NODE
		{
			if (key < NodePtr->n_key) {
				if (NodePtr->left == nullptr) {
					NodePtr->left = new Node;
					NodePtr->left->n_key = key;
					NodePtr->left->n_value = value;
					NodePtr->left->right = nullptr;
					NodePtr->left->left = nullptr;
					m_size++;
					return;
				}
				NodePtr = NodePtr->left;
			}
			else {
				if (NodePtr->right == nullptr) {
					NodePtr->right = new Node;
					NodePtr->right->n_key = key;
					NodePtr->right->n_value = value;
					NodePtr->right->right = nullptr;
					NodePtr->right->left = nullptr;
					m_size++;
					return;
				}
				NodePtr = NodePtr->right;
			}
		}
		// it will never get here
	}

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const {
		Node* NodePtr = root;
		ValueType* temp = nullptr;
		for (;;)
		{
			if (NodePtr == nullptr)
				return temp;
			if (key == NodePtr->n_key) {
				temp = &(NodePtr->n_value);
				return temp;
			}

			else if (key < NodePtr->n_key) {
				if (NodePtr->left == nullptr) {
					return temp;
				}
				NodePtr = NodePtr->left;
			}

			else {
				if (NodePtr->right == nullptr) {
					return temp;
				}
				NodePtr = NodePtr->right;
			}
		}						// end of for loop

	}

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	int m_size;
	struct Node {
		KeyType n_key;
		ValueType n_value;
		Node* left = nullptr;
		Node* right = nullptr;
	};
	Node* root;
	void deleteTree(Node* del) {
		if (del == nullptr)
			return;
		deleteTree(del->left);
		deleteTree(del->right);
		delete del;
	}
};
