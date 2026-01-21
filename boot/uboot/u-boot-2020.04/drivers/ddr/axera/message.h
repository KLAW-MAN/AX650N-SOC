/*
 * Copyright (c) 2022 AXERA.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
extern int early_printf(const char *fmt, ...);
static void wait_uct_shadow_regs(int val, ulong base_addr)
{
	int has_message = 0;
	int rd_cnt = 0;
	while (1) {
		/*
		 * read UctShadowRegs
		 */
		has_message = core_read_halfword(base_addr + 0x000d0004 * 2);
		rd_cnt++;
		if ((has_message & 1) == val)
			break;
		udelay(1);
		if(rd_cnt >= DDR_WAIT_TIMEOUT) {
			early_printf("wait_uct_shadow_regs fail, %x, %lx\n", val, base_addr);
			while(1);
		}
	}
}
/* mode sets 16 bit (major message) or 32 bit (streaming message) */
static int get_mail(ulong phy_base, int mode)
{
	int tmp, mail;
	wait_uct_shadow_regs(0, phy_base);
	mail = core_read_halfword(phy_base + (UCT_WRITE_ONLY_SHADOW << 1));
	if (mode == 32) {
		tmp = core_read_halfword(phy_base + (UCT_DAT_WRITE_ONLY_SHADOW << 1));
		mail |= (tmp << 16);
	}
	core_write_halfword(phy_base + (DCT_WRITE_PROT << 1),0);	//user ack
	wait_uct_shadow_regs(1, phy_base);
	core_write_halfword(phy_base + (DCT_WRITE_PROT << 1), 1);
	return mail;
}
#if PHY_DEBUG_ENABLE
static PMU_SMB_LPDDR4X_1D_t msgBlk_1d;
static PMU_SMB_LPDDR4X_2D_t msgBlk_2d;
static void msgBlk_print(void *msgblk_ptr, int train_2d)
{
	PMU_SMB_LPDDR4X_1D_t *msgBlk1d_ptr;
	PMU_SMB_LPDDR4X_2D_t *msgBlk2d_ptr;
	if (train_2d) {
		msgBlk2d_ptr = msgblk_ptr;
		early_printf("----------------------2D trained data--------------------\r\n");
		early_printf("CsTestFail = 0x%x\r\n", msgBlk2d_ptr->CsTestFail);
		early_printf("CDD_ChA_RR_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_RR_1_0);
		early_printf("CDD_ChA_RR_0_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_RR_0_1);
		early_printf("CDD_ChA_RW_1_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_RW_1_1);
		early_printf("CDD_ChA_RW_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_RW_1_0);
		early_printf("CDD_ChA_WR_1_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_WR_1_1);
		early_printf("CDD_ChA_WR_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_WR_1_0);
		early_printf("CDD_ChA_WR_0_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_WR_0_1);
		early_printf("CDD_ChA_WR_0_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_WR_0_0);
		early_printf("CDD_ChA_WW_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_WW_1_0);
		early_printf("CDD_ChA_WW_0_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChA_WW_0_1);
		early_printf("TrainedVREFDQ_A0 = 0x%x\r\n", msgBlk2d_ptr->TrainedVREFDQ_A0);
		early_printf("TrainedVREFDQ_A1 = 0x%x\r\n", msgBlk2d_ptr->TrainedVREFDQ_A1);
		early_printf("RxClkDly_Margin_A0 = 0x%x\r\n", msgBlk2d_ptr->RxClkDly_Margin_A0);
		early_printf("VrefDac_Margin_A0 = 0x%x\r\n", msgBlk2d_ptr->VrefDac_Margin_A0);
		early_printf("TxDqDly_Margin_A0 = 0x%x\r\n", msgBlk2d_ptr->TxDqDly_Margin_A0);
		early_printf("DeviceVref_Margin_A0 = 0x%x\r\n", msgBlk2d_ptr->DeviceVref_Margin_A0);
		early_printf("RxClkDly_Margin_A1 = 0x%x\r\n", msgBlk2d_ptr->RxClkDly_Margin_A1);
		early_printf("VrefDac_Margin_A1 = 0x%x\r\n", msgBlk2d_ptr->VrefDac_Margin_A1);
		early_printf("TxDqDly_Margin_A1 = 0x%x\r\n", msgBlk2d_ptr->TxDqDly_Margin_A1);
		early_printf("DeviceVref_Margin_A1 = 0x%x\r\n", msgBlk2d_ptr->DeviceVref_Margin_A1);
		early_printf("CDD_ChB_RR_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_RR_1_0);
		early_printf("CDD_ChB_RR_0_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_RR_0_1);
		early_printf("CDD_ChB_RW_1_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_RW_1_1);
		early_printf("CDD_ChB_RW_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_RW_1_0);
		early_printf("CDD_ChB_WR_1_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_WR_1_1);
		early_printf("CDD_ChB_WR_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_WR_1_0);
		early_printf("CDD_ChB_WR_0_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_WR_0_1);
		early_printf("CDD_ChB_WR_0_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_WR_0_0);
		early_printf("CDD_ChB_WW_1_0 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_WW_1_0);
		early_printf("CDD_ChB_WW_0_1 = 0x%x\r\n", msgBlk2d_ptr->CDD_ChB_WW_0_1);
		early_printf("TrainedVREFDQ_B0 = 0x%x\r\n", msgBlk2d_ptr->TrainedVREFDQ_B0);
		early_printf("TrainedVREFDQ_B1 = 0x%x\r\n", msgBlk2d_ptr->TrainedVREFDQ_B1);
		early_printf("RxClkDly_Margin_B0 = 0x%x\r\n", msgBlk2d_ptr->RxClkDly_Margin_B0);
		early_printf("VrefDac_Margin_B0 = 0x%x\r\n", msgBlk2d_ptr->VrefDac_Margin_B0);
		early_printf("TxDqDly_Margin_B0 = 0x%x\r\n", msgBlk2d_ptr->TxDqDly_Margin_B0);
		early_printf("DeviceVref_Margin_B0 = 0x%x\r\n", msgBlk2d_ptr->DeviceVref_Margin_B0);
		early_printf("RxClkDly_Margin_B1 = 0x%x\r\n", msgBlk2d_ptr->RxClkDly_Margin_B1);
		early_printf("VrefDac_Margin_B1 = 0x%x\r\n", msgBlk2d_ptr->VrefDac_Margin_B1);
		early_printf("TxDqDly_Margin_B1 = 0x%x\r\n", msgBlk2d_ptr->TxDqDly_Margin_B1);
		early_printf("DeviceVref_Margin_B1 = 0x%x\r\n", msgBlk2d_ptr->DeviceVref_Margin_B1);
	} else {
		msgBlk1d_ptr = msgblk_ptr;
		early_printf("----------------------1D trained data--------------------\r\n");
		early_printf("CsTestFail = 0x%x\r\n", msgBlk1d_ptr->CsTestFail);
		early_printf("CDD_ChA_RR_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_RR_1_0);
		early_printf("CDD_ChA_RR_0_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_RR_0_1);
		early_printf("CDD_ChA_RW_1_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_RW_1_1);
		early_printf("CDD_ChA_RW_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_RW_1_0);
		early_printf("CDD_ChA_WR_1_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_WR_1_1);
		early_printf("CDD_ChA_WR_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_WR_1_0);
		early_printf("CDD_ChA_WR_0_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_WR_0_1);
		early_printf("CDD_ChA_WR_0_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_WR_0_0);
		early_printf("CDD_ChA_WW_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_WW_1_0);
		early_printf("CDD_ChA_WW_0_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChA_WW_0_1);
		early_printf("CDD_ChB_RR_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_RR_1_0);
		early_printf("CDD_ChB_RR_0_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_RR_0_1);
		early_printf("CDD_ChB_RW_1_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_RW_1_1);
		early_printf("CDD_ChB_RW_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_RW_1_0);
		early_printf("CDD_ChB_WR_1_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_WR_1_1);
		early_printf("CDD_ChB_WR_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_WR_1_0);
		early_printf("CDD_ChB_WR_0_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_WR_0_1);
		early_printf("CDD_ChB_WR_0_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_WR_0_0);
		early_printf("CDD_ChB_WW_1_0 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_WW_1_0);
		early_printf("CDD_ChB_WW_0_1 = 0x%x\r\n", msgBlk1d_ptr->CDD_ChB_WW_0_1);
	}
	early_printf("-----------------------trained data end---------------------\r\n");
}
static int dwc_ddrphy_init_read_msg_block(int idx, int train_2d)
{
	unsigned long phy_base = ax650x_ddr_cfg.reg_cfg[idx].phy_base;
	u16 *ptr16_src, *ptr16_dst;
	int i;
	/*
	 *1.Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
	 */
	core_write_halfword(phy_base + (0xd0000 << 1), 0);
	/*
	 * 2. Read the Firmware Message Block to obtain the results from the training.
	 * This can be accomplished by issuing APB read commands to the DMEM addresses.
	 */
	if (train_2d) {
		ptr16_src = (u16 *) (phy_base + (DDR_DMEM_BASE * 2));
		ptr16_dst = (u16 *) & msgBlk_2d;
		for (i = 0; i < sizeof(PMU_SMB_LPDDR4X_2D_t); i++) {
			*ptr16_dst++ = core_read_halfword((ulong)ptr16_src++);
		}
		msgBlk_print(&msgBlk_2d, train_2d);
	} else {
		ptr16_src = (u16 *) (phy_base + (DDR_DMEM_BASE * 2));
		ptr16_dst = (u16 *) & msgBlk_1d;
		for (i = 0; i < sizeof(PMU_SMB_LPDDR4X_1D_t); i++) {
			*ptr16_dst++ = core_read_halfword((ulong)ptr16_src++);
		}
		msgBlk_print(&msgBlk_1d, train_2d);
	}
	/*
	 *Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
	 */
	core_write_halfword(phy_base + (0xd0000 << 1), 1);
	return 0;
}
static void decode_major_message(int mail)
{
	switch (mail) {
	case 0x00:
		early_printf("End of initialization\r\n");
		break;
	case 0x01:
		early_printf("End of fine write leveling\r\n");
		break;
	case 0x02:
		early_printf("End of read enable training\r\n");
		break;
	case 0x03:
		early_printf("End of read delay center optimization\r\n");
		break;
	case 0x04:
		early_printf("End of write delay center optimization\r\n");
		break;
	case 0x05:
		early_printf("End of 2D read delay/voltage center optimization\r\n");
		break;
	case 0x06:
		early_printf("End of 2D write delay /voltage center optimization\r\n");
		break;
	case 0x07:
		early_printf("Training has run successfully (firmware complete)\r\n");
		break;
	case 0x08:		//streaming message
		break;
	case 0x09:
		early_printf("End of max read latency training\r\n");
		break;
	case 0x0a:
		early_printf("End of read dq deskew training\r\n");
		break;
	case 0x0b:		//reserved
		break;
	case 0x0c:
		early_printf("End of all DB training (MREP/DWL/MRD/MWD complete)\r\n");
		break;
	case 0x0d:
		early_printf("End of CA training\r\n");
		break;
	case 0xfd:
		early_printf("End of MPR read delay center optimization\r\n");
		break;
	case 0xfe:
		early_printf("End of Write leveling coarse delay\r\n");
		break;
	case 0xff:
		early_printf("Training has failed (firmware complete)\r\n");
		break;
	default:
		break;
	}
}
struct string_index {
	u32 raw_val;
	u32 convert_val;
};
struct string_index convertd_string[] = {
	{0x00F30001,0x00F20001},
	{0x00ee0001,0x00ED0001},
	{0x00ef000b,0x00EE000B},
	{0x00f40001,0x00F30001},
	{0x00f50001,0x00F40001},
	{0x00e10002,0x00E00002},
	{0x00f60000,0x00F50000},
	{0x01860001,0x01850001},
	{0x0187000b,0x0186000B},
	{0x018f0000,0x018E0000},
};
u32 convert_string(int string_index)
{
	int i;
	for(i = 0; i < ARRAY_SIZE(convertd_string); i++) {
		if(string_index == convertd_string[i].raw_val) {
			string_index = convertd_string[i].convert_val;
		}
	}
	return string_index;
}
static void decode_streaming_message(ulong base, int train_2d)
{
	int string_index, i = 0;
	string_index = get_mail(base, 32);
	string_index = convert_string(string_index);
	early_printf("#0x%08x: ", string_index);
	while (i < (string_index & 0xffff)) {
		early_printf("0x%0x ", get_mail(base, 32));
		i++;
	}
	early_printf("\r\n");
}
#endif
