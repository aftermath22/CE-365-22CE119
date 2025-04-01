int main() {
    goto start;
start:
    goto middle;
middle:
    goto end;
end:
    return 0;
}

// int main() {
//     goto label1;
//     goto label2; // label2 is never declared
// label1:
//     return 0;
// }

// int main() {
//     goto entry;
// entry:
//     return 0;
// }
