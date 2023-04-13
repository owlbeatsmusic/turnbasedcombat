#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BASE_DODGE_PROB 0.01

typedef enum {
	SPD, HP, ATK, NRG, DEF, PRC, SNS
} Stat;

typedef struct {
	int always_hit;
	int always_second;
	float hit_prob;
	int speedmult;
	
	int self_SPD, self_HP, self_ATK, self_NRG, self_DEF, self_PRC, self_SNS;
	int opnt_SPD, opnt_HP, opnt_ATK, opnt_NRG, opnt_DEF, opnt_PRC, opnt_SNS;
} Move;

const Move punch = {0, 0, 1.0, 0,
	      0, 0,   0, -5, 0, 0, 0,
	      0, -50, 0, 0,  0, 0, 0};

const Move runkick = {0, 0, 0.9, 1,
	      0, 0,    0, -15, 0, 0, 0,
	      0, -100, 0, 0,   0, 0, 0};

const Move rock_throw = {0, 0, 0.8, 0,
	      0, 0,    0, -10, 0, 0, 0,
	      0, -150, 0, 0,   0, 0, 0};

const Move calm = {1, 0, 1.0, 0,
	      0, 0,   0, 20, 5, 5, 0,
	      0, 0,   0, 0,  0, 0, 0};

const Move attack_mindset = {1, 0, 1.0, 0,
	      0, 0, 10, 0, -10, 10, 0,
	      0, 0, 0,  0, 0,   0,  0};

const Move defense_mindset = {1, 0, 1.0, 0,
	      0, 0, -10, 0, 10, -10, 0,
	      0, 0, 0,   0, 0,  0,   0};

const Move fight_mindset = {1, 0, 1.0, 0,
	      0, 0, 5,  0,  5,  5, 0,
	      0, 0, 0,  0,  0,  0, 0};

const Move powernap = {1, 0, 1.0, 0,
	      0, 0, -5, 40, -5, -5, 0,
	      0, 0, 0,  0,  0,  0,  0};

typedef struct {
	int SPD, HP, ATK, NRG, DEF, PRC, SNS;
	Move* moves;
} Player;


Player create_player() {
	Player player;

	player.SPD = 10;
	player.HP  = 999;
	player.ATK = 10;
	player.NRG = 100;
	player.DEF = 10;
	player.PRC = 100;
	player.SNS = 20;
	
	player.moves = (Move*) malloc(5 * sizeof(Move));
	if (player.moves == NULL) {
		printf("Failed to allocate memory(player.moves)");
	}
	
	return player;
}

int set_player_moves(Player* player, Move m1, Move m2, Move m3, Move m4, Move m5) {
	player->moves[0] = m1;
	player->moves[1] = m2;
	player->moves[2] = m3;
	player->moves[3] = m4;
	player->moves[4] = m5;
	return 0;
}

// self and opnt can be reversed in the context of this function
int add_to_stat(Player* self, Player* opnt, Stat stat, Move move, int value) {
	switch(stat) {
		case SPD:
			self->SPD = value; 
			break;
		case HP :
			if (value < 0 ) self->HP  += value * ((100.0-self->DEF)/100.0) * ((100.0+opnt->ATK)/110.0); // movedamage * defensemult. * attackmult.
			if (value >= 0) self->HP  += value;
			break;
		case ATK: 
			self->ATK += value; 
			break;
		case NRG: 
			self->NRG += value; 
			break;
		case DEF: 
			self->DEF += value; 
			break;
		case PRC: 
			self->PRC += value; 
			break;
		case SNS: 
			self->SNS += value; 
			break;
	}	
	return 0;
}

int do_move(Player* self, Player* opnt, Move move) {
	
	// return if missed/dodged
	if (move.always_hit == 0) {
		float prob_of_hit = move.hit_prob * (self->PRC/100.0) - (BASE_DODGE_PROB) * ((100.0+opnt->SPD)/120.0); // moveprob. * precisionprob - dodgeprob.
		float random = (float)rand() / RAND_MAX;
		if (prob_of_hit < random) { 
			printf("[  MOVE  ]: Missed/Dodged %f < %f\n", prob_of_hit, random);
			return 0;
		}
		else printf("[  MOVE  ]: Hit %f < %f\n", prob_of_hit, random);
	}
	
	// do move
	add_to_stat(self, opnt, SPD, move, move.self_SPD);
	add_to_stat(self, opnt, HP , move, move.self_HP );
	add_to_stat(self, opnt, ATK, move, move.self_ATK);
	add_to_stat(self, opnt, NRG, move, move.self_NRG);
	add_to_stat(self, opnt, DEF, move, move.self_DEF);
	add_to_stat(self, opnt, PRC, move, move.self_PRC);
	add_to_stat(self, opnt, SNS, move, move.self_SNS);

	add_to_stat(opnt, self, SPD, move, move.opnt_SPD);
	add_to_stat(opnt, self, HP , move, move.opnt_HP );
	add_to_stat(opnt, self, ATK, move, move.opnt_ATK);
	add_to_stat(opnt, self, NRG, move, move.opnt_NRG);
	add_to_stat(opnt, self, DEF, move, move.opnt_DEF);
	add_to_stat(opnt, self, PRC, move, move.opnt_PRC);
	add_to_stat(opnt, self, SNS, move, move.opnt_SNS);	
	
	printf("[  MOVE  ]: all add_to_stats' complete \n");
	
	return 0;
}

int main(void) {
	srand(time(NULL));
	
	Player player1 = create_player();
	Player player2 = create_player();

	set_player_moves(&player1, punch, calm, runkick, rock_throw, fight_mindset);
	do_move(&player1, &player2, rock_throw);	

	return 0;
}
