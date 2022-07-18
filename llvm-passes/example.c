void bar();
void baz();
void bam();


void foo() {
    bar();
}

void bar() {
    baz();
    bam();
}

void baz() {
    // hw access
}

void bam() {
    baz();
}
