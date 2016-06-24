
typedef struct
{
	char c;
} Record;

typedef int KeyType ;  
#define m 5                   
typedef struct Node{  
    int keynum;               
    struct Node *parent;       
    KeyType key[m+1];          
    struct Node *ptr[m+1];     
    Record *recptr[m+1];      
}NodeType;                    
  
typedef struct{  
    NodeType *pt;             
    int i;                    
    int tag;                  
}Result;                      
  
Result SearchBTree(NodeType *t,KeyType kx)  
{     
    p=t;q=NULL;found=FALSE;i=0;   
    while(p&&!found)  
    {   n=p->keynum;i=Search(p,kx);            
        if(i>0&&p->key[i]= =kx) found=TRUE;   
        else {q=p;p=p->ptr[i];}  
    }  
    if(found) return (p,i,1);                 
    else return (q,i,0);                      
} 

int InserBTree(NodeType **t,KeyType kx,NodeType *q,int i){   
       
      
    x=kx;ap=NULL;finished=FALSE;  
    while(q&&!finished)  
    {   
        Insert(q,i,x,ap);                 
        if(q->keynum<m) finished=TRUE;      
        else  
        {                                 
            s=m/2;split(q,ap);x=q->key[s];  
              
            q=q->parent;  
            if(q) i=Search(q,kx);   
        }  
    }  
    if(!finished)             
    NewRoot(t,q,x,ap);   
} 

