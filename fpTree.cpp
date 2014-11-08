/*
argv[1] is the name of the transaction record file generated earlier using fileGenerrate.cpp
argv[2] is the number of attributes in a single transaction record

Scope of improvements
1. Sorting method should be heapsort or mergesort.
2. Dropping of records from the sorted list of items is not done. Just a modification to the last index is made.
   Useless data is still in memory
*/

#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<cmath>

using namespace std;

//structure declarations
typedef struct _node
{
	char label_id;
	int support_count;
	struct _node *parent;
	struct _node *child;
	struct _node *sibling;
	struct _node *next_node;
}node;

typedef struct _label
{
	char label_id;
	int support_count;
	node *list;
}label;

//Function declarations
void insert_node(int *,label *,int);	//inserts a node into the FP Tree
void initialize_attributes(label *,int); //initializes the sorted attribute list
void initialize_tree_head();	//initialize the FP-Tree head	
void bubblesort(label *,int);	//bubblesorts the attribute list
void FP_traversal(node *);	//traverses the FP Tree in post order format

node *tree_head=NULL;	//head of the FP-Tree

int main(int argc, char *argv[])
{
	ifstream dataFile;
	int attributes,min_support,limit_idx;
	label *attributeArray;
	int *transactionArray;
	string buffer;
	char *cbuffer;

	if(argc < 4)
	{
		cout<<"\nError! Please provide parameters in the following manner:";
		cout<<"\n<binary_file> <transaction_record_file> <attributes_of_a_transaction_record> <min_support_count>\n";
		return 0;
	}

	dataFile.open(argv[1],ios::in);

	attributes = atoi(argv[2]);
	min_support = atoi(argv[3]);
	limit_idx = attributes;
	
	attributeArray = new label[attributes];
	initialize_attributes(attributeArray,attributes);


	//reading database of transactions for the first time to create the sorted list of items -- attributeArray
	while(!dataFile.eof())
	{
		char *token;
		int idx=0;

		getline(dataFile,buffer);
		cbuffer = new char[buffer.length()+1];
		strcpy(cbuffer,buffer.c_str());
		buffer.clear();

		token = strtok(cbuffer,"	");
		while(token!=0)
		{
			attributeArray[idx].support_count += atoi(token); 
			token = strtok(NULL,"	");
			idx++;
		}
	}

	dataFile.close();	

	//bubblesort attributeArray
	bubblesort(attributeArray,attributes);

	//display attibuteArray
	cout<<"LIST OF ATTRIBUTES IN NON-INCREASING ORDER OF SUPPORT COUNT\n";
	for(int i=0;i<attributes;i++)
		cout<<attributeArray[i].label_id<<" "<<attributeArray[i].support_count<<"\n";
	
	//modifying the last index of attributeArray as per min_support count value 
	for(int i=0;i<attributes;i++)
	{
		if(attributeArray[i].support_count < min_support)
		{
			if(i == 0)
			{
				cout<<"\nNo transactions meeting minimum support count requirements!\n";
				return 0;
			}

			limit_idx = i;
			break;
		}
	}
	attributes = limit_idx;

	//display elements of attributeArray which satisfy minimum support count 
	cout<<"LIST OF ATTRIBUTES SATISFYING MINIMUM SUPPORT COUNT = "<<min_support<<"\n";
	for(int i=0;i<attributes;i++)
		cout<<attributeArray[i].label_id<<" "<<attributeArray[i].support_count<<"\n";

	initialize_tree_head();

	dataFile.open(argv[1],ios::in);
	
	//reading database of transaction records for the second time to insert records in the FP-tree
	while(!dataFile.eof())
	{
		char *token;
		int idx=0;

		getline(dataFile,buffer);
		cbuffer = new char[buffer.length()+1];
		strcpy(cbuffer,buffer.c_str());
		buffer.clear();

		transactionArray = new int[attributes];

		token = strtok(cbuffer,"	");
		while(token!=0)
		{
			transactionArray[idx] = atoi(token); 
			token = strtok(NULL,"	");
			idx++;
		}

		insert_node(transactionArray,attributeArray,attributes);
	}

	cout<<"POST ORDER TRAVERSAL OF FP-TREE (LABEL_ID:SUPPORT_COUNT)\n";

	FP_traversal(tree_head->child);

	cout<<"\n";

	cout<<"LIST OF EACH ITEM IN THE TREE WITH SUPPORT COUNTS\n";
	for(int i=0;i<attributes;i++)
	{
		node *temp_node = attributeArray[i].list;
		cout<<"LIST OF ITEM "<<attributeArray[i].label_id<<"-> ";
		while(temp_node!=NULL)
		{
			cout<<temp_node->label_id<<":"<<temp_node->support_count<<"\t";
			temp_node = temp_node->next_node;
		}
		cout<<"\n";
	}

	return 0;
}

void initialize_attributes(label *array,int rows)
{
	for(int i=0;i<rows;i++)
	{
		array[i].label_id = char(65+i);
		array[i].support_count = 0;
		array[i].list = NULL;
	}
}

void bubblesort(label *array,int attributes) //optimized  bubblesort with best time O(n)
{
	label temp;
	int swapped = 0;
	for(int i=0;i<attributes-1;i++)
	{
		swapped = 0;
		for(int j=0;j<attributes-i-1;j++)
		{
			if(array[j].support_count < array[j+1].support_count)
			{
				temp.label_id = array[j].label_id;
				temp.support_count = array[j].support_count;
				array[j].label_id = array[j+1].label_id;
				array[j].support_count = array[j+1].support_count;
				array[j+1].label_id = temp.label_id;
				array[j+1].support_count = temp.support_count;
				swapped = 1; 				
			}	
		}
		
		if(swapped == 0)
			break;
	}
}

void initialize_tree_head()
{
	tree_head = new node;
	tree_head->label_id = 'N';
	tree_head->support_count = 0;
	tree_head->parent = NULL;
	tree_head->child = NULL;
	tree_head->sibling = NULL;
	tree_head->next_node = NULL;
}

void insert_node(int *transactionArray,label *referenceList,int attributes) //takes a transaction record and inserts into FP-Tree
{
	node *temp_node=NULL,*prev_node=NULL,*current_node=NULL,*next=NULL;
	int flag_bit = 0;

	for(int i=0;i<attributes;i++)
	{
		flag_bit = 0;

		if(transactionArray[abs(int(referenceList[i].label_id) - 65)] == 1)
		{
			if(tree_head->child == NULL) //tree_head has no children
			{
				temp_node = new node;
				temp_node->label_id = referenceList[i].label_id;
				temp_node->support_count = 1;
				temp_node->parent = tree_head;
				temp_node->child = NULL;
				temp_node->sibling = NULL;
				tree_head->child = temp_node;
				current_node = temp_node;
				current_node->next_node = NULL;
				referenceList[i].list = current_node;
			}
			else if(current_node == NULL)	//tree_head has some children
			{
				//find a child that has the same label_id as the item being considered
				temp_node = tree_head->child;
				
				while(temp_node!=NULL)
				{
					prev_node = temp_node;
					if(temp_node->label_id == referenceList[i].label_id)
					{
						//do an insert -- here support_count++
						temp_node->support_count++;
						current_node = temp_node;
						flag_bit++;
					}
					temp_node = temp_node->sibling;
				}

				if(flag_bit == 0)	//no children with the same label_id as the item being considered
				{
					node *new_node;
					new_node = new node;
					new_node->label_id = referenceList[i].label_id;
					new_node->support_count = 1;
					new_node->parent = prev_node->parent;
					new_node->child = NULL;
					new_node->sibling = NULL;
					prev_node->sibling = new_node;
					current_node = new_node;

					referenceList[i].list = current_node;
				}
			}
			else	//in the middle of insertion of a transaction record
			{
				temp_node = current_node->child;
				if(temp_node == NULL)
				{
					temp_node = new node;
					temp_node->label_id = referenceList[i].label_id;
					temp_node->support_count = 1;
					temp_node->parent = current_node;
					temp_node->child = NULL;
					temp_node->sibling = NULL;
					current_node->child = temp_node;
					current_node = temp_node;

					next = referenceList[i].list;
					if(next!=NULL)
					{
						while(next->next_node!=NULL)
							next = next->next_node;

						next->next_node = current_node;
					}
					else
						referenceList[i].list = current_node;

					current_node->next_node = NULL;
				}
				else
				{
					while(temp_node!=NULL)
					{
						prev_node = temp_node;
						if(temp_node->label_id == referenceList[i].label_id)
						{
							temp_node->support_count++;
							current_node = temp_node;
							flag_bit++;
							break;
						}
						temp_node = temp_node->sibling;
					}

					if(flag_bit == 0)
					{
						node *new_node;
						new_node = new node;
						new_node->label_id = referenceList[i].label_id;
						new_node->support_count = 1;
						new_node->parent = prev_node->parent;
						new_node->child = NULL;
						new_node->sibling = NULL;
						prev_node->sibling = new_node;
						current_node = new_node;
						
						next = referenceList[i].list;
						if(next!=NULL)
						{
							while(next->next_node!=NULL)
								next = next->next_node;

							next->next_node = current_node;
						}
						else
							referenceList[i].list = current_node;

						current_node->next_node = NULL;
					}
				}
			}
		}
		else
			continue;
	}
}


//Post order traversal of FP-Tree. Code looks as though it performs inorder traversal but the structure of the tree causes this to be post order
void FP_traversal(node *object)
{
	if(object == NULL)
		return;

	FP_traversal(object->child);
	cout<<object->label_id<<":"<<object->support_count<<"\t";
	FP_traversal(object->sibling);
}
