/*
 * mod_hd.c
 *
 *  Created on: 2012-8-24
 *      Author: greenleaf
 */



/* mod_hd data */
id_t	mod_hd_id;




/*
 * mod_hd_open
 * ��Ӳ��
 */
void mod_hd_open()
{

}






/*
 * mod_hd_do
 * ��Ϣ����
 */
void mod_hd_do(MsgHead msg)
{
	return;
}





/*
 * mod_hd_main
 * ��ѭ��
 */
void mod_hd_main()
{
	msg_reg.sender = mod_hd_id;
	msg_reg.param = MSG_INTR_AT_WINCHESTER;
	msg_reg.receiver = mod_public_msg_id;
	post(msg_reg);

	enable_irq(MSG_INTR_AT_WINCHESTER);

	while(1)
	{
		MsgHead msg = recv();
		mod_hd_do(msg);
	}
}
