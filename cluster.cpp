#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <cstdlib>
#include <cmath>
#include <algorithm>

using namespace std;

#define AVGD 0
#define MAXD 1
#define MIND 2

int mode = AVGD;

double avg_dist(const vector<int> &ids, const vector<int> &ids2, vector<vector<double> > &dist_matrix) {
    double total_dist = 0;
    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < ids2.size(); ++j) {
            total_dist += dist_matrix[ids[i]][ids2[j]];
        }
    }

    return total_dist / ids.size() / ids2.size();
}

double max_dist(const vector<int> &ids, const vector<int> &ids2, vector<vector<double> > &dist_matrix) {
    double maxd = 0;
    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < ids2.size(); ++j) {
            maxd = max(maxd, dist_matrix[ids[i]][ids2[j]]);
        }
    }

    return maxd;
}

double min_dist(const vector<int> &ids, const vector<int> &ids2, vector<vector<double> > &dist_matrix) {
    double mind = 1e59;
    for (int i = 0; i < ids.size(); ++i) {
        for (int j = 0; j < ids2.size(); ++j) {
            mind = min(mind, dist_matrix[ids[i]][ids2[j]]);
        }
    }

    return mind;
}

struct Cluster {
    vector<int> ids;
    string newick; // newick representation of a tree

    Cluster(int id = 0, const string &name = "") {
        ids.push_back(id);
        newick = name;
    }

    void merge(const Cluster &rhs) {
        ids.insert(ids.end(), rhs.ids.begin(), rhs.ids.end());
        newick = "(" + newick + ":1," + rhs.newick + ":1)";
    }

    double dist(const Cluster &rhs, vector<vector<double> > &dist_matrix) {
        switch (mode) {
            case AVGD:
                return avg_dist(ids, rhs.ids, dist_matrix);
            case MAXD:
                return max_dist(ids, rhs.ids, dist_matrix);
            case MIND:
                return min_dist(ids, rhs.ids, dist_matrix);
        }
        return 0;
    }
};

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << "<vector_file> <mode=0|1|2>" << endl;
        exit(1);
    }

    list<Cluster> lc;
    vector<vector<double> > data;

    ifstream in(argv[1]);
    mode = atoi(argv[2]);
    double v;

    while (in >> v) {
        data.push_back(vector<double>());
        data.back().push_back(v);
        for (int i = 0; i < 255; ++i) {
            in >> v;
            data.back().push_back(v);
        }

        string label;
        in >> label;
        lc.push_back(Cluster(lc.size(), label + "_" + to_string(lc.size())));
    }

    // calculating dist
    vector<vector<double> > dist(lc.size(), vector<double>(lc.size()));

    for (int i = 0; i < lc.size(); ++i) {
        for (int j = i + 1; j < lc.size(); ++j) {
            for (int k = 0; k < 256; ++k) {
                dist[i][j] += (data[i][k] - data[j][k]) * (data[i][k] - data[j][k]);
            }
            dist[i][j] = sqrt(dist[i][j]);
            dist[j][i] = dist[i][j];
        }
    }

    // iterative clustering
    while (lc.size() > 1) {
        double mind = 1e59;
        list<Cluster>::iterator merge1, merge2;

        for (list<Cluster>::iterator i = lc.begin(); i != lc.end(); ++i) {
            list<Cluster>::iterator j = i;
            for (++j; j != lc.end(); ++j) {
                double dist_ij = i->dist(*j, dist);
                if (dist_ij < mind) {
                    mind = dist_ij;
                    merge1 = i;
                    merge2 = j;
                }
            }
        }

        merge1->merge(*merge2);
        lc.erase(merge2);
    }

    cout << lc.begin()->newick << ";" << endl;

    return 0;
}
