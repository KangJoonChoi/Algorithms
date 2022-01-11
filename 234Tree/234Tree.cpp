#include<stdio.h>
#include<stdlib.h>
#include<string>

typedef struct node Node;
Node* makenode(void);
Node* insert(Node* node, int key);
void leafinsert(Node* node, int key);
Node* split(node* node, int splitnum);
bool search(Node* node, int searchvalue);
void addtoqueue(Node* node);
Node* getfromqueue(void);
void printer(Node* root, FILE* file);
Node* deletion(Node* node, int key, int linknum);
Node* transfer(Node* node, int link);
Node* findpredecessornode(Node* node);
void fusion(Node* parentnode, int nodewithelement, int nodewithoutelement);

struct node
{
	int element[4];//�����ڷ�� ������ insertion�� �����ϱ� ���� ���� element1�� �ʿ�
	Node* link[5];//�����ڷ�� ������ insertion�����ϱ� ���� ���� link�ʿ�(�ڽ��ּ�)
	int celementnum;//���� node�� ����ִ� element�� ��
	bool leaf;//leaf������� �ƴ��� �Ǻ�
	int depth;//����� ���� ���
};

Node* queue[10000];//Print�ϱ� ���� ������ �迭(�ִ� 10000������ ���� ����)
//�޸𸮸� �Ƴ��� ���� ������ �� ���� �������� ��� �����ٶ�
int inlist = 0; //queue�� ��� �����Ͱ� ���Ծ����� �˷���
int nodetoprint = 0; //���° �迭�� �������

int main(void)
{
	FILE* input = fopen("input.txt", "r");
	if (input == NULL)
	{
		printf("File Open Error");
	}
	Node* root = makenode();
	FILE* output = fopen("output.txt", "w");
	while (1)
	{
		char string[15] = { 0 }; //15character �̻� �ް���� ��� �����ϸ� �ȴ�. �޸� ���񶧹��� 15�� �����Ͽ���.
		if (feof(input))break;//���� �� �������κ��̸� break�Ѵ�.
		fscanf(input, "%s", string);
		fprintf(output, "%s\n", string);
		char order = string[0];
		int value = 0;
		for (int i = 1; i <= strlen(string) - 1; i++)
		{
			value += string[i] - 48;
			value *= 10;
		}
		value /= 10;
		Node* newroot = makenode();
		newroot->leaf = false;
		newroot->link[0] = root;
		/// function���ο��� �� root��带 ����ԵǸ� ����������
		/// free���� �ʾҴµ��� ���� ������ ������ �߰��Ͽ���
		/// �׷��⿡ �ܺο��� ���ο� root��带 ����� split�Լ��� ����
		/// ���ڸ� �����Ϸ� �Ѵ�.
		if (order == 'i')//insert
		{
			insert(root, value);
			if (root->celementnum == 4)
			{
				split(newroot, 0); //�̹� child�� root���� ����� ���ұ⿡ 0���� split�Ѵ�
				root = newroot;//���ο� root���� �������ش�.
			}
			printer(root, output);
			fprintf(output, "\n\n");
		}
		if (order == 'd')//deletion
		{
			root = deletion(newroot, value, 0);
			if (root->link[0] == NULL || root == NULL) //�����غ� �ϴ� �߰��� ���� ���ۿ��� ������ �������� ��� �ǵ������δ� root.link[0]�̾���� �Ѵ�
			{
				root = makenode();//root�� ����Ű�� �Լ��� while�ٱ��� �����Ƿ� ���� delete�� ���ο� insert operation��
				//���ؼ� ����� �Ѵ�.
			}
			else if (root->element[0] == -1 && root->link[0])//root�� fusion���� ���Ͽ� ����� ���
			{
			root = root->link[0];
			}
			printer(root, output);
			fprintf(output, "\n\n");
		}
		if (order == 's')//search
		{
			bool exist = search(root, value);
			if (exist == 1)
			{
				fprintf(output, "exist\n\n");
			}
			else
			{
				fprintf(output, "not exist\n\n");
			}
		}
		
	}
	fclose(input);
	fclose(output);
}


Node* makenode(void) //���ο� node�� ���鶧���� �ʱ�ȭ ������ �߿��ϴ�
{
	Node* newnode;
	newnode = (Node*)malloc(sizeof(Node));//
	newnode->celementnum = 0;
	newnode->leaf = 1;
	newnode->element[0] = -1; //-1�� �ʱ�ȭ
	newnode->element[1] = -1;
	newnode->element[2] = -1;
	newnode->element[3] = -1;
	newnode->link[0] = NULL; //������ �ּҰ��� ����Ű�� �ȵǴ� NULL�� �ڽ��ּ� �ʱ�ȭ
	newnode->link[1] = NULL;
	newnode->link[2] = NULL;
	newnode->link[3] = NULL;
	newnode->link[4] = NULL;
	return newnode;
}
Node* insert(Node* node, int key)
{
	Node* determine;
	if (node->leaf == 0)//leaf �ƴҶ�
	{
		int count = 0;
		while (node->element[count] != -1 && (key >= node->element[count]) && count <= 2)
		{
			count++;
		}//while���� ������ �� node�� element�� ���Ͽ� �������� key value�� ��Ұ���, �� ��ġ�� ǥ���� �� �ִ�.
		determine = insert(node->link[count], key);
		if (determine->celementnum == 4) //insert�� ������ �� split���� �Ǵ�
		{
			return split(node, count);//�ش� count�� ����־ split�ؾ��ϴ� ���̱⿡
		}
		else return determine;//split���ϸ� �� �״�� ���� insert�� ����
	}
	else//leaf�� ��(�� �ܰ迡�� insert�� ���־�� �Ѵ�)
	{
		leafinsert(node, key);
		return node;
	}
}

void leafinsert(Node* node, int key)
{
	int elenum = node->celementnum;
	int insertcount = elenum - 1;//�迭�� ���� counting�� ���־ ������ -1
	node->celementnum++; //���� �����ϴ� 1�� �����ش�.
	while (node->element[insertcount] > key && insertcount >= 0) //elementary sorting���� key�� insert�Ѵ�
	{
		node->element[insertcount + 1] = node->element[insertcount];
		insertcount--;
	}
	node->element[insertcount + 1] = key;
}

Node* split(node* node, int splitnum) 
{
	Node* lchild = makenode();//split �� ���� child
	Node* rchild = makenode();//split �� ������ child
	int elenum = node->celementnum;
	int insertcount = elenum - 1;//leafinsert�� ���� �ƶ��̴�
	int comparenumber = (node->link[splitnum])->element[2];
	while (node->element[insertcount] > comparenumber && insertcount >= 0) //child�� 3��° Ű���� ������ �ø���.
	{
		node->element[insertcount + 1] = node->element[insertcount];
		node->link[insertcount + 2] = node->link[insertcount + 1];//������ �ִ� ��ũ�� �Űܾ��Ѵ�
		insertcount--;
	}
	lchild = node->link[splitnum];
	node->element[insertcount + 1] = (lchild)->element[2];//3��° key�� node�� �������ش�.
	node->link[insertcount + 2] = rchild;//���� child �Ҵ�
	node->celementnum++;
	rchild->leaf = lchild->leaf;  //���� ���� node���� ���� node�� ��������.
	rchild->element[0] = lchild->element[3]; //�� ���� key����
	lchild->element[2] = -1;//reset
	lchild->element[3] = -1;
	rchild->link[0] = lchild->link[3];
	lchild->link[3] = NULL;
	rchild->link[1] = lchild->link[4];
	lchild->link[4] = NULL;
	rchild->celementnum = 1;
	lchild->celementnum = 2;
	return node;//split�Ϸ�� node�ּ� ����
}
bool search(Node* node, int searchvalue)
{
	int counter = 0;
	int destination = 0;
	bool match = 0;
	while (counter < node->celementnum)
	{
		if(searchvalue > node->element[counter])counter++;
		else if (searchvalue == node->element[counter]) {
			match = 1;
			return match; //match�� �Ǹ� bool���� return�Ѵ�
		}
		else if (searchvalue < node->element[counter]) break;
	}
	if ((node->leaf) != 1) //leaf�� �ƴѰ�� recursive�ϰ� child���� ��������
	{
		match = search(node->link[counter], searchvalue);
	}

	return match; //leaf�ε��� match�� 0�̸� return���ش�
}
void addtoqueue(Node* node)
{
	queue[inlist++] = node; //node�� queue�� ����
}
Node* getfromqueue(void)
{
	if (queue[nodetoprint] != NULL)//���� �ִ� ��쿡 ���� ���� �÷��ְ�
	{
		return queue[nodetoprint++];
	}
	else return queue[nodetoprint];// ������ ����(������ ����� ����)���� �״�� �����Ѵ�
}
void printer(Node* root, FILE* file)
{
	addtoqueue(root); // root�� ���� ó���� ����
	root->depth = 0; //depth�ʱ�ȭ child�� ���� 1�� depth�� �þ��
	while(1)
	{
		root = getfromqueue();
		if (root) //queue���� return�� ���� ������
		{
			if (root->depth == 0) //root�϶� ���
			{
				if (root->element[0] >= 0)
				{
					fprintf(file, "(");
					if (root->element[0] != -1) fprintf(file, "%d", root->element[0]);
					if (root->element[1] != -1) fprintf(file, " %d", root->element[1]);
					if (root->element[2] != -1) fprintf(file, " %d", root->element[2]);
					fprintf(file, ")");
				}
			}
			else if (root->depth == queue[nodetoprint - 2]->depth) //������ �ִ� node�� depth ��Ұ� ������ ���⸸ �Ѵ�. depth0�� ���� �� ������ �޸� �������� ���ֱ� ���ؼ��̴�
			{
				if (root->element[0] >= 0)
				{
					fprintf(file, "(");
					if (root->element[0] != -1) fprintf(file, "%d", root->element[0]);
					if (root->element[1] != -1) fprintf(file, " %d", root->element[1]);
					if (root->element[2] != -1) fprintf(file, " %d", root->element[2]);
					fprintf(file, ") ");
				}
			}
			else//�����Ѵ�
			{
				if (root->element[0] >= 0)
				{
					fprintf(file, "\n(");
					if (root->element[0] != -1) fprintf(file, "%d", root->element[0]);
					if (root->element[1] != -1) fprintf(file, " %d", root->element[1]);
					if (root->element[2] != -1) fprintf(file, " %d", root->element[2]);
					fprintf(file, ") ");
				}
			}
			if (root->link[0] != NULL) {
				int depth = root->depth; //
				depth++;
				(root->link[0])->depth = depth; //1������ depth�� child�� depth�� �����Ѵ�
				addtoqueue(root->link[0]);
			}
			if (root->link[1] != NULL) {
				(root->link[1])->depth = ((root->link[0])->depth);
				addtoqueue(root->link[1]);
			}
			if (root->link[2] != NULL) {
				(root->link[2])->depth = (root->link[0])->depth;
				addtoqueue(root->link[2]);
			}
			if (root->link[3] != NULL) {
				(root->link[3])->depth = (root->link[0])->depth;
				addtoqueue(root->link[3]);
			}
			
		}
		else break;
	}
}

Node* deletion(Node* node, int key, int link) //parent node�� ���� �� 
{
	Node* child = node->link[link]; //child��� �������� ������ ���� �Ѵ�
	int count = 0;
	while (child->element[count] != -1 && (key > child->element[count]) && count <= 2)//�������� �ٸ��� key�� childelement���� ������ count++�� �����ش�
	{
		count++;
	}//while���� ������ �� childnode�� element�� ���Ͽ� �������� key value�� ��Ұ���, �� ��ġ�� ǥ���� �� �ִ�.
	if (child->element[count] == key) //��Ұ��踦 ���� delete�� ���� �߰��Ѱ��
	{
		if (child->leaf)//target�� leaf�� �����ϸ� �׳� ����
		{
			child->element[count] = -1;
			int temp;
			while (child->celementnum - 1 > count)
			{
				temp = child->element[count];
				child->element[count] = child->element[count + 1];
				child->element[count + 1] = temp;
				count++;
			}
			child->celementnum--;
			if (child->celementnum == 0)
			{
				return transfer(node, link);//childnode ���� ���ԵǸ� transfer�� �ʿ��� ��� �����ڷ�� �����ִ� ���� ���� transfer
				//�� �ǹ̰� �ƴ� transer�� merge�� ������ �Լ��̴�.
			}
			return node;
		}
		else//�� ��� target���� ã������ leaf�� �������� �ʴ� ����̴�. �����ڷῡ���� algorithm�� ������ڸ� ith child�� rightmost node
		//�� predecessor�� ã�Ƽ� �ٲ��־�� �Ѵ�
		{
			Node* predecessor = findpredecessornode(child->link[count]);
			key = (predecessor->element)[predecessor->celementnum - 1];
			child->element[count] = key;
			deletion(child, key, count);
		}
	}
	else if (child->leaf) return NULL; //leaf���� �����ߴµ��� �ұ��ϰ� ���� if�� ������� ���ߴٸ� delete�� ���� ���°��̴� �� ���� ������ ���� �ٺ��� �׻� ���� �־���Ѵ�
	//�ȱ׷��� ending condition������ ���� ���ϱ� �����̴� //���� �غ��ϴ� �߰��� ���ε�, ��� NULL���� ���������� �ȵƴ�. �����Ұ�� ���ο� ��带 �������� ���α׷��� ������ �������� �����̴�
	//���ǽ������� �׳� return 0�� ����� �������� �� ���� �����ַ� �ߴ�.
	else deletion(child, key, count);//recursive�ϰ� child node�� parent node�� ���� �� �ֵ��� �Լ��� ȣ���Ѵ�
	if (child->celementnum == 0)
	{
		transfer(node, link); // childnode ���� ���ԵǸ� transfer����
	}
	return child;
}

Node* findpredecessornode(Node* node)
{
	if (node->leaf)
	{
		return node;
	}
	return findpredecessornode(node->link[node->celementnum]);//���� ������ �ִ� ���� �������� �Ѵ�
}

Node* transfer(Node* node, int link)//parent���� �޾Ҵ�(link������ child node�� ������)
{//child�� �ƹ� ���� ���ٴ� ���� �Ͽ��� �̷������ �����̴�
	Node* destination = node->link[link];//transfer�� �޾����� ����ִ� child
	Node* source;//�� transfer�� ������ �� �ֵ��� key���� �������ִ� node
	if (link == 0 && node->celementnum != 0)//���� ���� node�� ���� ���� ��Ȳ
	{
		source = node->link[1];
		destination->element[0] = node->element[0];
		destination->celementnum++;
		if (source->celementnum == 1)
		{
			fusion(node, link, link + 1); //source�� element�� �ϳ��ۿ� ������� �� ��带 ���Ѵ�
			return node;//��ĥ �ڷᰡ �ִ� child�� link�� ��������, �ڷᰡ ����link�� ���� input���� �����Ѵ�
		}
		else
		{
			node->element[0] = source->element[0];//source ���� ����� ��� �θ𿡰� element 1�� �ο��ϰ�
			destination->link[1] = source->link[0];//�� key���� ���� key���� ������ �ִ� node�� ����Ű�� link�� �������� �ο����ش�.
			source->celementnum--;
			int elenum = 0;
			while (elenum < source->celementnum)//���ڸ� ä���
			{
				source->link[elenum] = source->link[elenum + 1];
				source->element[elenum] = source->element[elenum + 1];
				elenum++;
			}
			source->link[elenum] = NULL;
			source->element[elenum] = -1;//�ʱ�ȭ
			return node;
		} //����� �۾��� source�ϴ� node�� �ٲپ� ���� ���� node������ ������� �ָ� �ȴ�
	}
	else if (link == node->celementnum && node->celementnum != 0) //���� ���� node�� ���� ���� ��Ȳ
	{
		source = node->link[link - 1];
		destination->element[0] = node->element[link - 1];//���� ������ �ִ� �� ����
		destination->celementnum++;
		if (source->celementnum == 1)
		{
			fusion(node, link, link - 1); //source�� node�� ���� ����������� fusion�۾��� �ʿ��ϴ�
			return node;//��ĥ �ڷᰡ �ִ� child�� link�� ��������, �ڷᰡ ����link�� ���� input���� �����Ѵ�
		}
		else
		{
			node->element[link - 1] = source->element[source->celementnum - 1];
			destination->link[1] = destination->link[0];//
			destination->link[0] = source->link[source->celementnum]; //���� �θ�node ���� source�� �� ������ child�� ����Ű�� node�� ������ ������ 
			source->element[source->celementnum - 1] = -1;//�ʱ�ȭ
			source->celementnum--;//link�� �������� �ο����ش�
			/* �������� �������°� �ƴϹǷ� �Ʒ��� �۾��� �ʿ����.
			while (elenum < source->celementnum)//���ڸ� ä���
			{
				source->link[elenum] = source->link[elenum + 1];
				source->element[elenum] = source->element[elenum + 1];
				elenum++;
			}
			return node;
			*/
			return node;
		}
	}
	else if (node->celementnum != 0) //��/���� ��带 ������ �� �����ٵ�, ������ ���̴� �� ó�� ������带 source�� �ϴ� ���� �񱳿��� ���ؼ� �ո����̴�.
	{//��, �찡 �� �� 2node�� ��� ������� ������ �켱���� �Ѵ�. 
		if ((node->link[link + 1])->celementnum == 1 || ((node->link[link - 1])->celementnum) > 1)//�� ��� ������ �ʿ�� ���� �ʴ��̻� ������ ���������� �����ϰ� �ȴ�
		{
			source = node->link[link - 1];
			destination->element[0] = node->element[link - 1]; //�׳� link�� �ϸ� ������ �ִ� node�� ������ �ϰԵǴ� ���ǰ� �ʿ��ϴ�
			destination->celementnum++;
			if (source->celementnum == 1)
			{
				fusion(node, link, link - 1); //source�� node�� ���� ����������� fusion�۾��� �ʿ��ϴ�
				return node;//��ĥ �ڷᰡ �ִ� child�� link�� ��������, �ڷᰡ ����link�� ���� input���� �����Ѵ�
			}
			else
			{
				node->element[link - 1] = source->element[source->celementnum - 1];
				destination->link[1] = destination->link[0];//
				destination->link[0] = source->link[source->celementnum]; //���� �θ�node ���� source�� �� ������ child�� ����Ű�� node�� ������ ������
				source->element[source->celementnum - 1] = -1;//�ʱ�ȭ
				source->celementnum--;//link�� �������� �ο����ش�
				/* �������� �������°� �ƴϹǷ� �Ʒ��� �۾��� �ʿ����.
				while (elenum < source->celementnum)//���ڸ� ä���
				{
					source->link[elenum] = source->link[elenum + 1];
					source->element[elenum] = source->element[elenum + 1];
					elenum++;
				}
				source->link[elenum] = NULL;
				source->element[elenum] = -1;//�ʱ�ȭ
				return node;
				*/
				return node;
			}
		}
		else
		{
			source = node->link[link + 1]; //���� ������ �ִ� ��带 �����Ѵ�
			destination->element[0] = node->element[link];
			destination->celementnum++;
			/* �� �κ��� ������ ���� ù ��° ��Ȳ, �׸��� ����° ��Ȳ���� �ɷ����� ���̱⿡ �ʿ����
			if (source->celementnum == 1)
			{
				fusion(node, link, link + 1); //source�� element�� �ϳ��ۿ� ������� �� ��带 ���Ѵ�
				return node;//��ĥ �ڷᰡ �ִ� child�� link�� ��������, �ڷᰡ ����link�� ���� input���� �����Ѵ�
			}
			*/
			node->element[link] = source->element[0];//source ���� ����� ��� �θ𿡰� element 1�� �ο��ϰ�
			destination->link[1] = source->link[0];//�� key���� ���� key���� ������ �ִ� node�� ����Ű�� link�� �������� �ο����ش�.
			source->celementnum--;
			int elenum = 0;
			while (elenum < source->celementnum)//���ڸ� ä���
			{
				source->link[elenum] = source->link[elenum + 1];
				source->element[elenum] = source->element[elenum + 1];
				elenum++;
			}
			source->link[elenum] = NULL;
			source->element[elenum] = -1;//�ʱ�ȭ
			return node;
		}
	}
	else //���� parent�ܿ��� ���°��
	{
	return node->link[link];
	}
}

void fusion(Node* parentnode, int received, int source)
{//������ fusion�� ������ node�� ���� ������ 1���� Ű�� ������ �־��� ���� ���� �ϴ� 
//element�� ������� ������ 1�̴�
	Node* relement = parentnode->link[received];
	Node* selement = parentnode->link[source]; 
	if (received > source)
	{
		selement->element[1] = relement->element[0];
		selement->link[2] = relement->link[0];
		selement->celementnum++;
		int i = source;
		while (i < parentnode->celementnum - 1)//fusion�ҽÿ��� node���� �� ���޸� �ϰ� element������ �������� �ʾ����� �����ش�
		{
			parentnode->element[i] = parentnode->element[i + 1];
			parentnode->link[i + 1] = parentnode->link[i + 2];
			i++;
		}
		parentnode->element[i] = -1;
		parentnode->link[i + 1] = NULL;
		parentnode->celementnum--;
		relement->element[0] = -1;
		relement->link[0] = NULL;
		relement->celementnum--;
		free(relement);
	}
	else
	{
		relement->element[1] = selement->element[0];
		relement->link[1] = selement->link[0];
		relement->link[2] = selement->link[1];
		relement->celementnum++;
		int i = received;
		while (i < parentnode->celementnum - 1)//fusion�ҽÿ��� node���� �� ���޸� �ϰ� element������ �������� �ʾ����� �����ش�
		{
			parentnode->element[i] = parentnode->element[i + 1];
			parentnode->link[i + 1] = parentnode->link[i + 2];
			i++;
		}
		parentnode->element[i] = -1;
		parentnode->link[i + 1] = NULL;
		parentnode->celementnum--;
		selement->element[0] = -1;
		selement->link[0] = NULL;
		selement->link[1] = NULL;
		selement->celementnum--;
		free(selement);
	}
}