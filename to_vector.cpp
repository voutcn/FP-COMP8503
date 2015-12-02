#include <iostream>
#include <zlib.h>
#include "kseq.h"

using namespace std;

static const int K = 4;
static const int MASK = 255; // 4^K-1
static const int L = 100000;

KSEQ_INIT(gzFile, gzread)

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << "<genome.fna>" << endl;
        exit(1);
    }

    gzFile fp;
    kseq_t *seq;
    fp = gzopen(argv[1], "r");
    seq = kseq_init(fp);

    int cnt[MASK + 1];
    int dna_map[256];
    for (int i = 0; i < 10; ++i) {
        dna_map["ACGTNacgtn"[i]] = "0123201232"[i] - '0';
    }

    int l = kseq_read(seq);

    for (int start = 0; start + L < l; start += L) {
        memset(cnt, 0, sizeof(cnt));
        int value = 0;
        for (int i = start; i < start + L; ++i) {
            value = value * 4 + dna_map[seq->seq.s[i]];
            value &= MASK;
            if (i >= start + K - 1) { cnt[value]++; }
        }
        for (int i = 0; i <= MASK; ++i) {
            cout << cnt[i] * 1.0 / (L - K + 1) << ' ';
        }
        cout << argv[1] << endl;
    }

    return 0;
}
