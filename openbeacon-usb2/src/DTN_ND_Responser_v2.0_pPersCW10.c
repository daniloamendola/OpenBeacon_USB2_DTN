/***************************************************************
 *
 * DTNnode SlottedSIFT - Modificato da DAmendola - 10/04/2013 - ND_DTN_Responser con SIFT
 * 		Nodo DTN con ND Req e Res
 *
 * MODIFIED per ND test: Responser, resta in attesa di un NDReq e risponde appena ne arriva una
 * utilizzando il pPersisten di K. Masshri.
 * Con verie mofifiche in questa clase adesso implementeremo un metodo di ND che usa
 * la SIFT distribution.
 *
 * + Ottimizzata usando function
 * v0.2 eliminata NDRes ritardata di un #num di slot random. settata prob min a 0.
 *
 *
 *
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
#include "math.h"


/******************** PROTOTYPE OF ALLA FUNCTION USED ***************/
void plugged (void);
void logDataStorage(uint32_t time1, uint32_t time2, uint32_t seq, uint16_t from,
		uint8_t prop);
		//, uint16_t crc, uint16_t data, uint8_t proto, uint8_t temp, uint16_t info

void sendBackNDRes (uint16_t s, uint32_t seq, uint8_t slot);
void NDResAlgorithm (uint32_t seq);
void ReceiveDTN_MSG(void);


/****************************** END PROTOTYPE ***********************/


/* device UUID */
static uint16_t tag_id;
static const uint16_t sink = 0x1fbf;
static uint8_t TX_POWER = 1;

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
//static uint16_t MsgSeq = 0;
static uint32_t rs = 0;//200;
static TLogfileDTNMsg g_Log;


static QueueRecord sa;
static QueueRecord *Q = &sa;

/*
 * Prende in input un range e restitutisce un numero random.
 */
static uint32_t
rnd (uint32_t range)
{
	static uint32_t v1 = 0x52f7d319;
	static uint32_t v2 = 0x6e28014a;

	/* reseed random with timer */
	random_seed += LPC_TMR32B0->TC ^ rs++; //^ tag_id ^ rs++;

	/* MWC generator, period length 1014595583 */
	return ((((v1 = 36969 * (v1 & 0xffff) + (v1 >> 16)) << 16) ^
			(v2 = 30963 * (v2 & 0xffff) + (v2 >> 16))) ^ random_seed) % range;
}


/*
 * La funzione provvede ad inviare il messaggio nell'stnMsgEnvelope.
 * power assume valori: 0, 1, 2, 3 che regolano la potenza del segnale.
 */
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

/*
 * Return the mac address. To set mac address see var broadcast_mac.
 */
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
 * Blink the led.
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


inline void checkSleepForever(void){

	if(GPIOGetValue (1, 4)) //left button
		return;

	uint8_t t = 0;
	GPIOSetValue (1, 2, 1); //right LED
	while(!GPIOGetValue (1, 4) && ++t < 20)
		pmu_sleep_ms (100);

	if (t >= 20)
	{
		GPIOSetValue (1, 2, 0);
		GPIOSetValue (1, 1, 0);
		nRFAPI_PowerDown ();
		while(1)
			pmu_sleep_ms(1000);
	}

	GPIOSetValue (1, 2, 0);
}


/* **************** DAME Add 7/3/2013 - DTNnode ******************** */

/*
 * IF Plugged to computer upon reset ?
 */
void plugged (void){
	uint8_t cmd_buffer[64], cmd_pos, c;
	uint8_t i;
	uint8_t volatile *uart;
	volatile int t;

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
}//plugged


/*
 * Sift distribution for the slot r
 *
 */
static float
SiftDistribution(uint8_t r, float alfa, uint8_t CW)
{
	return (((1.0-alfa)*pow(alfa, CW))/(1.0-pow(alfa, CW)))*pow(alfa,-r);
	//logDataStorage(33, sif*100, 0x3333, 6666, sif*100);
}

/*
 * Modified by DAme UNICAL on March 2013.
 *
 */
void logDataStorage(uint32_t time1, uint32_t time2, uint32_t seq, uint16_t from,
		uint8_t prop){
	//unsigned char c = {'m','l','p','o','i','u','y','t','r','a'};
	g_Log.time1 = time1;
	g_Log.time2 = time2;// ci andrebbe msg.time
	g_Log.seq = seq;
	g_Log.from = from; //msg.from
	g_Log.prop = prop;
	//g_Log.data = &c;

	g_Log.crc = crc8 (((uint8_t *) & g_Log),sizeof (g_Log) - sizeof (g_Log.crc));
	//g_Log.crc = crc;

	// store data if space left on FLASH
	if (g_storage_items < (LOGFILE_STORAGE_SIZE/sizeof (g_Log)))
	{	storage_write (g_storage_items * sizeof (g_Log), sizeof (g_Log), &g_Log);
	// increment and store RAM persistent storage position
	g_storage_items ++;
	}
}


/*
 * Algoritmo 2 del paper Khalil M.
 *
 */
void NDResAlgorithm (uint32_t seq) {

	uint16_t s=0; //,r;
	uint8_t slot=1;
	uint8_t CW= 10;
	float alfa=0.7;
	float sift = 0;

	uint32_t probT; // probability treshold

	//Extract a time slot using Sift distribution
	//float randU = ((float)rnd(100))/100;
	//sift = -log10((-log(alfa))*randU/constA)/log10(alfa) - 0.5;
	//if(sift<=0) sift=1;
	//if(sift>CW) sift=CW;

	//logDataStorage(sift, probT, 0x3333, 6666, s);

	// ************************* SELECT A RANDOM INIT SLOT
	//      NO
	//**************************
	nRFAPI_SetRxMode(1);
	nRFCMD_CE (1);
	pmu_sleep_ms (2); //Carrier detect
	nRFCMD_CE (0);

	if((nRFAPI_CarrierDetect()))
	{	//se occupato
		pmu_sleep_ms (20);// mi sa che ci vuole 10
		s=s+20;
		slot++;
		logDataStorage(s, slot, 0xCCC1, 6666, 000);
	}

	while(s<200) // inizio finestra di Res
	{
			// calculate the treshold val for this slot
			sift = SiftDistribution(slot, alfa, CW);
			probT = (sift*100)*2.5 + 5;//  SIFT*SPREAD + BASE
			uint32_t rnd_m = rnd(100);
				//logDataStorage(2222, sift, rnd_m, 6666, 0);
			if (rnd_m<=0.2) 	// mod DanAme 20 testbed con 10 e 40
			{
				sendBackNDRes(s, seq, slot);
				logDataStorage(s, slot, 0x2222, 6666, probT);
				break;
			}
			else {
				s=s+20;
				slot++;
				pmu_sleep_ms (20);
			}

	}//while s< 00
}

/*
 * SubFase di invio risposta con il NDRes
 *
 */
void sendBackNDRes (uint16_t s, uint32_t seq, uint8_t slot)
{
	//uint8_t  status;
	volatile int t;
	//uint16_t crc;

	bzero (&dtnMsg, sizeof (dtnMsg));

	for(t=0;t<5;t++)
		dtnMsg.NDres.from[t] = my_mac[t];
	dtnMsg.proto = RFBPROTO_ND_RES;
	dtnMsg.NDres.time = htonl (LPC_TMR32B0->TC);
	dtnMsg.NDres.seq = htonl(((0x00000000 | slot)<<16)| tag_id); //htonl(((0x00000000 | tag_id)<<16)| seq) // ho messo slot al posto s
	dtnMsg.NDres.crc = htons (crc16(dtnMsg.byte, sizeof (dtnMsg) - sizeof (dtnMsg.NDres.crc)));
	nRFAPI_SetRxMode(0);
	//	nRFCMD_CmdExec (W_TX_PAYLOAD_NOACK);
	nRF_tx (TX_POWER);

	/* sleep for the rest of contention window */
	//pmu_sleep_ms (300-s);

	//DanAme 2 row added LOG risposta ad un REQ (9999=S 19999=R) // qui questo: ntohl(dtnMsg.NDres.seq) non va bene
	logDataStorage(s, LPC_TMR32B0->TC,  (((0x00000000 | seq) << 16) | tag_id), 9999, 0); // invece di s seq
	//GPIOSetValue (1, 1, 0); // spegne led1

	// switch to my_mac for unicast receiving......
}//send back NDReq

/*
 * Receiving phase after sent NDRes.
 * DELETED
 */


/*
 * Il main della classe.
 */
int
main (void)
{
	uint32_t SSPdiv;
	//uint8_t  c, cmd_buffer[64], cmd_pos;
	uint16_t crc;
//	uint8_t  status;
	//uint8_t volatile *uart;
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
			device_uuid[0] ^ device_uuid[1] ^ device_uuid[2] ^ device_uuid[3];
	my_mac[0]=device_uuid[0];
	my_mac[1]=device_uuid[1];
	my_mac[2]=device_uuid[2];
	my_mac[3]=device_uuid[3];

	/************ IF Plugged to computer upon reset ? ******************/
	if (GPIOGetValue (0, 3))
	{
		plugged();
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

	//blink (2);

	/* Initialize OpenBeacon nRF24L01 interface */
	if (!nRFAPI_Init(CONFIG_BROADCAST_CHANNEL, broadcast_mac, sizeof (broadcast_mac), 0))
		for (;;)
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
//	DTNMsg msg;
//	DTNMsg* msgp;

	time = LPC_TMR32B0->TC;

	// Empty DTNMsgs Queue
	MakeEmpty(Q);
//	uint8_t onemsg = 0;

	while (1)
	{
		checkSleepForever();

		/******** BEGIN Contention phase ***********/

		pmu_sleep_ms (2);
		nRFAPI_SetRxMode (1);
		nRFCMD_CE (1);
		pmu_sleep_ms (200+rnd(2000)); // Metto in attesa per la ricezione di messaggi
		nRFCMD_CE (0);

		/**** if there is incomming packet recieve it *******/
		if (nRFCMD_IRQ ())
		{ // check whether it is a NDReq or not
			nRFCMD_RegReadBuf (RD_RX_PLOAD, dtnMsg.byte,sizeof (dtnMsg));
			xxtea_decode (dtnMsg.block, XXTEA_BLOCK_COUNT, xxtea_key);
			crc = crc16 (dtnMsg.byte,sizeof (dtnMsg) - sizeof (dtnMsg.msg.crc));

			// Se è un NDreq
			if (ntohs(dtnMsg.msg.crc) == crc && dtnMsg.proto == RFBPROTO_ND_REQ)
			{
				//if(!Contains(Q,dtnMsg.NDreq.seq) ) // se la NDreq non è già arivata
				//{
					//seq = dtnMsg.NDreq.seq;
					//DanAme 2 row added LOG risposta ad un REQ (9999=S 19999=R)
					logDataStorage(ntohl(dtnMsg.NDreq.time), LPC_TMR32B0->TC, ntohl(dtnMsg.NDreq.seq), 19999, 0);

					// send NDRes during NDRes time Window
					NDResAlgorithm(ntohl(dtnMsg.NDreq.seq));

					blink(1);

					/*******************  FASE **************/
					/********** RICEZIONE DNT_MSG DOPO W **********/

					// switch to my_mac for unicast receiving......
					//nRFAPI_SetRxMAC (my_mac,sizeof(my_mac), 0);

//					ReceiveDTN_MSG();
//					GPIOSetValue (1, 1, 0);// spegne led
				//}
				//else /* already has the msg so sleep and don't disrupt other nodes*/
				//	pmu_sleep_ms (1);
			}
			nRFCMD_CE (0);
			nRFAPI_ClearIRQ (MASK_IRQ_FLAGS);
			nRFAPI_FlushRX ();
		}
		/***************************** END RES Begin REQ *****************************************/

//		else if (!IsEmpty(Q)) {   // if the queue is not empty sending NDReq
//			...
//		}

		nRFAPI_ClearIRQ (MASK_IRQ_FLAGS);
		nRFAPI_FlushTX ();
		nRFAPI_FlushRX ();
		nRFAPI_PowerDown ();
	}

	return 0;
}// RES v2.0 pPers CW10
