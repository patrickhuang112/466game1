#include "Game.hpp"
#include <cmath>
namespace Game {

int Game::game_winner () {
    if (this->turns_taken[1] != MAX_TURNS) {
        if (this->turns_taken[1] == this->turns_taken[2]) {
            printf("TOTAL TURNS TAKEN: %d\n", this->turns_taken[1]); 
        }
        return -1; 
    } 
    if (this->turns_taken[2] != MAX_TURNS) {
        if (this->turns_taken[1] == this->turns_taken[2]) {
            printf("TOTAL TURNS TAKEN: %d\n", this->turns_taken[1]); 
        }
        return -1; 
    } 
    uint8_t p1tiles = 0;
    uint8_t p1strength = 0;
    uint8_t p2tiles = 0;
    uint8_t p2strength = 0;
    for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
        for (uint8_t j = 0; j < BOARD_SIZE; ++j) {
            Piece * piece = (this->board)[i][j];
            uint8_t player = piece->get_player();
            if (player == 1) {
                ++p1tiles;
                p1strength += piece->get_strength(); 
            }
            else if (player == 2) {
                ++p2tiles;
                p2strength += piece->get_strength();
            }
        }
    } 
    if (p1tiles > p2tiles) {
        return 1;
    }
    else if (p2tiles > p1tiles) {
        return 2;
    }
    else if (p1strength > p2strength) {
        return 1; 
    }
    else if (p2strength > p1strength) {
        return 2;
    }
    else {
        return 0;
    }
}

uint8_t Game::get_cur_player() {
    assert(this->cur_player != 0);
    return this->cur_player;
}

void Game::finish_turn() {
    (this->turns_taken)[this->cur_player] += 1; 
    // End of each turn, current player loses half of all their pieces on tiles
    // with more than 1 piece
    for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
        for (uint8_t j = 0; j < BOARD_SIZE; ++j) {
            Piece *p = (this->board)[i][j]; 
            uint8_t strength = p->get_strength();
            uint8_t player = p->get_player();
            if (player == this->cur_player && strength > 1) {
                p->set_strength(strength / 2);
            }
        }
    } 

    this->cur_player = this->cur_player == 1 ? 2 : 1;
    if (this->cur_player == 1) {
        (this->board)[PLAYER1_START_ROW][PLAYER1_START_COL]->set_strength(6);
    }
    else if (this->cur_player == 2) {
        (this->board)[PLAYER2_START_ROW][PLAYER2_START_COL]->set_strength(6); 
    }
}

bool Game::valid_row_col(int row, int col) {
    return (row >= 0 && col >= 0 && row < BOARD_SIZE && col < BOARD_SIZE
            && (this->board)[row][col]->get_player() == this->cur_player);
}

bool Game::make_move(uint8_t src_row, uint8_t src_col, uint8_t dst_row, uint8_t dst_col) {

    Piece *src_piece = (this->board)[src_row][src_col];
    Piece *dst_piece = (this->board)[dst_row][dst_col];
    uint8_t src_player = src_piece->get_player();
    uint8_t dst_player = dst_piece->get_player();
    uint8_t src_strength = src_piece->get_strength();
    uint8_t dst_strength = dst_piece->get_strength();
    // Invalid src
    if (src_player == 0 || src_player != this->cur_player) {
        return false;
    }
    // Tile must be directly above, below, left or right or source
    if (abs(src_row - dst_row) >= 2 || abs(src_col - dst_col) >= 2 || 
        (abs(src_row - dst_row) == 1 && abs(src_col - dst_col) == 1)) {
        return false; 
    }

    // Own tile
    if (dst_player == src_player) {
        if (src_strength == 1) {
            return false;
        }
        uint8_t available = MAX_PIECE_STRENGTH - dst_strength;
        uint8_t delta = available < src_strength / 2 ? available : src_strength / 2;
        dst_piece->set_strength(dst_strength + delta);
        src_piece->set_strength(src_strength - delta);
    }
    // Empty tile
    else if (dst_player == NO_PLAYER) {
        assert(dst_strength == 0);
        if (src_strength == 1) {
            return false;
        }
        src_piece->set_strength(src_strength - src_strength / 2);
        dst_piece->set_strength(src_strength / 2);
        dst_piece->set_player(src_player);
    }
    // Opposing player
    else {
        if (src_strength >= dst_strength) {
            src_piece->set_strength(src_strength - dst_strength); 
            dst_piece->set_strength(0);
            dst_piece->set_player(NO_PLAYER);
        }
        else {
            src_piece->set_strength(0);
            dst_piece->set_strength(dst_strength - src_strength);
            src_piece->set_player(NO_PLAYER);
        }
    }
    assert(src_piece->get_strength() >= 0 && src_piece->get_strength() <= MAX_PIECE_STRENGTH);
    assert(dst_piece->get_strength() >= 0 && dst_piece->get_strength() <= MAX_PIECE_STRENGTH);
    return true;
}

Piece *Game::get_piece(uint8_t row, uint8_t col) {
    assert(row >= 0 && col >= 0 && row < BOARD_SIZE && col < BOARD_SIZE);
    return (this->board)[row][col]; 
}
}