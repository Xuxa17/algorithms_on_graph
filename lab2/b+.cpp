#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<climits>
#include <vector>
#include <Windows.h>
using namespace std;
const int MAX = 20; //size of each node

vector<char> punctuation = { '.', ',', '"', ';', ':', '!', '?', '@', '#', '%', '—'};//   '«', '»', '—'};
bool is_in_punct(char c) {
	bool flag = false;
	for (int i = 0; i < punctuation.size(); i++) {
		if (c == punctuation[i]) {
			flag = true;
			break;
		}
	}
	return flag;
}
class BPTree; //self explanatory classes
class Node//класс узла
{
	bool IS_LEAF;
	string* key;
	int size;
	Node** ptr;
	friend class BPTree;
public:
	Node();
};
class BPTree//класс дерева
{
	Node* root;
    
	void insertInternal(string, Node*, Node*);
	void removeInternal(string, Node*, Node*);
	Node* findParent(Node*, Node*);
public:
	BPTree();
	void search(string);
	void insert(string, int);
	void remove(string);
	void display(Node*);
	Node* getRoot();
	//void cleanUp(Node*);
	~BPTree();
	bool find(string x)
	{
		bool flag = 0;
		//search logic
		if (root == NULL)
		{
			//empty
			cout << "Tree empty\n";
		}
		else
		{
			Node* cursor = root;
			//in the following while loop, cursor will travel to the leaf node possibly consisting the key
			while (cursor->IS_LEAF == false)
			{
				for (int i = 0; i < cursor->size; i++)
				{
					if (x < cursor->key[i])
					{
						cursor = cursor->ptr[i];
						break;
					}
					if (i == cursor->size - 1)
					{
						cursor = cursor->ptr[i + 1];
						break;
					}
				}
			}
			//in the following for loop, we search for the key if it exists
			for (int i = 0; i < cursor->size; i++)
			{
				if (cursor->key[i] == x)
				{
					flag = 1;
				}
			}
			
		}
		return flag;
	}
    
    
	void read_from_file(const string& file) {
		ifstream from(file);
		string str;
		if (from.is_open()) {
			while (getline(from, str)) {
				istringstream iss(str);//считываем всю первую строку в iss, теперь проходясь по iss мы будем получать отдельные слова, слова считываются как отдельные через пробел, то есть слово add, будет как 4 буквы
				string word;
				while (iss >> word) {//то есть пока в iss(строке текста) есть слова
					word.erase(remove_if(word.begin(), word.end(), is_in_punct), word.end());//проходимся по всему слову и если там символ совпадет со знаком препинания, то переносим его в конец, а потом удаляем
					word[0] = tolower(word[0]);//делаем все буквы маленькими, ну а именно первую
					
					if (!find(word))
						insert(word, 1);
				}
			}
			//cout << "Файл считан" << endl;
		}
		from.close();
	}
};

void menu() {
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	setlocale(LC_ALL, "rus");
	BPTree bpt;//B+ tree object that carries out all the operations
	string command;
	string x;
	bool close = false;
	string logBuffer;//used to save into log
	ifstream fin;
	ofstream fout;
	cout << "Commands:\nsearch <value> to search\n";
	cout << "insert <value> to insert\n";
	cout << "delete <value> to delete\n";
	cout << "display to display\n";
	cout << "exit to exit\n";
	cout << "read 1 to read from file 1\n";
	cout << "read 2 to read from file 2\n";
	cout << "clear to clear tree\n";
	do
	{
		cout << "Enter command: ";
		getline(cin, command);
		if (!command.substr(0, 6).compare("search"))
		{
			stringstream argument(command.substr(7));
			argument >> x;
			bpt.search(x);
		}
		else if (!command.substr(0, 6).compare("insert"))
		{
			stringstream argument(command.substr(7));
			argument >> x;
			bpt.insert(x, 2);
			logBuffer.append(command);
			logBuffer.append("\n");
		}
		else if (!command.substr(0, 6).compare("delete"))
		{
			stringstream argument(command.substr(7));
			argument >> x;
			bpt.remove(x);
			logBuffer.append(command);
			logBuffer.append("\n");
		}
		else if (!command.compare("display"))
		{
			bpt.display(bpt.getRoot());
		}
		else if (!command.compare("read 1"))
		{
			bpt.read_from_file("D:/Xuxa_hash/text1.txt");
		}
		else if (!command.compare("read 2"))
		{
			bpt.read_from_file("D:/Xuxa_hash/text2.txt");
		}
		else if (!command.compare("exit"))
		{
			close = true;
		}
		else if (!command.compare("clear")) {
			BPTree bpt1;
			bpt = bpt1;
		}
		else
		{
			cout << "Invalid command\n";
		}
	} while (!close);
}
//give command line argument to load a tree from log
//to create a fresh tree, do not give any command line argument
int main(int argc, char* argv[])
{
	menu();
	return 0;
}

Node::Node()
{
	//dynamic memory allocation
	key = new string[MAX];
	ptr = new Node*[MAX + 1];
}
BPTree::BPTree()
{
	root = NULL;
}

void BPTree::search(string x)
{
	//search logic
	if (root == NULL)
	{
		//empty
		cout << "Tree empty\n";
	}
	else
	{
		Node* cursor = root;
		//in the following while loop, cursor will travel to the leaf node possibly consisting the key
		while (cursor->IS_LEAF == false)
		{
			for (int i = 0; i < cursor->size; i++)
			{
				if (x < cursor->key[i])
				{
					cursor = cursor->ptr[i];
					break;
				}
				if (i == cursor->size - 1)
				{
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}
		//in the following for loop, we search for the key if it exists
		for (int i = 0; i < cursor->size; i++)
		{
			if (cursor->key[i] == x)
			{
				cout << "Found\n";
				return;
			}
		}
		cout << "Not found\n";
	}
}



void BPTree::insert(string x, int t)
{
	//insert logic
	if (root == NULL)
	{
		root = new Node;
		root->key[0] = x;
		root->IS_LEAF = true;
		root->size = 1;
		if (t == 2) cout << "Created root\nInserted " << x << " successfully\n";
	}
	if (find(x) && (t == 2)) cout << "This word is already in dictionary" << endl;
	else
	{
		Node* cursor = root;
		Node* parent = nullptr;
		//in the following while loop, cursor will travel to the leaf node possibly consisting the key
		while (cursor->IS_LEAF == false)
		{
			parent = cursor;//пусть сначала родитель это корень
			for (int i = 0; i < cursor->size; i++)
			{
				if (x < cursor->key[i])
				{
					cursor = cursor->ptr[i];//теперь будем менять курсор, родителем всё так же является корень
					break;
				}
				if (i == cursor->size - 1)
				{
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}
		//now cursor is the leaf node in which we'll insert the new key
		if (cursor->size < MAX)
		{
			//if cursor is not full
			//find the correct position for new key
			int i = 0;
			while (x > cursor->key[i] && i < cursor->size) i++;
			//make space for new key
			for (int j = cursor->size; j > i; j--)
			{
				cursor->key[j] = cursor->key[j - 1];
			}
			cursor->key[i] = x;
			cursor->size++;
			cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
			cursor->ptr[cursor->size - 1] = NULL;
			if (t == 2) cout << "Inserted " << x << " successfully\n";
		}
		else
		{
			if (t == 2) cout << "Inserted " << x << " successfully\n";
			if (t == 2) cout << "Overflow in leaf node!\nSplitting leaf node\n";
			//overflow condition
			//create new leaf node
			Node* newLeaf = new Node;
			//create a virtual node and insert x into it
			string virtualNode[MAX + 1];
			for (int i = 0; i < MAX; i++)
			{
				virtualNode[i] = cursor->key[i];
			}
			int i = 0, j;
			while (x > virtualNode[i] && i < MAX) i++;
            
			//make space for new key
			for (int j = MAX; j > i; j--)
			{
				string c = virtualNode[j];
				string cc = virtualNode[j - 1];
				virtualNode[j] = virtualNode[j - 1];
			}
			virtualNode[i] = x;
			newLeaf->IS_LEAF = true;
			//split the cursor into two leaf nodes
			cursor->size = (MAX + 1) / 2;
			newLeaf->size = MAX + 1 - (MAX + 1) / 2;
			//make cursor point to new leaf node
			cursor->ptr[cursor->size] = newLeaf;
			//make new leaf node point to the next leaf node
			newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
			cursor->ptr[MAX] = NULL;
			//now give elements to new leaf nodes
			for (i = 0; i < cursor->size; i++)
			{
				cursor->key[i] = virtualNode[i];
			}
			for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++)
			{
				newLeaf->key[i] = virtualNode[j];
			}
			//modify the parent
			if (cursor == root)
			{
				//if cursor is a root node, we create a new root
				Node* newRoot = new Node;
				newRoot->key[0] = newLeaf->key[0];
				newRoot->ptr[0] = cursor;
				newRoot->ptr[1] = newLeaf;
				newRoot->IS_LEAF = false;
				newRoot->size = 1;
				root = newRoot;
				if (t == 2) cout << "Created new root\n";
			}
			else
			{
				//insert new key in parent node
				insertInternal(newLeaf->key[0], parent, newLeaf);
			}
		}
	}
}




void BPTree::insertInternal(string x, Node* cursor, Node* child)
{
	if (cursor->size < MAX)
	{
		//if cursor is not full
		//find the correct position for new key
		int i = 0;
		while (x > cursor->key[i] && i < cursor->size) i++;
		//make space for new key
		for (int j = cursor->size; j > i; j--)
		{
			cursor->key[j] = cursor->key[j - 1];
		}//make space for new pointer
		for (int j = cursor->size + 1; j > i + 1; j--)
		{
			cursor->ptr[j] = cursor->ptr[j - 1];
		}
		cursor->key[i] = x;
		cursor->size++;
		cursor->ptr[i + 1] = child;
		//cout << "Inserted key in an Internal node successfully\n";
	}
	else
	{
		//cout << "Inserted key in an Internal node successfully\n";
		//cout << "Overflow in internal node!\nSplitting internal node\n";
		//if overflow in internal node
		//create new internal node
		Node* newInternal = new Node;
		//create virtual Internal Node;
		string virtualKey[MAX + 1];
		Node* virtualPtr[MAX + 2];
		for (int i = 0; i < MAX; i++)
		{
			virtualKey[i] = cursor->key[i];
		}
		for (int i = 0; i < MAX + 1; i++)
		{
			virtualPtr[i] = cursor->ptr[i];
		}
		int i = 0, j;
		while (x > virtualKey[i] && i < MAX) i++;
		//make space for new key
		for (int j = MAX + 1; j > i; j--)
		{
			virtualKey[j] = virtualKey[j - 1];
		}
		virtualKey[i] = x;
		//make space for new ptr
		for (int j = MAX + 2; j > i + 1; j--)
		{
			virtualPtr[j] = virtualPtr[j - 1];
		}
		virtualPtr[i + 1] = child;
		newInternal->IS_LEAF = false;
		//split cursor into two nodes
		cursor->size = (MAX + 1) / 2;
		newInternal->size = MAX - (MAX + 1) / 2;
		//give elements and pointers to the new node
		for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++)
		{
			newInternal->key[i] = virtualKey[j];
		}
		for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++)
		{
			newInternal->ptr[i] = virtualPtr[j];
		}
		// m = cursor->key[cursor->size]
		if (cursor == root)
		{
			//if cursor is a root node, we create a new root
			Node* newRoot = new Node;
			newRoot->key[0] = cursor->key[cursor->size];
			newRoot->ptr[0] = cursor;
			newRoot->ptr[1] = newInternal;
			newRoot->IS_LEAF = false;
			newRoot->size = 1;
			root = newRoot;
			//cout << "Created new root\n";
		}
		else
		{
			//recursion
			//find depth first search to find parent of cursor
			insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
		}
	}
}
Node* BPTree::findParent(Node* cursor, Node* child)
{
	//finds parent using depth first traversal and ignores leaf nodes as they cannot be parents
	//also ignores second last level because we will never find parent of a leaf node during insertion using this function
	Node* parent = nullptr;
	if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF)
	{
		return NULL;
	}
	for (int i = 0; i < cursor->size + 1; i++)
	{
		if (cursor->ptr[i] == child)
		{
			parent = cursor;
			return parent;
		}
		else
		{
			parent = findParent(cursor->ptr[i], child);
			if (parent != NULL)return parent;
		}
	}
	return parent;
}







void BPTree::remove(string x)
{
	//delete logic
	if (root == NULL)
	{
		cout << "Tree empty\n";
	}
	else
	{
		Node* cursor = root;
		Node* parent = nullptr;
		int leftSibling, rightSibling;
		//in the following while loop, cursor will will travel to the leaf node possibly consisting the key
		while (cursor->IS_LEAF == false)
		{
			for (int i = 0; i < cursor->size; i++)
			{
				parent = cursor;
				leftSibling = i - 1; //leftSibling is the index of left sibling in the parent node
				rightSibling = i + 1; //rightSibling is the index of right sibling in the parent node
				if (x < cursor->key[i])
				{
					cursor = cursor->ptr[i];
					break;
				}
				if (i == cursor->size - 1)
				{
					leftSibling = i;
					rightSibling = i + 2;
					cursor = cursor->ptr[i + 1];
					break;
				}
			}
		}
		//in the following for loop, we search for the key if it exists
		bool found = false;
		int pos;
		for (pos = 0; pos < cursor->size; pos++)
		{
			if (cursor->key[pos] == x)
			{
				found = true;
				break;
			}
		}
		if (!found)//if key does not exist in that leaf node
		{
			cout << "Not found\n";
			return;
		}
		//deleting the key
		for (int i = pos; i < cursor->size; i++)
		{
			cursor->key[i] = cursor->key[i + 1];
		}
		cursor->size--;
		if (cursor == root)//if it is root node, then make all pointers NULL
		{
			cout << "Deleted " << x << " from leaf node successfully\n";
			for (int i = 0; i < MAX + 1; i++)
			{
				cursor->ptr[i] = NULL;
			}
			if (cursor->size == 0)//if all keys are deleted
			{
				cout << "Tree died\n";
				delete[] cursor->key;
				delete[] cursor->ptr;
				delete cursor;
				root = NULL;
			}
			return;
		}
		cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];
		cursor->ptr[cursor->size + 1] = NULL;
		cout << "Deleted " << x << " from leaf node successfully\n";
		if (cursor->size >= (MAX + 1) / 2)//no underflow
		{
			return;
		}
		cout << "Underflow in leaf node!\n";
		//underflow condition
		//first we try to transfer a key from sibling node
		//check if left sibling exists
		if (leftSibling >= 0)
		{
			Node* leftNode = parent->ptr[leftSibling];
			//check if it is possible to transfer
			if (leftNode->size >= (MAX + 1) / 2 + 1)
			{
				//make space for transfer
				for (int i = cursor->size; i > 0; i--)
				{
					cursor->key[i] = cursor->key[i - 1];
				}
				//shift pointer to next leaf
				cursor->size++;
				cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
				cursor->ptr[cursor->size - 1] = NULL;
				//transfer
				cursor->key[0] = leftNode->key[leftNode->size - 1];
				//shift pointer of leftsibling
				leftNode->size--;
				leftNode->ptr[leftNode->size] = cursor;
				leftNode->ptr[leftNode->size + 1] = NULL;
				//update parent
				parent->key[leftSibling] = cursor->key[0];
				cout << "Transferred " << cursor->key[0] << " from left sibling of leaf node\n";
				return;
			}
		}
		if (rightSibling <= parent->size)//check if right sibling exist
		{
			Node* rightNode = parent->ptr[rightSibling];
			//check if it is possible to transfer
			if (rightNode->size >= (MAX + 1) / 2 + 1)
			{
				//shift pointer to next leaf
				cursor->size++;
				cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
				cursor->ptr[cursor->size - 1] = NULL;
				//transfer
				cursor->key[cursor->size - 1] = rightNode->key[0];
				//shift pointer of rightsibling
				rightNode->size--;
				rightNode->ptr[rightNode->size] = rightNode->ptr[rightNode->size + 1];
				rightNode->ptr[rightNode->size + 1] = NULL;
				//shift conent of right sibling
				for (int i = 0; i < rightNode->size; i++)
				{
					rightNode->key[i] = rightNode->key[i + 1];
				}
				//update parent
				parent->key[rightSibling - 1] = rightNode->key[0];
				cout << "Transferred " << cursor->key[cursor->size - 1] << " from right sibling of leaf node\n";
				return;
			}
		}
		//must merge and delete a node
		if (leftSibling >= 0)//if left sibling exist
		{
			Node* leftNode = parent->ptr[leftSibling];
			// transfer all keys to leftsibling and then transfer pointer to next leaf node
			for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++)
			{
				leftNode->key[i] = cursor->key[j];
			}
			leftNode->ptr[leftNode->size] = NULL;
			leftNode->size += cursor->size;
			leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];
			cout << "Merging two leaf nodes\n";
			removeInternal(parent->key[leftSibling], parent, cursor);// delete parent node key
			delete[] cursor->key;
			delete[] cursor->ptr;
			delete cursor;
		}
		else if (rightSibling <= parent->size)//if right sibling exist
		{
			Node* rightNode = parent->ptr[rightSibling];
			// transfer all keys to cursor and then transfer pointer to next leaf node
			for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++)
			{
				cursor->key[i] = rightNode->key[j];
			}
			cursor->ptr[cursor->size] = NULL;
			cursor->size += rightNode->size;
			cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];
			cout << "Merging two leaf nodes\n";
			removeInternal(parent->key[rightSibling - 1], parent, rightNode);// delete parent node key
			delete[] rightNode->key;
			delete[] rightNode->ptr;
			delete rightNode;
		}
	}
}

void BPTree::removeInternal(string x, Node* cursor, Node* child)
{
	//deleting the key x first
	//checking if key from root is to be deleted
	if (cursor == root)
	{
		if (cursor->size == 1)//if only one key is left, change root
		{
			if (cursor->ptr[1] == child)
			{
				delete[] child->key;
				delete[] child->ptr;
				delete child;
				root = cursor->ptr[0];
				delete[] cursor->key;
				delete[] cursor->ptr;
				delete cursor;
				cout << "Changed root node\n";
				return;
			}
			else if (cursor->ptr[0] == child)
			{
				delete[] child->key;
				delete[] child->ptr;
				delete child;
				root = cursor->ptr[1];
				delete[] cursor->key;
				delete[] cursor->ptr;
				delete cursor;
				cout << "Changed root node\n";
				return;
			}
		}
	}
	int pos;
	for (pos = 0; pos < cursor->size; pos++)
	{
		if (cursor->key[pos] == x)
		{
			break;
		}
	}
	for (int i = pos; i < cursor->size; i++)
	{
		cursor->key[i] = cursor->key[i + 1];
	}
	//now deleting the pointer child
	for (pos = 0; pos < cursor->size + 1; pos++)
	{
		if (cursor->ptr[pos] == child)
		{
			break;
		}
	}
	for (int i = pos; i < cursor->size + 1; i++)
	{
		cursor->ptr[i] = cursor->ptr[i + 1];
	}
	cursor->size--;
	if (cursor->size >= (MAX + 1) / 2 - 1)//no underflow
	{
		cout << "Deleted " << x << " from internal node successfully\n";
		return;
	}
	cout << "Underflow in internal node!\n";
	//underflow, try to transfer first
	if (cursor == root)return;
	Node* parent = findParent(root, cursor);
	int leftSibling, rightSibling;
	//finding left n right sibling of cursor
	for (pos = 0; pos < parent->size + 1; pos++)
	{
		if (parent->ptr[pos] == cursor)
		{
			leftSibling = pos - 1;
			rightSibling = pos + 1;
			break;
		}
	}
	//try to transfer
	if (leftSibling >= 0)//if left sibling exists
	{
		Node* leftNode = parent->ptr[leftSibling];
		//check if it is possible to transfer
		if (leftNode->size >= (MAX + 1) / 2)
		{
			//make space for transfer of key
			for (int i = cursor->size; i > 0; i--)
			{
				cursor->key[i] = cursor->key[i - 1];
			}
			//transfer key from left sibling through parent
			cursor->key[0] = parent->key[leftSibling];
			parent->key[leftSibling] = leftNode->key[leftNode->size - 1];
			//transfer last pointer from leftnode to cursor
			//make space for transfer of ptr
			for (int i = cursor->size + 1; i > 0; i--)
			{
				cursor->ptr[i] = cursor->ptr[i - 1];
			}
			//transfer ptr
			cursor->ptr[0] = leftNode->ptr[leftNode->size];
			cursor->size++;
			leftNode->size--;
			cout << "Transferred " << cursor->key[0] << " from left sibling of internal node\n";
			return;
		}
	}
	if (rightSibling <= parent->size)//check if right sibling exist
	{
		Node* rightNode = parent->ptr[rightSibling];
		//check if it is possible to transfer
		if (rightNode->size >= (MAX + 1) / 2)
		{
			//transfer key from right sibling through parent
			cursor->key[cursor->size] = parent->key[pos];
			parent->key[pos] = rightNode->key[0];
			for (int i = 0; i < rightNode->size - 1; i++)
			{
				rightNode->key[i] = rightNode->key[i + 1];
			}
			//transfer first pointer from rightnode to cursor
			//transfer ptr
			cursor->ptr[cursor->size + 1] = rightNode->ptr[0];
			for (int i = 0; i < rightNode->size; ++i)
			{
				rightNode->ptr[i] = rightNode->ptr[i + 1];
			}
			cursor->size++;
			rightNode->size--;
			cout << "Transferred " << cursor->key[0] << " from right sibling of internal node\n";
			return;
		}
	}
	//transfer wasnt posssible hence do merging
	if (leftSibling >= 0)
	{
		//leftnode + parent key + cursor
		Node* leftNode = parent->ptr[leftSibling];
		leftNode->key[leftNode->size] = parent->key[leftSibling];
		for (int i = leftNode->size + 1, j = 0; j < cursor->size; j++)
		{
			leftNode->key[i] = cursor->key[j];
		}
		for (int i = leftNode->size + 1, j = 0; j < cursor->size + 1; j++)
		{
			leftNode->ptr[i] = cursor->ptr[j];
			cursor->ptr[j] = NULL;
		}
		leftNode->size += cursor->size + 1;
		cursor->size = 0;
		//delete cursor
		removeInternal(parent->key[leftSibling], parent, cursor);
		cout << "Merged with left sibling\n";

	}
	else if (rightSibling <= parent->size)
	{
		//cursor + parent key + rightnode
		Node* rightNode = parent->ptr[rightSibling];
		cursor->key[cursor->size] = parent->key[rightSibling - 1];
		for (int i = cursor->size + 1, j = 0; j < rightNode->size; j++)
		{
			cursor->key[i] = rightNode->key[j];
		}
		for (int i = cursor->size + 1, j = 0; j < rightNode->size + 1; j++)
		{
			cursor->ptr[i] = rightNode->ptr[j];
			rightNode->ptr[j] = NULL;
		}
		cursor->size += rightNode->size + 1;
		rightNode->size = 0;
		//delete cursor
		removeInternal(parent->key[rightSibling - 1], parent, rightNode);
		cout << "Merged with right sibling\n";
	}
}






void BPTree::display(Node* cursor)
{
	//depth first display
	if (cursor != NULL)
	{
		if (cursor->IS_LEAF) cout << "Leaf: ";
		for (int i = 0; i < cursor->size; i++)
		{
			cout << cursor->key[i] << " ";
		}
		cout << "\n";
		if (cursor->IS_LEAF != true)
		{
			for (int i = 0; i < cursor->size + 1; i++)
			{
				display(cursor->ptr[i]);
			}
		}
	}
}





Node* BPTree::getRoot()
{
	return root;
}
void BPTree::cleanUp(Node* cursor)
{
	//clean up logic
	if (cursor != NULL)
	{
		if (cursor->IS_LEAF != true)
		{
			for (int i = 0; i < cursor->size + 1; i++)
			{
				cleanUp(cursor->ptr[i]);
			}
		}
		for (int i = 0; i < cursor->size; i++)
		{
			cout << "Deleted key from memory: " << cursor->key[i] << "\n";
		}
		delete[] cursor->key;
		delete[] cursor->ptr;
		delete cursor;
	}
}
BPTree::~BPTree()
{
	//calling cleanUp routine
	cleanUp(root);
}

