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
	int element[4];//수업자료와 동일한 insertion을 구현하기 위해 예비 element1개 필요
	Node* link[5];//수업자료와 동일한 insertion구현하기 위해 예비 link필요(자식주소)
	int celementnum;//현재 node에 들어있는 element의 수
	bool leaf;//leaf노드인지 아닌지 판별
	int depth;//출력을 위한 요소
};

Node* queue[10000];//Print하기 위해 선언한 배열(최대 10000개까지 보관 가능)
//메모리를 아끼기 위해 제한함 더 많은 데이터의 경우 수정바람
int inlist = 0; //queue에 몇개의 데이터가 들어왔었는지 알려줌
int nodetoprint = 0; //몇번째 배열을 출력할지

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
		char string[15] = { 0 }; //15character 이상 받고싶은 경우 수정하면 된다. 메모리 낭비때문에 15로 설정하였다.
		if (feof(input))break;//파일 맨 마지막부분이면 break한다.
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
		/// function내부에서 새 root노드를 만들게되면 간헐적으로
		/// free하지 않았는데도 값이 깨지는 문제를 발견하였다
		/// 그렇기에 외부에서 새로운 root노드를 만들어 split함수를 통해
		/// 인자를 전달하려 한다.
		if (order == 'i')//insert
		{
			insert(root, value);
			if (root->celementnum == 4)
			{
				split(newroot, 0); //이미 child로 root값을 만들어 놓았기에 0에서 split한다
				root = newroot;//새로운 root값을 지정해준다.
			}
			printer(root, output);
			fprintf(output, "\n\n");
		}
		if (order == 'd')//deletion
		{
			root = deletion(newroot, value, 0);
			if (root->link[0] == NULL || root == NULL) //구술준비 하다 발견한 오류 동작에는 문제는 없었지만 사실 의도상으로는 root.link[0]이었어야 한다
			{
				root = makenode();//root를 가르키는 함수는 while바깥에 있으므로 완전 delete후 새로운 insert operation을
				//위해선 해줘야 한다.
			}
			else if (root->element[0] == -1 && root->link[0])//root만 fusion으로 인하여 사라진 경우
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


Node* makenode(void) //새로운 node를 만들때마다 초기화 설정이 중요하다
{
	Node* newnode;
	newnode = (Node*)malloc(sizeof(Node));//
	newnode->celementnum = 0;
	newnode->leaf = 1;
	newnode->element[0] = -1; //-1로 초기화
	newnode->element[1] = -1;
	newnode->element[2] = -1;
	newnode->element[3] = -1;
	newnode->link[0] = NULL; //임의의 주소값을 가르키면 안되니 NULL로 자식주소 초기화
	newnode->link[1] = NULL;
	newnode->link[2] = NULL;
	newnode->link[3] = NULL;
	newnode->link[4] = NULL;
	return newnode;
}
Node* insert(Node* node, int key)
{
	Node* determine;
	if (node->leaf == 0)//leaf 아닐때
	{
		int count = 0;
		while (node->element[count] != -1 && (key >= node->element[count]) && count <= 2)
		{
			count++;
		}//while문이 끝났을 대 node의 element와 비교하여 넣으려는 key value의 대소관계, 즉 위치를 표현할 수 있다.
		determine = insert(node->link[count], key);
		if (determine->celementnum == 3) //insert가 끝났을 때 split여부 판단
		{
			return split(node, count);//해당 count에 집어넣어서 split해야하는 것이기에
		}
		else return determine;//split안하면 값 그대로 상위 insert에 전달
	}
	else//leaf일 때(이 단계에서 insert를 해주어야 한다)
	{
		leafinsert(node, key);
		return node;
	}
}

void leafinsert(Node* node, int key)
{
	int elenum = node->celementnum;
	int insertcount = elenum - 1;//배열에 대한 counting을 해주어여 함으로 -1
	node->celementnum++; //새로 삽입하니 1개 더해준다.
	while (node->element[insertcount] > key && insertcount >= 0) //elementary sorting으로 key값 insert한다
	{
		node->element[insertcount + 1] = node->element[insertcount];
		insertcount--;
	}
	node->element[insertcount + 1] = key;
}

Node* split(node* node, int splitnum)
{
	Node* lchild = makenode();//split 후 왼쪽 child
	Node* rchild = makenode();//split 후 오른쪽 child
	int elenum = node->celementnum;
	int insertcount = elenum - 1;//leafinsert와 같은 맥락이다
	int comparenumber = (node->link[splitnum])->element[2];
	while (node->element[insertcount] > comparenumber && insertcount >= 0) //child의 3번째 키값을 무조건 올린다.
	{
		node->element[insertcount + 1] = node->element[insertcount];
		node->link[insertcount + 2] = node->link[insertcount + 1];//우측에 있는 링크만 옮겨야한다
		insertcount--;
	}
	lchild = node->link[splitnum];
	node->element[insertcount + 1] = (lchild)->element[2];//3번째 key를 node에 전달해준다.
	node->link[insertcount + 2] = rchild;//우측 child 할당
	node->celementnum++;
	rchild->leaf = lchild->leaf;  //기존 좌측 node에서 우측 node로 정보전달.
	rchild->element[0] = lchild->element[3]; //최 우측 key전달
	lchild->element[2] = -1;//reset
	lchild->element[3] = -1;
	rchild->link[0] = lchild->link[3];
	lchild->link[3] = NULL;
	rchild->link[1] = lchild->link[4];
	lchild->link[4] = NULL;
	rchild->celementnum = 1;
	lchild->celementnum = 2;
	return node;//split완료된 node주소 전달
}
bool search(Node* node, int searchvalue)
{
	int counter = 0;
	int destination = 0;
	bool match = 0;
	while (counter < node->celementnum)
	{
		if (searchvalue > node->element[counter])counter++;
		else if (searchvalue == node->element[counter]) {
			match = 1;
			return match; //match가 되면 bool값을 return한다
		}
		else if (searchvalue < node->element[counter]) break;
	}
	if ((node->leaf) != 1) //leaf가 아닌경우 recursive하게 child노드로 내려간다
	{
		match = search(node->link[counter], searchvalue);
	}

	return match; //leaf인데도 match가 0이면 return해준다
}
void addtoqueue(Node* node)
{
	queue[inlist++] = node; //node를 queue에 저장
}
Node* getfromqueue(void)
{
	if (queue[nodetoprint] != NULL)//값이 있는 경우에 변수 값을 올려주고
	{
		return queue[nodetoprint++];
	}
	else return queue[nodetoprint];// 없으면 변수(포인터 비슷한 개념)값을 그대로 유지한다
}
void printer(Node* root, FILE* file)
{
	addtoqueue(root); // root를 제일 처음에 저장
	root->depth = 0; //depth초기화 child로 가면 1씩 depth가 늘어간다
	while (1)
	{
		root = getfromqueue();
		if (root) //queue에서 return된 값이 있으면
		{
			if (root->depth == 1) //root일때 출력
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
			else if (root->depth == queue[nodetoprint - 1]->depth) //이전에 있던 node와 depth 요소가 같으면 띄어쓰기만 한다. depth0과 따로 둔 이유는 메모리 참조오류 없애기 위해서이다
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
			else//개행한다
			{
				if (root->element[0] >= 0)
				{
					fprintf(file, "\n(");
					if (root->element[1] != -1) fprintf(file, " %d", root->element[0]);
					if (root->element[2] != -1) fprintf(file, " %d", root->element[1]);
					fprintf(file, ") ");
				}
			}
			if (root->link[0] != NULL) {
				int depth = root->depth; //
				depth++;
				(root->link[0])->depth = depth; //1증가된 depth를 child의 depth로 저장한다
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

Node* deletion(Node* node, int key, int link) //parent node를 받은 후 
{
	Node* child = node->link[link]; //child노드 관점으로 내려다 봐야 한다
	int count = 0;
	while (child->element[count] != -1 && (key > child->element[count]) && count <= 2)//기존과는 다르게 key가 childelement값과 같으면 count++를 안해준다
	{
		count++;
	}//while문이 끝났을 대 childnode의 element와 비교하여 넣으려는 key value의 대소관계, 즉 위치를 표현할 수 있다.
	if (child->element[count] == key) //대소관계를 토대로 delete할 값을 발견한경우
	{
		if (child->leaf)//target이 leaf에 존재하면 그냥 삭제
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
				return transfer(node, link);//childnode 값이 없게되면 transfer가 필요함 사실 강의자료상에 나와있는 곧이 곧대로 transfer
				//의 의미가 아닌 transer와 merge가 합쳐진 함수이다.
			}
			return node;
		}
		else//이 경우 target값을 찾았지만 leaf에 존재하지 않는 경우이다. 강의자료에서의 algorithm을 사용하자면 ith child의 rightmost node
		//즉 predecessor를 찾아서 바꿔주어야 한다
		{
			Node* predecessor = findpredecessornode(child->link[count]);
			key = (predecessor->element)[predecessor->celementnum - 1];
			child->element[count] = key;
			deletion(child, key, count);
		}
	}
	else if (child->leaf) return NULL; //leaf까지 도달했는데도 불구하고 위의 if를 통과하지 못했다면 delete할 값이 없는것이다 이 줄은 다음에 나올 줄보다 항상 먼저 있어야한다
	//안그러면 ending condition만족을 하지 못하기 때문이다 //구술 준비하다 발견한 점인데, 사실 NULL값을 리턴했으면 안됐다. 리턴할경우 새로운 노드를 만들어버려 프로그램이 오류가 나버리기 때문이다
	//무의식적으로 그냥 return 0와 비슷한 개념으로 빈 값을 돌려주려 했다.
	else deletion(child, key, count);//recursive하게 child node를 parent node로 있을 수 있도록 함수를 호출한다
	if (child->celementnum == 0)
	{
		transfer(node, link); // childnode 값이 없게되면 transfer시행
	}
	return child;
}

Node* findpredecessornode(Node* node)
{
	if (node->leaf)
	{
		return node;
	}
	return findpredecessornode(node->link[node->celementnum]);//제일 우측에 있는 노드로 내려가야 한다
}

Node* transfer(Node* node, int link)//parent값을 받았다(link에서의 child node가 없을때)
{//child에 아무 값도 없다는 전제 하에서 이루어지는 연산이다
	Node* destination = node->link[link];//transfer을 받았을때 비어있던 child
	Node* source;//값 transfer를 시작할 수 있도록 key값을 제공해주는 node
	if (link == 0 && node->celementnum != 0)//제일 좌측 node에 값이 없는 상황
	{
		source = node->link[1];
		destination->element[0] = node->element[0];
		destination->celementnum++;
		if (source->celementnum == 1)
		{
			fusion(node, link, link + 1); //source의 element가 하나밖에 없을경우 양 노드를 합한다
			return node;//합칠 자료가 있는 child의 link를 좌측으로, 자료가 없는link를 우측 input으로 설정한다
		}
		else
		{
			node->element[0] = source->element[0];//source 값이 충분할 경우 부모에게 element 1개 부여하고
			destination->link[1] = source->link[0];//준 key보다 작은 key값을 가지고 있는 node를 가르키는 link를 좌측으로 부여해준다.
			source->celementnum--;
			int elenum = 0;
			while (elenum < source->celementnum)//빈자리 채우기
			{
				source->link[elenum] = source->link[elenum + 1];
				source->element[elenum] = source->element[elenum + 1];
				elenum++;
			}
			source->link[elenum] = NULL;
			source->element[elenum] = -1;//초기화
			return node;
		} //비슷한 작업을 source하는 node만 바꾸어 제일 우측 node에서도 진행시켜 주면 된다
	}
	else if (link == node->celementnum && node->celementnum != 0) //제일 우측 node에 값이 없는 상황
	{
		source = node->link[link - 1];
		destination->element[0] = node->element[link - 1];//제일 우측에 있는 값 전달
		destination->celementnum++;
		if (source->celementnum == 1)
		{
			fusion(node, link, link - 1); //source의 node에 값이 없어졌을경우 fusion작업이 필요하다
			return node;//합칠 자료가 있는 child의 link를 좌측으로, 자료가 없는link를 우측 input으로 설정한다
		}
		else
		{
			node->element[link - 1] = source->element[source->celementnum - 1];
			destination->link[1] = destination->link[0];//
			destination->link[0] = source->link[source->celementnum]; //이제 부모node 값이 source의 맨 마지막 child가 가르키는 node의 값보다 작으니 
			source->element[source->celementnum - 1] = -1;//초기화
			source->celementnum--;//link를 우측으로 부여해준다
			/* 우측에서 빌려오는게 아니므로 아래의 작업은 필요없다.
			while (elenum < source->celementnum)//빈자리 채우기
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
	else if (node->celementnum != 0) //좌/우의 노드를 참조할 수 있을텐데, 위에서 보이는 것 처럼 좌측노드를 source로 하는 것이 비교연산 안해서 합리적이다.
	{//좌, 우가 둘 다 2node인 경우 좌측노드 참조를 우선으로 한다. 
		if ((node->link[link + 1])->celementnum == 1 || ((node->link[link - 1])->celementnum) > 1)//이 경우 우측을 필요로 하지 않는이상 무조건 좌측노드부터 참조하게 된다
		{
			source = node->link[link - 1];
			destination->element[0] = node->element[link - 1]; //그냥 link를 하면 우측에 있는 node와 교류를 하게되니 주의가 필요하다
			destination->celementnum++;
			if (source->celementnum == 1)
			{
				fusion(node, link, link - 1); //source의 node에 값이 없어졌을경우 fusion작업이 필요하다
				return node;//합칠 자료가 있는 child의 link를 좌측으로, 자료가 없는link를 우측 input으로 설정한다
			}
			else
			{
				node->element[link - 1] = source->element[source->celementnum - 1];
				destination->link[1] = destination->link[0];//
				destination->link[0] = source->link[source->celementnum]; //이제 부모node 값이 source의 맨 마지막 child가 가르키는 node의 값보다 작으니
				source->element[source->celementnum - 1] = -1;//초기화
				source->celementnum--;//link를 우측으로 부여해준다
				/* 우측에서 빌려오는게 아니므로 아래의 작업은 필요없다.
				while (elenum < source->celementnum)//빈자리 채우기
				{
					source->link[elenum] = source->link[elenum + 1];
					source->element[elenum] = source->element[elenum + 1];
					elenum++;
				}
				source->link[elenum] = NULL;
				source->element[elenum] = -1;//초기화
				return node;
				*/
				return node;
			}
		}
		else
		{
			source = node->link[link + 1]; //이제 우측에 있는 노드를 참조한다
			destination->element[0] = node->element[link];
			destination->celementnum++;
			/* 이 부분은 어차피 제일 첫 번째 상황, 그리고 세번째 상황에서 걸러졌을 것이기에 필요없다
			if (source->celementnum == 1)
			{
				fusion(node, link, link + 1); //source의 element가 하나밖에 없을경우 양 노드를 합한다
				return node;//합칠 자료가 있는 child의 link를 좌측으로, 자료가 없는link를 우측 input으로 설정한다
			}
			*/
			node->element[link] = source->element[0];//source 값이 충분할 경우 부모에게 element 1개 부여하고
			destination->link[1] = source->link[0];//준 key보다 작은 key값을 가지고 있는 node를 가르키는 link를 좌측으로 부여해준다.
			source->celementnum--;
			int elenum = 0;
			while (elenum < source->celementnum)//빈자리 채우기
			{
				source->link[elenum] = source->link[elenum + 1];
				source->element[elenum] = source->element[elenum + 1];
				elenum++;
			}
			source->link[elenum] = NULL;
			source->element[elenum] = -1;//초기화
			return node;
		}
	}
	else //값이 parent단에도 없는경우
	{
		return node->link[link];
	}
}

void fusion(Node* parentnode, int received, int source)
{//어차피 fusion은 기존의 node에 값이 없었고 1개의 키를 전달해 주었던 것을 토대로 하니 
//element의 사이즈는 어차피 1이다
	Node* relement = parentnode->link[received];
	Node* selement = parentnode->link[source];
	if (received > source)
	{
		selement->element[1] = relement->element[0];
		selement->link[2] = relement->link[0];
		selement->celementnum++;
		int i = source;
		while (i < parentnode->celementnum - 1)//fusion할시에는 node에서 값 전달만 하고 element상으로 지우지를 않았으니 지워준다
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
		while (i < parentnode->celementnum - 1)//fusion할시에는 node에서 값 전달만 하고 element상으로 지우지를 않았으니 지워준다
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
