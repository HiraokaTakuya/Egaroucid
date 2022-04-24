#include <iostream>
#include "./../ai.hpp"

using namespace std;

int main(int argc, char *argv[]){
    bit_init();
    flip_init();
    board_init();
    evaluate_init();
    //book_init();
    parent_transpose_table.first_init();
    child_transpose_table.first_init();
    #if USE_MULTI_THREAD
        thread_pool.resize(16);
    #endif
    int level = 60;
    constexpr int book_error = 0;
    Board board;
    Search_result result;
    uint64_t strt, elapsed;
    while (true){
        board = input_board();
        strt = tim();
        result = ai(board, level, true, book_error);
        elapsed = tim() - strt;
        cout << "depth " << (HW2 - board.n) << " value " << result.value << " policy " << idx_to_coord(result.policy) << " nodes " << result.nodes << " time " << elapsed << " nps " << result.nodes * 1000 / max(1ULL, elapsed) << endl;
    }

    return 0;
}
