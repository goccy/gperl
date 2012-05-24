	CASE(A_MOV, {
		GPERL_MOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_MOV, {
		GPERL_MOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_MOV, {
		GPERL_MOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_MOV, {
		GPERL_MOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iMOV, {
		GPERL_iMOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dMOV, {
		GPERL_dMOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sMOV, {
		GPERL_sMOV(0, pc->name);
		pc++;
		BREAK();
	});
	CASE(A_oMOV, {
		GPERL_oMOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iMOV, {
		GPERL_iMOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dMOV, {
		GPERL_dMOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sMOV, {
		GPERL_sMOV(1, pc->name);
		pc++;
		BREAK();
	});
	CASE(B_oMOV, {
		GPERL_oMOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iMOV, {
		GPERL_iMOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dMOV, {
		GPERL_dMOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sMOV, {
		GPERL_sMOV(2, pc->name);
		pc++;
		BREAK();
	});
	CASE(C_oMOV, {
		GPERL_oMOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iMOV, {
		GPERL_iMOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dMOV, {
		GPERL_dMOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sMOV, {
		GPERL_sMOV(3, pc->name);
		pc++;
		BREAK();
	});
	CASE(D_oMOV, {
		GPERL_oMOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(AB_ADD, {
		GPERL_ADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_ADD, {
		GPERL_ADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_ADD, {
		GPERL_ADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_ADD, {
		GPERL_ADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_ADD, {
		GPERL_ADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_ADD, {
		GPERL_ADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(AB_iADD, {
		GPERL_iADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_dADD, {
		GPERL_dADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_sADD, {
		GPERL_sADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oADD, {
		GPERL_oADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_iADD, {
		GPERL_iADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_dADD, {
		GPERL_dADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_sADD, {
		GPERL_sADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oADD, {
		GPERL_oADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_iADD, {
		GPERL_iADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_dADD, {
		GPERL_dADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_sADD, {
		GPERL_sADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oADD, {
		GPERL_oADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_iADD, {
		GPERL_iADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_dADD, {
		GPERL_dADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_sADD, {
		GPERL_sADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oADD, {
		GPERL_oADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_iADD, {
		GPERL_iADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_dADD, {
		GPERL_dADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_sADD, {
		GPERL_sADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oADD, {
		GPERL_oADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iADD, {
		GPERL_iADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_dADD, {
		GPERL_dADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_sADD, {
		GPERL_sADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oADD, {
		GPERL_oADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_iADDC, {
		GPERL_iADDC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dADDC, {
		GPERL_dADDC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sADDC, {
		GPERL_sADDC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iADDC, {
		GPERL_iADDC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dADDC, {
		GPERL_dADDC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sADDC, {
		GPERL_sADDC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iADDC, {
		GPERL_iADDC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dADDC, {
		GPERL_dADDC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sADDC, {
		GPERL_sADDC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iADDC, {
		GPERL_iADDC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dADDC, {
		GPERL_dADDC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sADDC, {
		GPERL_sADDC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(AB_SUB, {
		GPERL_SUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_SUB, {
		GPERL_SUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_SUB, {
		GPERL_SUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_SUB, {
		GPERL_SUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_SUB, {
		GPERL_SUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_SUB, {
		GPERL_SUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(AB_iSUB, {
		GPERL_iSUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_dSUB, {
		GPERL_dSUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oSUB, {
		GPERL_oSUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_iSUB, {
		GPERL_iSUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_dSUB, {
		GPERL_dSUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oSUB, {
		GPERL_oSUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_iSUB, {
		GPERL_iSUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_dSUB, {
		GPERL_dSUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oSUB, {
		GPERL_oSUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_iSUB, {
		GPERL_iSUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_dSUB, {
		GPERL_dSUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oSUB, {
		GPERL_oSUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_iSUB, {
		GPERL_iSUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_dSUB, {
		GPERL_dSUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oSUB, {
		GPERL_oSUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iSUB, {
		GPERL_iSUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_dSUB, {
		GPERL_dSUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oSUB, {
		GPERL_oSUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_iSUBC, {
		GPERL_iSUBC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dSUBC, {
		GPERL_dSUBC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iSUBC, {
		GPERL_iSUBC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dSUBC, {
		GPERL_dSUBC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iSUBC, {
		GPERL_iSUBC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dSUBC, {
		GPERL_dSUBC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iSUBC, {
		GPERL_iSUBC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dSUBC, {
		GPERL_dSUBC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(AB_MUL, {
		GPERL_MUL(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_MUL, {
		GPERL_MUL(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_MUL, {
		GPERL_MUL(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_MUL, {
		GPERL_MUL(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_MUL, {
		GPERL_MUL(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_MUL, {
		GPERL_MUL(2, 3);
		pc++;
		BREAK();
	});
	CASE(AB_iMUL, {
		GPERL_iMUL(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_dMUL, {
		GPERL_dMUL(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_sMUL, {
		GPERL_sMUL(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oMUL, {
		GPERL_oMUL(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_iMUL, {
		GPERL_iMUL(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_dMUL, {
		GPERL_dMUL(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_sMUL, {
		GPERL_sMUL(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oMUL, {
		GPERL_oMUL(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_iMUL, {
		GPERL_iMUL(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_dMUL, {
		GPERL_dMUL(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_sMUL, {
		GPERL_sMUL(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oMUL, {
		GPERL_oMUL(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_iMUL, {
		GPERL_iMUL(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_dMUL, {
		GPERL_dMUL(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_sMUL, {
		GPERL_sMUL(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oMUL, {
		GPERL_oMUL(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_iMUL, {
		GPERL_iMUL(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_dMUL, {
		GPERL_dMUL(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_sMUL, {
		GPERL_sMUL(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oMUL, {
		GPERL_oMUL(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iMUL, {
		GPERL_iMUL(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_dMUL, {
		GPERL_dMUL(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_sMUL, {
		GPERL_sMUL(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oMUL, {
		GPERL_oMUL(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_iMULC, {
		GPERL_iMULC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dMULC, {
		GPERL_dMULC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sMULC, {
		GPERL_sMULC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iMULC, {
		GPERL_iMULC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dMULC, {
		GPERL_dMULC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sMULC, {
		GPERL_sMULC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iMULC, {
		GPERL_iMULC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dMULC, {
		GPERL_dMULC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sMULC, {
		GPERL_sMULC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iMULC, {
		GPERL_iMULC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dMULC, {
		GPERL_dMULC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sMULC, {
		GPERL_sMULC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(AB_DIV, {
		GPERL_DIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_DIV, {
		GPERL_DIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_DIV, {
		GPERL_DIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_DIV, {
		GPERL_DIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_DIV, {
		GPERL_DIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_DIV, {
		GPERL_DIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(AB_iDIV, {
		GPERL_iDIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_dDIV, {
		GPERL_dDIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oDIV, {
		GPERL_oDIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_iDIV, {
		GPERL_iDIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_dDIV, {
		GPERL_dDIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oDIV, {
		GPERL_oDIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_iDIV, {
		GPERL_iDIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_dDIV, {
		GPERL_dDIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oDIV, {
		GPERL_oDIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_iDIV, {
		GPERL_iDIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_dDIV, {
		GPERL_dDIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oDIV, {
		GPERL_oDIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_iDIV, {
		GPERL_iDIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_dDIV, {
		GPERL_dDIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oDIV, {
		GPERL_oDIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iDIV, {
		GPERL_iDIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_dDIV, {
		GPERL_dDIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oDIV, {
		GPERL_oDIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_iDIVC, {
		GPERL_iDIVC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dDIVC, {
		GPERL_dDIVC(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iDIVC, {
		GPERL_iDIVC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dDIVC, {
		GPERL_dDIVC(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iDIVC, {
		GPERL_iDIVC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dDIVC, {
		GPERL_dDIVC(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iDIVC, {
		GPERL_iDIVC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dDIVC, {
		GPERL_dDIVC(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_JG, {
		GPERL_CMP_JMP(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_JG, {
		GPERL_CMP_JMP(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_JG, {
		GPERL_CMP_JMP(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_JG, {
		GPERL_CMP_JMP(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJG, {
		GPERL_iCMP_JMP(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJG, {
		GPERL_dCMP_JMP(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJG, {
		GPERL_sCMP_JMP(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJG, {
		GPERL_oCMP_JMP(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJG, {
		GPERL_iCMP_JMP(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJG, {
		GPERL_dCMP_JMP(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJG, {
		GPERL_sCMP_JMP(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJG, {
		GPERL_oCMP_JMP(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJG, {
		GPERL_iCMP_JMP(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJG, {
		GPERL_dCMP_JMP(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJG, {
		GPERL_sCMP_JMP(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJG, {
		GPERL_oCMP_JMP(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJG, {
		GPERL_iCMP_JMP(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJG, {
		GPERL_dCMP_JMP(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJG, {
		GPERL_sCMP_JMP(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJG, {
		GPERL_oCMP_JMP(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJGC, {
		GPERL_iCMP_JMPC(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJGC, {
		GPERL_dCMP_JMPC(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJGC, {
		GPERL_sCMP_JMPC(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJGC, {
		GPERL_oCMP_JMPC(>, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJGC, {
		GPERL_iCMP_JMPC(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJGC, {
		GPERL_dCMP_JMPC(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJGC, {
		GPERL_sCMP_JMPC(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJGC, {
		GPERL_oCMP_JMPC(>, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJGC, {
		GPERL_iCMP_JMPC(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJGC, {
		GPERL_dCMP_JMPC(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJGC, {
		GPERL_sCMP_JMPC(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJGC, {
		GPERL_oCMP_JMPC(>, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJGC, {
		GPERL_iCMP_JMPC(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJGC, {
		GPERL_dCMP_JMPC(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJGC, {
		GPERL_sCMP_JMPC(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJGC, {
		GPERL_oCMP_JMPC(>, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_JL, {
		GPERL_CMP_JMP(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_JL, {
		GPERL_CMP_JMP(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_JL, {
		GPERL_CMP_JMP(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_JL, {
		GPERL_CMP_JMP(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJL, {
		GPERL_iCMP_JMP(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJL, {
		GPERL_dCMP_JMP(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJL, {
		GPERL_sCMP_JMP(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJL, {
		GPERL_oCMP_JMP(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJL, {
		GPERL_iCMP_JMP(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJL, {
		GPERL_dCMP_JMP(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJL, {
		GPERL_sCMP_JMP(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJL, {
		GPERL_oCMP_JMP(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJL, {
		GPERL_iCMP_JMP(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJL, {
		GPERL_dCMP_JMP(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJL, {
		GPERL_sCMP_JMP(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJL, {
		GPERL_oCMP_JMP(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJL, {
		GPERL_iCMP_JMP(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJL, {
		GPERL_dCMP_JMP(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJL, {
		GPERL_sCMP_JMP(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJL, {
		GPERL_oCMP_JMP(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJLC, {
		GPERL_iCMP_JMPC(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJLC, {
		GPERL_dCMP_JMPC(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJLC, {
		GPERL_sCMP_JMPC(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJLC, {
		GPERL_oCMP_JMPC(<, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJLC, {
		GPERL_iCMP_JMPC(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJLC, {
		GPERL_dCMP_JMPC(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJLC, {
		GPERL_sCMP_JMPC(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJLC, {
		GPERL_oCMP_JMPC(<, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJLC, {
		GPERL_iCMP_JMPC(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJLC, {
		GPERL_dCMP_JMPC(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJLC, {
		GPERL_sCMP_JMPC(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJLC, {
		GPERL_oCMP_JMPC(<, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJLC, {
		GPERL_iCMP_JMPC(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJLC, {
		GPERL_dCMP_JMPC(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJLC, {
		GPERL_sCMP_JMPC(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJLC, {
		GPERL_oCMP_JMPC(<, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_JGE, {
		GPERL_CMP_JMP(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_JGE, {
		GPERL_CMP_JMP(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_JGE, {
		GPERL_CMP_JMP(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_JGE, {
		GPERL_CMP_JMP(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJGE, {
		GPERL_iCMP_JMP(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJGE, {
		GPERL_dCMP_JMP(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJGE, {
		GPERL_sCMP_JMP(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJGE, {
		GPERL_oCMP_JMP(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJGE, {
		GPERL_iCMP_JMP(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJGE, {
		GPERL_dCMP_JMP(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJGE, {
		GPERL_sCMP_JMP(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJGE, {
		GPERL_oCMP_JMP(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJGE, {
		GPERL_iCMP_JMP(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJGE, {
		GPERL_dCMP_JMP(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJGE, {
		GPERL_sCMP_JMP(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJGE, {
		GPERL_oCMP_JMP(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJGE, {
		GPERL_iCMP_JMP(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJGE, {
		GPERL_dCMP_JMP(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJGE, {
		GPERL_sCMP_JMP(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJGE, {
		GPERL_oCMP_JMP(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJGEC, {
		GPERL_iCMP_JMPC(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJGEC, {
		GPERL_dCMP_JMPC(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJGEC, {
		GPERL_sCMP_JMPC(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJGEC, {
		GPERL_oCMP_JMPC(>=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJGEC, {
		GPERL_iCMP_JMPC(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJGEC, {
		GPERL_dCMP_JMPC(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJGEC, {
		GPERL_sCMP_JMPC(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJGEC, {
		GPERL_oCMP_JMPC(>=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJGEC, {
		GPERL_iCMP_JMPC(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJGEC, {
		GPERL_dCMP_JMPC(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJGEC, {
		GPERL_sCMP_JMPC(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJGEC, {
		GPERL_oCMP_JMPC(>=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJGEC, {
		GPERL_iCMP_JMPC(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJGEC, {
		GPERL_dCMP_JMPC(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJGEC, {
		GPERL_sCMP_JMPC(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJGEC, {
		GPERL_oCMP_JMPC(>=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_JLE, {
		GPERL_CMP_JMP(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_JLE, {
		GPERL_CMP_JMP(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_JLE, {
		GPERL_CMP_JMP(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_JLE, {
		GPERL_CMP_JMP(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJLE, {
		GPERL_iCMP_JMP(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJLE, {
		GPERL_dCMP_JMP(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJLE, {
		GPERL_sCMP_JMP(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJLE, {
		GPERL_oCMP_JMP(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJLE, {
		GPERL_iCMP_JMP(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJLE, {
		GPERL_dCMP_JMP(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJLE, {
		GPERL_sCMP_JMP(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJLE, {
		GPERL_oCMP_JMP(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJLE, {
		GPERL_iCMP_JMP(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJLE, {
		GPERL_dCMP_JMP(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJLE, {
		GPERL_sCMP_JMP(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJLE, {
		GPERL_oCMP_JMP(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJLE, {
		GPERL_iCMP_JMP(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJLE, {
		GPERL_dCMP_JMP(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJLE, {
		GPERL_sCMP_JMP(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJLE, {
		GPERL_oCMP_JMP(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJLEC, {
		GPERL_iCMP_JMPC(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJLEC, {
		GPERL_dCMP_JMPC(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJLEC, {
		GPERL_sCMP_JMPC(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJLEC, {
		GPERL_oCMP_JMPC(<=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJLEC, {
		GPERL_iCMP_JMPC(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJLEC, {
		GPERL_dCMP_JMPC(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJLEC, {
		GPERL_sCMP_JMPC(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJLEC, {
		GPERL_oCMP_JMPC(<=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJLEC, {
		GPERL_iCMP_JMPC(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJLEC, {
		GPERL_dCMP_JMPC(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJLEC, {
		GPERL_sCMP_JMPC(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJLEC, {
		GPERL_oCMP_JMPC(<=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJLEC, {
		GPERL_iCMP_JMPC(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJLEC, {
		GPERL_dCMP_JMPC(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJLEC, {
		GPERL_sCMP_JMPC(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJLEC, {
		GPERL_oCMP_JMPC(<=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_JE, {
		GPERL_CMP_JMP(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_JE, {
		GPERL_CMP_JMP(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_JE, {
		GPERL_CMP_JMP(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_JE, {
		GPERL_CMP_JMP(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJE, {
		GPERL_iCMP_JMP(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJE, {
		GPERL_dCMP_JMP(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJE, {
		GPERL_sCMP_JMP(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJE, {
		GPERL_oCMP_JMP(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJE, {
		GPERL_iCMP_JMP(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJE, {
		GPERL_dCMP_JMP(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJE, {
		GPERL_sCMP_JMP(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJE, {
		GPERL_oCMP_JMP(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJE, {
		GPERL_iCMP_JMP(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJE, {
		GPERL_dCMP_JMP(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJE, {
		GPERL_sCMP_JMP(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJE, {
		GPERL_oCMP_JMP(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJE, {
		GPERL_iCMP_JMP(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJE, {
		GPERL_dCMP_JMP(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJE, {
		GPERL_sCMP_JMP(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJE, {
		GPERL_oCMP_JMP(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJEC, {
		GPERL_iCMP_JMPC(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJEC, {
		GPERL_dCMP_JMPC(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJEC, {
		GPERL_sCMP_JMPC(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJEC, {
		GPERL_oCMP_JMPC(==, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJEC, {
		GPERL_iCMP_JMPC(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJEC, {
		GPERL_dCMP_JMPC(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJEC, {
		GPERL_sCMP_JMPC(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJEC, {
		GPERL_oCMP_JMPC(==, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJEC, {
		GPERL_iCMP_JMPC(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJEC, {
		GPERL_dCMP_JMPC(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJEC, {
		GPERL_sCMP_JMPC(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJEC, {
		GPERL_oCMP_JMPC(==, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJEC, {
		GPERL_iCMP_JMPC(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJEC, {
		GPERL_dCMP_JMPC(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJEC, {
		GPERL_sCMP_JMPC(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJEC, {
		GPERL_oCMP_JMPC(==, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_JNE, {
		GPERL_CMP_JMP(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_JNE, {
		GPERL_CMP_JMP(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_JNE, {
		GPERL_CMP_JMP(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_JNE, {
		GPERL_CMP_JMP(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJNE, {
		GPERL_iCMP_JMP(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJNE, {
		GPERL_dCMP_JMP(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJNE, {
		GPERL_sCMP_JMP(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJNE, {
		GPERL_oCMP_JMP(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJNE, {
		GPERL_iCMP_JMP(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJNE, {
		GPERL_dCMP_JMP(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJNE, {
		GPERL_sCMP_JMP(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJNE, {
		GPERL_oCMP_JMP(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJNE, {
		GPERL_iCMP_JMP(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJNE, {
		GPERL_dCMP_JMP(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJNE, {
		GPERL_sCMP_JMP(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJNE, {
		GPERL_oCMP_JMP(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJNE, {
		GPERL_iCMP_JMP(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJNE, {
		GPERL_dCMP_JMP(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJNE, {
		GPERL_sCMP_JMP(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJNE, {
		GPERL_oCMP_JMP(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iJNEC, {
		GPERL_iCMP_JMPC(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dJNEC, {
		GPERL_dCMP_JMPC(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sJNEC, {
		GPERL_sCMP_JMPC(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oJNEC, {
		GPERL_oCMP_JMPC(!=, 0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iJNEC, {
		GPERL_iCMP_JMPC(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dJNEC, {
		GPERL_dCMP_JMPC(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sJNEC, {
		GPERL_sCMP_JMPC(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oJNEC, {
		GPERL_oCMP_JMPC(!=, 1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iJNEC, {
		GPERL_iCMP_JMPC(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dJNEC, {
		GPERL_dCMP_JMPC(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sJNEC, {
		GPERL_sCMP_JMPC(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oJNEC, {
		GPERL_oCMP_JMPC(!=, 2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iJNEC, {
		GPERL_iCMP_JMPC(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dJNEC, {
		GPERL_dCMP_JMPC(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sJNEC, {
		GPERL_sCMP_JMPC(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oJNEC, {
		GPERL_oCMP_JMPC(!=, 3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_RET, {
		GPERL_RET(0);
		pc++;
		BREAK();
	});
	CASE(B_RET, {
		GPERL_RET(1);
		pc++;
		BREAK();
	});
	CASE(C_RET, {
		GPERL_RET(2);
		pc++;
		BREAK();
	});
	CASE(D_RET, {
		GPERL_RET(3);
		pc++;
		BREAK();
	});
	CASE(A_iRET, {
		GPERL_iRET(0);
		pc++;
		BREAK();
	});
	CASE(A_dRET, {
		GPERL_dRET(0);
		pc++;
		BREAK();
	});
	CASE(A_sRET, {
		GPERL_sRET(0);
		pc++;
		BREAK();
	});
	CASE(A_oRET, {
		GPERL_oRET(0);
		pc++;
		BREAK();
	});
	CASE(B_iRET, {
		GPERL_iRET(1);
		pc++;
		BREAK();
	});
	CASE(B_dRET, {
		GPERL_dRET(1);
		pc++;
		BREAK();
	});
	CASE(B_sRET, {
		GPERL_sRET(1);
		pc++;
		BREAK();
	});
	CASE(B_oRET, {
		GPERL_oRET(1);
		pc++;
		BREAK();
	});
	CASE(C_iRET, {
		GPERL_iRET(2);
		pc++;
		BREAK();
	});
	CASE(C_dRET, {
		GPERL_dRET(2);
		pc++;
		BREAK();
	});
	CASE(C_sRET, {
		GPERL_sRET(2);
		pc++;
		BREAK();
	});
	CASE(C_oRET, {
		GPERL_oRET(2);
		pc++;
		BREAK();
	});
	CASE(D_iRET, {
		GPERL_iRET(3);
		pc++;
		BREAK();
	});
	CASE(D_dRET, {
		GPERL_dRET(3);
		pc++;
		BREAK();
	});
	CASE(D_sRET, {
		GPERL_sRET(3);
		pc++;
		BREAK();
	});
	CASE(D_oRET, {
		GPERL_oRET(3);
		pc++;
		BREAK();
	});
	CASE(A_WRITE, {
		GPERL_WRITE(0);
		pc++;
		BREAK();
	});
	CASE(B_WRITE, {
		GPERL_WRITE(1);
		pc++;
		BREAK();
	});
	CASE(C_WRITE, {
		GPERL_WRITE(2);
		pc++;
		BREAK();
	});
	CASE(D_WRITE, {
		GPERL_WRITE(3);
		pc++;
		BREAK();
	});
	CASE(A_iWRITE, {
		GPERL_iWRITE(0);
		pc++;
		BREAK();
	});
	CASE(A_dWRITE, {
		GPERL_dWRITE(0);
		pc++;
		BREAK();
	});
	CASE(A_sWRITE, {
		GPERL_sWRITE(0);
		pc++;
		BREAK();
	});
	CASE(A_oWRITE, {
		GPERL_oWRITE(0);
		pc++;
		BREAK();
	});
	CASE(B_iWRITE, {
		GPERL_iWRITE(1);
		pc++;
		BREAK();
	});
	CASE(B_dWRITE, {
		GPERL_dWRITE(1);
		pc++;
		BREAK();
	});
	CASE(B_sWRITE, {
		GPERL_sWRITE(1);
		pc++;
		BREAK();
	});
	CASE(B_oWRITE, {
		GPERL_oWRITE(1);
		pc++;
		BREAK();
	});
	CASE(C_iWRITE, {
		GPERL_iWRITE(2);
		pc++;
		BREAK();
	});
	CASE(C_dWRITE, {
		GPERL_dWRITE(2);
		pc++;
		BREAK();
	});
	CASE(C_sWRITE, {
		GPERL_sWRITE(2);
		pc++;
		BREAK();
	});
	CASE(C_oWRITE, {
		GPERL_oWRITE(2);
		pc++;
		BREAK();
	});
	CASE(D_iWRITE, {
		GPERL_iWRITE(3);
		pc++;
		BREAK();
	});
	CASE(D_dWRITE, {
		GPERL_dWRITE(3);
		pc++;
		BREAK();
	});
	CASE(D_sWRITE, {
		GPERL_sWRITE(3);
		pc++;
		BREAK();
	});
	CASE(D_oWRITE, {
		GPERL_oWRITE(3);
		pc++;
		BREAK();
	});
	CASE(A_CALL, {
		GPERL_CALL(0, pc->src, A_CALL);
		pc++;
		BREAK();
	});
	CASE(B_CALL, {
		GPERL_CALL(1, pc->src, B_CALL);
		pc++;
		BREAK();
	});
	CASE(C_CALL, {
		GPERL_CALL(2, pc->src, C_CALL);
		pc++;
		BREAK();
	});
	CASE(D_CALL, {
		GPERL_CALL(3, pc->src, D_CALL);
		pc++;
		BREAK();
	});
	CASE(A_iCALL, {
		GPERL_iCALL(0, pc->src, A_iCALL);
		pc++;
		BREAK();
	});
	CASE(A_dCALL, {
		GPERL_dCALL(0, pc->src, A_dCALL);
		pc++;
		BREAK();
	});
	CASE(A_sCALL, {
		GPERL_sCALL(0, pc->src, A_sCALL);
		pc++;
		BREAK();
	});
	CASE(A_oCALL, {
		GPERL_oCALL(0, pc->src, A_oCALL);
		pc++;
		BREAK();
	});
	CASE(B_iCALL, {
		GPERL_iCALL(1, pc->src, B_iCALL);
		pc++;
		BREAK();
	});
	CASE(B_dCALL, {
		GPERL_dCALL(1, pc->src, B_dCALL);
		pc++;
		BREAK();
	});
	CASE(B_sCALL, {
		GPERL_sCALL(1, pc->src, B_sCALL);
		pc++;
		BREAK();
	});
	CASE(B_oCALL, {
		GPERL_oCALL(1, pc->src, B_oCALL);
		pc++;
		BREAK();
	});
	CASE(C_iCALL, {
		GPERL_iCALL(2, pc->src, C_iCALL);
		pc++;
		BREAK();
	});
	CASE(C_dCALL, {
		GPERL_dCALL(2, pc->src, C_dCALL);
		pc++;
		BREAK();
	});
	CASE(C_sCALL, {
		GPERL_sCALL(2, pc->src, C_sCALL);
		pc++;
		BREAK();
	});
	CASE(C_oCALL, {
		GPERL_oCALL(2, pc->src, C_oCALL);
		pc++;
		BREAK();
	});
	CASE(D_iCALL, {
		GPERL_iCALL(3, pc->src, D_iCALL);
		pc++;
		BREAK();
	});
	CASE(D_dCALL, {
		GPERL_dCALL(3, pc->src, D_dCALL);
		pc++;
		BREAK();
	});
	CASE(D_sCALL, {
		GPERL_sCALL(3, pc->src, D_sCALL);
		pc++;
		BREAK();
	});
	CASE(D_oCALL, {
		GPERL_oCALL(3, pc->src, D_oCALL);
		pc++;
		BREAK();
	});
	CASE(A_SELFCALL, {
		GPERL_SELFCALL(0, A_SELFCALL);
		pc++;
		BREAK();
	});
	CASE(B_SELFCALL, {
		GPERL_SELFCALL(1, B_SELFCALL);
		pc++;
		BREAK();
	});
	CASE(C_SELFCALL, {
		GPERL_SELFCALL(2, C_SELFCALL);
		pc++;
		BREAK();
	});
	CASE(D_SELFCALL, {
		GPERL_SELFCALL(3, D_SELFCALL);
		pc++;
		BREAK();
	});
	CASE(A_iSELFCALL, {
		GPERL_iSELFCALL(0, A_iSELFCALL);
		pc++;
		BREAK();
	});
	CASE(A_dSELFCALL, {
		GPERL_dSELFCALL(0, A_dSELFCALL);
		pc++;
		BREAK();
	});
	CASE(A_sSELFCALL, {
		GPERL_sSELFCALL(0, A_sSELFCALL);
		pc++;
		BREAK();
	});
	CASE(A_oSELFCALL, {
		GPERL_oSELFCALL(0, A_oSELFCALL);
		pc++;
		BREAK();
	});
	CASE(B_iSELFCALL, {
		GPERL_iSELFCALL(1, B_iSELFCALL);
		pc++;
		BREAK();
	});
	CASE(B_dSELFCALL, {
		GPERL_dSELFCALL(1, B_dSELFCALL);
		pc++;
		BREAK();
	});
	CASE(B_sSELFCALL, {
		GPERL_sSELFCALL(1, B_sSELFCALL);
		pc++;
		BREAK();
	});
	CASE(B_oSELFCALL, {
		GPERL_oSELFCALL(1, B_oSELFCALL);
		pc++;
		BREAK();
	});
	CASE(C_iSELFCALL, {
		GPERL_iSELFCALL(2, C_iSELFCALL);
		pc++;
		BREAK();
	});
	CASE(C_dSELFCALL, {
		GPERL_dSELFCALL(2, C_dSELFCALL);
		pc++;
		BREAK();
	});
	CASE(C_sSELFCALL, {
		GPERL_sSELFCALL(2, C_sSELFCALL);
		pc++;
		BREAK();
	});
	CASE(C_oSELFCALL, {
		GPERL_oSELFCALL(2, C_oSELFCALL);
		pc++;
		BREAK();
	});
	CASE(D_iSELFCALL, {
		GPERL_iSELFCALL(3, D_iSELFCALL);
		pc++;
		BREAK();
	});
	CASE(D_dSELFCALL, {
		GPERL_dSELFCALL(3, D_dSELFCALL);
		pc++;
		BREAK();
	});
	CASE(D_sSELFCALL, {
		GPERL_sSELFCALL(3, D_sSELFCALL);
		pc++;
		BREAK();
	});
	CASE(D_oSELFCALL, {
		GPERL_oSELFCALL(3, D_oSELFCALL);
		pc++;
		BREAK();
	});
	CASE(A_SHIFT, {
		GPERL_SHIFT(0);
		pc++;
		BREAK();
	});
	CASE(B_SHIFT, {
		GPERL_SHIFT(1);
		pc++;
		BREAK();
	});
	CASE(C_SHIFT, {
		GPERL_SHIFT(2);
		pc++;
		BREAK();
	});
	CASE(D_SHIFT, {
		GPERL_SHIFT(3);
		pc++;
		BREAK();
	});
	CASE(A_iSHIFT, {
		GPERL_iSHIFT(0);
		pc++;
		BREAK();
	});
	CASE(A_dSHIFT, {
		GPERL_dSHIFT(0);
		pc++;
		BREAK();
	});
	CASE(A_sSHIFT, {
		GPERL_sSHIFT(0);
		pc++;
		BREAK();
	});
	CASE(A_oSHIFT, {
		GPERL_oSHIFT(0);
		pc++;
		BREAK();
	});
	CASE(B_iSHIFT, {
		GPERL_iSHIFT(1);
		pc++;
		BREAK();
	});
	CASE(B_dSHIFT, {
		GPERL_dSHIFT(1);
		pc++;
		BREAK();
	});
	CASE(B_sSHIFT, {
		GPERL_sSHIFT(1);
		pc++;
		BREAK();
	});
	CASE(B_oSHIFT, {
		GPERL_oSHIFT(1);
		pc++;
		BREAK();
	});
	CASE(C_iSHIFT, {
		GPERL_iSHIFT(2);
		pc++;
		BREAK();
	});
	CASE(C_dSHIFT, {
		GPERL_dSHIFT(2);
		pc++;
		BREAK();
	});
	CASE(C_sSHIFT, {
		GPERL_sSHIFT(2);
		pc++;
		BREAK();
	});
	CASE(C_oSHIFT, {
		GPERL_oSHIFT(2);
		pc++;
		BREAK();
	});
	CASE(D_iSHIFT, {
		GPERL_iSHIFT(3);
		pc++;
		BREAK();
	});
	CASE(D_dSHIFT, {
		GPERL_dSHIFT(3);
		pc++;
		BREAK();
	});
	CASE(D_sSHIFT, {
		GPERL_sSHIFT(3);
		pc++;
		BREAK();
	});
	CASE(D_oSHIFT, {
		GPERL_oSHIFT(3);
		pc++;
		BREAK();
	});
	CASE(A_PUSH, {
		GPERL_PUSH(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_PUSH, {
		GPERL_PUSH(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_PUSH, {
		GPERL_PUSH(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_PUSH, {
		GPERL_PUSH(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_iPUSH, {
		GPERL_iPUSH(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_dPUSH, {
		GPERL_dPUSH(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_sPUSH, {
		GPERL_sPUSH(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_oPUSH, {
		GPERL_oPUSH(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_iPUSH, {
		GPERL_iPUSH(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_dPUSH, {
		GPERL_dPUSH(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_sPUSH, {
		GPERL_sPUSH(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_oPUSH, {
		GPERL_oPUSH(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_iPUSH, {
		GPERL_iPUSH(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_dPUSH, {
		GPERL_dPUSH(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_sPUSH, {
		GPERL_sPUSH(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_oPUSH, {
		GPERL_oPUSH(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_iPUSH, {
		GPERL_iPUSH(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_dPUSH, {
		GPERL_dPUSH(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_sPUSH, {
		GPERL_sPUSH(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_oPUSH, {
		GPERL_oPUSH(3, pc->src);
		pc++;
		BREAK();
	});
