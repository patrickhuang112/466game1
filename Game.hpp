#pragma once

#include <Stdint.h>
#include <vector>
#include <array>
#include <memory>
#include <cassert>

namespace Game {
constexpr uint8_t BOARD_SIZE = 7;
constexpr uint8_t PLAYER1_START_ROW = 0;
constexpr uint8_t PLAYER1_START_COL = 0;
constexpr uint8_t PLAYER2_START_ROW = BOARD_SIZE - 1;
constexpr uint8_t PLAYER2_START_COL = BOARD_SIZE - 1;
constexpr uint8_t INITIAL_PIECE_STRENGTH = 1;
constexpr uint8_t MAX_PIECE_STRENGTH = 6;
constexpr uint8_t NO_PLAYER = 0;
constexpr uint8_t NUM_PLAYERS = 2;
constexpr uint8_t MAX_TURNS = 3;

struct Piece {  
    Piece (uint8_t row, uint8_t col) {
        this->player = NO_PLAYER; 
        this->strength = 0;
        this->row = row;
        this->col = col;
        this->sprite_ind = 64; // INVALID AS PER PPU466
    }
    ~Piece() = default;

    uint8_t get_strength (void) { return this->strength; }
    uint8_t get_player(void) { return this->player; }
    void set_strength(uint8_t s) { this->strength = s; }
    void set_player(uint8_t p) {this->player = p; } 

    // Draw related functions
    uint8_t get_sprite(void) {return this->sprite_ind; }
    void set_sprite(uint8_t s) { this->sprite_ind = s; }

    private:
        uint8_t player;    
        uint8_t strength;
        uint8_t row;
        uint8_t col;
        uint8_t sprite_ind;
};

struct Game {
    Game () {
        // Initialize game board        
        for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
            for (uint8_t j = 0; j < BOARD_SIZE; ++j) {
                (this->board)[i][j] = new Piece(i,j); 
            }
        } 

        // Initialize 2 player game
        (this->board)[PLAYER1_START_ROW][PLAYER1_START_COL]->set_player(1);
        (this->board)[PLAYER2_START_ROW][PLAYER2_START_COL]->set_player(2);

        (this->board)[PLAYER1_START_ROW][PLAYER1_START_COL]->set_strength(6);
        (this->board)[PLAYER2_START_ROW][PLAYER2_START_COL]->set_strength(6);
        this->cur_player = 1;

        turns_taken[1] = 0;
        turns_taken[2] = 0;
    }

    ~Game () {
        // Clean up game board pieces
        for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
            for (uint8_t j = 0; j < BOARD_SIZE; ++j) {
                assert((this->board)[i][j] != nullptr);
                delete ((this->board)[i][j]); 
            }
        } 

    } 
    bool make_move(uint8_t src_row, uint8_t src_col, uint8_t dst_row, uint8_t dst_col); 
    bool valid_row_col(int row, int col);
    Piece *get_piece(uint8_t row, uint8_t col);
    void finish_turn (void); 
    uint8_t get_cur_player(void); 
    // 0 == tie game
    // -1 == no winner (game isn't over yet) 
    // 1 == player 1
    // 2 == player 2
    int game_winner (void);

    private: 
        // 0 indicates NOPLAYER, otherwise number indicates player number
        uint8_t cur_player;
        std::array<uint8_t, NUM_PLAYERS+1> turns_taken;
        std::array<std::array<Piece *, BOARD_SIZE>, BOARD_SIZE> board;
};
}