#include "engine.h"
#include <stdlib.h>
#include <stdio.h>
board board_init(int sizex,int sizey,int win_count,int win_rule){

    board bo;
    bo.cells = malloc(sizeof(cell*)*sizex);
    bo.win_rule = win_rule;
    bo.win_count = win_count;

    for(int i = 0; i< sizex; i++){
        bo.cells[i] = malloc(sizeof(cell)*sizey);
        for(int j = 0; j< sizey; j++){
            bo.cells[i][j].color = 0;
        }
    }

    bo.filled_cells = malloc(sizeof(int));
    bo.size_x = sizex;
    bo.size_y = sizey;

    return bo;
}

void board_free(board bo){
    for(int i = 0; i< bo.size_x; i++){
        free(bo.cells[i]);
    }
    free(bo.cells);
    free(bo.filled_cells);
}

const int directions[4][2] = {{0,1},{1,1},{1,0},{1,-1}};

int in_bounds(board bo, int x, int y){
    if(x<0||x>=bo.size_x||y<0||y>=bo.size_y){
        return 0;
    }
    return 1;
}

int board_play(board bo,int x, int y, int color){

    if(!in_bounds(bo,x,y)){
        fprintf(stderr,"Attempted to play out of bounds!!!");
        return INVALID_MOVE;
    }
    if (bo.cells[x][y].color != 0){
        return INVALID_MOVE;
    }

    bo.cells[x][y].color = color;
    (*bo.filled_cells)++;
    for(int i = 0; i<4; i++){
        int line_lenght = 1;
        int dx = directions[i][0];
        int dy = directions[i][1];

        int a = x+dx;
        int b = y+dy;

        while(in_bounds(bo,a,b)&&bo.cells[a][b].color == color){
            a+=dx;
            b+=dy;

            line_lenght++;
        }
        a = x-dx;
        b = y-dy;

        while(in_bounds(bo,a,b)&&bo.cells[a][b].color == color){
            a-=dx;
            b-=dy;

            line_lenght++;
        }
        if(bo.win_rule == WIN_RULE_EXACT){
            if(bo.win_count == line_lenght){
                return color;
            }
        }else{
            if(bo.win_count <= line_lenght){
                return color;
            }
        }
    }
    if ((*bo.filled_cells) == bo.size_x*bo.size_y){
        return DRAW;
    }
    return PLAYING;
}

