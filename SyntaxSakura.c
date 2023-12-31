#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH         20
#define CARDS_NUM               20
#define NUMBER_MAX_IDX          5
#define ONE                     1

typedef enum card_color
{
    Blue    =3,
    Green   =4,
    White   =7,
    Red     =10,
}card_color;

typedef enum state
{
    E_OK,
    E_NOK,
    Start,
    Parsing,
    Inserting,
    Sorting,
    ActionPhase,
    FATAL,
    Finish,
}state;

typedef struct card
{
    char number;
    char color;
}card;

typedef struct node
{
    card   card_node;
    struct node * next;
}node;

/*Functions Prototype*/
char creat_num (char* c);
void StateMachine(FILE* ptr_to_file,card * cards);
void AssignToAllCards(card * TempCard,card * cards,char ch_card_idx);
char InsertNode(card *cards , node**LinkedList_H, node**LinkedList_T, char ch_card_idx,node * ptr);
void printLinkedList (node* LinkedList_H);
void SortLinkedList(node** LinkedList);
char SearchNode (int cardNumber,node * LinkedList,char * Flag);
char DeleteNode (int index , node** LinkedList_H , node**LinkedList_T);


void main(void)
{
    /*File I/O*/
    FILE* ptr_to_file;
    /*Allocating the Size for cards in Heap*/
    card * cards = (card *)calloc(CARDS_NUM , sizeof(card));
    if (cards == NULL)
    {
        printf("No Memory Availabe in Heap\n");
        exit(1);
    }
    else
    {
        StateMachine(ptr_to_file,cards);
        free(cards);
    }
    printf("End\n");
}


void StateMachine(FILE* ptr_to_file,card * cards)
{
    /*Player 1 Linked List Head & Tail*/
    node *Player_1_H = NULL;
    node *Player_1_T = NULL;
    /*Player 2 Linked List Head & Tail*/
    node *Player_2_H = NULL;
    node *Player_2_T = NULL;        
    /*Temporary Struct*/
    card TempCard;
    /*Line Read from file*/
    char Line[MAX_LINE_LENGTH];
    /*State Machine Variable*/
    char state = Start;
    /*For Loop Counter*/
    char i=0;
    /*Line Counter in File*/
    char Line_Counter =0;
    /*Card Color*/
    char ch_col=0;
    /*Card Number*/
    char ch_card_num=0;
    /*Card Index*/
    char ch_card_idx=0;
    /*Search Node State*/
    char Found_State = 0;
    while((state != Finish) && (state != FATAL))
    {
        switch(state)
        {
            /*Read configuration file.*/
            case Start:
            {
                ptr_to_file = fopen("config_file.txt","r");
                
                if(ptr_to_file == NULL)
                {
                    printf("Cannot open the File\n");
                    state = FATAL;
                }
                else
                {
                    /*change state to parse data if the file is valid.*/
                    state = Parsing;
                }

                break;
            }

            case Parsing:
            {
                /*Line by Line read for configuration file.*/
                while(fgets(Line,MAX_LINE_LENGTH,ptr_to_file))
                {
                    /*Neglect the first two lines not needed.*/
                    if(Line_Counter > ONE)
                    {
                        ch_col= 0;
                        for(i=0; (i<MAX_LINE_LENGTH) && (Line[i] != '\0'); i++)
                        {
                            if(Line[i] == '_')
                            ch_col = Line[i+ONE];
                            break;
                        }
                        /*Convert the Decimal in the string to number.*/
                        ch_card_num = creat_num(Line);
                        /*Stor color and number in the temporary struct to be sotred in Heap equavilant struct.*/
                        TempCard.color = ch_col;
                        TempCard.number = ch_card_num;
                        /*Assign it to the Allocated struct in the Heap.*/
                        AssignToAllCards(&TempCard,cards,ch_card_idx);
                        ch_card_idx++;
                    }
                    Line_Counter++;
                }
                /*Close File.*/
                fclose(ptr_to_file);
                /*Change State to Action Phase*/
                state = Inserting;
                break;
            }

            case Inserting:
            {
                /*Implemented Code For Now.*/
                ch_card_idx = 0;
                for(i=0; i<CARDS_NUM; i++)
                { 
                    node * ptr = (node*)calloc(ONE,sizeof(node));
                    if(ptr != NULL)
                    {
                        if(ch_card_idx % 2 == 0)
                        {
                            InsertNode(cards,&Player_1_H,&Player_1_T,ch_card_idx,ptr);
                        }
                        else
                        {
                            InsertNode(cards,&Player_2_H,&Player_2_T,ch_card_idx,ptr);                        
                        }
                    }
                    else
                    {
                        printf("No Memory Availabe in Heap\n");
                        exit(1);
                    }
                    ch_card_idx++;    
                }
                state = Sorting;
                free(cards);
                break;
            }

            case Sorting:
            {
                SortLinkedList(&Player_1_H);
                SortLinkedList(&Player_2_H);
                printLinkedList(Player_1_H);
                printLinkedList(Player_2_H); 
                state = ActionPhase;

                break;          
            }

            case ActionPhase:
            {
                Found_State = SearchNode(28,Player_1_H,&ch_card_idx);
                if(Found_State != E_NOK)
                {
                    DeleteNode(ch_card_idx , &Player_1_H,&Player_1_T);
                }
                else 
                {
                    printf("Card Not found\n");
                }
                
                Found_State = SearchNode(14,Player_1_H,&ch_card_idx);
                if(Found_State != E_NOK)
                {
                    DeleteNode(ch_card_idx , &Player_1_H,&Player_1_T);
                }
                else 
                {
                    printf("Card Not found\n");
                }

                Found_State = SearchNode(115,Player_2_H,&ch_card_idx);
                if(Found_State != E_NOK)
                {
                    DeleteNode(ch_card_idx , &Player_2_H,&Player_2_T);
                }
                else 
                {
                    printf("Card Not found\n");
                }

                Found_State = SearchNode(0,Player_1_H,&ch_card_idx);
                if(Found_State != E_NOK)
                {
                    DeleteNode(ch_card_idx,&Player_1_H,&Player_1_T);
                }
                else
                {
                    printf("Card Not Found\n");
                }

                printLinkedList(Player_1_H);
                printLinkedList(Player_2_H);
                state =Finish;
                break;
                
            }

            case Finish:
            {

                break;
            }
            case FATAL:
            {
                printf("ERROR\n");
                exit(1);
                break;
            }

        }//Switch
    }//While
}

char creat_num (char* c)
{
    char num = 0;
    if(c != NULL)
    {
        num = atoi(c);
        return num;
    }
    else
    {
        /*Do Nothing.*/
    }
}

void AssignToAllCards(card * TempCard,card * cards,char ch_card_idx)
{
    if((TempCard != NULL) && (cards != NULL) )
    {
        if(TempCard->color == 'r')
        {
            cards[ch_card_idx].color = Red;
        }
        else if(TempCard->color == 'w')
        {
            cards[ch_card_idx].color = White;
        }
        else if(TempCard->color == 'b')
        {
            cards[ch_card_idx].color = Blue;            
        }
        else if(TempCard->color == 'g')
        {
            cards[ch_card_idx].color = Green;            
        }
        else 
        {
            printf("invalid input for color !");
            exit(1);
        }                
        cards[ch_card_idx].number = TempCard->number;
    }
}



char InsertNode(card *cards , node**LinkedList_H, node**LinkedList_T, char ch_card_idx,node * ptr)
{   
    
    char i = 0;
    node *TempNode;
    ptr->card_node.color  = cards[ch_card_idx].color;
    ptr->card_node.number = cards[ch_card_idx].number;
    if(*(LinkedList_H) == NULL)
    {
        /*Empty List*/
        *LinkedList_H = ptr;
        *LinkedList_T = ptr;
    }
    else 
    {
        TempNode = *LinkedList_H;
        for(i=0; (i<(CARDS_NUM/2)); i++)
        {
            if(TempNode->next != NULL)
            {
                /*Not Reached the end of the List*/
                TempNode = TempNode->next;
            }
            else
            {
                /*Reached End of the List*/
                TempNode->next = ptr;
                (*LinkedList_T) = ptr;
                ptr->next = NULL;
            }
        }
    }
}


void printLinkedList (node* LinkedList_H)
{
    char i = 0;
    node * ptr;
    ptr = LinkedList_H ; 
    if(ptr != NULL)
    {
        for(i =0; ((i<CARDS_NUM/2) && (ptr != NULL)); i++)
        {       
            if(ptr != NULL)
            {
                printf("Card[%d] -> {Number = %d , Color = %d} \n",i,ptr->card_node.number,ptr->card_node.color);
            }
            else 
            {
                /*Done*/
            }
            ptr = ptr->next;
        }
     }
     else 
     {
        printf("ERROR\n");
        exit(1);
     }
}

void SortLinkedList(node** LinkedList)
{
    node * current;
    node * index;
    card Temp;

    current = *LinkedList;
    /*Selection Sort Algorithm Used.*/
    while(current != NULL)
    {
        index = current->next;
        while(index != NULL)
        {
            if(current->card_node.number > index->card_node.number)
            {
                /*Swap*/
                Temp.number = current->card_node.number;
                Temp.color  = current->card_node.color;

                current->card_node.number = index->card_node.number;
                current->card_node.color  = index->card_node.color;

                index->card_node.number = Temp.number;
                index->card_node.color  = Temp.color;
            }
            index = index->next;
        }
        current = current->next;
    }
}


char SearchNode (int cardNumber,node * LinkedList,char * index)
{
    char state=E_NOK;
    char i=0;
    node * ptr;
    ptr = (node *)LinkedList;
    if(ptr != NULL)
    {
        for (i=0; (i<CARDS_NUM/2) && (ptr != NULL); i++)
        {
            if(ptr->card_node.number == cardNumber)
            {
                printf("Found Card in index = %d , %d , %d\n",i,cardNumber,ptr->card_node.number);
                *index = i;
                state = E_OK;
                break;
            }
            else 
            {
                /*Go for the Next Node.*/   
                ptr = ptr->next;
            }
        }
    }    
    else 
    {
        printf("Empty Linked List\n");
    }
    return state;
}


char DeleteNode (int index , node** LinkedList_H , node**LinkedList_T)
{ 
    char state = E_NOK;
    unsigned char i =0;
    node * ptr;
    node * Temp;
    ptr = *LinkedList_H;
    if(ptr != NULL)
    {
        if(index == 0)
        {
            /*The Desired Node is the Head.*/
            Temp = *LinkedList_H;
            /*Update Head Node.*/
            *LinkedList_H =  ptr->next;
            /*Free Space.*/
            free(Temp);
        }
        else 
        {
           /*Loop Until reaches the Node before the desired node.*/
           for (i = 0; i<index-1; i++)
           {
               ptr = ptr->next;
           }                  
           if (ptr->next->next == NULL)
           {
               /*The Desired Node is the Tail.*/
               Temp = *LinkedList_T;
               /*Update the Tail Node.*/
               *LinkedList_T = ptr;
               /*Unlink the Next Ptr*/
               ptr->next = NULL;
               /*Free Space.*/
               free(Temp);
           }
           else
           {
               /*In between Node.*/
               /*Make the pointer to stand on the desired node to be deleted.*/
               Temp = ptr->next;
               /*Make the current node to point on the next node after the deleted one.*/
               ptr->next = ptr->next->next;
               /*Free Space.*/
               free(Temp);
           }    
        }        
    }
    else 
    {
        /*Empty Linked List*/
    }
   
}

    

