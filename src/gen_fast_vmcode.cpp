	CASE(A_MOV, {
		GPERL_MOV(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_MOV, {
		GPERL_MOV(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_MOV, {
		GPERL_MOV(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_MOV, {
		GPERL_MOV(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_vMOV, {
		GPERL_vMOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_vMOV, {
		GPERL_vMOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_vMOV, {
		GPERL_vMOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_vMOV, {
		GPERL_vMOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_gMOV, {
		GPERL_gMOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_gMOV, {
		GPERL_gMOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_gMOV, {
		GPERL_gMOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_gMOV, {
		GPERL_gMOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_ARGMOV, {
		GPERL_ARGMOV(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_ARGMOV, {
		GPERL_ARGMOV(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_ARGMOV, {
		GPERL_ARGMOV(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_ARGMOV, {
		GPERL_ARGMOV(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_ArrayARGMOV, {
		GPERL_ArrayARGMOV(0);
		pc++;
		BREAK();
	});
	CASE(B_ArrayARGMOV, {
		GPERL_ArrayARGMOV(1);
		pc++;
		BREAK();
	});
	CASE(C_ArrayARGMOV, {
		GPERL_ArrayARGMOV(2);
		pc++;
		BREAK();
	});
	CASE(D_ArrayARGMOV, {
		GPERL_ArrayARGMOV(3);
		pc++;
		BREAK();
	});
	CASE(AB_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AB_STATIC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AB_dADD, {
		GPERL_dADD(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_iADD, {
		GPERL_iADD(0, 1);
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
	CASE(AC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AC_STATIC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AC_dADD, {
		GPERL_dADD(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_iADD, {
		GPERL_iADD(0, 2);
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
	CASE(AD_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AD_STATIC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AD_dADD, {
		GPERL_dADD(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_iADD, {
		GPERL_iADD(0, 3);
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
	CASE(BC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BC_STATIC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BC_dADD, {
		GPERL_dADD(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_iADD, {
		GPERL_iADD(1, 2);
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
	CASE(BD_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BD_STATIC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BD_dADD, {
		GPERL_dADD(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_iADD, {
		GPERL_iADD(1, 3);
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
	CASE(CD_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(CD_STATIC_ADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(CD_dADD, {
		GPERL_dADD(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iADD, {
		GPERL_iADD(2, 3);
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
	CASE(A_ADDC, {
		GPERL_ADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dADDC, {
		GPERL_dADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iADDC, {
		GPERL_iADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_sADDC, {
		GPERL_sADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_ADDC, {
		GPERL_ADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dADDC, {
		GPERL_dADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iADDC, {
		GPERL_iADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_sADDC, {
		GPERL_sADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_ADDC, {
		GPERL_ADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dADDC, {
		GPERL_dADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iADDC, {
		GPERL_iADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_sADDC, {
		GPERL_sADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_ADDC, {
		GPERL_ADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dADDC, {
		GPERL_dADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iADDC, {
		GPERL_iADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_sADDC, {
		GPERL_sADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(AB_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AB_STATIC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AB_dSUB, {
		GPERL_dSUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_iSUB, {
		GPERL_iSUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oSUB, {
		GPERL_oSUB(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AC_STATIC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AC_dSUB, {
		GPERL_dSUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_iSUB, {
		GPERL_iSUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oSUB, {
		GPERL_oSUB(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AD_STATIC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AD_dSUB, {
		GPERL_dSUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_iSUB, {
		GPERL_iSUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oSUB, {
		GPERL_oSUB(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BC_STATIC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BC_dSUB, {
		GPERL_dSUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_iSUB, {
		GPERL_iSUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oSUB, {
		GPERL_oSUB(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BD_STATIC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BD_dSUB, {
		GPERL_dSUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_iSUB, {
		GPERL_iSUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oSUB, {
		GPERL_oSUB(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(CD_STATIC_SUB, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(CD_dSUB, {
		GPERL_dSUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iSUB, {
		GPERL_iSUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oSUB, {
		GPERL_oSUB(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_SUBC, {
		GPERL_SUBC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dSUBC, {
		GPERL_dSUBC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iSUBC, {
		GPERL_iSUBC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_SUBC, {
		GPERL_SUBC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dSUBC, {
		GPERL_dSUBC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iSUBC, {
		GPERL_iSUBC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_SUBC, {
		GPERL_SUBC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dSUBC, {
		GPERL_dSUBC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iSUBC, {
		GPERL_iSUBC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_SUBC, {
		GPERL_SUBC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dSUBC, {
		GPERL_dSUBC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iSUBC, {
		GPERL_iSUBC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(AB_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AB_STATIC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AB_dMUL, {
		GPERL_dMUL(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_iMUL, {
		GPERL_iMUL(0, 1);
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
	CASE(AC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AC_STATIC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AC_dMUL, {
		GPERL_dMUL(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_iMUL, {
		GPERL_iMUL(0, 2);
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
	CASE(AD_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AD_STATIC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AD_dMUL, {
		GPERL_dMUL(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_iMUL, {
		GPERL_iMUL(0, 3);
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
	CASE(BC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BC_STATIC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BC_dMUL, {
		GPERL_dMUL(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_iMUL, {
		GPERL_iMUL(1, 2);
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
	CASE(BD_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BD_STATIC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BD_dMUL, {
		GPERL_dMUL(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_iMUL, {
		GPERL_iMUL(1, 3);
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
	CASE(CD_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(CD_STATIC_MUL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(CD_dMUL, {
		GPERL_dMUL(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iMUL, {
		GPERL_iMUL(2, 3);
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
	CASE(A_MULC, {
		GPERL_MULC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dMULC, {
		GPERL_dMULC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iMULC, {
		GPERL_iMULC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_sMULC, {
		GPERL_sMULC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_MULC, {
		GPERL_MULC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dMULC, {
		GPERL_dMULC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iMULC, {
		GPERL_iMULC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_sMULC, {
		GPERL_sMULC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_MULC, {
		GPERL_MULC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dMULC, {
		GPERL_dMULC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iMULC, {
		GPERL_iMULC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_sMULC, {
		GPERL_sMULC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_MULC, {
		GPERL_MULC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dMULC, {
		GPERL_dMULC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iMULC, {
		GPERL_iMULC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_sMULC, {
		GPERL_sMULC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(AB_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AB_STATIC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AB_dDIV, {
		GPERL_dDIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_iDIV, {
		GPERL_iDIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oDIV, {
		GPERL_oDIV(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AC_STATIC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AC_dDIV, {
		GPERL_dDIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_iDIV, {
		GPERL_iDIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oDIV, {
		GPERL_oDIV(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AD_STATIC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AD_dDIV, {
		GPERL_dDIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_iDIV, {
		GPERL_iDIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oDIV, {
		GPERL_oDIV(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BC_STATIC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BC_dDIV, {
		GPERL_dDIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_iDIV, {
		GPERL_iDIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oDIV, {
		GPERL_oDIV(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BD_STATIC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BD_dDIV, {
		GPERL_dDIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_iDIV, {
		GPERL_iDIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oDIV, {
		GPERL_oDIV(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(CD_STATIC_DIV, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(CD_dDIV, {
		GPERL_dDIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iDIV, {
		GPERL_iDIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oDIV, {
		GPERL_oDIV(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_DIVC, {
		GPERL_DIVC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dDIVC, {
		GPERL_dDIVC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iDIVC, {
		GPERL_iDIVC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_DIVC, {
		GPERL_DIVC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dDIVC, {
		GPERL_dDIVC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iDIVC, {
		GPERL_iDIVC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_DIVC, {
		GPERL_DIVC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dDIVC, {
		GPERL_dDIVC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iDIVC, {
		GPERL_iDIVC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_DIVC, {
		GPERL_DIVC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dDIVC, {
		GPERL_dDIVC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iDIVC, {
		GPERL_iDIVC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(AB_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AB_STATIC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AB_dLSHIFT, {
		GPERL_dLSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_iLSHIFT, {
		GPERL_iLSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_sLSHIFT, {
		GPERL_sLSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oLSHIFT, {
		GPERL_oLSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AC_STATIC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AC_dLSHIFT, {
		GPERL_dLSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_iLSHIFT, {
		GPERL_iLSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_sLSHIFT, {
		GPERL_sLSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oLSHIFT, {
		GPERL_oLSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AD_STATIC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AD_dLSHIFT, {
		GPERL_dLSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_iLSHIFT, {
		GPERL_iLSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_sLSHIFT, {
		GPERL_sLSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oLSHIFT, {
		GPERL_oLSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BC_STATIC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BC_dLSHIFT, {
		GPERL_dLSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_iLSHIFT, {
		GPERL_iLSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_sLSHIFT, {
		GPERL_sLSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oLSHIFT, {
		GPERL_oLSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BD_STATIC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BD_dLSHIFT, {
		GPERL_dLSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_iLSHIFT, {
		GPERL_iLSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_sLSHIFT, {
		GPERL_sLSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oLSHIFT, {
		GPERL_oLSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(CD_STATIC_LSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(CD_dLSHIFT, {
		GPERL_dLSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iLSHIFT, {
		GPERL_iLSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_sLSHIFT, {
		GPERL_sLSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oLSHIFT, {
		GPERL_oLSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_LSHIFTC, {
		GPERL_LSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dLSHIFTC, {
		GPERL_dLSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iLSHIFTC, {
		GPERL_iLSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_sLSHIFTC, {
		GPERL_sLSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_oLSHIFTC, {
		GPERL_oLSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_LSHIFTC, {
		GPERL_LSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dLSHIFTC, {
		GPERL_dLSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iLSHIFTC, {
		GPERL_iLSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_sLSHIFTC, {
		GPERL_sLSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_oLSHIFTC, {
		GPERL_oLSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_LSHIFTC, {
		GPERL_LSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dLSHIFTC, {
		GPERL_dLSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iLSHIFTC, {
		GPERL_iLSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_sLSHIFTC, {
		GPERL_sLSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_oLSHIFTC, {
		GPERL_oLSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_LSHIFTC, {
		GPERL_LSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dLSHIFTC, {
		GPERL_dLSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iLSHIFTC, {
		GPERL_iLSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_sLSHIFTC, {
		GPERL_sLSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_oLSHIFTC, {
		GPERL_oLSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(AB_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AB_STATIC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AB_dRSHIFT, {
		GPERL_dRSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_iRSHIFT, {
		GPERL_iRSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_sRSHIFT, {
		GPERL_sRSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AB_oRSHIFT, {
		GPERL_oRSHIFT(0, 1);
		pc++;
		BREAK();
	});
	CASE(AC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AC_STATIC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AC_dRSHIFT, {
		GPERL_dRSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_iRSHIFT, {
		GPERL_iRSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_sRSHIFT, {
		GPERL_sRSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AC_oRSHIFT, {
		GPERL_oRSHIFT(0, 2);
		pc++;
		BREAK();
	});
	CASE(AD_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(AD_STATIC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(AD_dRSHIFT, {
		GPERL_dRSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_iRSHIFT, {
		GPERL_iRSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_sRSHIFT, {
		GPERL_sRSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(AD_oRSHIFT, {
		GPERL_oRSHIFT(0, 3);
		pc++;
		BREAK();
	});
	CASE(BC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BC_STATIC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BC_dRSHIFT, {
		GPERL_dRSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_iRSHIFT, {
		GPERL_iRSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_sRSHIFT, {
		GPERL_sRSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BC_oRSHIFT, {
		GPERL_oRSHIFT(1, 2);
		pc++;
		BREAK();
	});
	CASE(BD_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(BD_STATIC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(BD_dRSHIFT, {
		GPERL_dRSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_iRSHIFT, {
		GPERL_iRSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_sRSHIFT, {
		GPERL_sRSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(BD_oRSHIFT, {
		GPERL_oRSHIFT(1, 3);
		pc++;
		BREAK();
	});
	CASE(CD_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(CD_STATIC_RSHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(CD_dRSHIFT, {
		GPERL_dRSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_iRSHIFT, {
		GPERL_iRSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_sRSHIFT, {
		GPERL_sRSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(CD_oRSHIFT, {
		GPERL_oRSHIFT(2, 3);
		pc++;
		BREAK();
	});
	CASE(A_RSHIFTC, {
		GPERL_RSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dRSHIFTC, {
		GPERL_dRSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iRSHIFTC, {
		GPERL_iRSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_sRSHIFTC, {
		GPERL_sRSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_oRSHIFTC, {
		GPERL_oRSHIFTC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_RSHIFTC, {
		GPERL_RSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dRSHIFTC, {
		GPERL_dRSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iRSHIFTC, {
		GPERL_iRSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_sRSHIFTC, {
		GPERL_sRSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_oRSHIFTC, {
		GPERL_oRSHIFTC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_RSHIFTC, {
		GPERL_RSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dRSHIFTC, {
		GPERL_dRSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iRSHIFTC, {
		GPERL_iRSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_sRSHIFTC, {
		GPERL_sRSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_oRSHIFTC, {
		GPERL_oRSHIFTC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_RSHIFTC, {
		GPERL_RSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dRSHIFTC, {
		GPERL_dRSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iRSHIFTC, {
		GPERL_iRSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_sRSHIFTC, {
		GPERL_sRSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_oRSHIFTC, {
		GPERL_oRSHIFTC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_STATIC_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dJG, {
		GPERL_dCMP_JMP(>, 0, pc->src);
		BREAK();
	});
	CASE(A_iJG, {
		GPERL_iCMP_JMP(>, 0, pc->src);
		BREAK();
	});
	CASE(A_sJG, {
		GPERL_sCMP_JMP(>, 0, pc->src);
		BREAK();
	});
	CASE(A_oJG, {
		GPERL_oCMP_JMP(>, 0, pc->src);
		BREAK();
	});
	CASE(B_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_STATIC_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dJG, {
		GPERL_dCMP_JMP(>, 1, pc->src);
		BREAK();
	});
	CASE(B_iJG, {
		GPERL_iCMP_JMP(>, 1, pc->src);
		BREAK();
	});
	CASE(B_sJG, {
		GPERL_sCMP_JMP(>, 1, pc->src);
		BREAK();
	});
	CASE(B_oJG, {
		GPERL_oCMP_JMP(>, 1, pc->src);
		BREAK();
	});
	CASE(C_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_STATIC_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dJG, {
		GPERL_dCMP_JMP(>, 2, pc->src);
		BREAK();
	});
	CASE(C_iJG, {
		GPERL_iCMP_JMP(>, 2, pc->src);
		BREAK();
	});
	CASE(C_sJG, {
		GPERL_sCMP_JMP(>, 2, pc->src);
		BREAK();
	});
	CASE(C_oJG, {
		GPERL_oCMP_JMP(>, 2, pc->src);
		BREAK();
	});
	CASE(D_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_STATIC_JG, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dJG, {
		GPERL_dCMP_JMP(>, 3, pc->src);
		BREAK();
	});
	CASE(D_iJG, {
		GPERL_iCMP_JMP(>, 3, pc->src);
		BREAK();
	});
	CASE(D_sJG, {
		GPERL_sCMP_JMP(>, 3, pc->src);
		BREAK();
	});
	CASE(D_oJG, {
		GPERL_oCMP_JMP(>, 3, pc->src);
		BREAK();
	});
	CASE(A_JGC, {
		GPERL_CMP_JMPC(>, 0, pc->v);
		BREAK();
	});
	CASE(A_dJGC, {
		GPERL_dCMP_JMPC(>, 0, pc->v);
		BREAK();
	});
	CASE(A_iJGC, {
		GPERL_iCMP_JMPC(>, 0, pc->v);
		BREAK();
	});
	CASE(A_sJGC, {
		GPERL_sCMP_JMPC(>, 0, pc->v);
		BREAK();
	});
	CASE(A_oJGC, {
		GPERL_oCMP_JMPC(>, 0, pc->v);
		BREAK();
	});
	CASE(B_JGC, {
		GPERL_CMP_JMPC(>, 1, pc->v);
		BREAK();
	});
	CASE(B_dJGC, {
		GPERL_dCMP_JMPC(>, 1, pc->v);
		BREAK();
	});
	CASE(B_iJGC, {
		GPERL_iCMP_JMPC(>, 1, pc->v);
		BREAK();
	});
	CASE(B_sJGC, {
		GPERL_sCMP_JMPC(>, 1, pc->v);
		BREAK();
	});
	CASE(B_oJGC, {
		GPERL_oCMP_JMPC(>, 1, pc->v);
		BREAK();
	});
	CASE(C_JGC, {
		GPERL_CMP_JMPC(>, 2, pc->v);
		BREAK();
	});
	CASE(C_dJGC, {
		GPERL_dCMP_JMPC(>, 2, pc->v);
		BREAK();
	});
	CASE(C_iJGC, {
		GPERL_iCMP_JMPC(>, 2, pc->v);
		BREAK();
	});
	CASE(C_sJGC, {
		GPERL_sCMP_JMPC(>, 2, pc->v);
		BREAK();
	});
	CASE(C_oJGC, {
		GPERL_oCMP_JMPC(>, 2, pc->v);
		BREAK();
	});
	CASE(D_JGC, {
		GPERL_CMP_JMPC(>, 3, pc->v);
		BREAK();
	});
	CASE(D_dJGC, {
		GPERL_dCMP_JMPC(>, 3, pc->v);
		BREAK();
	});
	CASE(D_iJGC, {
		GPERL_iCMP_JMPC(>, 3, pc->v);
		BREAK();
	});
	CASE(D_sJGC, {
		GPERL_sCMP_JMPC(>, 3, pc->v);
		BREAK();
	});
	CASE(D_oJGC, {
		GPERL_oCMP_JMPC(>, 3, pc->v);
		BREAK();
	});
	CASE(A_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_STATIC_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dJL, {
		GPERL_dCMP_JMP(<, 0, pc->src);
		BREAK();
	});
	CASE(A_iJL, {
		GPERL_iCMP_JMP(<, 0, pc->src);
		BREAK();
	});
	CASE(A_sJL, {
		GPERL_sCMP_JMP(<, 0, pc->src);
		BREAK();
	});
	CASE(A_oJL, {
		GPERL_oCMP_JMP(<, 0, pc->src);
		BREAK();
	});
	CASE(B_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_STATIC_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dJL, {
		GPERL_dCMP_JMP(<, 1, pc->src);
		BREAK();
	});
	CASE(B_iJL, {
		GPERL_iCMP_JMP(<, 1, pc->src);
		BREAK();
	});
	CASE(B_sJL, {
		GPERL_sCMP_JMP(<, 1, pc->src);
		BREAK();
	});
	CASE(B_oJL, {
		GPERL_oCMP_JMP(<, 1, pc->src);
		BREAK();
	});
	CASE(C_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_STATIC_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dJL, {
		GPERL_dCMP_JMP(<, 2, pc->src);
		BREAK();
	});
	CASE(C_iJL, {
		GPERL_iCMP_JMP(<, 2, pc->src);
		BREAK();
	});
	CASE(C_sJL, {
		GPERL_sCMP_JMP(<, 2, pc->src);
		BREAK();
	});
	CASE(C_oJL, {
		GPERL_oCMP_JMP(<, 2, pc->src);
		BREAK();
	});
	CASE(D_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_STATIC_JL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dJL, {
		GPERL_dCMP_JMP(<, 3, pc->src);
		BREAK();
	});
	CASE(D_iJL, {
		GPERL_iCMP_JMP(<, 3, pc->src);
		BREAK();
	});
	CASE(D_sJL, {
		GPERL_sCMP_JMP(<, 3, pc->src);
		BREAK();
	});
	CASE(D_oJL, {
		GPERL_oCMP_JMP(<, 3, pc->src);
		BREAK();
	});
	CASE(A_JLC, {
		GPERL_CMP_JMPC(<, 0, pc->v);
		BREAK();
	});
	CASE(A_dJLC, {
		GPERL_dCMP_JMPC(<, 0, pc->v);
		BREAK();
	});
	CASE(A_iJLC, {
		GPERL_iCMP_JMPC(<, 0, pc->v);
		BREAK();
	});
	CASE(A_sJLC, {
		GPERL_sCMP_JMPC(<, 0, pc->v);
		BREAK();
	});
	CASE(A_oJLC, {
		GPERL_oCMP_JMPC(<, 0, pc->v);
		BREAK();
	});
	CASE(B_JLC, {
		GPERL_CMP_JMPC(<, 1, pc->v);
		BREAK();
	});
	CASE(B_dJLC, {
		GPERL_dCMP_JMPC(<, 1, pc->v);
		BREAK();
	});
	CASE(B_iJLC, {
		GPERL_iCMP_JMPC(<, 1, pc->v);
		BREAK();
	});
	CASE(B_sJLC, {
		GPERL_sCMP_JMPC(<, 1, pc->v);
		BREAK();
	});
	CASE(B_oJLC, {
		GPERL_oCMP_JMPC(<, 1, pc->v);
		BREAK();
	});
	CASE(C_JLC, {
		GPERL_CMP_JMPC(<, 2, pc->v);
		BREAK();
	});
	CASE(C_dJLC, {
		GPERL_dCMP_JMPC(<, 2, pc->v);
		BREAK();
	});
	CASE(C_iJLC, {
		GPERL_iCMP_JMPC(<, 2, pc->v);
		BREAK();
	});
	CASE(C_sJLC, {
		GPERL_sCMP_JMPC(<, 2, pc->v);
		BREAK();
	});
	CASE(C_oJLC, {
		GPERL_oCMP_JMPC(<, 2, pc->v);
		BREAK();
	});
	CASE(D_JLC, {
		GPERL_CMP_JMPC(<, 3, pc->v);
		BREAK();
	});
	CASE(D_dJLC, {
		GPERL_dCMP_JMPC(<, 3, pc->v);
		BREAK();
	});
	CASE(D_iJLC, {
		GPERL_iCMP_JMPC(<, 3, pc->v);
		BREAK();
	});
	CASE(D_sJLC, {
		GPERL_sCMP_JMPC(<, 3, pc->v);
		BREAK();
	});
	CASE(D_oJLC, {
		GPERL_oCMP_JMPC(<, 3, pc->v);
		BREAK();
	});
	CASE(A_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_STATIC_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dJGE, {
		GPERL_dCMP_JMP(>=, 0, pc->src);
		BREAK();
	});
	CASE(A_iJGE, {
		GPERL_iCMP_JMP(>=, 0, pc->src);
		BREAK();
	});
	CASE(A_sJGE, {
		GPERL_sCMP_JMP(>=, 0, pc->src);
		BREAK();
	});
	CASE(A_oJGE, {
		GPERL_oCMP_JMP(>=, 0, pc->src);
		BREAK();
	});
	CASE(B_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_STATIC_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dJGE, {
		GPERL_dCMP_JMP(>=, 1, pc->src);
		BREAK();
	});
	CASE(B_iJGE, {
		GPERL_iCMP_JMP(>=, 1, pc->src);
		BREAK();
	});
	CASE(B_sJGE, {
		GPERL_sCMP_JMP(>=, 1, pc->src);
		BREAK();
	});
	CASE(B_oJGE, {
		GPERL_oCMP_JMP(>=, 1, pc->src);
		BREAK();
	});
	CASE(C_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_STATIC_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dJGE, {
		GPERL_dCMP_JMP(>=, 2, pc->src);
		BREAK();
	});
	CASE(C_iJGE, {
		GPERL_iCMP_JMP(>=, 2, pc->src);
		BREAK();
	});
	CASE(C_sJGE, {
		GPERL_sCMP_JMP(>=, 2, pc->src);
		BREAK();
	});
	CASE(C_oJGE, {
		GPERL_oCMP_JMP(>=, 2, pc->src);
		BREAK();
	});
	CASE(D_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_STATIC_JGE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dJGE, {
		GPERL_dCMP_JMP(>=, 3, pc->src);
		BREAK();
	});
	CASE(D_iJGE, {
		GPERL_iCMP_JMP(>=, 3, pc->src);
		BREAK();
	});
	CASE(D_sJGE, {
		GPERL_sCMP_JMP(>=, 3, pc->src);
		BREAK();
	});
	CASE(D_oJGE, {
		GPERL_oCMP_JMP(>=, 3, pc->src);
		BREAK();
	});
	CASE(A_JGEC, {
		GPERL_CMP_JMPC(>=, 0, pc->v);
		BREAK();
	});
	CASE(A_dJGEC, {
		GPERL_dCMP_JMPC(>=, 0, pc->v);
		BREAK();
	});
	CASE(A_iJGEC, {
		GPERL_iCMP_JMPC(>=, 0, pc->v);
		BREAK();
	});
	CASE(A_sJGEC, {
		GPERL_sCMP_JMPC(>=, 0, pc->v);
		BREAK();
	});
	CASE(A_oJGEC, {
		GPERL_oCMP_JMPC(>=, 0, pc->v);
		BREAK();
	});
	CASE(B_JGEC, {
		GPERL_CMP_JMPC(>=, 1, pc->v);
		BREAK();
	});
	CASE(B_dJGEC, {
		GPERL_dCMP_JMPC(>=, 1, pc->v);
		BREAK();
	});
	CASE(B_iJGEC, {
		GPERL_iCMP_JMPC(>=, 1, pc->v);
		BREAK();
	});
	CASE(B_sJGEC, {
		GPERL_sCMP_JMPC(>=, 1, pc->v);
		BREAK();
	});
	CASE(B_oJGEC, {
		GPERL_oCMP_JMPC(>=, 1, pc->v);
		BREAK();
	});
	CASE(C_JGEC, {
		GPERL_CMP_JMPC(>=, 2, pc->v);
		BREAK();
	});
	CASE(C_dJGEC, {
		GPERL_dCMP_JMPC(>=, 2, pc->v);
		BREAK();
	});
	CASE(C_iJGEC, {
		GPERL_iCMP_JMPC(>=, 2, pc->v);
		BREAK();
	});
	CASE(C_sJGEC, {
		GPERL_sCMP_JMPC(>=, 2, pc->v);
		BREAK();
	});
	CASE(C_oJGEC, {
		GPERL_oCMP_JMPC(>=, 2, pc->v);
		BREAK();
	});
	CASE(D_JGEC, {
		GPERL_CMP_JMPC(>=, 3, pc->v);
		BREAK();
	});
	CASE(D_dJGEC, {
		GPERL_dCMP_JMPC(>=, 3, pc->v);
		BREAK();
	});
	CASE(D_iJGEC, {
		GPERL_iCMP_JMPC(>=, 3, pc->v);
		BREAK();
	});
	CASE(D_sJGEC, {
		GPERL_sCMP_JMPC(>=, 3, pc->v);
		BREAK();
	});
	CASE(D_oJGEC, {
		GPERL_oCMP_JMPC(>=, 3, pc->v);
		BREAK();
	});
	CASE(A_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_STATIC_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dJLE, {
		GPERL_dCMP_JMP(<=, 0, pc->src);
		BREAK();
	});
	CASE(A_iJLE, {
		GPERL_iCMP_JMP(<=, 0, pc->src);
		BREAK();
	});
	CASE(A_sJLE, {
		GPERL_sCMP_JMP(<=, 0, pc->src);
		BREAK();
	});
	CASE(A_oJLE, {
		GPERL_oCMP_JMP(<=, 0, pc->src);
		BREAK();
	});
	CASE(B_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_STATIC_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dJLE, {
		GPERL_dCMP_JMP(<=, 1, pc->src);
		BREAK();
	});
	CASE(B_iJLE, {
		GPERL_iCMP_JMP(<=, 1, pc->src);
		BREAK();
	});
	CASE(B_sJLE, {
		GPERL_sCMP_JMP(<=, 1, pc->src);
		BREAK();
	});
	CASE(B_oJLE, {
		GPERL_oCMP_JMP(<=, 1, pc->src);
		BREAK();
	});
	CASE(C_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_STATIC_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dJLE, {
		GPERL_dCMP_JMP(<=, 2, pc->src);
		BREAK();
	});
	CASE(C_iJLE, {
		GPERL_iCMP_JMP(<=, 2, pc->src);
		BREAK();
	});
	CASE(C_sJLE, {
		GPERL_sCMP_JMP(<=, 2, pc->src);
		BREAK();
	});
	CASE(C_oJLE, {
		GPERL_oCMP_JMP(<=, 2, pc->src);
		BREAK();
	});
	CASE(D_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_STATIC_JLE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dJLE, {
		GPERL_dCMP_JMP(<=, 3, pc->src);
		BREAK();
	});
	CASE(D_iJLE, {
		GPERL_iCMP_JMP(<=, 3, pc->src);
		BREAK();
	});
	CASE(D_sJLE, {
		GPERL_sCMP_JMP(<=, 3, pc->src);
		BREAK();
	});
	CASE(D_oJLE, {
		GPERL_oCMP_JMP(<=, 3, pc->src);
		BREAK();
	});
	CASE(A_JLEC, {
		GPERL_CMP_JMPC(<=, 0, pc->v);
		BREAK();
	});
	CASE(A_dJLEC, {
		GPERL_dCMP_JMPC(<=, 0, pc->v);
		BREAK();
	});
	CASE(A_iJLEC, {
		GPERL_iCMP_JMPC(<=, 0, pc->v);
		BREAK();
	});
	CASE(A_sJLEC, {
		GPERL_sCMP_JMPC(<=, 0, pc->v);
		BREAK();
	});
	CASE(A_oJLEC, {
		GPERL_oCMP_JMPC(<=, 0, pc->v);
		BREAK();
	});
	CASE(B_JLEC, {
		GPERL_CMP_JMPC(<=, 1, pc->v);
		BREAK();
	});
	CASE(B_dJLEC, {
		GPERL_dCMP_JMPC(<=, 1, pc->v);
		BREAK();
	});
	CASE(B_iJLEC, {
		GPERL_iCMP_JMPC(<=, 1, pc->v);
		BREAK();
	});
	CASE(B_sJLEC, {
		GPERL_sCMP_JMPC(<=, 1, pc->v);
		BREAK();
	});
	CASE(B_oJLEC, {
		GPERL_oCMP_JMPC(<=, 1, pc->v);
		BREAK();
	});
	CASE(C_JLEC, {
		GPERL_CMP_JMPC(<=, 2, pc->v);
		BREAK();
	});
	CASE(C_dJLEC, {
		GPERL_dCMP_JMPC(<=, 2, pc->v);
		BREAK();
	});
	CASE(C_iJLEC, {
		GPERL_iCMP_JMPC(<=, 2, pc->v);
		BREAK();
	});
	CASE(C_sJLEC, {
		GPERL_sCMP_JMPC(<=, 2, pc->v);
		BREAK();
	});
	CASE(C_oJLEC, {
		GPERL_oCMP_JMPC(<=, 2, pc->v);
		BREAK();
	});
	CASE(D_JLEC, {
		GPERL_CMP_JMPC(<=, 3, pc->v);
		BREAK();
	});
	CASE(D_dJLEC, {
		GPERL_dCMP_JMPC(<=, 3, pc->v);
		BREAK();
	});
	CASE(D_iJLEC, {
		GPERL_iCMP_JMPC(<=, 3, pc->v);
		BREAK();
	});
	CASE(D_sJLEC, {
		GPERL_sCMP_JMPC(<=, 3, pc->v);
		BREAK();
	});
	CASE(D_oJLEC, {
		GPERL_oCMP_JMPC(<=, 3, pc->v);
		BREAK();
	});
	CASE(A_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_STATIC_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dJE, {
		GPERL_dCMP_JMP(==, 0, pc->src);
		BREAK();
	});
	CASE(A_iJE, {
		GPERL_iCMP_JMP(==, 0, pc->src);
		BREAK();
	});
	CASE(A_sJE, {
		GPERL_sCMP_JMP(==, 0, pc->src);
		BREAK();
	});
	CASE(A_oJE, {
		GPERL_oCMP_JMP(==, 0, pc->src);
		BREAK();
	});
	CASE(B_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_STATIC_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dJE, {
		GPERL_dCMP_JMP(==, 1, pc->src);
		BREAK();
	});
	CASE(B_iJE, {
		GPERL_iCMP_JMP(==, 1, pc->src);
		BREAK();
	});
	CASE(B_sJE, {
		GPERL_sCMP_JMP(==, 1, pc->src);
		BREAK();
	});
	CASE(B_oJE, {
		GPERL_oCMP_JMP(==, 1, pc->src);
		BREAK();
	});
	CASE(C_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_STATIC_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dJE, {
		GPERL_dCMP_JMP(==, 2, pc->src);
		BREAK();
	});
	CASE(C_iJE, {
		GPERL_iCMP_JMP(==, 2, pc->src);
		BREAK();
	});
	CASE(C_sJE, {
		GPERL_sCMP_JMP(==, 2, pc->src);
		BREAK();
	});
	CASE(C_oJE, {
		GPERL_oCMP_JMP(==, 2, pc->src);
		BREAK();
	});
	CASE(D_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_STATIC_JE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dJE, {
		GPERL_dCMP_JMP(==, 3, pc->src);
		BREAK();
	});
	CASE(D_iJE, {
		GPERL_iCMP_JMP(==, 3, pc->src);
		BREAK();
	});
	CASE(D_sJE, {
		GPERL_sCMP_JMP(==, 3, pc->src);
		BREAK();
	});
	CASE(D_oJE, {
		GPERL_oCMP_JMP(==, 3, pc->src);
		BREAK();
	});
	CASE(A_JEC, {
		GPERL_CMP_JMPC(==, 0, pc->v);
		BREAK();
	});
	CASE(A_dJEC, {
		GPERL_dCMP_JMPC(==, 0, pc->v);
		BREAK();
	});
	CASE(A_iJEC, {
		GPERL_iCMP_JMPC(==, 0, pc->v);
		BREAK();
	});
	CASE(A_sJEC, {
		GPERL_sCMP_JMPC(==, 0, pc->v);
		BREAK();
	});
	CASE(A_oJEC, {
		GPERL_oCMP_JMPC(==, 0, pc->v);
		BREAK();
	});
	CASE(B_JEC, {
		GPERL_CMP_JMPC(==, 1, pc->v);
		BREAK();
	});
	CASE(B_dJEC, {
		GPERL_dCMP_JMPC(==, 1, pc->v);
		BREAK();
	});
	CASE(B_iJEC, {
		GPERL_iCMP_JMPC(==, 1, pc->v);
		BREAK();
	});
	CASE(B_sJEC, {
		GPERL_sCMP_JMPC(==, 1, pc->v);
		BREAK();
	});
	CASE(B_oJEC, {
		GPERL_oCMP_JMPC(==, 1, pc->v);
		BREAK();
	});
	CASE(C_JEC, {
		GPERL_CMP_JMPC(==, 2, pc->v);
		BREAK();
	});
	CASE(C_dJEC, {
		GPERL_dCMP_JMPC(==, 2, pc->v);
		BREAK();
	});
	CASE(C_iJEC, {
		GPERL_iCMP_JMPC(==, 2, pc->v);
		BREAK();
	});
	CASE(C_sJEC, {
		GPERL_sCMP_JMPC(==, 2, pc->v);
		BREAK();
	});
	CASE(C_oJEC, {
		GPERL_oCMP_JMPC(==, 2, pc->v);
		BREAK();
	});
	CASE(D_JEC, {
		GPERL_CMP_JMPC(==, 3, pc->v);
		BREAK();
	});
	CASE(D_dJEC, {
		GPERL_dCMP_JMPC(==, 3, pc->v);
		BREAK();
	});
	CASE(D_iJEC, {
		GPERL_iCMP_JMPC(==, 3, pc->v);
		BREAK();
	});
	CASE(D_sJEC, {
		GPERL_sCMP_JMPC(==, 3, pc->v);
		BREAK();
	});
	CASE(D_oJEC, {
		GPERL_oCMP_JMPC(==, 3, pc->v);
		BREAK();
	});
	CASE(A_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_STATIC_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dJNE, {
		GPERL_dCMP_JMP(!=, 0, pc->src);
		BREAK();
	});
	CASE(A_iJNE, {
		GPERL_iCMP_JMP(!=, 0, pc->src);
		BREAK();
	});
	CASE(A_sJNE, {
		GPERL_sCMP_JMP(!=, 0, pc->src);
		BREAK();
	});
	CASE(A_oJNE, {
		GPERL_oCMP_JMP(!=, 0, pc->src);
		BREAK();
	});
	CASE(B_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_STATIC_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dJNE, {
		GPERL_dCMP_JMP(!=, 1, pc->src);
		BREAK();
	});
	CASE(B_iJNE, {
		GPERL_iCMP_JMP(!=, 1, pc->src);
		BREAK();
	});
	CASE(B_sJNE, {
		GPERL_sCMP_JMP(!=, 1, pc->src);
		BREAK();
	});
	CASE(B_oJNE, {
		GPERL_oCMP_JMP(!=, 1, pc->src);
		BREAK();
	});
	CASE(C_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_STATIC_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dJNE, {
		GPERL_dCMP_JMP(!=, 2, pc->src);
		BREAK();
	});
	CASE(C_iJNE, {
		GPERL_iCMP_JMP(!=, 2, pc->src);
		BREAK();
	});
	CASE(C_sJNE, {
		GPERL_sCMP_JMP(!=, 2, pc->src);
		BREAK();
	});
	CASE(C_oJNE, {
		GPERL_oCMP_JMP(!=, 2, pc->src);
		BREAK();
	});
	CASE(D_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_STATIC_JNE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dJNE, {
		GPERL_dCMP_JMP(!=, 3, pc->src);
		BREAK();
	});
	CASE(D_iJNE, {
		GPERL_iCMP_JMP(!=, 3, pc->src);
		BREAK();
	});
	CASE(D_sJNE, {
		GPERL_sCMP_JMP(!=, 3, pc->src);
		BREAK();
	});
	CASE(D_oJNE, {
		GPERL_oCMP_JMP(!=, 3, pc->src);
		BREAK();
	});
	CASE(A_JNEC, {
		GPERL_CMP_JMPC(!=, 0, pc->v);
		BREAK();
	});
	CASE(A_dJNEC, {
		GPERL_dCMP_JMPC(!=, 0, pc->v);
		BREAK();
	});
	CASE(A_iJNEC, {
		GPERL_iCMP_JMPC(!=, 0, pc->v);
		BREAK();
	});
	CASE(A_sJNEC, {
		GPERL_sCMP_JMPC(!=, 0, pc->v);
		BREAK();
	});
	CASE(A_oJNEC, {
		GPERL_oCMP_JMPC(!=, 0, pc->v);
		BREAK();
	});
	CASE(B_JNEC, {
		GPERL_CMP_JMPC(!=, 1, pc->v);
		BREAK();
	});
	CASE(B_dJNEC, {
		GPERL_dCMP_JMPC(!=, 1, pc->v);
		BREAK();
	});
	CASE(B_iJNEC, {
		GPERL_iCMP_JMPC(!=, 1, pc->v);
		BREAK();
	});
	CASE(B_sJNEC, {
		GPERL_sCMP_JMPC(!=, 1, pc->v);
		BREAK();
	});
	CASE(B_oJNEC, {
		GPERL_oCMP_JMPC(!=, 1, pc->v);
		BREAK();
	});
	CASE(C_JNEC, {
		GPERL_CMP_JMPC(!=, 2, pc->v);
		BREAK();
	});
	CASE(C_dJNEC, {
		GPERL_dCMP_JMPC(!=, 2, pc->v);
		BREAK();
	});
	CASE(C_iJNEC, {
		GPERL_iCMP_JMPC(!=, 2, pc->v);
		BREAK();
	});
	CASE(C_sJNEC, {
		GPERL_sCMP_JMPC(!=, 2, pc->v);
		BREAK();
	});
	CASE(C_oJNEC, {
		GPERL_oCMP_JMPC(!=, 2, pc->v);
		BREAK();
	});
	CASE(D_JNEC, {
		GPERL_CMP_JMPC(!=, 3, pc->v);
		BREAK();
	});
	CASE(D_dJNEC, {
		GPERL_dCMP_JMPC(!=, 3, pc->v);
		BREAK();
	});
	CASE(D_iJNEC, {
		GPERL_iCMP_JMPC(!=, 3, pc->v);
		BREAK();
	});
	CASE(D_sJNEC, {
		GPERL_sCMP_JMPC(!=, 3, pc->v);
		BREAK();
	});
	CASE(D_oJNEC, {
		GPERL_oCMP_JMPC(!=, 3, pc->v);
		BREAK();
	});
	CASE(A_StringADD, {
		GPERL_StringADD(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_STATIC_StringADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dStringADD, {
		GPERL_dStringADD(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iStringADD, {
		GPERL_iStringADD(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_sStringADD, {
		GPERL_sStringADD(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_oStringADD, {
		GPERL_oStringADD(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_StringADD, {
		GPERL_StringADD(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_STATIC_StringADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dStringADD, {
		GPERL_dStringADD(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iStringADD, {
		GPERL_iStringADD(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_sStringADD, {
		GPERL_sStringADD(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_oStringADD, {
		GPERL_oStringADD(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_StringADD, {
		GPERL_StringADD(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_STATIC_StringADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dStringADD, {
		GPERL_dStringADD(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iStringADD, {
		GPERL_iStringADD(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_sStringADD, {
		GPERL_sStringADD(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_oStringADD, {
		GPERL_oStringADD(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_StringADD, {
		GPERL_StringADD(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_STATIC_StringADD, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dStringADD, {
		GPERL_dStringADD(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iStringADD, {
		GPERL_iStringADD(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_sStringADD, {
		GPERL_sStringADD(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_oStringADD, {
		GPERL_oStringADD(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_StringADDC, {
		GPERL_StringADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_dStringADDC, {
		GPERL_dStringADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_iStringADDC, {
		GPERL_iStringADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(A_sStringADDC, {
		GPERL_sStringADDC(0, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_StringADDC, {
		GPERL_StringADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_dStringADDC, {
		GPERL_dStringADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_iStringADDC, {
		GPERL_iStringADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(B_sStringADDC, {
		GPERL_sStringADDC(1, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_StringADDC, {
		GPERL_StringADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_dStringADDC, {
		GPERL_dStringADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_iStringADDC, {
		GPERL_iStringADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(C_sStringADDC, {
		GPERL_sStringADDC(2, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_StringADDC, {
		GPERL_StringADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_dStringADDC, {
		GPERL_dStringADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_iStringADDC, {
		GPERL_iStringADDC(3, pc->v);
		pc++;
		BREAK();
	});
	CASE(D_sStringADDC, {
		GPERL_sStringADDC(3, pc->v);
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
	CASE(A_JIT_COUNTDOWN_RET, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_RET, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_RET, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_RET, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_WRITE, {
		GPERL_WRITE(0);
		pc++;
		BREAK();
	});
	CASE(A_STATIC_WRITE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dWRITE, {
		GPERL_dWRITE(0);
		pc++;
		BREAK();
	});
	CASE(A_iWRITE, {
		GPERL_iWRITE(0);
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
	CASE(B_WRITE, {
		GPERL_WRITE(1);
		pc++;
		BREAK();
	});
	CASE(B_STATIC_WRITE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dWRITE, {
		GPERL_dWRITE(1);
		pc++;
		BREAK();
	});
	CASE(B_iWRITE, {
		GPERL_iWRITE(1);
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
	CASE(C_WRITE, {
		GPERL_WRITE(2);
		pc++;
		BREAK();
	});
	CASE(C_STATIC_WRITE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dWRITE, {
		GPERL_dWRITE(2);
		pc++;
		BREAK();
	});
	CASE(C_iWRITE, {
		GPERL_iWRITE(2);
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
	CASE(D_WRITE, {
		GPERL_WRITE(3);
		pc++;
		BREAK();
	});
	CASE(D_STATIC_WRITE, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dWRITE, {
		GPERL_dWRITE(3);
		pc++;
		BREAK();
	});
	CASE(D_iWRITE, {
		GPERL_iWRITE(3);
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
	CASE(A_LET, {
		GPERL_LET(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_LET, {
		GPERL_LET(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_LET, {
		GPERL_LET(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_LET, {
		GPERL_LET(3, pc->src);
		pc++;
		BREAK();
	});
	CASE(A_MLET, {
		GPERL_MLET(0, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(B_MLET, {
		GPERL_MLET(1, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(C_MLET, {
		GPERL_MLET(2, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(D_MLET, {
		GPERL_MLET(3, pc->src, pc->idx);
		pc++;
		BREAK();
	});
	CASE(A_CLOSURE, {
		GPERL_CLOSURE(0, pc->src, A_CLOSURE);
		pc++;
		BREAK();
	});
	CASE(B_CLOSURE, {
		GPERL_CLOSURE(1, pc->src, B_CLOSURE);
		pc++;
		BREAK();
	});
	CASE(C_CLOSURE, {
		GPERL_CLOSURE(2, pc->src, C_CLOSURE);
		pc++;
		BREAK();
	});
	CASE(D_CLOSURE, {
		GPERL_CLOSURE(3, pc->src, D_CLOSURE);
		pc++;
		BREAK();
	});
	CASE(A_STATIC_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_STATIC_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_STATIC_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_STATIC_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_DYNAMIC_CALL, {
		GPERL_DYNAMIC_CALL(0, pc->src, A_DYNAMIC_CALL);
		pc++;
		BREAK();
	});
	CASE(B_DYNAMIC_CALL, {
		GPERL_DYNAMIC_CALL(1, pc->src, B_DYNAMIC_CALL);
		pc++;
		BREAK();
	});
	CASE(C_DYNAMIC_CALL, {
		GPERL_DYNAMIC_CALL(2, pc->src, C_DYNAMIC_CALL);
		pc++;
		BREAK();
	});
	CASE(D_DYNAMIC_CALL, {
		GPERL_DYNAMIC_CALL(3, pc->src, D_DYNAMIC_CALL);
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
	CASE(A_JIT_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_CALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_FASTCALL0, {
		GPERL_FASTCALL0(pc->arg0, 0, pc->src, A_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(B_FASTCALL0, {
		GPERL_FASTCALL0(pc->arg0, 1, pc->src, B_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(C_FASTCALL0, {
		GPERL_FASTCALL0(pc->arg0, 2, pc->src, C_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(D_FASTCALL0, {
		GPERL_FASTCALL0(pc->arg0, 3, pc->src, D_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_FASTCALL1, {
		GPERL_FASTCALL1(pc->arg0, pc->arg1, 0, pc->src, A_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(B_FASTCALL1, {
		GPERL_FASTCALL1(pc->arg0, pc->arg1, 1, pc->src, B_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(C_FASTCALL1, {
		GPERL_FASTCALL1(pc->arg0, pc->arg1, 2, pc->src, C_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(D_FASTCALL1, {
		GPERL_FASTCALL1(pc->arg0, pc->arg1, 3, pc->src, D_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_FASTCALL2, {
		GPERL_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 0, pc->src, A_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(B_FASTCALL2, {
		GPERL_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 1, pc->src, B_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(C_FASTCALL2, {
		GPERL_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 2, pc->src, C_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(D_FASTCALL2, {
		GPERL_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 3, pc->src, D_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_FASTCALL3, {
		GPERL_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 0, pc->src, A_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(B_FASTCALL3, {
		GPERL_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 1, pc->src, B_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(C_FASTCALL3, {
		GPERL_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 2, pc->src, C_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(D_FASTCALL3, {
		GPERL_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 3, pc->src, D_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
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
	CASE(A_JIT_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_SELFCALL, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_SELF_FASTCALL0, {
		GPERL_SELF_FASTCALL0(pc->arg0, 0, A_SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(B_SELF_FASTCALL0, {
		GPERL_SELF_FASTCALL0(pc->arg0, 1, B_SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(C_SELF_FASTCALL0, {
		GPERL_SELF_FASTCALL0(pc->arg0, 2, C_SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(D_SELF_FASTCALL0, {
		GPERL_SELF_FASTCALL0(pc->arg0, 3, D_SELF_FASTCALL0);
		pc++;
		BREAK();
	});
	CASE(A_JIT_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_SELF_FASTCALL0, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_SELF_FASTCALL1, {
		GPERL_SELF_FASTCALL1(pc->arg0, pc->arg1, 0, A_SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(B_SELF_FASTCALL1, {
		GPERL_SELF_FASTCALL1(pc->arg0, pc->arg1, 1, B_SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(C_SELF_FASTCALL1, {
		GPERL_SELF_FASTCALL1(pc->arg0, pc->arg1, 2, C_SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(D_SELF_FASTCALL1, {
		GPERL_SELF_FASTCALL1(pc->arg0, pc->arg1, 3, D_SELF_FASTCALL1);
		pc++;
		BREAK();
	});
	CASE(A_JIT_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_SELF_FASTCALL1, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_SELF_FASTCALL2, {
		GPERL_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 0, A_SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(B_SELF_FASTCALL2, {
		GPERL_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 1, B_SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(C_SELF_FASTCALL2, {
		GPERL_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 2, C_SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(D_SELF_FASTCALL2, {
		GPERL_SELF_FASTCALL2(pc->arg0, pc->arg1, pc->arg2, 3, D_SELF_FASTCALL2);
		pc++;
		BREAK();
	});
	CASE(A_JIT_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_SELF_FASTCALL2, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_SELF_FASTCALL3, {
		GPERL_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 0, A_SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(B_SELF_FASTCALL3, {
		GPERL_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 1, B_SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(C_SELF_FASTCALL3, {
		GPERL_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 2, C_SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(D_SELF_FASTCALL3, {
		GPERL_SELF_FASTCALL3(pc->arg0, pc->arg1, pc->arg2, pc->arg3, 3, D_SELF_FASTCALL3);
		pc++;
		BREAK();
	});
	CASE(A_JIT_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_JIT_COUNTDOWN_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(B_JIT_COUNTDOWN_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(C_JIT_COUNTDOWN_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(D_JIT_COUNTDOWN_SELF_FASTCALL3, {
		int type = TYPE_CHECK(reg[pc->dst]);
		goto *jmp_table[pc->op + 2 + type];
		BREAK();
	});
	CASE(A_SHIFT, {
		GPERL_SHIFT(0);
		pc++;
		BREAK();
	});
	CASE(A_STATIC_SHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(A_dSHIFT, {
		GPERL_dSHIFT(0);
		pc++;
		BREAK();
	});
	CASE(A_iSHIFT, {
		GPERL_iSHIFT(0);
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
	CASE(B_SHIFT, {
		GPERL_SHIFT(1);
		pc++;
		BREAK();
	});
	CASE(B_STATIC_SHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(B_dSHIFT, {
		GPERL_dSHIFT(1);
		pc++;
		BREAK();
	});
	CASE(B_iSHIFT, {
		GPERL_iSHIFT(1);
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
	CASE(C_SHIFT, {
		GPERL_SHIFT(2);
		pc++;
		BREAK();
	});
	CASE(C_STATIC_SHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(C_dSHIFT, {
		GPERL_dSHIFT(2);
		pc++;
		BREAK();
	});
	CASE(C_iSHIFT, {
		GPERL_iSHIFT(2);
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
	CASE(D_SHIFT, {
		GPERL_SHIFT(3);
		pc++;
		BREAK();
	});
	CASE(D_STATIC_SHIFT, {
		int type = TYPE_CHECK(reg[pc->dst]);
		pc->opnext = jmp_table[pc->op + 1 + type];
		BREAK();
	});
	CASE(D_dSHIFT, {
		GPERL_dSHIFT(3);
		pc++;
		BREAK();
	});
	CASE(D_iSHIFT, {
		GPERL_iSHIFT(3);
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
	CASE(A_aPUSH, {
		GPERL_aPUSH(0, pc->src);
		pc++;
		BREAK();
	});
	CASE(B_aPUSH, {
		GPERL_aPUSH(1, pc->src);
		pc++;
		BREAK();
	});
	CASE(C_aPUSH, {
		GPERL_aPUSH(2, pc->src);
		pc++;
		BREAK();
	});
	CASE(D_aPUSH, {
		GPERL_aPUSH(3, pc->src);
		pc++;
		BREAK();
	});
