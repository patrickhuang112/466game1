#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//for game logic
#include "Game.hpp"

//for game draw
#include "GameView.hpp"

#include <map>
#include <random>



PlayMode::PlayMode() : game() {
	this->game_view.set_ppu(&(this->ppu));	
	this->game_view.load_textures();

	this->src_set = false;
	this->src_row = -1;
	this->src_col = -1;
	this->game_over = false;
	this->handling_event = false;
	draw_pieces();
}

PlayMode::~PlayMode() {
}

void PlayMode::draw_pieces() {
	for (uint8_t r = 0; r < Game::BOARD_SIZE; ++r) {
		for (uint8_t c = 0; c < Game::BOARD_SIZE; ++c) {
			Game::Piece *p = this->game.get_piece(r,c);	
			if (p->get_strength() == 0) {
				this->game_view.release_sprite(p->get_sprite());	
				p->set_sprite(64);
			}
			else {
				uint8_t sprite_ind = p->get_sprite();
				if (sprite_ind == 64) {
					sprite_ind = this->game_view.create_sprite(p->get_player(), p->get_strength());
					this->game_view.place_sprite(sprite_ind, r, c);
					p->set_sprite(sprite_ind);
				}	
				else {
					this->game_view.update_sprite(sprite_ind, p->get_player(), p->get_strength());
					this->game_view.place_sprite(sprite_ind, r, c);
				}
			}
		}	
	}	
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	this->handling_event = true;
	if (this->game_over) {
		this->handling_event = false;
		return false;
	}
	if (evt.type == SDL_MOUSEBUTTONUP) {
		uint8_t r;
		uint8_t c;
		// printf("PRESSING HERE %d %d\n", evt.button.x, evt.button.y);
		if (!this->game_view.xy_to_row_col(evt.button.x, evt.button.y, window_size.x, window_size.y, &r, &c)) {
			this->handling_event = false;
			return false;
		}
		if (!this->src_set) {
			if (this->game.valid_row_col(r, c)) {
				this->src_row = r;
				this->src_col = c;
				this->src_set = true;	
				Game::Piece *p = this->game.get_piece(r, c);
				uint8_t sprite_ind = p->get_sprite();
				assert(sprite_ind != 64);
				this->game_view.select_sprite(sprite_ind);
			}
		}	
		else {
			// Unselect by reclicking src
			if (this->src_row == r && this->src_col == c) {
				this->src_set = false;	
				Game::Piece *p = this->game.get_piece(r, c);
				uint8_t sprite_ind = p->get_sprite();
				this->game_view.update_sprite(sprite_ind, p->get_player(), p->get_strength());
			}	
			else {
				bool result = this->game.make_move((uint8_t)this->src_row, 
					(uint8_t)this->src_col, r, c);	
				if (result) {
					this->src_set = false;
					draw_pieces();
				}
			}
		}
		this->handling_event = false;
		return true;
	}
	else if (evt.type == SDL_KEYUP && evt.key.keysym.sym == SDLK_RETURN) {
		this->game.finish_turn();	
		draw_pieces();
		int winner = this->game.game_winner();
		printf("TURN FINISHED, SWITCHING PLAYERS!\n");
		if (winner >= 0) {
			this->game_view.display_winner(uint8_t(winner));
			this->game_over = true;	
			switch(winner) {
				case 0:
					printf("TIE GAME!");
					break;
				case 1:
					printf("PLAYER 1 WINS!");
					break;
				case 2:	
					printf("PLAYER 2 WINS!");
					break;
			}
		}
		this->handling_event = false;
		return true;
	}
	this->handling_event = false;
	return false;
}

void PlayMode::update(float elapsed) {
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	if (this->handling_event) {
		return;	
	}	
	//--- set ppu state based on game state ---
	for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
		for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
			ppu.background[x+PPU466::BackgroundWidth*y] = 0;
		}
	}
	for (const auto& rc : GameView::BOARD_OUTLINE_POSITIONS) {
		ppu.background[rc.c+PPU466::BackgroundWidth*rc.r] = 0b0000000100000000;
	}
	//--- actually draw ---
	ppu.draw(drawable_size);
}
