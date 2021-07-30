#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

//randomly selects the word from the file
char *str_ans_func(int *len_);

//chooses difficulty
int diff_func(void);

//determines only when the letter was appropriate
int letter_memory(char x_,char *memory_);

//determines the letter correct or incorrect
int determine_func(char x_,char *str_ans_);

//reveals the letter
void correct_func(char x_,char *str_ans_,char *str_disp_);

/****************************************************************************************************
defines structure for linked list of score if the user found the string (in case of gameover, no score)

score = length / ( (misses number +1)*(difficulty - misses number +1) )
****************************************************************************************************/
typedef struct score_structure
{
    double Str_score_;
    char *Str_str_ans_;
    int Str_len_;
    int Str_diff_;
    int Str_miss_;
    struct score_structure *next;
} ScrStr;

//inserts the score to the ranking
ScrStr *insert_func(ScrStr *head_,double score_,char *str_ans_,int len_,int diff_,int miss_);

//prints the ranking in-order
void print_ranking(ScrStr *head_);

/****************************************************************************************************
the user chooses length of the word
the user chooses the difficulty (allowed misses number)

replaces to dashes

determines only when the letter was appropriate while the user inputs a letter
    determines the letter correct or incorrect
        correct => reveals the letter    incorrect => counts misses number

displays the revealed letters and misses number

escapes from this loop if the string was completed
    inserts the score to the ranking
escapes from this loop if the misses number exceeded the difficulty

puts the score to the ranking
    displays the ranking

free
****************************************************************************************************/
int main(void)
{
    //head : the pointer of linked list of the score ranking
    ScrStr *head=NULL;

    while(1)
    {
        //x : the letter which the user inputs
        //str_ans : the pointer of answer of the string (word)
        //str_disp : the pointer of the string (word) for display (dashes)
        //memory : the memory of letters which the user has already putted
        //len : length of the word
        //diff : difficulty (allowed misses number)
        //miss: present misses number
        //score : score for ranking system (in case of gameover, no score)
        char x, *str_ans=NULL, *str_disp=NULL, *memory=(char*)malloc(sizeof(char));
        int len=0, diff=0, miss=0;
        double score=0;

        //randomly selects the word from the file
        str_ans=str_ans_func(&len);

        //the user chooses difficulty
        diff=diff_func();

        //replaces to dashes
        /**modification**/
        str_disp=(char*)calloc(1,sizeof(char));
        strcpy(str_disp,str_ans);
        for(int i=0; i<len; i++)
        {
            strncpy(str_disp+i,"_",1);
        }
        /*char disp[len+1];
        for(int i=0; i<len; i++)
            disp[i]='_';
        disp[len]='\0';
        str_disp=&disp[0];*/
        printf("\n%s\n\n",str_disp);

        //while the user inputs a letter
        while(printf("\nenter one letter : "),(scanf(" %c",&x))==1)
        {
            //determines only when the letter was appropriate
            //  memories the letter checked by the user
            //      0 => counts as memory    1 => not alphabet or already checked
            if(letter_memory(x,memory)==0)
            {
                //determines the letter correct or incorrect
                //correct => 0    incorrect => counts misses number
                if(determine_func(x,str_ans)==0)
                {
                    //reveals the letter
                    char *s=str_ans;
                    correct_func(x,s,str_disp);
                    printf("correct!");
                }
                else
                {
                    miss+=1;
                    printf("incorrect!");
                }
            }

            //displays the revealed letters and misses number
            printf("\n%s    misses number:%d\n\n",str_disp,miss);

            //escapes from this loop if the string was completed
            if(strcmp(str_ans,str_disp)==0)
            {
                //score=length/((misses number+1)*(difficulty-misses number+1))
                score=(double)len/(((double)miss+1)*((double)diff-(double)miss+1));
                printf("\ncongratulations!\nthe string was : %s\nthis score is : %lf\n\n\n\n\n",str_ans,score);

                //inserts the score to the ranking
                head=insert_func(head,score,str_ans,len,diff,miss);

                break;
            }

            //escapes from this loop if the misses number exceeded the difficulty
            if(miss>diff)
            {
                printf("\ngame over!\nthe string was : %s\n\n\n\n\n",str_ans);
                break;
            }
        }

        //prints the ranking
        print_ranking(head);

        //free all pointer array
        free(memory);
    }

    return 1;
}

/****************************************************************************************************
opens the text file
    failed to open => error message

str_a : the answer string (return value)
    failed to create => error message

determines the number of bytes for creating random number
determines the maximum length of word for user input

scans the number of the length of user input

creates random number
    keeps to create random number until the word of the length will be appeared
****************************************************************************************************/
char *str_ans_func(int *len_)
{
    //opens the file named "words.txt" as reader
    FILE *fp=fopen("words.txt","r");

    //prints an error message if it failed to open
    if(fp==NULL)
    {
        printf("\n\nfailed to open the file [words.txt] :(");
        return 1;
    }

    //str_a : return value for str_ans
    char *str_a=(char*)calloc(1,sizeof(char));
    if(str_a==NULL)
    {
        printf("\n\nfailed to create a string :(");
        return 1;
    }

    //counts the bytes (the limit of random number) and the maximum length of the word (show for user)
    char c;
    int max_len=0;
    int word_len=0;

    //new line => reset length of the word
    //  otherwise => counts length of the word
    while((c=fgetc(fp))!=EOF)
    {
        if(c=='\n')
            word_len=0;
        else
            word_len++;
        //compares to previous maximum length
        if(word_len>max_len)
            max_len=word_len;
    }

    //finds no existing value of the length of the word, if needed
    //len_exist[i]=0 => the word whose length is "i" does not exist
    /**
    int len_exist[max_len];
    FILE *fp=fopen("words.txt","r");
    for(int i=1; i<=max_len; i++)
    {
        while((c=fgetc(fp))!=EOF)
        {
            if(c=='\n')
            {
                if(word_len==i)
                {
                    len_exist[i]=1;
                    break;
                }
                word_len=0;
            }
            else
                word_len++;
        }
    }
    fclose(fp);
    **/

    //the user chooses the length of the word
    while(1)
    {
        printf("enter length of the word (2-%d) : ",max_len);

        //reads string of user input => reads as integer from the string
        //  in order to prevent unintentional loop
        char *num_len=(char*)calloc(1,sizeof(char));
        scanf("%s",num_len);
        sscanf(num_len,"%d",len_);

        //2<=user input<=maximum => ok
        //  otherwise => inputs again
        if(*len_>=2 && *len_<=max_len)
            break;
        else
            printf("\nenter in interval [ 2-%d ] only!\n\n",max_len);
    }

    //repositions the pointer to end of the file
    //substitutes the number of the bytes of the texts
    fseek(fp,0,SEEK_END);
    int byte=ftell(fp);

    //sets random function depending on time
    srand((unsigned int)time(NULL));
    while(1)
    {
        //creates random number up to byte
        int random=rand()%byte;

        //repositions the pointer to random position
        //scans by the end of the line (because the string may not full word)
        //scans one full word
        fseek(fp,random,SEEK_SET);
        fscanf(fp,"%s",str_a);
        fscanf(fp,"%s",str_a);

        //if the length of the word equals to user input => ok
        //  otherwise => loop
        if(*len_==strlen(str_a))
            break;
    }

    //closes the text file
    fclose(fp);

    //return the pointer for str_ans
    str_a[*len_]='\0';
    return str_a;
}

/****************************************************************************************************
chooses difficulty
    if the user inputs improper value => asks again
****************************************************************************************************/
int diff_func(void)
{
    char *num_diff=(char*)calloc(1,sizeof(char));
    int diff_;
    while(1)
    {
        printf("\nenter difficulty (allowed misses number 1-50): ");

        //reads string of user input => reads as integer from the string
        //  in order to prevent unintentional loop
        scanf("%s",num_diff);
        sscanf(num_diff,"%d",&diff_);

        //1<=user input<=26 => ok
        //  otherwise => inputs again
        if(diff_>=1 && diff_<=50)
            break;
        else
            printf("\nenter 1-50 integer only!\n");
    }
    return diff_;
}

/****************************************************************************************************
determines if the letter is an alphabet or not
    not alphabet => asks again

determines if the letter has already checked
    already checked => asks again

adds one allocation to put the letter at the end of array
    if the allocation was not created, return error value
****************************************************************************************************/
int letter_memory(char x_,char *memory_)
{
    //determines if the letter is an alphabet or not
    /**modification**/
    if(isalpha(x_)==0)
    {
        printf("\nenter one alphabet only!");
        return 1;
    }
    /*if(x_<'A' || (x_>'Z' && x_<'a') || x_>'z')
    {
        printf("\nenter one alphabet only!");
        return 1;
    }*/

    //determines if the letter has already checked
    /**modification**/
    if(strchr(memory_,x_)!=NULL)
    {
        printf("\nthe letter [ %c ] has already checked!",x_);
            return 1;
    }
    /*for(int i=0; memory_[i]!=NULL; i++)
    {
        if(memory_[i]==x_)
        {
            printf("\nthe letter [ %c ] has already checked!",x_);
            return 1;
        }
    }*/

    //adds one allocation to put the letter at the end of array
    /**modification**/
    char s[2];
    s[0]=x_;
    s[1]='\0';
    strcat(memory_,s);
    return 0;
    /*memory_=(char*)realloc((char*)memory_,sizeof(char));
    for(int i=0;; i++)
    {
        if(memory_[i]==NULL)
        {
            memory_[i]=x_;
            return 0;
        }
    }

    //if the allocation was not created, return error value
    return 1;*/
}

/****************************************************************************************************
determines if the letter is included in the string of answer
correct => 0
incorrect => 1 (not 0)
****************************************************************************************************/
int determine_func(char x_,char *str_ans_)
{
    /**modification**/
    if(strchr(str_ans_,x_)!=NULL)
    {
        return 0;
    }
    /*for(int i=0; str_ans_[i]!=NULL; i++)
    {
        if(str_ans_[i]==x_)
            return 0;
    }*/
    return 1;
}

/****************************************************************************************************
reveals the letter if it was correct
****************************************************************************************************/
void correct_func(char x_,char *str_ans_,char *str_disp_)
{
    /**modification**/
    /*for(int i=0; str_ans_[i]!=NULL; i++)
    {
        if(str_ans_[i]==x_)
            str_disp_[i]=str_ans_[i];
    }*/

    /**I think above code is better than using standard library function.**/
    char s[2];
    s[0]=x_;
    s[1]='\0';
    int p=strcspn(str_ans_,s);
    if(p==strlen(str_ans_))
        return;
    str_disp_[p]=x_;
    correct_func(x_,str_ans_+p+1,str_disp_+p+1);
}

/****************************************************************************************************
inserts the ScrStr to the linked list

compares each score of ScrStr
searches the smaller score
puts and change the pointers
****************************************************************************************************/
ScrStr *insert_func(ScrStr *head_,double score_,char *str_ans_,int len_,int diff_,int miss_)
{
    //makes allocation of new list and temporary pointer
    ScrStr *new_list=(ScrStr*)malloc(sizeof(ScrStr));
    ScrStr *forward;

    //puts the score data except for next pointer of the list
    new_list->Str_score_=score_;
    new_list->Str_str_ans_=str_ans_;
    new_list->Str_len_=len_;
    new_list->Str_diff_=diff_;
    new_list->Str_miss_=miss_;

    //first time to insert the score
    if(head_==NULL)
    {
        new_list->next=head_;
        head_=new_list;
        return head_;
    }

    //finds appropriate position for inserting
    //  finds the first score less than this score and puts the data right before it
    for(forward=head_; forward->next!=NULL; forward=forward->next)
    {
        if(forward->next->Str_score_<score_)
            break;
    }
    new_list->next=forward->next;
    forward->next=new_list;

    return head_;
}

/****************************************************************************************************
prints the ranking in-order
****************************************************************************************************/
void print_ranking(ScrStr *head_)
{
    printf("\nranking\n");
    ScrStr *p;
    int i=1;
    for(p=head_; p!=NULL; p=p->next)
    {
        printf("\nNo.%d   score : [ %lf ]   word : [ %s ]   length : [ %d ]\n       difficulty : [ %d ]   misses number : [ %d ]\n",
               i,p->Str_score_,p->Str_str_ans_,p->Str_len_,
               p->Str_diff_,p->Str_miss_);
        i++;
    }
    printf("\n\n");
}
