#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include "setting.hpp"
#include "common.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "search.hpp"
#include "transpose_table.hpp"
#include "level.hpp"
#if USE_MULTI_THREAD
    #include "thread_pool.hpp"
#endif

using namespace std;

int nega_alpha(board *b, bool skipped, int depth, int alpha, int beta, int *n_nodes);
inline bool mpc_higher(board *b, bool skipped, int depth, int beta, double t, int *n_nodes);
inline bool mpc_lower(board *b, bool skipped, int depth, int alpha, double t, int *n_nodes);

int nega_alpha_ordering_nomemo(board *b, bool skipped, int depth, int alpha, int beta, bool use_mpc, double mpct_in, int *n_nodes){
    if (!global_searching)
        return -inf;
    if (depth <= simple_mid_threshold)
        return nega_alpha(b, skipped, depth, alpha, beta, n_nodes);
    ++(*n_nodes);
    #if USE_MID_SC
        if (stability_cut(b, &alpha, &beta))
            return alpha;
    #endif
    #if USE_MID_MPC
        if (mpc_min_depth <= depth && depth <= mpc_max_depth){
            if (mpc_higher(b, skipped, depth, beta, mpct_in, n_nodes))
                return beta;
            if (mpc_lower(b, skipped, depth, alpha, mpct_in, n_nodes))
                return alpha;
        }
    #endif
    unsigned long long legal = b->mobility_ull();
    if (legal == 0){
        if (skipped)
            return end_evaluate(b);
        b->p = 1 - b->p;
        int res = -nega_alpha_ordering_nomemo(b, true, depth, -beta, -alpha, use_mpc, mpct_in, n_nodes);
        b->p = 1 - b->p;
        return res;
    }
    vector<pair<int, int>> policies;
    mobility mob;
    int hash = b->hash() & search_hash_mask;
    for (const int &cell: vacant_lst){
        if (1 & (legal >> cell))
            policies.emplace_back(make_pair(cell, move_ordering(b, hash, cell)));
    }
    int canput = (int)policies.size();
    if (canput >= 2)
        sort(policies.begin(), policies.end(), move_ordering_sort_int_int);
    int g, v = -inf;
    for (pair<int, int> &policy: policies){
        calc_flip(&mob, b, policy.first);
        b->move(&mob);
        g = -nega_alpha_ordering_nomemo(b, false, depth - 1, -beta, -alpha, use_mpc, mpct_in, n_nodes);
        b->undo(&mob);
        if (beta <= g)
            return g;
        alpha = max(alpha, g);
        v = max(v, g);
    }
    return v;
}

inline bool mpc_higher(board *b, bool skipped, int depth, int beta, double t, int *n_nodes){
    int bound = beta + ceil(t * mpcsd[b->phase()][depth - mpc_min_depth]);
    if (bound > hw2)
        bound = hw2; //return false;
    return nega_alpha_ordering_nomemo(b, skipped, mpcd[depth], bound - search_epsilon, bound, true, t, n_nodes) >= bound;
}

inline bool mpc_lower(board *b, bool skipped, int depth, int alpha, double t, int *n_nodes){
    int bound = alpha - ceil(t * mpcsd[b->phase()][depth - mpc_min_depth]);
    if (bound < -hw2)
        bound = -hw2; //return false;
    return nega_alpha_ordering_nomemo(b, skipped, mpcd[depth], bound, bound + search_epsilon, true, t, n_nodes) <= bound;
}

int nega_alpha(board *b, bool skipped, int depth, int alpha, int beta, int *n_nodes){
    if (!global_searching)
        return -inf;
    ++(*n_nodes);
    if (depth == 0)
        return mid_evaluate(b);
    #if USE_MID_SC
        if (stability_cut(b, &alpha, &beta))
            return alpha;
    #endif
    int g, v = -inf;
    unsigned long long legal = b->mobility_ull();
    if (legal == 0){
        if (skipped)
            return end_evaluate(b);
        b->p = 1 - b->p;
        int res = -nega_alpha(b, true, depth, -beta, -alpha, n_nodes);
        b->p = 1 - b->p;
        return res;
    }
    mobility mob;
    for (const int &cell: vacant_lst){
        if (1 & (legal >> cell)){
            calc_flip(&mob, b, cell);
            b->move(&mob);
            g = -nega_alpha(b, false, depth - 1, -beta, -alpha, n_nodes);
            b->undo(&mob);
            alpha = max(alpha, g);
            if (beta <= alpha)
                return alpha;
            v = max(v, g);
        }
    }
    return v;
}

int nega_alpha_ordering(board *b, bool skipped, const int depth, int alpha, int beta, bool use_multi_thread, bool use_mpc, double mpct_in, int *n_nodes){
    if (!global_searching)
        return -inf;
    if (depth <= simple_mid_threshold)
        return nega_alpha(b, skipped, depth, alpha, beta, n_nodes);
    ++(*n_nodes);
    #if USE_MID_SC
        if (stability_cut(b, &alpha, &beta))
            return alpha;
    #endif
    int hash = b->hash() & search_hash_mask;
    int l, u;
    transpose_table.get_now(b, hash, &l, &u);
    #if USE_MID_TC
        if (u == l)
            return u;
        if (l >= beta)
            return l;
        if (alpha >= u)
            return u;
    #endif
    alpha = max(alpha, l);
    beta = min(beta, u);
    #if USE_MID_MPC
        if (mpc_min_depth <= depth && depth <= mpc_max_depth && use_mpc){
            if (mpc_higher(b, skipped, depth, beta, mpct_in, n_nodes))
                return beta;
            if (mpc_lower(b, skipped, depth, alpha, mpct_in, n_nodes))
                return alpha;
        }
    #endif
    unsigned long long legal = b->mobility_ull();
    if (legal == 0){
        if (skipped)
            return end_evaluate(b);
        b->p = 1 - b->p;
        int res = -nega_alpha_ordering(b, true, depth, -beta, -alpha, use_multi_thread, use_mpc, mpct_in, n_nodes);
        b->p = 1 - b->p;
        return res;
    }
    vector<pair<int, int>> policies;
    mobility mob;
    for (const int &cell: vacant_lst){
        if (1 & (legal >> cell))
            policies.emplace_back(make_pair(cell, move_ordering(b, hash, cell)));
    }
    int canput = (int)policies.size();
    if (canput >= 2)
        sort(policies.begin(), policies.end(), move_ordering_sort_int_int);
    int first_alpha = alpha, g, v = -inf;
    #if USE_MULTI_THREAD
        if (use_multi_thread){
            int i;
            calc_flip(&mob, b, policies[0].first);
            b->move(&mob);
            g = -nega_alpha_ordering(b, false, depth - 1, -beta, -alpha, true, use_mpc, mpct_in, n_nodes);
            b->undo(&mob);
            alpha = max(alpha, g);
            if (beta <= alpha){
                if (l < alpha)
                    transpose_table.reg(b, hash, alpha, u);
                return alpha;
            }
            v = max(v, g);
            vector<future<int>> future_tasks;
            vector<board> nb;
            vector<int> n_n_nodes;
            for (i = 1; i < canput; ++i){
                n_n_nodes.emplace_back(0);
                calc_flip(&mob, b, policies[i].first);
                nb.emplace_back(b->move_copy(&mob));
            }
            for (i = 1; i < canput; ++i)
                future_tasks.emplace_back(thread_pool.push(bind(&nega_alpha_ordering, &nb[i - 1], false, depth - 1, -beta, -alpha, false, use_mpc, mpct_in, &(n_n_nodes[i - 1]))));
            for (i = 1; i < canput; ++i){
                g = -future_tasks[i - 1].get();
                alpha = max(alpha, g);
                v = max(v, g);
                *n_nodes += n_n_nodes[i - 1];
            }
            if (beta <= alpha){
                if (l < alpha)
                    transpose_table.reg(b, hash, alpha, u);
                return alpha;
            }
        } else{
            for (int i = 0; i < canput; ++i){
                calc_flip(&mob, b, policies[i].first);
                b->move(&mob);
                g = -nega_alpha_ordering(b, false, depth - 1, -beta, -alpha, false, use_mpc, mpct_in, n_nodes);
                b->undo(&mob);
                alpha = max(alpha, g);
                if (beta <= alpha){
                    if (l < g)
                        transpose_table.reg(b, hash, g, u);
                    return alpha;
                }
                v = max(v, g);
            }
        }
    #else
        for (pair<int, int> &policy: policies){
            calc_flip(&mob, b, policy.first);
            b->move(&mob);
            g = -nega_alpha_ordering(b, false, depth - 1, -beta, -alpha, false, use_mpc, mpct_in, n_nodes);
            b->undo(&mob);
            transpose_table.child_reg(b, hash, policy.first, g);
            alpha = max(alpha, g);
            if (beta <= alpha){
                if (l < alpha)
                    transpose_table.reg(b, hash, alpha, u);
                return alpha;
            }
            v = max(v, g);
        }
    #endif
    if (v <= first_alpha)
        transpose_table.reg(b, hash, l, v);
    else
        transpose_table.reg(b, hash, v, v);
    return v;
}

int nega_scout_nomemo(board *b, bool skipped, const int depth, int alpha, int beta, bool use_mpc, double mpct_in, int *n_nodes){
    if (!global_searching)
        return -inf;
    if (depth <= simple_mid_threshold)
        return nega_alpha(b, skipped, depth, alpha, beta, n_nodes);
    ++(*n_nodes);
    #if USE_MID_SC
        if (stability_cut(b, &alpha, &beta))
            return alpha;
    #endif
    #if USE_MID_MPC
        if (mpc_min_depth <= depth && depth <= mpc_max_depth && use_mpc){
            if (mpc_higher(b, skipped, depth, beta, mpct_in, n_nodes))
                return beta;
            if (mpc_lower(b, skipped, depth, alpha, mpct_in, n_nodes))
                return alpha;
        }
    #endif
    unsigned long long legal = b->mobility_ull();
    if (legal == 0){
        if (skipped)
            return end_evaluate(b);
        b->p = 1 - b->p;
        int res = -nega_scout_nomemo(b, true, depth, -beta, -alpha, use_mpc, mpct_in, n_nodes);
        b->p = 1 - b->p;
        return res;
    }
    vector<pair<int, int>> policies;
    mobility mob;
    int hash = b->hash() & search_hash_mask;
    for (const int &cell: vacant_lst){
        if (1 & (legal >> cell))
            policies.emplace_back(make_pair(cell, move_ordering(b, hash, cell)));
    }
    int canput = (int)policies.size();
    if (canput >= 2)
        sort(policies.begin(), policies.end(), move_ordering_sort_int_int);
    int g = alpha, v = -inf;
    #if USE_MULTI_THREAD && false
        int i;
        g = -nega_scout_nomemo(&nb[0], false, depth - 1, -beta, -alpha, use_mpc, mpct_in);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
        int first_alpha = alpha;
        vector<future<int>> future_tasks;
        vector<bool> re_search;
        vector<int> n_n_nodes;
        for (i = 1; i < canput; ++i){
            n_n_nodes.emplace_back(0);
            future_tasks.emplace_back(thread_pool.push(bind(&nega_alpha_ordering, &nb[i], false, depth - 1, -alpha - search_epsilon, -alpha, false, use_mpc, mpct_in, &n_n_nodes[i])));
        }
        for (i = 1; i < canput; ++i){
            g = -future_tasks[i - 1].get();
            alpha = max(alpha, g);
            v = max(v, g);
            re_search.emplace_back(first_alpha < g);
            *n_nodes += n_n_nodes[i - 1];
        }
        for (i = 1; i < canput; ++i){
            if (re_search[i - 1]){
                g = -nega_scout_nomemo(&nb[i], false, depth - 1, -beta, -alpha, use_mpc, mpct_in, n_nodes);
                if (beta <= g){
                    if (l < g)
                        transpose_table.reg(b, hash, g, u);
                    return g;
                }
                alpha = max(alpha, g);
                v = max(v, g);
            }
        }
    #else
        for (int i = 0; i < canput; ++i){
            calc_flip(&mob, b, policies[i].first);
            b->move(&mob);
            if (i > 0){
                g = -nega_alpha_ordering_nomemo(b, false, depth - 1, -alpha - search_epsilon, -alpha, use_mpc, mpct_in, n_nodes);
                if (beta < g){
                    b->undo(&mob);
                    return g;
                }
            }
            if (alpha <= g){
                alpha = g;
                g = -nega_scout_nomemo(b, false, depth - 1, -beta, -alpha, use_mpc, mpct_in, n_nodes);
                alpha = max(alpha, g);
                if (beta <= alpha){
                    b->undo(&mob);
                    return alpha;
                }
                v = max(v, g);
            }
            b->undo(&mob);
        }
    #endif
    return v;
}

int mtd(board *b, bool skipped, int depth, int l, int u, bool use_mpc, double use_mpct, int *n_nodes){
    int g, beta;
    g = nega_alpha(b, skipped, 5, l, u, n_nodes);
    while (u - l > 0){
        beta = max(l + search_epsilon, g);
        g = nega_alpha_ordering(b, skipped, depth, beta - search_epsilon, beta, true, use_mpc, use_mpct, n_nodes);
        if (g < beta)
            u = g;
        else
            l = g;
    }
    return g;
    //return nega_scout_nomemo(b, skipped, depth, l, u, use_mpc, use_mpct);
}

inline search_result midsearch(board b, long long strt, int max_depth, bool use_mpc, double use_mpct){
    vector<pair<int, int>> policies;
    mobility mob;
    int i;
    int hash = b.hash() & search_hash_mask;
    unsigned long long legal = b.mobility_ull();
    for (const int &cell: vacant_lst){
        if (1 & (legal >> cell))
            policies.emplace_back(make_pair(cell, move_ordering(&b, hash, cell)));
    }
    int canput = (int)policies.size();
    //cerr << "canput: " << canput << endl;
    int res_depth = -1;
    int policy = -1;
    int tmp_policy;
    int alpha, beta, g, value = -inf, former_value = -inf;
    int searched_nodes = 0;
    transpose_table.hash_get = 0;
    transpose_table.hash_reg = 0;
    transpose_table.init_now();
    transpose_table.init_prev();
    for (int depth = min(16, max(0, max_depth - 5)); depth <= min(hw2 - b.n, max_depth); ++depth){
        alpha = -hw2;
        beta = hw2;
        transpose_table.init_now();
        for (i = 0; i < canput; ++i)
            policies[i].second = move_ordering(&b, hash, policies[i].first);
        if (canput >= 2)
            sort(policies.begin(), policies.end(), move_ordering_sort_int_int);
        calc_flip(&mob, &b, policies[0].first);
        b.move(&mob);
        g = -mtd(&b, false, depth, -beta, -alpha, use_mpc, use_mpct, &searched_nodes);
        b.undo(&mob);
        transpose_table.child_reg(&b, hash, policies[0].first, g);
        alpha = max(alpha, g);
        tmp_policy = policies[0].first;
        for (i = 1; i < canput; ++i){
            calc_flip(&mob, &b, policies[i].first);
            b.move(&mob);
            g = -mtd(&b, false, depth, -beta, -g, use_mpc, use_mpct, &searched_nodes);
            b.undo(&mob);
            transpose_table.child_reg(&b, hash, policies[i].first, g);
            if (alpha < g){
                alpha = g;
                tmp_policy = policies[i].first;
            }
        }
        swap(transpose_table.now, transpose_table.prev);
        if (global_searching){
            policy = tmp_policy;
            if (value != -inf)
                former_value = value;
            else
                former_value = alpha;
            value = alpha;
            res_depth = depth;
            cerr << "depth: " << depth << " time: " << tim() - strt << " policy: " << policy << " value: " << alpha << " nodes: " << searched_nodes << " nps: " << (long long)searched_nodes * 1000 / max(1LL, tim() - strt) << endl;
        } else 
            break;
    }
    search_result res;
    res.policy = policy;
    res.value = (value + former_value) / 2;
    res.depth = res_depth;
    res.nps = searched_nodes * 1000 / max(1LL, tim() - strt);
    return res;
}

inline search_result midsearch_value(board b, long long strt, int max_depth, bool use_mpc, double use_mpct){
    int searched_nodes = 0;
    int value = nega_scout_nomemo(&b, false, max_depth, -hw2, hw2, use_mpc, use_mpct, &searched_nodes);
    search_result res;
    res.policy = -1;
    res.value = value;
    //cerr << res.value << endl;
    res.depth = max_depth;
    res.nps = searched_nodes * 1000 / max(1LL, tim() - strt);
    return res;
}

inline search_result midsearch_value_book(board b, long long strt, int max_depth, bool use_mpc, double use_mpct){
    transpose_table.init_now();
    transpose_table.init_prev();
    int searched_nodes = 0;
    int value = mtd(&b, false, max_depth - 1, -hw2, hw2, use_mpc, use_mpct, &searched_nodes);
    //int value = nega_alpha_ordering_nomemo(&b, false, max_depth, -hw2, hw2, use_mpc, use_mpct);
    swap(transpose_table.now, transpose_table.prev);
    transpose_table.init_now();
    value += mtd(&b, false, max_depth, -hw2, hw2, use_mpc, use_mpct, &searched_nodes);
    search_result res;
    res.policy = -1;
    res.value = value / 2;
    //cerr << res.value << endl;
    res.depth = max_depth;
    res.nps = searched_nodes * 1000 / max(1LL, tim() - strt);
    return res;
}

inline search_result midsearch_value_nomemo(board b, long long strt, int max_depth, bool use_mpc, double use_mpct){
    //int value = nega_alpha_ordering_nomemo(&b, false, max_depth, -hw2, hw2, use_mpc, use_mpct);
    int searched_nodes = 0;
    int value = nega_scout_nomemo(&b, false, max_depth, -hw2, hw2, use_mpc, use_mpct, &searched_nodes);
    search_result res;
    res.policy = -1;
    res.value = value;
    res.depth = max_depth;
    res.nps = searched_nodes * 1000 / max(1LL, tim() - strt);
    return res;
}