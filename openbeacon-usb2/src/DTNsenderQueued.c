/***************************************************************
 *
 * Modificato da DAmendola - 6/3/13 - DTN SENDER (NON FINITO!)
 * OpenBeacon.org - main file for OpenBeacon USB II Bluetooth
 *
 * Copyright 2010 Milosch Meriac <meriac@openbeacon.de>
 *
 ***************************************************************

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */
#include <openbeacon.h>
#include "3d_acceleration.h"
#include "storage.h"
#include "bluetooth.h"
#include "pmu.h"
#include "iap.h"
#include "spi.h"
#include "nRF_API.h"
#include "nRF_CMD.h"
#include "xxtea.h"
#include "openbeacon-proto.h"
#include "dtn_queue.h"


/* device UUID */
static uint16_t tag_id;
static const uint16_t sink = 0x1fbf;

static TDeviceUID device_uuid;
/* random seed */
static uint32_t random_seed;
/* logfile position */
static uint32_t g_storage_items;


#define TX_STRENGTH_OFFSET 2

#define MAINCLKSEL_IRC 0
#define MAINCLKSEL_SYSPLL_IN 1
#define MAINCLKSEL_WDT 2
#define MAINCLKSEL_SYSPLL_OUT 3

#ifdef  CUSTOM_ENCRYPTION_KEY
#include "custom-encryption-key.h"
#else /*CUSTOM_ENCRYPTION_KEY */
/* Default TEA encryption key of the tag - MUST CHANGE ! */
static const uint32_t xxtea_key[XXTEA_BLOCK_COUNT] = {
		0x00112233,
		0x44556677,
		0x8899AABB,
		0xCCDDEEFF
};
#endif /*CUSTOM_ENCRYPTION_KEY */

/* set nRF24L01 broadcast mac */
static const unsigned char broadcast_mac[NRF_MAX_MAC_SIZE] = {0xE7,0xD3,0xF0,0x35,0x77};
static unsigned char my_mac[NRF_MAX_MAC_SIZE] = {0xAA,0xD3,0xF0,0x35,0xAA};

/* OpenBeacon packet */
static DTNMsgEnvelope dtnMsg;
static uint16_t MsgSeq = 0;
static uint32_t rs = 0;
static TLogfileDTNMsg g_Log;


static QueueRecord sa;
static QueueRecord *Q = &sa;


static uint32_t
rnd (uint32_t range)
{
	static uint32_t v1 = 0x52f7d319;
	static uint32_t v2 = 0x6e28014a;

	/* reseed random with timer */
	random_seed += LPC_TMR32B0->TC ^ rs++;

	/* MWC generator, period length 1014595583 */
	return ((((v1 = 36969 * (v1 & 0xffff) + (v1 >> 16)) << 16) ^
			(v2 = 30963 * (v2 & 0xffff) + (v2 >> 16))) ^ random_seed) % range;
}



static void
nRF_tx (uint8_t power)
{

	// encrypt data //
	xxtea_encode (dtnMsg.block, XXTEA_BLOCK_COUNT, xxtea_key);

	// set TX power //
	nRFAPI_SetTxPower (power & 0x3);

	// upload data to nRF24L01 //
	nRFAPI_TX (dtnMsg.byte, sizeof (dtnMsg));

	// transmit data //
	nRFCMD_CE (1);

	// wait for packet to be transmitted //
	pmu_sleep_ms (2);

	///transmit data //
	nRFCMD_CE (0);
}

void
nrf_off (void)
{
	/* disable RX mode */
	nRFCMD_CE (0);

	/* wait till RX is done */
	pmu_sleep_ms (5);

	/* switch to TX mode */
	nRFAPI_SetRxMode (0);
}



static inline void
pin_init (void)
{
	LPC_IOCON->PIO2_0 = 0;
	GPIOSetDir (2, 0, 1);		//OUT
	GPIOSetValue (2, 0, 0);

	LPC_IOCON->RESET_PIO0_0 = 0;
	GPIOSetDir (0, 0, 0);		//IN

	LPC_IOCON->PIO0_1 = 0;
	GPIOSetDir (0, 1, 0);		//IN

	LPC_IOCON->PIO1_8 = 0;
	GPIOSetDir (1, 8, 1);		//OUT
	GPIOSetValue (1, 8, 1);

	LPC_IOCON->PIO0_2 = 0;
	GPIOSetDir (0, 2, 1);		//OUT
	GPIOSetValue (0, 2, 0);

	LPC_IOCON->PIO0_3 = 0;
	GPIOSetDir (0, 3, 0);		//IN

	LPC_IOCON->PIO0_4 = 1 << 8;
	GPIOSetDir (0, 4, 1);		//OUT
	GPIOSetValue (0, 4, 1);

	/* switch PMU to high power mode */
	LPC_IOCON->PIO0_5 = 1 << 8;
	GPIOSetDir (0, 5, 1);		//OUT
	GPIOSetValue (0, 5, 0);

	LPC_IOCON->PIO1_9 = 0;	//FIXME
	GPIOSetDir (1, 9, 1);		//OUT
	GPIOSetValue (1, 9, 0);

	LPC_IOCON->PIO0_6 = 0;
	GPIOSetDir (0, 6, 1);		//OUT
	GPIOSetValue (0, 6, 1);

	LPC_IOCON->PIO0_7 = 0;
	GPIOSetDir (0, 7, 1);		//OUT
	GPIOSetValue (0, 7, 0);

	LPC_IOCON->PIO1_7 = 0;
	GPIOSetDir (1, 7, 1);		//OUT
	GPIOSetValue (1, 7, 0);

	LPC_IOCON->PIO1_6 = 0;
	GPIOSetDir (1, 6, 1);		//OUT
	GPIOSetValue (1, 6, 0);

	LPC_IOCON->PIO1_5 = 0;
	GPIOSetDir (1, 5, 1);		//OUT
	GPIOSetValue (1, 5, 0);

	LPC_IOCON->PIO3_2 = 0;	// FIXME
	GPIOSetDir (3, 2, 1);		//OUT
	GPIOSetValue (3, 2, 1);

	LPC_IOCON->PIO1_11 = 0x80;	//FIXME
	GPIOSetDir (1, 11, 1);	// OUT
	GPIOSetValue (1, 11, 0);

	LPC_IOCON->PIO1_4 = 0x80;
	GPIOSetDir (1, 4, 0);		// IN

	LPC_IOCON->ARM_SWDIO_PIO1_3 = 0x81;
	GPIOSetDir (1, 3, 1);		// OUT
	GPIOSetValue (1, 3, 0);

	LPC_IOCON->JTAG_nTRST_PIO1_2 = 0x81;
	GPIOSetDir (1, 2, 1);		// OUT
	GPIOSetValue (1, 2, 0);

	LPC_IOCON->JTAG_TDO_PIO1_1 = 0x81;
	GPIOSetDir (1, 1, 1);		// OUT
	GPIOSetValue (1, 1, 0);

	LPC_IOCON->JTAG_TMS_PIO1_0 = 0x81;
	GPIOSetDir (1, 0, 1);		// OUT
	GPIOSetValue (1, 0, 0);

	LPC_IOCON->JTAG_TDI_PIO0_11 = 0x81;
	GPIOSetDir (0, 11, 1);	// OUT
	GPIOSetValue (0, 11, 0);

	LPC_IOCON->PIO1_10 = 0x80;
	GPIOSetDir (1, 10, 1);	// OUT
	GPIOSetValue (1, 10, 1);

	LPC_IOCON->JTAG_TCK_PIO0_10 = 0x81;
	GPIOSetDir (0, 10, 1);	// OUT
	GPIOSetValue (0, 10, 0);

	LPC_IOCON->PIO0_9 = 0;
	GPIOSetDir (0, 9, 1);		// OUT
	GPIOSetValue (0, 9, 0);

	/* select MISO function for PIO0_8 */
	LPC_IOCON->PIO0_8 = 1;
}

static inline void
show_version (void)
{
	debug_printf (" * Device UID: %08X:%08X:%08X:%08X\n",
			device_uuid[0], device_uuid[1],
			device_uuid[2], device_uuid[3]);
	debug_printf (" * OpenBeacon MAC: %02X:%02X:%02X:%02X:%02X\n",
			broadcast_mac[0], broadcast_mac[1], broadcast_mac[2],
			broadcast_mac[3], broadcast_mac[4]);
	debug_printf (" *         Tag ID: %04X\n", tag_id);
	debug_printf (" * Stored Logfile Items: %i\n", g_storage_items);
}

static inline void
main_menue (uint8_t cmd)
{
	TLogfileBeaconPacket pkt;

	/* ignore non-printable characters */
	if (cmd <= ' ')
		return;
	/* show key pressed */
	debug_printf ("%c\n", cmd);
	/* map lower case to upper case */
	if (cmd > 'a' && cmd < 'z')
		cmd -= ('a' - 'A');

	switch (cmd)
	{
	case '?':
	case 'H':
		debug_printf ("\n"
				" *****************************************************\n"
				" * OpenBeacon Tag - Bluetooth Console\n"
				" *                  Version v" PROGRAM_VERSION "\n"
				" * (C) 2011 Milosch Meriac <meriac@openbeacon.de>\n"
				" *****************************************************\n"
				" * H,?          - this help screen\n"
				" * S            - Show device status\n"
				" *\n"
				" * E            - Erase Storage\n"
				" * W            - Test Write Storage\n"
				" * R            - Test Read Storage\n"
				" * F            - Test WriteFill Storage\n"
				" * M            - write 3 times and read them\n"
				" *****************************************************\n"
				"\n");
		break;
	case 'S':
		debug_printf ("\n"
				" *****************************************************\n"
				" * OpenBeacon Status Information                     *\n"
				" *****************************************************\n");
		show_version ();
		spi_status ();
		acc_status ();
		storage_status ();
		debug_printf (" *****************************************************\n"
				"\n");
		break;




	case 'M' :
	{
		uint32_t counter;
		debug_printf ("\nErasing Storage...\n\n");
		storage_erase ();
		debug_printf ("\nWriting Khalil 3 times...\n");
		counter = 0;
		const char data[] = "Khalil";
		const uint8_t buffer[32];
		while(counter < 3)
		{
			storage_write (counter*sizeof(buffer), sizeof (buffer), &data);
			counter ++;
		}
		debug_printf ("\n[DONE]\n");

		debug_printf("\n reading the data...\n");

		counter = 0;

		while(counter < 3)
		{
			storage_read (counter*sizeof(buffer), counter*sizeof(buffer) + sizeof (buffer), &buffer);
			hex_dump (buffer,0,sizeof (buffer));
			counter++;
		}

		break;
	}



	case 'E':
		debug_printf ("\nErasing Storage...\n\n");
		storage_erase ();
		g_storage_items = 0;
		break;

	case 'W':
	{
		const char hello[] = "Hello World!";
		debug_printf ("\n * writing '%s' (%i bytes)\n", hello,
				sizeof (hello));
		storage_write (0, sizeof (hello), &hello);
	}
	break;

	case 'R':
	{
		const uint8_t buffer[32];
		debug_printf ("\n * reading %i bytes\n", sizeof (buffer));
		storage_read (0, sizeof (buffer), &buffer);
		hex_dump (buffer, 0, sizeof (buffer));
	}
	break;

	case 'F':
	{
		uint32_t counter;
		debug_printf ("\nErasing Storage...\n\n");
		storage_erase ();
		debug_printf ("\nFilling Storage...\n");
		counter = 0;
		while(counter < LOGFILE_STORAGE_SIZE)
		{
			pkt.time = htonl(counter);
			pkt.oid = htons(counter / sizeof(pkt));
			pkt.strength = (counter / sizeof(pkt)) % MAX_POWER_LEVELS;
			pkt.crc = crc8 (((uint8_t *) & pkt), sizeof (pkt) - sizeof (pkt.crc));
			storage_write (counter, sizeof (pkt), &pkt);

			counter += sizeof(pkt);
		}
		debug_printf ("\n[DONE]\n");
		break;
	}

	default:
		debug_printf ("Unknown command '%c' - please press 'H' for help \n",
				cmd);
	}
	debug_printf ("\n# ");
}

/*
 * Blink the led for a number of times
 */
void
blink (uint8_t times)
{
	while (times)
	{
		times--;

		GPIOSetValue (1, 1, 1);
		pmu_sleep_ms (100);
		GPIOSetValue (1, 1, 0);
		pmu_sleep_ms (200);
	}
	pmu_sleep_ms (500);
}

/*
 * Check for set the device in Sleep forever
 */
inline void checkSleepForever(void){

	if(GPIOGetValue (1, 4)) //left button
		return;

	uint8_t t = 0;
	GPIOSetValue (1, 2, 1); //right LED
	while(!GPIOGetValue (1, 4) && ++t < 20) //tenedo premuto il pulsante sx per almeno 2 sec passa e va sotto;
		pmu_sleep_ms (100);

	if (t >= 20) //dopo i 2 sec;
	{
		GPIOSetValue (1, 2, 0); //spegne il LED right
		GPIOSetValue (1, 1, 0); //spegne il LED left  (ATT. 3 dovrebbe essere il reset btn)
		nRFAPI_PowerDown (); // spegne la radio
		while(1)
			pmu_sleep_ms(1000); // il processor dorme? e perché non più di 1000?
	}

	GPIOSetValue (1, 2, 0);
}

/*
 * Main dell'applicazione.
 */
int
main (void)
{

	uint32_t SSPdiv;
	uint8_t cmd_buffer[64], cmd_pos, c;
	uint16_t crc;
	uint8_t  status,i;
	uint8_t volatile *uart;
	volatile int t;


	/* wait on boot - debounce */
	for (t = 0; t < 2000000; t++);

	/* Initialize GPIO (sets up clock) */
	GPIOInit ();

	/* initialize pins */
	pin_init ();

	/* fire up LED 1 */
	GPIOSetValue (1, 1, 1);

	/* initialize SPI */
	spi_init ();

	/* read device UUID */
	bzero (&device_uuid, sizeof (device_uuid));
	iap_read_uid (&device_uuid);
	tag_id = crc16 ((uint8_t *) & device_uuid, sizeof (device_uuid));

	random_seed =
			device_uuid[0] ^ device_uuid[1] ^ device_uuid[2] ^ device_uuid[3]; // ^ è uno xor per generare un random id
	my_mac[0]=device_uuid[0];
	my_mac[1]=device_uuid[1];
	my_mac[2]=device_uuid[2];
	my_mac[3]=device_uuid[3];

	/************ IF Plugged to computer upon reset ? ******************/
	if (GPIOGetValue (0, 3)) // indica che l'usb è inserita
	{
		/* wait some time till Bluetooth is off */
		for (t = 0; t < 2000000; t++);

		/* Init 3D acceleration sensor */
		acc_init (1);
		/* Init Flash Storage with USB */
		storage_init (TRUE, tag_id);
		g_storage_items = storage_items ();

		/* Init Bluetooth */
		bt_init (TRUE, tag_id);

		/* switch to LED 2 */
		GPIOSetValue (1, 1, 0);
		GPIOSetValue (1, 2, 1);

		/* set command buffer to empty */
		cmd_pos = 0;

		/* spin in loop */
		while (1)
		{
			/* reset after USB unplug */
			if (!GPIOGetValue (0, 3))
				NVIC_SystemReset ();

			/* if UART rx send to menue */
			if (UARTCount)
			{
				/* blink LED1 upon Bluetooth command */
				GPIOSetValue (1, 1, 1);
				/* execute menue command with last character received */

				/* scan through whole UART buffer */
				uart = UARTBuffer;
				for (i = UARTCount; i > 0; i--)
				{
					UARTCount--;
					c = *uart++;
					if ((c < ' ') && cmd_pos)
					{
						/* if one-character command - execute */
						if (cmd_pos == 1)
							main_menue (cmd_buffer[0]);
						else
						{
							cmd_buffer[cmd_pos] = 0;
							debug_printf
							("Unknown command '%s' - please press H+[Enter] for help\n# ",
									cmd_buffer);
						}

						/* set command buffer to empty */
						cmd_pos = 0;
					}
					else if (cmd_pos < (sizeof (cmd_buffer) - 2))
						cmd_buffer[cmd_pos++] = c;
				}

				/* reset UART buffer */
				UARTCount = 0;
				/* un-blink LED1 */
				GPIOSetValue (1, 1, 0);
			}
		}
	} /* End of if plugged to computer*/


	/***************** IF UNPLUGGED TO PC ........********/



	/* Init Bluetooth */
	bt_init (FALSE, tag_id);

	/* shut down up LED 1 */
	GPIOSetValue (1, 1, 0);

	/* Init Flash Storage without USB */
	storage_init (FALSE, tag_id);

	/*Erase old content*/
	storage_erase ();

	/* get current FLASH storage write postition */
	g_storage_items = storage_items ();

	/* initialize power management */
	pmu_init ();

	blink (2);

	/* Initialize OpenBeacon nRF24L01 interface */
	if (!nRFAPI_Init(CONFIG_BROADCAST_CHANNEL, broadcast_mac, sizeof (broadcast_mac), 0))
		for (;;) // entriamo in una sorta di errore di sistema
		{
			GPIOSetValue (1, 2, 1);
			pmu_sleep_ms (500);
			GPIOSetValue (1, 2, 0);
			pmu_sleep_ms (500);
		}

	nRFAPI_SetTxPower (1);
	nRFCMD_Power (1);


	//nRFAPI_TxRetries (0);
	/* enable ACK */
	//nRFAPI_SetPipeSizeRX (0, NRF_MAX_MAC_SIZE);
	//nRFAPI_PipesEnable (ERX_P0);
	//nRFAPI_PipesAck (ERX_P0);

	/* blink three times to show flash initialized RF interface */
	blink (3);

	/***************////****************MAIN TASK************/////**********************/
	/***************////****************MAIN TASK************/////**********************/
	/***************////****************MAIN TASK************/////**********************/

	/* disable unused jobs */
	SSPdiv = LPC_SYSCON->SSPCLKDIV;
	uint32_t time;
	DTNMsg msg;
	DTNMsg* msgp;

	time = LPC_TMR32B0->TC;

	// Empty DTNMsgs Queue
	MakeEmpty(Q);
	uint8_t onemsg = 0;

	while (1)
	{
		checkSleepForever();

		// DTNMsg generation. time in second
		if(LPC_TMR32B0->TC - time >= 30)
			//if(onemsg<1)
		{
			msg.from = htons (tag_id);
			msg.proto = RFBPROTO_DTN_MSG;
			msg.prop = 1;
			msg.time= htonl (LPC_TMR32B0->TC);
			msg.seq = htonl(((0x00000000 | tag_id)<<16) | ++MsgSeq); //Msg Id is tagId:MsgSeq
			Enqueue(msg, Q);
			time = LPC_TMR32B0->TC;
			onemsg++;

			//Log of the created msg.
			g_Log.time1 = ntohl(msg.time);
			g_Log.time2 = ntohl(msg.time);
			g_Log.seq = ntohl(msg.seq);
			g_Log.from = ntohs (msg.from);
			g_Log.prop = msg.prop;
			g_Log.crc = crc8 (((uint8_t *) & g_Log),sizeof (g_Log) - sizeof (g_Log.crc));
			// store data if space left on FLASH
			if (g_storage_items < (LOGFILE_STORAGE_SIZE/sizeof (g_Log)))
			{	storage_write (g_storage_items * sizeof (g_Log), sizeof (g_Log), &g_Log);
			// increment and store RAM persistent storage position
			g_storage_items ++;
			}
		}//end creazione DTN msg

		/**********************************************************************/
		// ********* Sending phase ***********
		pmu_sleep_ms (2);
		nRFAPI_SetRxMode (1);
		nRFCMD_CE (1);
		pmu_sleep_ms (200+rnd(2000)); // for randomize the sending of msg
		nRFCMD_CE (0);

		if (!IsEmpty(Q)) {   // if the queue is not empty sending NDReq
			//nRFCMD_CE (1); // Attiva la radio?
			//pmu_sleep_ms (2); //Carrier detect
			//nRFCMD_CE (0);

			if ((nRFAPI_CarrierDetect () != 0x01)) // se il canale non è libero?
			{
				//GPIOSetValue (1, 1, 1); // accende led 1

				bzero (&dtnMsg, sizeof (dtnMsg));
				msgp = Front(Q);
				//crea un messaggio di NDReq
				dtnMsg.proto = RFBPROTO_DTN_MSG; //crea un messaggio di tipo DTN_MSG
				dtnMsg.msg.from = htons (tag_id);
				dtnMsg.msg.time= htonl (LPC_TMR32B0->TC);
				dtnMsg.msg.seq = msgp->seq;  // msgp dove lo setta?
				dtnMsg.msg.crc = htons (crc16(dtnMsg.byte, sizeof (dtnMsg) - sizeof (dtnMsg.NDreq.crc)));
				nRFAPI_SetRxMode(0);
				//	nRFCMD_CmdExec (W_TX_PAYLOAD_NOACK);
				nRF_tx (1);  // Sending NDReq

				//		DTNmsg		uint8_t proto;
				//					uint32_t time;
				//					uint32_t seq;
				//					uint16_t from;
				//					uint16_t data;
				//					uint8_t prop;
				//					uint16_t crc;

				//collecting neighbors
				uint8_t N = 0,added;
				uint16_t w=0;
				unsigned char Nei[10][5]; // array degli indirizzi di max 10 vicini
//
				do{
					nRFAPI_SetRxMode (1);
					nRFCMD_CE (1);
					pmu_sleep_ms (10); //incomming NDRes time window, must be long to accept first at least
					nRFCMD_CE (0);

					/**** if there is incomming packet recieve it *******/
					if (nRFCMD_IRQ ())
					{
						do
						{
							added = 0;
							nRFCMD_RegReadBuf (RD_RX_PLOAD, dtnMsg.byte,sizeof (dtnMsg));
							xxtea_decode (dtnMsg.block, XXTEA_BLOCK_COUNT, xxtea_key);
							crc = crc16 (dtnMsg.byte,sizeof (dtnMsg) - sizeof (dtnMsg.msg.crc));
							if (ntohs (dtnMsg.msg.crc) == crc && dtnMsg.proto == RFBPROTO_ND_RES)
							{
								//update neighbor
								for(t=0;t<N;t++) // controllo se ho già questo vicino nel Nei
									if(Nei[t][0] == dtnMsg.NDres.from[0] && Nei[t][1] == dtnMsg.NDres.from[1] && Nei[t][2] == dtnMsg.NDres.from[2] && Nei[t][3] == dtnMsg.NDres.from[3] && Nei[t][4] == dtnMsg.NDres.from[4])
										added = 1;
								if(!added) // se non c'è l'indirizzo tra i conosciuti, lo aggiungo al posto N
								{
									for(t=0;t<5;t++)
										Nei[N][t] = dtnMsg.NDres.from[t];
									N++;
								}
							}
							status = nRFAPI_GetFifoStatus ();
						}while ((status & FIFO_RX_EMPTY) == 0);
						nRFAPI_ClearIRQ (MASK_IRQ_FLAGS);
					}

				}
				while(++w<=30); // Fine contention windows. Do-While di 30 slot di 10mills  ciascuno.
				GPIOSetValue (1, 1, 0);

				if(N)
				{
					GPIOSetValue (1, 2, 1);

					pmu_sleep_ms (100);
					//sending DTNMsg
					bzero (&dtnMsg, sizeof (dtnMsg));
					dtnMsg.msg = *msgp;
					//for test
					//dtnMsg.msg.prop = 1;//N;
					dtnMsg.msg.crc = htons (crc16(dtnMsg.byte, sizeof (dtnMsg) - sizeof (dtnMsg.msg.crc)));


					uint8_t isSink = 0;
					for(t=0;t<N;t++)
						if(crc16 ((uint8_t *) & Nei[t], sizeof (Nei[t])) == 0xEA18 || crc16 ((uint8_t *) & Nei[t], sizeof (Nei[t])) == 0xCDF6)
						{
							isSink = 1;
							break;
						}

					if(isSink)
					{
						nRFAPI_SetTxMAC (Nei[t],sizeof(my_mac));
						nRFAPI_SetChannel(CONFIG_UNICAST_CHANNEL);
						nRFAPI_SetRxMode(0);
						nRF_tx (1);
						Dequeue(Q);
						nRFAPI_SetTxMAC (broadcast_mac,sizeof(my_mac));
						nRFAPI_SetChannel(CONFIG_BROADCAST_CHANNEL);
					}
					else
					{
						nRFAPI_SetRxMode(0);
						nRF_tx (1);
						RotQueue(Q);

					}

					//modify Msg properity
					//msgp->prop = msgp->prop -1;
					//if(msgp->prop == 0)
					//Dequeue(Q);
					GPIOSetValue (1, 2, 0);
				}
			}
		}

		nRFAPI_ClearIRQ (MASK_IRQ_FLAGS);
		nRFAPI_FlushTX ();
		nRFAPI_FlushRX ();
		nRFAPI_PowerDown ();
	}

	return 0;
}
