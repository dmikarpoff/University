#include <cstdlib>
#include "DecisionTree.h"

int main(int argc, char** argv) {
    if (argc == 2) {
        int tree_amount = atoi(argv[1]);
        DecisionForest forest(tree_amount);
        forest.loadTrain();
        forest.train();
        forest.check();
        return 0;
    }
    return 0;
}
