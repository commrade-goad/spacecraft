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

int main(void) {
    init_curses_screen();
    int ch;
    size_t point = 0;
    bool running = true;
    Entity player = {BOARD_X/2-1, BOARD_Y-3};
    Entity beam = {player.x, player.y};
    Entity snapshot = player;

    // ENEMY
    std::vector<Entity> enemy;
    for (int i = 0; i < gen_random_int(4, 10); i++) {
        enemy.push_back({.x = gen_random_int(1, BOARD_X-2), .y = gen_random_int(-2, 1)});
    }
    // return 0;
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
            for (size_t i = 0; i < 5; i++) {
                enemy.push_back({.x = gen_random_int(1, BOARD_X-2), .y = gen_random_int(-2, 1)});
            }
            enemy_spawn_speed = current_time;
        }
        if (enemy_speed_internal.count() >= 0.8) {
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
                point++;
            } else if (enemy[i].x == player.x && enemy[i].y == player.y) {
                return 1;
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

        if (ch == 'q') {
            break;
        }

        if (player.x <= 0 || player.x >= BOARD_X-1 || player.y <= 0 || player.y >= BOARD_Y-1) {
            player = snapshot;
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
            default: break;
        }
    }
    close_curses_screen();
    return 0;
}
