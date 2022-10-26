
//  main.c
//  將棋

//  香桂銀金王金銀桂香
//  十飛十十十十十角十
//  步十步十步十步十步
//  十十十十十十十十十
//  十十十十十十十十十
//  十十十十十十十十十
//  步十步十步十步十步
//  十角十十十十十飛十
//  香桂銀金王金銀桂香

//

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "libev-4.33/ev.h"
#define ROW 9
#define R(piece) "\033[31m"#piece"\033[0m"//紅色棋子
//#define B(piece) "\033[30m"#piece"\033[0m"//黑色棋子
#define B(piece) "\033[34m"#piece"\033[0m"//藍色棋子
#define CROSS "\033[33m十\033[0m"

//定義外部變量,棋盤坐標
char* array[10][10];
char* copyarray;
char instruct = 1;//回放指令
char saving = 1;//儲存指令
int xi,yi;//要移動的棋子
int xj,yj;//移動的目標位置
int decision;//新局還是舊局
int minx,miny;//移動前跟移動後相差的距離
bool isStandard = 1;//是否符合規則，初始值1，符合
bool gameOverSign = 1;//遊戲是否結束，0結束
bool functionOverSign = 1;//遊戲是否結束，0結束
bool restart = 0;
bool reprint = 0;
//生成棋盤
void chessboardBuilding();
//打印棋盤
void printChessboard();
//判斷是紅棋還是黑棋,紅旗返回1,黑棋返回-1,否則返回0
int redOrBlack(int x,int y);
//讀檔
void readMove();
//存檔
void saveMove();
//f或b移動
void printMove();
//紅棋移動
void redMove();
//黑棋移動
void blackMove();
//每種棋子的規則
void rulesOfAllKindsOfChessPieces();
//判斷遊戲結束
void isGameOver();

int turns;
int now;
int c=0;
int indep=0;
ev_io stdin_watcher;
ev_timer timeout_watcherx;
ev_timer timeout_watchery;

#define MAXSTACK 100 /*定義最大堆疊容量*/
int stackfront1[MAXSTACK];  //堆疊的陣列宣告 
int stackfront2[MAXSTACK];  //堆疊的陣列宣告 
int stackend1[MAXSTACK];  //堆疊的陣列宣告 
int stackend2[MAXSTACK];  //堆疊的陣列宣告 
char *stackword[MAXSTACK];  //堆疊的陣列宣告 
int top=-1;		//堆疊的頂端
int printstep = 0; 
int isEmpty();
void push(int,int,int,int,char*); 
int pop();


/*判斷是否為空堆疊*/
int isEmpty(){
	if(top==-1){
		return 1; 
	}else{
		return 0;
	}
} 
/*將指定的資料存入堆疊*/
void push(int data1,int data2,int data3,int data4,char* copyarray){
	if(top>=MAXSTACK){
		printf("堆疊已滿,無法再加入\n");	
	}else{
		top++;
		stackfront1[top]=data1;
        stackfront2[top]=data2;
        stackend1[top]=data3;
        stackend2[top]=data4;
        stackword[top]=copyarray;
	}
 
} 
/*從堆疊取出資料*/
int pop(){
	int dataout1,dataout2,dataout3,dataout4;
	if(isEmpty()){
		printf("堆疊已空\n");
	}else{
        dataout1=stackfront1[top];
        dataout2=stackfront2[top];
        dataout3=stackend1[top];
        dataout4=stackend2[top];
		top--;
		return dataout1,dataout2,dataout3,dataout4; 
	}
}


static void stdin_cb (EV_P_ ev_io *w, int revents){
    ev_timer_stop (loop, &timeout_watcherx);	
    ev_timer_stop (loop, &timeout_watchery);
	if(turns%2){	
		ev_timer_stop (loop, &timeout_watcherx);	
		ev_timer_start (loop, &timeout_watchery);
	}
	else{
		ev_timer_stop (loop, &timeout_watchery);
		ev_timer_start (loop, &timeout_watcherx);
	}
}
static void timeout_cbx (EV_P_ ev_timer *w, int revents){
	if(turns==0){
        printf("The time of player x use: ");printf("%d\n",(int)ev_now(loop)-now);  //first time
        indep=(int)ev_now(loop)-now;
        printf("indep: %d\n",indep);
    }
    else{
        printf("The time of player x use: ");printf("%d\n",(int)ev_now(loop)-now-indep);
        indep=(int)ev_now(loop)-now;
        printf("cby\n");
        printf("indep: %d\n",indep);
    }
    printf("cbx\n");
    ev_timer_stop (loop, &timeout_watcherx);
    ev_break(loop, EVBREAK_ONE);
    printf("InCBX,turns++\n\n\n");
    turns=-1;
}
static void timeout_cby (EV_P_ ev_timer *w, int revents){
    printf("The time of player y use: ");printf("%d\n",(int)ev_now(loop)-now-indep);
    indep=(int)ev_now(loop)-now;
    printf("indep: %d\n",indep);
    printf("cby\n");
    ev_timer_stop (loop, &timeout_watchery);
    ev_break(loop, EVBREAK_ONE);
    printf("InCBY,turns++\n\n\n");
    turns=1;
}
//**************************主函數******************************
int main()
{
    struct ev_loop *loop = EV_DEFAULT;
    now=ev_now(loop);
    //int x,y,z;    
    ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
	ev_io_start(loop, &stdin_watcher);
	ev_timer_init(&timeout_watcherx, timeout_cbx, 0, 1);
    ev_timer_init(&timeout_watchery, timeout_cby, 0, 1);
    while (functionOverSign) {
    printf("請輸入引數，1為開新局，2為載入棋譜。\n");
    scanf("%d",&decision);
    if(decision == 1){
    //生成棋盤
    chessboardBuilding();
    //打印棋盤
    printChessboard();
    //開始下棋
    int turn = 1;
    while (gameOverSign) {
        isStandard = 1;
        turn *= (-1);//雙方交替下棋
        switch (turn) {
            case 1:
                redMove();
                turn = (restart) ? (turn*-1) : turn;
                
                if(restart == 0) {
                    /*ev_timer_start(loop, &timeout_watchery);//總時間在哪*/
                    printf("您將紅棋從%d %d到%d %d\n",yi,xi,yj,xj);
                    push(xi,yi,xj,yj,copyarray);
                    printf("top = %d \n",top);
                     
                        
                     saveMove();

                    }
                    ev_timer_start(loop, &timeout_watchery);//總時間在哪
                break;
            case -1:
                blackMove();
                turn = (restart) ? (turn*-1) : turn;
                
                if(restart == 0){
                    /*ev_timer_start(loop, &timeout_watcherx);*/
                    printf("您將藍棋從%d %d到%d %d\n",yi,xi,yj,xj);
                    push(xi,yi,xj,yj,copyarray);
                    printf("top = %d \n",top);
                    
                       
                    saveMove();
                    } 
                    ev_timer_start(loop, &timeout_watcherx);
                break;
        }
        if(restart== 0)
            ev_run(loop, 0);
        isGameOver();
        
    }
    ev_timer_stop (loop, &timeout_watcherx);
    ev_timer_stop (loop, &timeout_watchery);
    printf("遊戲結束!\n");
    }
    else if (decision == 2){
        //讀檔
        readMove();
        //生成棋盤
        chessboardBuilding();
        //打印棋盤
        printChessboard();
        //開始下棋
        
        while (gameOverSign) {
            printf("請輸入小寫f或b。\n");
            
            scanf(" %c",&instruct);
            getchar();
            printMove();
            isGameOver();
            
        }
        
    }
    else {printf("請重新輸入引數。\n");}

    }
}//主函數結束


//*************************自定義函數*****************************


//讀檔
void readMove()
{  
   
        FILE *fp_r = fopen("data_save.txt", "r");
        /*if (fp_w == NULL)
        return -1;*/
        
        while(!feof(fp_r)) {
        top = top + 1;    
        stackword[top] = malloc(100); 
        
        fscanf(fp_r, "%d %d %d %d %s\n", &stackfront1[top],&stackfront2[top],&stackend1[top],&stackend2[top],*&stackword[top]);
        printf("stackword = %s \n",stackword[top]);
        }
        printf("top = %d \n",top);
        printf("讀檔成功。\n");
        fclose(fp_r);
        

}


//存檔
void saveMove()
{  
    printf("是否存檔，請按小寫s，否則請輸入任意數字。\n");
                    scanf(" %c",&saving);
                    getchar();
    if(saving == 's'){
        
        FILE *fp_w = fopen("data_save.txt", "w");
        /*if (fp_w == NULL)
        return -1;*/
        
        for(int i = 0 ; i < top + 1 ; i++) {
        fprintf(fp_w, "%d %d %d %d %s\n", stackfront1[i],stackfront2[i],stackend1[i],stackend2[i],stackword[i]);
        }
        printf("存檔成功。\n");
        fclose(fp_w);
        }

}


//f或b
void printMove()
{  
    if(instruct == 'f' && printstep < top + 1){
        printf("請f。\n");
        xi = stackfront1[printstep];
        yi = stackfront2[printstep];
        xj = stackend1[printstep];
        yj = stackend2[printstep];
        rulesOfAllKindsOfChessPieces();
        printChessboard();
        printstep = printstep + 1;
    }
    else if(instruct == 'b' && printstep > 0){
        printf("請b。\n");
        printstep = printstep - 1;
        xi = stackend1[printstep];
        yi = stackend2[printstep];
        xj = stackfront1[printstep];
        yj = stackfront2[printstep];
        char *copy = array[xj][yj];
        array[xj][yj] = array[xi][yi];
        array[xi][yi] = stackword[printstep];
        printChessboard();
        
    }
        
    else {
        printf("存檔已結束\n");
        printf("請重新輸入小寫f或b。\n");
        }
}
            
            
            




//生成棋盤
void chessboardBuilding()
{
    //全部為十字
    for (int i = 1; i < 10; i ++) {
        for (int j = 1; j < 10 ; j ++) {
            array[i][j] = CROSS;
        }
        printf("\n");
    }
   
    
    //布置紅棋
    array[1][1] = array[1][9] = R(香);
    array[1][2] = array[1][8] = R(桂);
    array[1][3] = array[1][7] = R(銀);
    array[1][4] = array[1][6] = R(金);
    array[1][5] = R(王);
    array[2][2] = R(飛);
    array[2][8] = R(角);
    array[3][1] = array[3][3] = array[3][5] = array[3][7] = array[3][9] = R(步);
    
    //布置黑棋
    array[9][1] = array[9][9] = B(香);
    array[9][2] = array[9][8] = B(桂);
    array[9][3] = array[9][7] = B(銀);
    array[9][4] = array[9][6] = B(金);
    array[9][5] = B(王);
    array[8][2] = B(角);
    array[8][8] = B(飛);
    array[7][1] = array[7][3] = array[7][5] = array[7][7] = array[7][9] = B(步);
}

//打印棋盤
void printChessboard()
{
	system("clear");
    
    //顯示
    printf("  \033[43;30m日本將棋歡迎您\033[0m\n\n");
    for (int i = 1; i < 10;  i ++) {
        for (int j = 1; j < 10; j ++) {
            printf("%s",array[i][j]);
        }
        printf("\n");
    }

}

//判斷是紅棋還是黑棋,紅旗返回1,黑棋返回-1,否則返回0
int redOrBlack(int x,int y)
{
    if (array[x][y] == R(香) || array[x][y] == R(桂) || array[x][y] == R(銀) || array[x][y] == R(金) || array[x][y] == R(王) || array[x][y] == R(飛) || array[x][y] == R(步) || array[x][y] == R(角))
    {
        return  1;
    }
    else if (array[x][y] == B(香) || array[x][y] == B(桂) || array[x][y] == B(銀) || array[x][y] == B(金) || array[x][y] == B(王) || array[x][y] == B(飛) || array[x][y] == B(步) || array[x][y] == B(角))
    {
        return -1;
    }
    else
        return 0;
}

//紅棋移動
void redMove()
{
    if (restart) {
        printf("違反遊戲規則，請重新輸入\n");
        restart = 0;
    }
    printf("[紅棋]請輸入你要移動的棋子(行+空白鍵+列):\n");
    scanf("%d %d",&yi,&xi);
    printf("[紅棋]請輸入你要放置的位置(行+空白鍵+列):\n");
    scanf("%d %d",&yj,&xj);

    copyarray = array[xj][yj];
    if (yi != 0 && xi != 0 && yj != 0 && xj != 0 && (redOrBlack(xi, yi) == 1)){
        
        rulesOfAllKindsOfChessPieces();
        printChessboard();
    }
    else
        {restart = 1; }
    
}

//藍棋移動
void blackMove()
{
    if (restart) {
        printf("違反遊戲規則，請重新輸入\n");
        restart = 0;
    }
    printf("[藍棋]請輸入你要移動的棋子(行+空白鍵+列):\n");
    scanf("%d %d",&yi,&xi);
    printf("[藍棋]請輸入你要放置的位置(行+空白鍵+列):\n");
    scanf("%d %d",&yj,&xj);
    copyarray = array[xj][yj];
    if (yi != 0 && xi != 0 && yj != 0 && xj != 0 && (redOrBlack(xi, yi) == -1)){
        
        rulesOfAllKindsOfChessPieces();
        printChessboard();
    }
    else
        restart = 1;
}

//判斷遊戲結束
void isGameOver()
{
    bool sign_r = 0;
    bool sign_b = 0;
    for (int i = 1; i < 10; i ++) {
        for (int j = 1; j < 10; j ++) {
            if (array[i][j] == R(王)) {
                sign_r = 1;
            }
            else if (array[i][j] == B(王))
            {
                sign_b = 1;
            }
        }
    }
    if ((sign_r == 0)||(sign_b == 0)) {
        gameOverSign = 0;
    }
}

//每種棋子的規則
void rulesOfAllKindsOfChessPieces()
{
//R（香）----------------------------------------
    if (array[xi][yi] == R(香))
    {
        if (yi == yj)//列坐標不變，同列移動
        {
             for (int i = xi+1; i < xj; i ++)
            {
                if (array[i][yi] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
        }
         if (xi >= xj)//行坐標不變，同行移動
        {
                    isStandard = 0;
        }
        
        if ((yi == yj )&& isStandard && (redOrBlack(xj, yj) != 1))//如果棋子直行、沒有犯規且落點不是紅棋，可以移動
        {
            
            array[xi][yi] = CROSS;
            array[xj][yj] = R(香);
        }
        else
        {
            restart = 1;
            
        }
    }

//B（香）----------------------------------------
    else if (array[xi][yi] == B(香))
    {
        
        if (yi == yj)//列坐標不變，同列移動
        {
            for (int i = xi-1; i > xj; i --)
            {
                if (array[i][yi] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
        }
         if (xi <= xj)//行坐標不變，同行移動
        {
                    isStandard = 0;
        }
        if ((yi == yj )&& isStandard && redOrBlack(xj, yj) != -1)//如果棋子直行、沒有犯規且落點不是紅棋，可以移動
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(香);
        }
        else
        {
            restart = 1;
        }
    }
//R（飛）----------------------------------------
    else if (array[xi][yi] == R(飛))
    {
        if (yi == yj)//列坐標不變，同列移動
        {
             for (int i = xi+1; i < xj; i ++)
            {
                if (array[i][yi] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi-1; i > xj; i --)
            {
                if (array[i][yi] != CROSS)
                    isStandard = 0;
            }        
        }
        else if (xi == xj)//行坐標不變，同行移動
        {
            for (int i = yi+1; i < yj; i ++)
                if (array[xi][i] != CROSS)
                    isStandard = 0;
            for (int i = yi-1; i > yj; i --)
                if (array[xi][i] != CROSS)
                    isStandard = 0;
        }
        
        if ((yi == yj || xi == xj)&& isStandard && (redOrBlack(xj, yj) != 1))//如果棋子直行、沒有犯規且落點不是紅棋，可以移動
        {
            
            array[xi][yi] = CROSS;
            array[xj][yj] = R(飛);
        }
        else
        {
            restart = 1;
        }
    }

//B（飛）----------------------------------------
    else if (array[xi][yi] == B(飛))
    {
        
        if (yi == yj)//列坐標不變，同列移動
        {
            for (int i = xi+1; i < xj; i ++)
            {
                if (array[i][yi] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi-1; i > xj; i --)
            {
                if (array[i][yi] != CROSS)
                    isStandard = 0;
            }      
        }
        else if (xi == xj)//行坐標不變，同行移動
        {
            for (int i = yi+1; i < yj; i ++)
                if (array[xi][i] != CROSS)
                    isStandard = 0;
            for (int i = yi-1; i > yj; i --)
                if (array[xi][i] != CROSS)
                    isStandard = 0;
        }
        if ((yi == yj || xi == xj )&& isStandard && redOrBlack(xj, yj) != -1)//如果棋子直行、沒有犯規且落點不是紅棋，可以移動
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(飛);
        }
        else
        {
            restart = 1;
        }
    }
//R（角）----------------------------------------
    else if (array[xi][yi] == R(角))
    {   
        minx = abs(xi - xj);
        miny = abs(yi - yj);
        if(minx == miny){
            for (int i = xi-1,j = yi-1 ; i > xj && j > yj ; i --,j --)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi-1,j = yi+1 ; i > xj && j < yj ; i --,j ++)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi+1,j = yi+1 ; i < xj && j < yj ; i ++,j ++)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi+1,j = yi-1 ; i < xj && j > yj ; i ++,j --)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            if ((redOrBlack(xj, yj) != 1) && isStandard)
            {
                array[xi][yi] = CROSS;
                array[xj][yj] = R(角);
            }
            else
            {
                restart = 1;
            }
        }
            
        else
        {
            restart = 1;
        }
    }
    
//B（角）----------------------------------------
    else if (array[xi][yi] == B(角))
        {   
        minx = abs(xi - xj);
        miny = abs(yi - yj);
        if(minx == miny){
            for (int i = xi-1,j = yi-1 ; i > xj && j > yj ; i --,j --)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi-1,j = yi+1 ; i > xj && j < yj ; i --,j ++)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi+1,j = yi+1 ; i < xj && j < yj ; i ++,j ++)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            for (int i = xi+1,j = yi-1 ; i < xj && j > yj ; i ++,j --)
            {
                if (array[i][j] != CROSS)
                    isStandard = 0;//如果初始位置和目標位置之間有棋子，則不符合規則
            } 
            if ((redOrBlack(xj, yj) != -1) && isStandard)
            {
                array[xi][yi] = CROSS;
                array[xj][yj] = B(角);
            }
            else
            {
                restart = 1;
            }
        }
            
        else
        {
            restart = 1;
        }
    }
    
//R（馬）----------------------------------------
    else if (array[xi][yi] == R(桂))
    {
        if ((redOrBlack(xj, yj) != 1) && (  (xj == xi+2 && yj == yi-1 &&redOrBlack(xi+1, yi) == 0) || (xj == xi+2 && yj == yi+1 &&redOrBlack(xi+1, yi) == 0)))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(桂);
        }
        else
        {
            restart = 1;
        }
    }
    
//B（馬）----------------------------------------
    else if (array[xi][yi] == B(桂))
    {
        if ((redOrBlack(xj, yj) != -1) && ((xj == xi-2 && yj == yi-1 &&redOrBlack(xi-1, yi) == 0) || (xj == xi-2 && yj == yi+1 &&redOrBlack(xi-1, yi) == 0) ))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(桂);
        }
        else
        {
            restart = 1;
        }
    }
    

    
//R（兵）----------------------------------------
    else if (array[xi][yi] == R(步))
    {
        if (xi > xj)
            isStandard = 0;//如果倒退，則不符合規範
        
        if ((xj == xi+1 && yi ==yj))
        {
                
        }
        else
            isStandard = 0;
        
        if (isStandard && redOrBlack(xj, yj) != 1)//
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R (步);
        }
        else
        {
            restart = 1;
        }
    }
    
//B（卒）----------------------------------------
    else if (array[xi][yi] == B(步))
    {
        if (xi < xj)
            isStandard = 0;//如果倒退，則不符合規範
        if ((xj == xi-1 && yi ==yj))
        {
                
        }
        else
            isStandard = 0;
        
        if (isStandard && redOrBlack(xj, yj) != -1)//
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B (步);
        }
        else
        {
            restart = 1;
        }
    }

//R（相）----------------------------------------
    else if (array[xi][yi] == R(銀))
    {
        if ((redOrBlack(xj, yj) != 1) && ((xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || (xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 ) || ((yj == yi)&&(xj == xi + 1 ))))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(銀);
        }
        else
        {
            restart = 1;
        }
    }
   /* else if (array[xi][yi] == R(銀))
    {
        if ((redOrBlack(xj, yj) != 1) && ((xj == xi-2 && yj == yi-2 &&redOrBlack(xi-1, yi-1) == 0) || (xj == xi-2 && yj == yi+2 &&redOrBlack(xi-1, yi+1) == 0) || (xj == xi+2 && yj == yi-2 &&redOrBlack(xi+1, yi-1) == 0) || (xj == xi+2 && yj == yi+2 &&redOrBlack(xi+1, yi+1) == 0)))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(銀);
        }
        else
        {
            restart = 1;
        }
    }*/
    
//B（象）----------------------------------------
    else if (array[xi][yi] == B(銀))
    {
        if ((redOrBlack(xj, yj) != -1) && ((xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || (xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 ) || ((yj == yi)&&(xj == xi - 1 ))))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(銀);
        }
        else
        {
            restart = 1;
        }
    }
    /*else if (array[xi][yi] == B(銀))
    {
        if ((xj >= 6)&&(redOrBlack(xj, yj) != -1) && ((xj == xi-2 && yj == yi-2 &&redOrBlack(xi-1, yi-1) == 0) || (xj == xi-2 && yj == yi+2 &&redOrBlack(xi-1, yi+1) == 0) || (xj == xi+2 && yj == yi-2 &&redOrBlack(xi+1, yi-1) == 0) || (xj == xi+2 && yj == yi+2 &&redOrBlack(xi+1, yi+1) == 0)))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(銀);
        }
        else
        {
            restart = 1;
        }
    }*/

//R（仕）----------------------------------------
    else if (array[xi][yi] == R(金))
    {
        if ((redOrBlack(xj, yj) != 1) && ((xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 ) || ((xj == xi)&&(yj == yi + 1 || yj == yi - 1))||((yj == yi)&&(xj == xi + 1 || xj == xi - 1))))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(金);
        }
        else
        {
            restart = 1;
        }
    }
    /*else if (array[xi][yi] == R(仕))
    {
        if ((xj <= 2)&&(redOrBlack(xj, yj) != 1) && ((xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || (xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 )))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(仕);
        }
        else
        {
            restart = 1;
        }
    }*/

//B（士）----------------------------------------
    else if (array[xi][yi] == B(金))
    {
        if ((redOrBlack(xj, yj) != -1) && ((xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || ((xj == xi)&&(yj == yi + 1 || yj == yi - 1))||((yj == yi)&&(xj == xi + 1 || xj == xi - 1))))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(金);
        }
        else
        {
            restart = 1;
        }
    }
    /*else if (array[xi][yi] == B(士))
    {
        if ((xj >= 8)&&(redOrBlack(xj, yj) != 1) && ((xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || (xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 )))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(士);
        }
        else
        {
            restart = 1;
        }
    }*/

//R（帥）----------------------------------------
    else if (array[xi][yi] == R(王))
    {
        if ((redOrBlack(xj, yj) != 1) && (((xj == xi)&&(yj == yi + 1 || yj == yi - 1))||((yj == yi)&&(xj == xi + 1 || xj == xi - 1)) || (xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || (xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 )))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(王);
        }
        else
        {
            restart = 1;
        }
    }
    /*else if (array[xi][yi] == R(帥))
    {
        if ((xj <= 2 && yj <= 5 && yj >=3)&&(redOrBlack(xj, yj) != 1) && (((xj == xi)&&(yj == yi + 1 || yj == yi - 1))||((yj == yi)&&(xj == xi + 1 || xj == xi - 1))))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = R(帥);
        }
        else
        {
            restart = 1;
        }
    }*/

//B（將）----------------------------------------
    else if (array[xi][yi] == B(王))
    {
        if ((redOrBlack(xj, yj) != -1) && (((xj == xi)&&(yj == yi + 1 || yj == yi - 1))||((yj == yi)&&(xj == xi + 1 || xj == xi - 1)) || (xj == xi-1 && yj == yi-1 ) || (xj == xi-1 && yj == yi+1 ) || (xj == xi+1 && yj == yi-1 ) || (xj == xi+1 && yj == yi+1 )))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(王);
        }
        else
        {
            restart = 1;
        }
        /*if ((xj >= 8 && yj <= 5 && yj >=3)&&(redOrBlack(xj, yj) != -1) && (((xj == xi)&&(yj == yi + 1 || yj == yi - 1))||((yj == yi)&&(xj == xi + 1 || xj == xi - 1))))
        {
            array[xi][yi] = CROSS;
            array[xj][yj] = B(將);
        }
        else
        {
            restart = 1;
        }*/
    }
    else {
        restart = 1;
    }
}

