.macro		_asm_extable, insn, fixup
	.pushsection	__ex_table, "a"
	.align		3
	.long		(\insn - .), (\fixup - .)
	.popsection
.endm

.macro		_cond_extable, insn, fixup
	.ifnc		\fixup,
	_asm_extable	\insn, \fixup
	.endif
.endm

.macro	read_ctr, reg
	mrs	\reg, ctr_el0			// read CTR
	nop
.endm

.macro __dcache_op_workaround_clean_cache, op, addr
	dc	\op, \addr
.endm

.macro	dcache_line_size, reg, tmp
	read_ctr	\tmp
	ubfm		\tmp, \tmp, #16, #19	// cache line size encoding
	mov		\reg, #4		// bytes per word
	lsl		\reg, \reg, \tmp	// actual cache line size
.endm

.macro dcache_by_line_op op, domain, start, end, tmp1, tmp2, fixup
	dcache_line_size \tmp1, \tmp2
	sub	\tmp2, \tmp1, #1
	bic	\start, \start, \tmp2
.Ldcache_op\@:
	.ifc	\op, cvau
	__dcache_op_workaround_clean_cache \op, \start
	.else
	.ifc	\op, cvac
	__dcache_op_workaround_clean_cache \op, \start
	.else
	.ifc	\op, cvap
	sys	3, c7, c12, 1, \start	// dc cvap
	.else
	.ifc	\op, cvadp
	sys	3, c7, c13, 1, \start	// dc cvadp
	.else
	dc	\op, \start
	.endif
	.endif
	.endif
	.endif
	add	\start, \start, \tmp1
	cmp	\start, \end
	b.lo	.Ldcache_op\@
	dsb	\domain

	_cond_extable .Ldcache_op\@, \fixup
.endm

.global ax_dcache_clean_inval_poc
ax_dcache_clean_inval_poc:
    dcache_by_line_op civac, sy, x0, x1, x2, x3
	ret

.global ax_dcache_inval_poc
ax_dcache_inval_poc:
	dcache_line_size x2, x3
	sub	x3, x2, #1
	tst	x1, x3				// end cache line aligned?
	bic	x1, x1, x3
	b.eq	1f
	dc	civac, x1			// clean & invalidate D / U line
1:	tst	x0, x3				// start cache line aligned?
	bic	x0, x0, x3
	b.eq	2f
	dc	civac, x0			// clean & invalidate D / U line
	b	3f
2:	dc	ivac, x0			// invalidate D / U line
3:	add	x0, x0, x2
	cmp	x0, x1
	b.lo	2b
	dsb	sy
	ret