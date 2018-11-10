{
    int c;
    bool a = true;
    bool b = !a;

    bool d = -a; /* Error due to not arithmetic and type mismatch */
    bool e = !c;
}
