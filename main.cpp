#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <curses.h>
#include <chrono>

const int BOARD_X = 30;
const int BOARD_Y = 10;

typedef struct {
    int x;
    int y;
} Entity;

void init_curses_screen() {
    initscr();
    raw();
    curs_set(0);
    nodelay(stdscr, true);
    timeout(0);
}

void close_curses_screen() {
    endwin();
}

int gen_random_int(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(min, max);
    return distr(gen);
}

void gen_enemy(std::vector<Entity> &enemyVec){
    for (int i = 0; i < gen_random_int(4, 10); i++) {
        enemyVec.push_back({.x = gen_random_int(1, BOARD_X-2), .y = gen_random_int(-2, 1)});
    }
}

void game() {
    init_curses_screen();
    int ch;
    size_t point = 0;
    bool running = true;
    Entity player = {BOARD_X/2-1, BOARD_Y-3};
    Entity beam = {player.x, player.y};
    Entity snapshot = player;

    // ENEMY
    std::vector<Entity> enemy;
    gen_enemy(enemy);
    // END GEN ENEMY

    auto beam_timeout = std::chrono::high_resolution_clock::now();
    auto beam_speed = std::chrono::high_resolution_clock::now();
    auto enemy_speed = std::chrono::high_resolution_clock::now();
    auto enemy_spawn_speed = std::chrono::high_resolution_clock::now();
    while (running == true) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto beam_timeout_internal = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - beam_timeout);
        auto beam_speed_internal = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - beam_speed);
        auto enemy_speed_internal = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - enemy_speed);
        auto enemy_spawn_speed_internal = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - enemy_spawn_speed);
        if (beam_timeout_internal.count() >= 0.5){
            if (beam.y <= 0 || beam.y >= BOARD_Y) {
                beam = {player.x, player.y};
                beam_timeout = current_time;
            }
        }
        if (beam_speed_internal.count() >= 0.1){
            beam.y -= 1;
            if (beam.y <= 0) beam = {BOARD_X+1, BOARD_Y+1};
            beam_speed = current_time;
        }
        if (enemy_spawn_speed_internal.count() > 10) {
            gen_enemy(enemy);
            enemy_spawn_speed = current_time;
        }
        if (enemy_speed_internal.count() >= 0.6) {
            for(size_t i = 0; i < enemy.size(); i++) {
                if (enemy[i].y >= BOARD_Y-2) {
                    enemy.erase(enemy.begin()+i);
                } else {
                    enemy[i].y += 1;
                }
            }
            enemy_speed = current_time;
        }
        for (size_t i = 0; i < enemy.size(); i++) {
            if (enemy[i].x == beam.x && enemy[i].y == beam.y) {
                enemy.erase(enemy.begin()+i);
                beam.x = BOARD_X+1;
                point++;
            } else if (enemy[i].x == player.x && enemy[i].y == player.y) {
                close_curses_screen();
                return;
            }
        }
        for (size_t y = 0; y < BOARD_Y; y++) {
            for (size_t x = 0; x < BOARD_X; x++) {
                if (y == 0 || x == 0 || y == BOARD_Y-1 || x == BOARD_X-1) {
                    mvprintw(y, x, "%c", '#');
                } else {
                    mvprintw(y, x, " ");
                }
            }
        } 
        if (beam.y != BOARD_Y+1 && beam.x != BOARD_X+1) mvprintw(beam.y, beam.x, "%c", '|');
        mvprintw(player.y, player.x, "%c", 'M');
        for (const auto indv_en: enemy) {
            if (indv_en.y >= 1) {
                mvprintw(indv_en.y, indv_en.x, "%c", 'W');
            }
        }

        mvprintw(0, 2, "POINT : %lu", point);
        curs_set(0);
        refresh();
        ch = getch();

        if (player.x <= 0 || player.x >= BOARD_X-1 || player.y <= 0 || player.y >= BOARD_Y-1) { player = snapshot;
        } else {
            snapshot = player;
        }
        switch (ch) {
            case 'w': {
                --player.y;
                break;
            }
            case 'a': {
                --player.x;
                break;
            }
            case 's': {
                ++player.y;
                break;
            }
            case 'd': {
                ++player.x;
                break;
            }
            case 'q':{
                close_curses_screen();
                return;
            }
            default: break;
        }
    }
    close_curses_screen();
}

void game_menu(void) {
    init_curses_screen();
    int ch;
    while (1) {
        for (size_t y = 0; y < BOARD_Y; y++) {
            for (size_t x = 0; x < BOARD_X; x++) {
                if (y == 0 || x == 0 || y == BOARD_Y-1 || x == BOARD_X-1) {
                    mvprintw(y, x, "%c", '#');
                } else {
                    mvprintw(y, x, " ");
                }
            }
        }
        std::string title = "SPACECRAFT";
        std::string play_s = "(p) Play";
        std::string quit_s = "(q) Quit";

        size_t halfScX_title = BOARD_X / 2 - (title.length() / 2);
        size_t halfScX_play = BOARD_X /2 - (play_s.length() / 2);
        size_t halfScX_quit = BOARD_X /2 - (quit_s.length() / 2);
        size_t halfScY = BOARD_Y / 4;
        mvprintw(halfScY, halfScX_title, "%s", title.c_str());
        mvprintw(halfScY+2, halfScX_play, "%s", play_s.c_str());
        mvprintw(halfScY+3, halfScX_quit, "%s", quit_s.c_str());
        refresh();
        ch = getch();
        switch (ch) {
            case 'p':{
                game();
                break;
            }
            case 'q':{
                close_curses_screen();
                exit(0);
            }
            default: break;
        }
    }
    close_curses_screen();
}

int main(void) {
    game_menu();
    // game();
    return 0;
}
