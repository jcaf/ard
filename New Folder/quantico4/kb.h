#ifndef KB_H_
#define KB_H_

    void kb_job(void);
    void InitTCNT1(void);

    void kb_stackPush(void);
    void kb_stackPop(void);
    void kb_change_keyDo(PTRFX_retVOID *keyDo);
    void kb_change_keyDo_pgm(PTRFX_retVOID const *  keyDo);

    void kb_processKeyRead_D(void);
#endif // KB_H_



