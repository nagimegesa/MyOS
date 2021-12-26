void hlt(void);

void OSMain(void)
{
fin:
    hlt();
    goto fin;
}
