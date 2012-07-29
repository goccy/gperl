	static void *jmp_table[] = {
	    &&L(UNDEF), &&L(MOV), &&L(vMOV),
	    &&L(gMOV), &&L(ARGMOV), &&L(ArrayARGMOV),
	    &&L(ADD), &&L(dADD), &&L(iADD),
	    &&L(sADD), &&L(oADD), &&L(dADDC),
	    &&L(iADDC), &&L(SUB), &&L(dSUB),
	    &&L(iSUB), &&L(oSUB), &&L(dSUBC),
	    &&L(iSUBC), &&L(MUL), &&L(dMUL),
	    &&L(iMUL), &&L(sMUL), &&L(oMUL),
	    &&L(dMULC), &&L(iMULC), &&L(DIV),
	    &&L(dDIV), &&L(iDIV), &&L(oDIV),
	    &&L(dDIVC), &&L(iDIVC), &&L(JG),
	    &&L(dJG), &&L(iJG), &&L(sJG),
	    &&L(oJG), &&L(dJGC), &&L(iJGC),
	    &&L(JL), &&L(dJL), &&L(iJL),
	    &&L(sJL), &&L(oJL), &&L(dJLC),
	    &&L(iJLC), &&L(JGE), &&L(dJGE),
	    &&L(iJGE), &&L(sJGE), &&L(oJGE),
	    &&L(dJGEC), &&L(iJGEC), &&L(JLE),
	    &&L(dJLE), &&L(iJLE), &&L(sJLE),
	    &&L(oJLE), &&L(dJLEC), &&L(iJLEC),
	    &&L(JE), &&L(dJE), &&L(iJE),
	    &&L(sJE), &&L(oJE), &&L(dJEC),
	    &&L(iJEC), &&L(JNE), &&L(dJNE),
	    &&L(iJNE), &&L(sJNE), &&L(oJNE),
	    &&L(dJNEC), &&L(iJNEC), &&L(INC),
	    &&L(dINC), &&L(iINC), &&L(sINC),
	    &&L(oINC), &&L(gINC), &&L(dgINC),
	    &&L(igINC), &&L(sgINC), &&L(ogINC),
	    &&L(DEC), &&L(dDEC), &&L(iDEC),
	    &&L(sDEC), &&L(oDEC), &&L(gDEC),
	    &&L(dgDEC), &&L(igDEC), &&L(sgDEC),
	    &&L(ogDEC), &&L(RET), &&L(THCODE),
	    &&L(NOP), &&L(WRITE), &&L(dWRITE),
	    &&L(iWRITE), &&L(sWRITE), &&L(oWRITE),
	    &&L(FLUSH), &&L(JMP), &&L(LET),
	    &&L(gLET), &&L(SETv), &&L(FUNCSET),
	    &&L(CALL), &&L(SELFCALL), &&L(SHIFT),
	    &&L(dSHIFT), &&L(iSHIFT), &&L(sSHIFT),
	    &&L(oSHIFT), &&L(PUSH), &&L(NEW),
	    &&L(NEW_STRING), &&L(ARRAY_PUSH), &&L(ARRAY_AT),
	    &&L(ARRAY_gAT), &&L(ARRAY_DREF),	    &&L(A_MOV), &&L(B_MOV), &&L(C_MOV),
	    &&L(D_MOV), &&L(A_vMOV), &&L(B_vMOV),
	    &&L(C_vMOV), &&L(D_vMOV), &&L(A_gMOV),
	    &&L(B_gMOV), &&L(C_gMOV), &&L(D_gMOV),
	    &&L(A_ARGMOV), &&L(B_ARGMOV), &&L(C_ARGMOV),
	    &&L(D_ARGMOV), &&L(A_ArrayARGMOV), &&L(B_ArrayARGMOV),
	    &&L(C_ArrayARGMOV), &&L(D_ArrayARGMOV), &&L(AB_ADD),
	    &&L(AB_dADD), &&L(AB_iADD), &&L(AB_sADD),
	    &&L(AB_oADD), &&L(AC_ADD), &&L(AC_dADD),
	    &&L(AC_iADD), &&L(AC_sADD), &&L(AC_oADD),
	    &&L(AD_ADD), &&L(AD_dADD), &&L(AD_iADD),
	    &&L(AD_sADD), &&L(AD_oADD), &&L(BC_ADD),
	    &&L(BC_dADD), &&L(BC_iADD), &&L(BC_sADD),
	    &&L(BC_oADD), &&L(BD_ADD), &&L(BD_dADD),
	    &&L(BD_iADD), &&L(BD_sADD), &&L(BD_oADD),
	    &&L(CD_ADD), &&L(CD_dADD), &&L(CD_iADD),
	    &&L(CD_sADD), &&L(CD_oADD), &&L(A_ADDC),
	    &&L(A_dADDC), &&L(A_iADDC), &&L(A_sADDC),
	    &&L(B_ADDC), &&L(B_dADDC), &&L(B_iADDC),
	    &&L(B_sADDC), &&L(C_ADDC), &&L(C_dADDC),
	    &&L(C_iADDC), &&L(C_sADDC), &&L(D_ADDC),
	    &&L(D_dADDC), &&L(D_iADDC), &&L(D_sADDC),
	    &&L(AB_SUB), &&L(AB_dSUB), &&L(AB_iSUB),
	    &&L(AB_oSUB), &&L(AC_SUB), &&L(AC_dSUB),
	    &&L(AC_iSUB), &&L(AC_oSUB), &&L(AD_SUB),
	    &&L(AD_dSUB), &&L(AD_iSUB), &&L(AD_oSUB),
	    &&L(BC_SUB), &&L(BC_dSUB), &&L(BC_iSUB),
	    &&L(BC_oSUB), &&L(BD_SUB), &&L(BD_dSUB),
	    &&L(BD_iSUB), &&L(BD_oSUB), &&L(CD_SUB),
	    &&L(CD_dSUB), &&L(CD_iSUB), &&L(CD_oSUB),
	    &&L(A_SUBC), &&L(A_dSUBC), &&L(A_iSUBC),
	    &&L(B_SUBC), &&L(B_dSUBC), &&L(B_iSUBC),
	    &&L(C_SUBC), &&L(C_dSUBC), &&L(C_iSUBC),
	    &&L(D_SUBC), &&L(D_dSUBC), &&L(D_iSUBC),
	    &&L(AB_MUL), &&L(AB_dMUL), &&L(AB_iMUL),
	    &&L(AB_sMUL), &&L(AB_oMUL), &&L(AC_MUL),
	    &&L(AC_dMUL), &&L(AC_iMUL), &&L(AC_sMUL),
	    &&L(AC_oMUL), &&L(AD_MUL), &&L(AD_dMUL),
	    &&L(AD_iMUL), &&L(AD_sMUL), &&L(AD_oMUL),
	    &&L(BC_MUL), &&L(BC_dMUL), &&L(BC_iMUL),
	    &&L(BC_sMUL), &&L(BC_oMUL), &&L(BD_MUL),
	    &&L(BD_dMUL), &&L(BD_iMUL), &&L(BD_sMUL),
	    &&L(BD_oMUL), &&L(CD_MUL), &&L(CD_dMUL),
	    &&L(CD_iMUL), &&L(CD_sMUL), &&L(CD_oMUL),
	    &&L(A_MULC), &&L(A_dMULC), &&L(A_iMULC),
	    &&L(A_sMULC), &&L(B_MULC), &&L(B_dMULC),
	    &&L(B_iMULC), &&L(B_sMULC), &&L(C_MULC),
	    &&L(C_dMULC), &&L(C_iMULC), &&L(C_sMULC),
	    &&L(D_MULC), &&L(D_dMULC), &&L(D_iMULC),
	    &&L(D_sMULC), &&L(AB_DIV), &&L(AB_dDIV),
	    &&L(AB_iDIV), &&L(AB_oDIV), &&L(AC_DIV),
	    &&L(AC_dDIV), &&L(AC_iDIV), &&L(AC_oDIV),
	    &&L(AD_DIV), &&L(AD_dDIV), &&L(AD_iDIV),
	    &&L(AD_oDIV), &&L(BC_DIV), &&L(BC_dDIV),
	    &&L(BC_iDIV), &&L(BC_oDIV), &&L(BD_DIV),
	    &&L(BD_dDIV), &&L(BD_iDIV), &&L(BD_oDIV),
	    &&L(CD_DIV), &&L(CD_dDIV), &&L(CD_iDIV),
	    &&L(CD_oDIV), &&L(A_DIVC), &&L(A_dDIVC),
	    &&L(A_iDIVC), &&L(B_DIVC), &&L(B_dDIVC),
	    &&L(B_iDIVC), &&L(C_DIVC), &&L(C_dDIVC),
	    &&L(C_iDIVC), &&L(D_DIVC), &&L(D_dDIVC),
	    &&L(D_iDIVC), &&L(A_JG), &&L(A_dJG),
	    &&L(A_iJG), &&L(A_sJG), &&L(A_oJG),
	    &&L(B_JG), &&L(B_dJG), &&L(B_iJG),
	    &&L(B_sJG), &&L(B_oJG), &&L(C_JG),
	    &&L(C_dJG), &&L(C_iJG), &&L(C_sJG),
	    &&L(C_oJG), &&L(D_JG), &&L(D_dJG),
	    &&L(D_iJG), &&L(D_sJG), &&L(D_oJG),
	    &&L(A_JGC), &&L(A_dJGC), &&L(A_iJGC),
	    &&L(A_sJGC), &&L(A_oJGC), &&L(B_JGC),
	    &&L(B_dJGC), &&L(B_iJGC), &&L(B_sJGC),
	    &&L(B_oJGC), &&L(C_JGC), &&L(C_dJGC),
	    &&L(C_iJGC), &&L(C_sJGC), &&L(C_oJGC),
	    &&L(D_JGC), &&L(D_dJGC), &&L(D_iJGC),
	    &&L(D_sJGC), &&L(D_oJGC), &&L(A_JL),
	    &&L(A_dJL), &&L(A_iJL), &&L(A_sJL),
	    &&L(A_oJL), &&L(B_JL), &&L(B_dJL),
	    &&L(B_iJL), &&L(B_sJL), &&L(B_oJL),
	    &&L(C_JL), &&L(C_dJL), &&L(C_iJL),
	    &&L(C_sJL), &&L(C_oJL), &&L(D_JL),
	    &&L(D_dJL), &&L(D_iJL), &&L(D_sJL),
	    &&L(D_oJL), &&L(A_JLC), &&L(A_dJLC),
	    &&L(A_iJLC), &&L(A_sJLC), &&L(A_oJLC),
	    &&L(B_JLC), &&L(B_dJLC), &&L(B_iJLC),
	    &&L(B_sJLC), &&L(B_oJLC), &&L(C_JLC),
	    &&L(C_dJLC), &&L(C_iJLC), &&L(C_sJLC),
	    &&L(C_oJLC), &&L(D_JLC), &&L(D_dJLC),
	    &&L(D_iJLC), &&L(D_sJLC), &&L(D_oJLC),
	    &&L(A_JGE), &&L(A_dJGE), &&L(A_iJGE),
	    &&L(A_sJGE), &&L(A_oJGE), &&L(B_JGE),
	    &&L(B_dJGE), &&L(B_iJGE), &&L(B_sJGE),
	    &&L(B_oJGE), &&L(C_JGE), &&L(C_dJGE),
	    &&L(C_iJGE), &&L(C_sJGE), &&L(C_oJGE),
	    &&L(D_JGE), &&L(D_dJGE), &&L(D_iJGE),
	    &&L(D_sJGE), &&L(D_oJGE), &&L(A_JGEC),
	    &&L(A_dJGEC), &&L(A_iJGEC), &&L(A_sJGEC),
	    &&L(A_oJGEC), &&L(B_JGEC), &&L(B_dJGEC),
	    &&L(B_iJGEC), &&L(B_sJGEC), &&L(B_oJGEC),
	    &&L(C_JGEC), &&L(C_dJGEC), &&L(C_iJGEC),
	    &&L(C_sJGEC), &&L(C_oJGEC), &&L(D_JGEC),
	    &&L(D_dJGEC), &&L(D_iJGEC), &&L(D_sJGEC),
	    &&L(D_oJGEC), &&L(A_JLE), &&L(A_dJLE),
	    &&L(A_iJLE), &&L(A_sJLE), &&L(A_oJLE),
	    &&L(B_JLE), &&L(B_dJLE), &&L(B_iJLE),
	    &&L(B_sJLE), &&L(B_oJLE), &&L(C_JLE),
	    &&L(C_dJLE), &&L(C_iJLE), &&L(C_sJLE),
	    &&L(C_oJLE), &&L(D_JLE), &&L(D_dJLE),
	    &&L(D_iJLE), &&L(D_sJLE), &&L(D_oJLE),
	    &&L(A_JLEC), &&L(A_dJLEC), &&L(A_iJLEC),
	    &&L(A_sJLEC), &&L(A_oJLEC), &&L(B_JLEC),
	    &&L(B_dJLEC), &&L(B_iJLEC), &&L(B_sJLEC),
	    &&L(B_oJLEC), &&L(C_JLEC), &&L(C_dJLEC),
	    &&L(C_iJLEC), &&L(C_sJLEC), &&L(C_oJLEC),
	    &&L(D_JLEC), &&L(D_dJLEC), &&L(D_iJLEC),
	    &&L(D_sJLEC), &&L(D_oJLEC), &&L(A_JE),
	    &&L(A_dJE), &&L(A_iJE), &&L(A_sJE),
	    &&L(A_oJE), &&L(B_JE), &&L(B_dJE),
	    &&L(B_iJE), &&L(B_sJE), &&L(B_oJE),
	    &&L(C_JE), &&L(C_dJE), &&L(C_iJE),
	    &&L(C_sJE), &&L(C_oJE), &&L(D_JE),
	    &&L(D_dJE), &&L(D_iJE), &&L(D_sJE),
	    &&L(D_oJE), &&L(A_JEC), &&L(A_dJEC),
	    &&L(A_iJEC), &&L(A_sJEC), &&L(A_oJEC),
	    &&L(B_JEC), &&L(B_dJEC), &&L(B_iJEC),
	    &&L(B_sJEC), &&L(B_oJEC), &&L(C_JEC),
	    &&L(C_dJEC), &&L(C_iJEC), &&L(C_sJEC),
	    &&L(C_oJEC), &&L(D_JEC), &&L(D_dJEC),
	    &&L(D_iJEC), &&L(D_sJEC), &&L(D_oJEC),
	    &&L(A_JNE), &&L(A_dJNE), &&L(A_iJNE),
	    &&L(A_sJNE), &&L(A_oJNE), &&L(B_JNE),
	    &&L(B_dJNE), &&L(B_iJNE), &&L(B_sJNE),
	    &&L(B_oJNE), &&L(C_JNE), &&L(C_dJNE),
	    &&L(C_iJNE), &&L(C_sJNE), &&L(C_oJNE),
	    &&L(D_JNE), &&L(D_dJNE), &&L(D_iJNE),
	    &&L(D_sJNE), &&L(D_oJNE), &&L(A_JNEC),
	    &&L(A_dJNEC), &&L(A_iJNEC), &&L(A_sJNEC),
	    &&L(A_oJNEC), &&L(B_JNEC), &&L(B_dJNEC),
	    &&L(B_iJNEC), &&L(B_sJNEC), &&L(B_oJNEC),
	    &&L(C_JNEC), &&L(C_dJNEC), &&L(C_iJNEC),
	    &&L(C_sJNEC), &&L(C_oJNEC), &&L(D_JNEC),
	    &&L(D_dJNEC), &&L(D_iJNEC), &&L(D_sJNEC),
	    &&L(D_oJNEC), &&L(A_RET), &&L(B_RET),
	    &&L(C_RET), &&L(D_RET), &&L(A_WRITE),
	    &&L(A_dWRITE), &&L(A_iWRITE), &&L(A_sWRITE),
	    &&L(A_oWRITE), &&L(B_WRITE), &&L(B_dWRITE),
	    &&L(B_iWRITE), &&L(B_sWRITE), &&L(B_oWRITE),
	    &&L(C_WRITE), &&L(C_dWRITE), &&L(C_iWRITE),
	    &&L(C_sWRITE), &&L(C_oWRITE), &&L(D_WRITE),
	    &&L(D_dWRITE), &&L(D_iWRITE), &&L(D_sWRITE),
	    &&L(D_oWRITE), &&L(A_LET), &&L(B_LET),
	    &&L(C_LET), &&L(D_LET), &&L(A_CALL),
	    &&L(B_CALL), &&L(C_CALL), &&L(D_CALL),
	    &&L(A_SELFCALL), &&L(B_SELFCALL), &&L(C_SELFCALL),
	    &&L(D_SELFCALL), &&L(A_SHIFT), &&L(A_dSHIFT),
	    &&L(A_iSHIFT), &&L(A_sSHIFT), &&L(A_oSHIFT),
	    &&L(B_SHIFT), &&L(B_dSHIFT), &&L(B_iSHIFT),
	    &&L(B_sSHIFT), &&L(B_oSHIFT), &&L(C_SHIFT),
	    &&L(C_dSHIFT), &&L(C_iSHIFT), &&L(C_sSHIFT),
	    &&L(C_oSHIFT), &&L(D_SHIFT), &&L(D_dSHIFT),
	    &&L(D_iSHIFT), &&L(D_sSHIFT), &&L(D_oSHIFT),
	    &&L(A_PUSH), &&L(B_PUSH), &&L(C_PUSH),
	    &&L(D_PUSH),
	};
	static InstBlock block_table[] = {
	    B(UNDEF), B(MOV), B(vMOV),
	    B(gMOV), B(ARGMOV), B(ArrayARGMOV),
	    B(ADD), B(dADD), B(iADD),
	    B(sADD), B(oADD), B(dADDC),
	    B(iADDC), B(SUB), B(dSUB),
	    B(iSUB), B(oSUB), B(dSUBC),
	    B(iSUBC), B(MUL), B(dMUL),
	    B(iMUL), B(sMUL), B(oMUL),
	    B(dMULC), B(iMULC), B(DIV),
	    B(dDIV), B(iDIV), B(oDIV),
	    B(dDIVC), B(iDIVC), B(JG),
	    B(dJG), B(iJG), B(sJG),
	    B(oJG), B(dJGC), B(iJGC),
	    B(JL), B(dJL), B(iJL),
	    B(sJL), B(oJL), B(dJLC),
	    B(iJLC), B(JGE), B(dJGE),
	    B(iJGE), B(sJGE), B(oJGE),
	    B(dJGEC), B(iJGEC), B(JLE),
	    B(dJLE), B(iJLE), B(sJLE),
	    B(oJLE), B(dJLEC), B(iJLEC),
	    B(JE), B(dJE), B(iJE),
	    B(sJE), B(oJE), B(dJEC),
	    B(iJEC), B(JNE), B(dJNE),
	    B(iJNE), B(sJNE), B(oJNE),
	    B(dJNEC), B(iJNEC), B(INC),
	    B(dINC), B(iINC), B(sINC),
	    B(oINC), B(gINC), B(dgINC),
	    B(igINC), B(sgINC), B(ogINC),
	    B(DEC), B(dDEC), B(iDEC),
	    B(sDEC), B(oDEC), B(gDEC),
	    B(dgDEC), B(igDEC), B(sgDEC),
	    B(ogDEC), B(RET), B(THCODE),
	    B(NOP), B(WRITE), B(dWRITE),
	    B(iWRITE), B(sWRITE), B(oWRITE),
	    B(FLUSH), B(JMP), B(LET),
	    B(gLET), B(SETv), B(FUNCSET),
	    B(CALL), B(SELFCALL), B(SHIFT),
	    B(dSHIFT), B(iSHIFT), B(sSHIFT),
	    B(oSHIFT), B(PUSH), B(NEW),
	    B(NEW_STRING), B(ARRAY_PUSH), B(ARRAY_AT),
	    B(ARRAY_gAT), B(ARRAY_DREF),	    B(A_MOV), B(B_MOV), B(C_MOV),
	    B(D_MOV), B(A_vMOV), B(B_vMOV),
	    B(C_vMOV), B(D_vMOV), B(A_gMOV),
	    B(B_gMOV), B(C_gMOV), B(D_gMOV),
	    B(A_ARGMOV), B(B_ARGMOV), B(C_ARGMOV),
	    B(D_ARGMOV), B(A_ArrayARGMOV), B(B_ArrayARGMOV),
	    B(C_ArrayARGMOV), B(D_ArrayARGMOV), B(AB_ADD),
	    B(AB_dADD), B(AB_iADD), B(AB_sADD),
	    B(AB_oADD), B(AC_ADD), B(AC_dADD),
	    B(AC_iADD), B(AC_sADD), B(AC_oADD),
	    B(AD_ADD), B(AD_dADD), B(AD_iADD),
	    B(AD_sADD), B(AD_oADD), B(BC_ADD),
	    B(BC_dADD), B(BC_iADD), B(BC_sADD),
	    B(BC_oADD), B(BD_ADD), B(BD_dADD),
	    B(BD_iADD), B(BD_sADD), B(BD_oADD),
	    B(CD_ADD), B(CD_dADD), B(CD_iADD),
	    B(CD_sADD), B(CD_oADD), B(A_ADDC),
	    B(A_dADDC), B(A_iADDC), B(A_sADDC),
	    B(B_ADDC), B(B_dADDC), B(B_iADDC),
	    B(B_sADDC), B(C_ADDC), B(C_dADDC),
	    B(C_iADDC), B(C_sADDC), B(D_ADDC),
	    B(D_dADDC), B(D_iADDC), B(D_sADDC),
	    B(AB_SUB), B(AB_dSUB), B(AB_iSUB),
	    B(AB_oSUB), B(AC_SUB), B(AC_dSUB),
	    B(AC_iSUB), B(AC_oSUB), B(AD_SUB),
	    B(AD_dSUB), B(AD_iSUB), B(AD_oSUB),
	    B(BC_SUB), B(BC_dSUB), B(BC_iSUB),
	    B(BC_oSUB), B(BD_SUB), B(BD_dSUB),
	    B(BD_iSUB), B(BD_oSUB), B(CD_SUB),
	    B(CD_dSUB), B(CD_iSUB), B(CD_oSUB),
	    B(A_SUBC), B(A_dSUBC), B(A_iSUBC),
	    B(B_SUBC), B(B_dSUBC), B(B_iSUBC),
	    B(C_SUBC), B(C_dSUBC), B(C_iSUBC),
	    B(D_SUBC), B(D_dSUBC), B(D_iSUBC),
	    B(AB_MUL), B(AB_dMUL), B(AB_iMUL),
	    B(AB_sMUL), B(AB_oMUL), B(AC_MUL),
	    B(AC_dMUL), B(AC_iMUL), B(AC_sMUL),
	    B(AC_oMUL), B(AD_MUL), B(AD_dMUL),
	    B(AD_iMUL), B(AD_sMUL), B(AD_oMUL),
	    B(BC_MUL), B(BC_dMUL), B(BC_iMUL),
	    B(BC_sMUL), B(BC_oMUL), B(BD_MUL),
	    B(BD_dMUL), B(BD_iMUL), B(BD_sMUL),
	    B(BD_oMUL), B(CD_MUL), B(CD_dMUL),
	    B(CD_iMUL), B(CD_sMUL), B(CD_oMUL),
	    B(A_MULC), B(A_dMULC), B(A_iMULC),
	    B(A_sMULC), B(B_MULC), B(B_dMULC),
	    B(B_iMULC), B(B_sMULC), B(C_MULC),
	    B(C_dMULC), B(C_iMULC), B(C_sMULC),
	    B(D_MULC), B(D_dMULC), B(D_iMULC),
	    B(D_sMULC), B(AB_DIV), B(AB_dDIV),
	    B(AB_iDIV), B(AB_oDIV), B(AC_DIV),
	    B(AC_dDIV), B(AC_iDIV), B(AC_oDIV),
	    B(AD_DIV), B(AD_dDIV), B(AD_iDIV),
	    B(AD_oDIV), B(BC_DIV), B(BC_dDIV),
	    B(BC_iDIV), B(BC_oDIV), B(BD_DIV),
	    B(BD_dDIV), B(BD_iDIV), B(BD_oDIV),
	    B(CD_DIV), B(CD_dDIV), B(CD_iDIV),
	    B(CD_oDIV), B(A_DIVC), B(A_dDIVC),
	    B(A_iDIVC), B(B_DIVC), B(B_dDIVC),
	    B(B_iDIVC), B(C_DIVC), B(C_dDIVC),
	    B(C_iDIVC), B(D_DIVC), B(D_dDIVC),
	    B(D_iDIVC), B(A_JG), B(A_dJG),
	    B(A_iJG), B(A_sJG), B(A_oJG),
	    B(B_JG), B(B_dJG), B(B_iJG),
	    B(B_sJG), B(B_oJG), B(C_JG),
	    B(C_dJG), B(C_iJG), B(C_sJG),
	    B(C_oJG), B(D_JG), B(D_dJG),
	    B(D_iJG), B(D_sJG), B(D_oJG),
	    B(A_JGC), B(A_dJGC), B(A_iJGC),
	    B(A_sJGC), B(A_oJGC), B(B_JGC),
	    B(B_dJGC), B(B_iJGC), B(B_sJGC),
	    B(B_oJGC), B(C_JGC), B(C_dJGC),
	    B(C_iJGC), B(C_sJGC), B(C_oJGC),
	    B(D_JGC), B(D_dJGC), B(D_iJGC),
	    B(D_sJGC), B(D_oJGC), B(A_JL),
	    B(A_dJL), B(A_iJL), B(A_sJL),
	    B(A_oJL), B(B_JL), B(B_dJL),
	    B(B_iJL), B(B_sJL), B(B_oJL),
	    B(C_JL), B(C_dJL), B(C_iJL),
	    B(C_sJL), B(C_oJL), B(D_JL),
	    B(D_dJL), B(D_iJL), B(D_sJL),
	    B(D_oJL), B(A_JLC), B(A_dJLC),
	    B(A_iJLC), B(A_sJLC), B(A_oJLC),
	    B(B_JLC), B(B_dJLC), B(B_iJLC),
	    B(B_sJLC), B(B_oJLC), B(C_JLC),
	    B(C_dJLC), B(C_iJLC), B(C_sJLC),
	    B(C_oJLC), B(D_JLC), B(D_dJLC),
	    B(D_iJLC), B(D_sJLC), B(D_oJLC),
	    B(A_JGE), B(A_dJGE), B(A_iJGE),
	    B(A_sJGE), B(A_oJGE), B(B_JGE),
	    B(B_dJGE), B(B_iJGE), B(B_sJGE),
	    B(B_oJGE), B(C_JGE), B(C_dJGE),
	    B(C_iJGE), B(C_sJGE), B(C_oJGE),
	    B(D_JGE), B(D_dJGE), B(D_iJGE),
	    B(D_sJGE), B(D_oJGE), B(A_JGEC),
	    B(A_dJGEC), B(A_iJGEC), B(A_sJGEC),
	    B(A_oJGEC), B(B_JGEC), B(B_dJGEC),
	    B(B_iJGEC), B(B_sJGEC), B(B_oJGEC),
	    B(C_JGEC), B(C_dJGEC), B(C_iJGEC),
	    B(C_sJGEC), B(C_oJGEC), B(D_JGEC),
	    B(D_dJGEC), B(D_iJGEC), B(D_sJGEC),
	    B(D_oJGEC), B(A_JLE), B(A_dJLE),
	    B(A_iJLE), B(A_sJLE), B(A_oJLE),
	    B(B_JLE), B(B_dJLE), B(B_iJLE),
	    B(B_sJLE), B(B_oJLE), B(C_JLE),
	    B(C_dJLE), B(C_iJLE), B(C_sJLE),
	    B(C_oJLE), B(D_JLE), B(D_dJLE),
	    B(D_iJLE), B(D_sJLE), B(D_oJLE),
	    B(A_JLEC), B(A_dJLEC), B(A_iJLEC),
	    B(A_sJLEC), B(A_oJLEC), B(B_JLEC),
	    B(B_dJLEC), B(B_iJLEC), B(B_sJLEC),
	    B(B_oJLEC), B(C_JLEC), B(C_dJLEC),
	    B(C_iJLEC), B(C_sJLEC), B(C_oJLEC),
	    B(D_JLEC), B(D_dJLEC), B(D_iJLEC),
	    B(D_sJLEC), B(D_oJLEC), B(A_JE),
	    B(A_dJE), B(A_iJE), B(A_sJE),
	    B(A_oJE), B(B_JE), B(B_dJE),
	    B(B_iJE), B(B_sJE), B(B_oJE),
	    B(C_JE), B(C_dJE), B(C_iJE),
	    B(C_sJE), B(C_oJE), B(D_JE),
	    B(D_dJE), B(D_iJE), B(D_sJE),
	    B(D_oJE), B(A_JEC), B(A_dJEC),
	    B(A_iJEC), B(A_sJEC), B(A_oJEC),
	    B(B_JEC), B(B_dJEC), B(B_iJEC),
	    B(B_sJEC), B(B_oJEC), B(C_JEC),
	    B(C_dJEC), B(C_iJEC), B(C_sJEC),
	    B(C_oJEC), B(D_JEC), B(D_dJEC),
	    B(D_iJEC), B(D_sJEC), B(D_oJEC),
	    B(A_JNE), B(A_dJNE), B(A_iJNE),
	    B(A_sJNE), B(A_oJNE), B(B_JNE),
	    B(B_dJNE), B(B_iJNE), B(B_sJNE),
	    B(B_oJNE), B(C_JNE), B(C_dJNE),
	    B(C_iJNE), B(C_sJNE), B(C_oJNE),
	    B(D_JNE), B(D_dJNE), B(D_iJNE),
	    B(D_sJNE), B(D_oJNE), B(A_JNEC),
	    B(A_dJNEC), B(A_iJNEC), B(A_sJNEC),
	    B(A_oJNEC), B(B_JNEC), B(B_dJNEC),
	    B(B_iJNEC), B(B_sJNEC), B(B_oJNEC),
	    B(C_JNEC), B(C_dJNEC), B(C_iJNEC),
	    B(C_sJNEC), B(C_oJNEC), B(D_JNEC),
	    B(D_dJNEC), B(D_iJNEC), B(D_sJNEC),
	    B(D_oJNEC), B(A_RET), B(B_RET),
	    B(C_RET), B(D_RET), B(A_WRITE),
	    B(A_dWRITE), B(A_iWRITE), B(A_sWRITE),
	    B(A_oWRITE), B(B_WRITE), B(B_dWRITE),
	    B(B_iWRITE), B(B_sWRITE), B(B_oWRITE),
	    B(C_WRITE), B(C_dWRITE), B(C_iWRITE),
	    B(C_sWRITE), B(C_oWRITE), B(D_WRITE),
	    B(D_dWRITE), B(D_iWRITE), B(D_sWRITE),
	    B(D_oWRITE), B(A_LET), B(B_LET),
	    B(C_LET), B(D_LET), B(A_CALL),
	    B(B_CALL), B(C_CALL), B(D_CALL),
	    B(A_SELFCALL), B(B_SELFCALL), B(C_SELFCALL),
	    B(D_SELFCALL), B(A_SHIFT), B(A_dSHIFT),
	    B(A_iSHIFT), B(A_sSHIFT), B(A_oSHIFT),
	    B(B_SHIFT), B(B_dSHIFT), B(B_iSHIFT),
	    B(B_sSHIFT), B(B_oSHIFT), B(C_SHIFT),
	    B(C_dSHIFT), B(C_iSHIFT), B(C_sSHIFT),
	    B(C_oSHIFT), B(D_SHIFT), B(D_dSHIFT),
	    B(D_iSHIFT), B(D_sSHIFT), B(D_oSHIFT),
	    B(A_PUSH), B(B_PUSH), B(C_PUSH),
	    B(D_PUSH),
	};
