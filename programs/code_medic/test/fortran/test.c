#include        <stdio.h>

extern  void fsub_ (int* values, int* sum);

int main (int argc, char* argv[]) {

        int values[100];
        int i;
        int sum;

        for (i=0; i<100; i++) {
                values[i] = i;
        }

        fsub_(&values[0], &sum);

        return 0;
}
