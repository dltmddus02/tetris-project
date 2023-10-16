#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}


void InitTetris(){
    int i,j;
    for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
    blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* 그림자 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

    /* nextBlock 보여주는 공간의 테두리를 그린다.*/
    move(9,WIDTH+10);
    DrawBox(9,WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

    for( i=0;i<4;i++){
        move(10+i,WIDTH+13);
        for(j=0;j<4;j++){
            if(block[nextBlock[2]][0][i][j]==1){
                attron(A_REVERSE);
                printw(" ");
                attroff(A_REVERSE);
            }
            else printw(" ");
        }
    }
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
                if(tile=='.')
                    printw("%c",tile);
                else
                    attron(A_REVERSE);
                    printw("%c",tile);
				    attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
    for (int i=0;i<BLOCK_HEIGHT;i++){
        for(int j=0;j<BLOCK_WIDTH;j++){
            if(block[currentBlock][blockRotate][i][j]){
                if(f[blockY+i][blockX+j])
                    return 0;
                else if(blockY+i>=HEIGHT)
                    return 0;
                else if(blockX+j>=WIDTH)
                    return 0;
                else if(blockX+j<0)
                    return 0;
            }
        }
    }
    return 1;

}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다.
    int oldR=blockRotate;
    int oldX=blockX;
    int oldY=blockY;

    if(command==KEY_UP)
        oldR=(oldR+3)%4;
    else if(command==KEY_DOWN)
        oldY--;
    else if(command==KEY_RIGHT)
        oldX--;
    else if(command==KEY_LEFT)
        oldX++;

    //이전 위치 찾았으니 이제 이전 블록 지워야됨
    DrawBlock(oldY,oldX,currentBlock,oldR,'.');
    int check=1;
    while(CheckToMove(field, currentBlock, oldR, oldY, oldX)){
        oldY++;
    }
    oldY--;

    DrawBlock(oldY,oldX,currentBlock,oldR,'.');
    DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);


}

void BlockDown(int sig){
	// user codd
	
    //강의자료 p26-27의 플로우차트를 참고한다.
    int check=CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX);
    if(check){
        blockY++;
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
        move(HEIGHT, WIDTH+10);
    }
    else if(check==0 && blockY==-1){
        gameOver=1;
        return;
    }
    else if(check==0 && blockY!=-1){
        score+=AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
        score+=DeleteLine(field);
        
        nextBlock[0]=nextBlock[1];
        nextBlock[1]=nextBlock[2];
        blockRotate=0;
        blockY=-1;
        blockX=WIDTH/2-2;
        nextBlock[2]=rand()%7;

        DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
        DrawNextBlock(nextBlock);
        PrintScore(score);
        DrawField();
    }
    timed_out=0;


}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

    int touched=0;
	//Block이 추가된 영역의 필드값을 바꾼다.
    for(int i=0;i<BLOCK_HEIGHT;i++){
        for(int j=0;j<BLOCK_WIDTH;j++){
            if(block[currentBlock][blockRotate][i][j]){
                f[blockY+i][blockX+j]=1;
                if(blockY+i==HEIGHT-1 || f[blockY+i+1][blockX+j]==1) touched++;
            }
        }
    }
    int score=touched*10;
    return score;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
    int count=0;

    for(int i=0;i<HEIGHT;i++){
        int flag=0;
        for(int j=0;j<WIDTH;j++){
            if(f[i][j]) //필드 1이면 flag추가
                flag++;        
        }
        if(flag==WIDTH){ //필드 다 찬 경우
            count++;
            for(int j=i;j>0;j--){
                for(int l=0;l<WIDTH;l++){
                    f[j][l]=f[j-1][l];
                }
            }
        }
    }
    return count*count*100;

}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
    
    while(CheckToMove(field, blockID, blockRotate, y, x)){
        y+=1;
    }
    y--;
    DrawBlock(y, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    DrawShadow(y,x,blockID,blockRotate);
    DrawBlock(y,x,blockID,blockRotate,' ');
}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
