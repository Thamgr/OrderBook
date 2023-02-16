#include <bits/stdc++.h>

using namespace std;

mt19937 randMT(time(0));

struct Node {
    int price, volume, prior, size = 1, sum = 0;
    shared_ptr<Node> l = nullptr;
    shared_ptr<Node> r = nullptr;
    Node(int n_price, int n_volume) {
        price = n_price, volume = n_volume, prior = randMT(), sum = volume;
    }
};

class order_book {
    shared_ptr<Node> buy = nullptr;
    shared_ptr<Node> sell = nullptr;
    private:
        int sz(shared_ptr<Node> t) {
            return (t == nullptr ? 0 : t->size);
        }

        int sum(shared_ptr<Node> t) {
            return (t == nullptr ? 0 : t->sum);
        }

        void update(shared_ptr<Node> t) {
            t->size = 1 + sz(t->l) + sz(t->r);
            t->sum = t->volume + sum(t->l) + sum(t->r);
        }

        pair<shared_ptr<Node>, shared_ptr<Node>> split(shared_ptr<Node> t, int k) {
            if (t == nullptr) {
                return {nullptr, nullptr};
            }
            if (sz(t->l) + 1 <= k) {
                auto q = split(t->r, k - sz(t->l) - 1);
                t->r = q.first;
                update(t);
                return {t, q.second};
            }
            else {
                auto q = split(t->l, k);
                t->l = q.second;
                update(t);
                return {q.first, t};
            }
        }

        shared_ptr<Node> merge(shared_ptr<Node> l, shared_ptr<Node> r) {
            if (l == nullptr) {
                return r;
            }
            if (r == nullptr) {
                return l;
            }
            if (l->prior > r->prior) {
                l->r = merge(l->r, r);
                update(l);
                return l;
            }
            else {
                r->l = merge(l, r->l);
                update(r);
                return r;
            }
        }

        int sum(shared_ptr<Node> root, int l, int r) {
            auto t1 = split(root, r);
            auto t2 = split(t1.first, l);
            int res = sum(t2.second);
            root = merge(t2.first, merge(t2.second, t1.second));
            return res;
        }

        shared_ptr<Node> insert(shared_ptr<Node> root, int price, int volume, int ind) {
            auto q = split(root, ind);
            shared_ptr<Node> t(new Node(price, volume));
            return merge(q.first, merge(t, q.second));
        }

        shared_ptr<Node> del(shared_ptr<Node> root, int ind) {
            return split(root, ind).second;
        }

        pair<int, int> get_by_ind(shared_ptr<Node> root, int ind) {
            auto t1 = split(root, ind + 1);
            auto t2 = split(t1.first, 1);
            pair<int, int> res = {t2.first->price, t2.first->volume};
            root = merge(t2.first, merge(t2.second, t1.second));
            return res;
        }
    public:
        void add_order(int side, int price, int volume) {
            if (side == 0) {
                int ind_price = -1, ind_volume = -1;

                int l, r;

                l = -1;
                r = sz(sell);
                while(r - l > 1) {
                    int mid = (r + l) / 2;
                    if (get_by_ind(sell, mid).first >= price) {
                        l = mid;
                    }
                    else {
                        r = mid;
                    }
                }
                ind_price = l;

                l = -1;
                r = sz(sell);
                while(r - l > 1) {
                    int mid = (r + l) / 2;
                    if (sum(sell, 0, mid + 1) <= volume) {
                        l = mid;
                    }
                    else {
                        r = mid;
                    }
                }
                ind_volume = l;

                int ind = min(ind_price, ind_volume);
                if (ind != -1) {
                    volume -= sum(sell, 0, ind + 1);
                    sell = del(sell, ind + 1);
                }

                if (sell != nullptr) {
                    auto best = get_by_ind(sell, 0);
                    if (best.first >= price) {
                        sell = del(sell, 1);
                        sell = insert(sell, best.first, best.second - volume, 0);
                        volume = 0;
                    }
                    if (volume == 0) {
                        return;
                    }
                }

                int new_ind = 0;
                l = -1;
                r = sz(buy);
                while(r - l > 1) {
                    int mid = (r + l) / 2;
                    if (get_by_ind(buy, mid).first <= price) {
                        l = mid;
                    }
                    else {
                        r = mid;
                    }
                }
                new_ind = r;
                buy = insert(buy, price, volume, new_ind);
            }
            else if (side == 1) {
                int ind_price = -1, ind_volume = -1;

                int l, r;

                l = -1;
                r = sz(buy);
                while(r - l > 1) {
                    int mid = (r + l) / 2;
                    if (get_by_ind(buy, mid).first <= price) {
                        l = mid;
                    }
                    else {
                        r = mid;
                    }
                }
                ind_price = l;

                l = -1;
                r = sz(buy);
                while(r - l > 1) {
                    int mid = (r + l) / 2;
                    if (sum(buy, 0, mid + 1) <= volume) {
                        l = mid;
                    }
                    else {
                        r = mid;
                    }
                }
                ind_volume = l;

                int ind = min(ind_price, ind_volume);
                if (ind != -1) {
                    volume -= sum(buy, 0, ind + 1);
                    buy = del(buy, ind + 1);
                }

                if (buy != nullptr) {
                    auto best = get_by_ind(buy, 0);
                    if (best.first <= price) {
                        buy = del(buy, 1);
                        buy = insert(buy, best.first, best.second - volume, 0);
                        volume = 0;
                    }
                    if (volume == 0) {
                        return;
                    }
                }

                int new_ind = 0;
                l = -1;
                r = sz(sell);
                while(r - l > 1) {
                    int mid = (r + l) / 2;
                    if (get_by_ind(sell, mid).first >= price) {
                        l = mid;
                    }
                    else {
                        r = mid;
                    }
                }
                new_ind = r;
                sell = insert(sell, price, volume, new_ind);
            }

        }
        int best_order(int side) {
            if (side == 0) {
                if (buy == nullptr) {
                    return -1;
                }
                return get_by_ind(buy, 0).first;
            }
            else if (side == 1) {
                if (sell == nullptr) {
                    return -1;
                }
                return get_by_ind(sell, 0).first;
            }
        }
};

signed main() {
    /*
        0 - sell
        1 - buy
    */
    order_book book;
    book.add_order(0, 3, 3);
    book.add_order(0, 5, 3);
    cout << book.best_order(0) << endl;
    // best price - 3
    book.add_order(1, 5, 4);
    cout << book.best_order(0);
    // best price - 5
}
