#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <curses.h>
#include <chrono>

const size_t BOARD_X = 30;
const size_t BOARD_Y = 10;

typedef struct {
    size_t x;
    size_t y;
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
    bool running = true;
    Entity player = {BOARD_X/2-1, BOARD_Y-3};
    Entity beam = {player.x, player.y};
    Entity snapshot = player;
    auto beam_timeout = std::chrono::high_resolution_clock::now();
    auto beam_speed = std::chrono::high_resolution_clock::now();
    while (running == true) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto beam_timeout_internal = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - beam_timeout);
        auto beam_speed_internal = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - beam_speed);
        if (beam_timeout_internal.count() >= 0.5){
            if (beam.y <= 0 || beam.y >= BOARD_Y) {
                beam = {player.x, player.y};
                beam_timeout = current_time;
            }
        }
        if (beam_speed_internal.count() >= 0.1){
            beam.y -= 1;
            beam_speed = current_time;
        }
        for (size_t y = 0; y < BOARD_Y; y++) {
            for (size_t x = 0; x < BOARD_X; x++) {
                if (y == 0 || x == 0 || y == BOARD_Y-1 || x == BOARD_X-1) {
                    mvprintw(y, x, "%c", '#');
                } else if (y == player.y && x == player.x) {
                    mvprintw(y, x, "%c", 'M');
                } else if (x == beam.x && y == beam.y) {
                        mvprintw(y, x, "%c", '|');
                } else {
                        mvprintw(y, x, " ");
                    }
            }
        } 
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
