int main() {
    int a = 0x999999;
    asm volatile("int $0x80");
    while (1)
        ;
}
