#include <SPI.h>
#include <LiquidCrystal.h>


//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidCrystal lcd(39, 41, 31, 33, 35, 37); //LCD所用到的腳位
#define VS_XCS    6// C1 Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS   7 // C2 Data Chip Select / BSYNC Pin
#define VS_DREQ   2 //C4  Data Request Pin: Player asks for more data
#define checkout  3 //C5
#define VS_RESET  8 //C3 Reset is active low

//----------------------Define lcd key
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//----------------Pedal----------
#define padelpin 19
boolean sustainEnable = false;

//-------------------Define -----------------
byte d[800] = {}; //data buffer  暫存收到的數據
int dataNum = 0; //收到的資料資料數量,用來紀錄一次傳輸所收到的總byte數
byte onpin[500] = {}; //notonpin 0~127   on的note音（0-127）
byte vel[500] = {}; //存力道大小的
byte offpin[500] = {};
boolean changectrl = false;
int chennalenable[6] = {1, 0, 0, 0, 0, 0};
int ctrl = 0;
int fucNum[2] = {0, 0};
int programNum[6] = {};
int sustainnum[6] = {};
int tuneNum[6] = {4, 4, 4, 4, 4, 4};
int Volume[6] = {127, 127, 127, 127, 127, 127};


//--------------------------------------------Setup Midi-chip-----------------
const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] = { /* Compressed plugin */
	0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, /*    0 */
	0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, /*    8 */
	0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, /*   10 */
	0x0200, 0x000a, 0x0001, 0x0050,
};
//Write to VS10xx register
//SCI: Data transfers are always 16bit. When a new SCI operation comes in
//DREQ goes low. We then have to wait for DREQ to go high again.
//XCS should be low for the full duration of operation.
void VSWriteRegister(unsigned char addressbyte, unsigned char highbyte, unsigned char lowbyte) {
	while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating IC is available
	digitalWrite(VS_XCS, LOW); //Select control

	//SCI consists of instruction byte, address byte, and 16-bit data word.
	SPI.transfer(0x02); //Write instruction
	SPI.transfer(addressbyte);
	SPI.transfer(highbyte);
	SPI.transfer(lowbyte);
	while (!digitalRead(VS_DREQ)) ; //Wait for DREQ to go high indicating command is complete
	digitalWrite(VS_XCS, HIGH); //Deselect Control
}

//
// Plugin to put VS10XX into realtime MIDI mode
// Originally from http://www.vlsi.fi/fileadmin/software/VS10XX/vs1053b-rtmidistart.zip
// Permission to reproduce here granted by VLSI solution.
//


void VSLoadUserCode(void) {
	int i = 0;

	while (i < sizeof(sVS1053b_Realtime_MIDI_Plugin) / sizeof(sVS1053b_Realtime_MIDI_Plugin[0])) {
		unsigned short addr, n, val;
		addr = sVS1053b_Realtime_MIDI_Plugin[i++];
		n = sVS1053b_Realtime_MIDI_Plugin[i++];
		while (n--) {
			val = sVS1053b_Realtime_MIDI_Plugin[i++];
			VSWriteRegister(addr, val >> 8, val & 0xFF);
		}
	}
}


void setupmidi() {
	digitalWrite(VS_XCS, HIGH); //Deselect Control
	digitalWrite(VS_XDCS, HIGH); //Deselect Data
	digitalWrite(VS_RESET, LOW); //Put VS1053 into hardware reset
	//------------------SPI for midi-chip
	pinMode(53, OUTPUT);
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);

	//From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz.
	//Internal clock multiplier is 1.0x after power up.
	//Therefore, max SPI speed is 1.75MHz. We will use 1MHz to be safe.
	SPI.setClockDivider(SPI_CLOCK_DIV32); //Set SPI bus speed to 1MHz (16MHz / 16 = 1MHz)
	SPI.transfer(0xFF); //Throw a dummy byte at the bus

	delayMicroseconds(1);
	digitalWrite(VS_RESET, HIGH); //Bring up VS1053
	VSLoadUserCode();
	talkMIDI(0xB0, 0, 0x00); //Default bank GM1
	talkMIDI(0xB0, 0x07, 127);
	sustain(0, 0);

	//---------------------play some sound
	noteOn(0, 60, 127);
	delay(100);
	noteOff(0, 60, 0);

}





void setup() {
	Serial.begin(115200);
	Serial1.begin(400000);
	pinMode(VS_DREQ, INPUT);
	pinMode(VS_XCS, OUTPUT);
	pinMode(VS_XDCS, OUTPUT);
	pinMode(VS_RESET, OUTPUT);
	pinMode(checkout, OUTPUT);
	setupmidi();
//---------------start LCD
	lcd.begin(16, 2);  //設定LCD行列數目
	lcd.setCursor(0, 0); //Set cursor to line 0  zero  char.
	lcd.print("Welcom  to PIANO");
//Padel
	attachInterrupt(digitalPinToInterrupt(19), padel, CHANGE );
}

void padel() {
	if (!sustainEnable) {
		sustainEnable = true;
		for (int j = 0; j < 6; j++) {
			if (chennalenable[j] == 1) {
				sustain(j, sustainnum[j]);
			}
		}
	}
	else {
		sustainEnable = false;
		for (int j = 0; j < 6; j++) {
			if (chennalenable[j] == 1) {
				sustain(j, 0);
			}
		}
	}



}





void sendMIDI(byte data)
{
	SPI.transfer(0);
	SPI.transfer(data);
	// Serial.write(data);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {

	//
	// Wait for chip to be ready (Unlikely to be an issue with real time MIDI)
	//
	while (!digitalRead(VS_DREQ))
		;
	digitalWrite(VS_XDCS, LOW);

	sendMIDI(cmd);
	//Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
	//(sort of: http://253.ccarh.org/handout/midiprotocol/)
	if ( (cmd & 0xF0) <= 0xB0 || (cmd & 0xF0) >= 0xE0) {
		sendMIDI(data1);
		sendMIDI(data2);
	} else {
		sendMIDI(data1);
	}


	digitalWrite(VS_XDCS, HIGH);

}




//-----------------------MIDI FUNCTOIN-------------------------
//Send a MIDI note-on message.  Like pressing a piano key
//channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {

	talkMIDI( (0x90 | channel), note, attack_velocity);

}
void noteOff(byte channel, byte note, byte release_velocity) {
	talkMIDI( (0x80 | channel), note, release_velocity);
}


void sustain(byte channel, byte value) {
	talkMIDI(0x40 | channel, value, 0);

}

void changeProgram(byte channel, byte instrument) {
	talkMIDI(0xC0 | channel, instrument, 0);
}

void volumeChange(byte channel, byte value) {
	talkMIDI(0xB0 | channel, 0x07, value);
}



void loop() {

	receivedata();
	datatoNote();
	sendnote();
	lcdControl();
	/*
	  delay(1000);
	  talkMIDI(0xB0, 0x07, 127); //0xB0 is channel message, set channel volume to near max (127)

	  talkMIDI(0xB0, 0, 0x00); //Default bank GM1

	  //Change to different instrument
	  for (int instrument = 0 ; instrument < 127 ; instrument++) {

	    talkMIDI(0xC0, instrument, 0); //Set instrument number. 0xC0 is a 1 data byte command

	    //Play notes from F#-0 (30) to F#-5 (90):
	    for (int note = 60 ; note < 70 ; note++) {

	      //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
	      noteOn(0, note, 127);
	      delay(200);

	      //Turn off the note with a given off/release velocity
	      noteOff(0, note, 60);
	      delay(50);
	    }

	    delay(100); //Delay between instruments
	  }
	*/
}








void receivedata() {

	int i = 0;
	byte a = 0;
	dataNum = 0;
	digitalWrite(checkout, HIGH);//將check pin high，等待傳輸
	while (true) {
		if (Serial1.available() > 0) { //當收到的byte數大於0時，將收到的byte存到 暫存的d-array
			// read the incoming byte:
			a = Serial1.read();
			if (a == 150) { //如果收到255則停止傳輸
				dataNum = i;
				break;
			}
			d[i] = a;//紀錄收到的總資料數
			i++;

		}

	}


	digitalWrite(checkout, LOW);//將check pin 改為low 暫停傳輸


	dataNum = i;
	/*
	  for(int j=0;j<dataNum;j++){
	    Serial.println(d[j]);
	    }
	    */
}



int onnum = 0;
int offnum = 0;
void datatoNote() {//將收到的數據轉換為note on 或note off 以及速度

	for (int i = 0; i < dataNum; i++) {


		if ((d[i] < 127) && (d[i] > 24)) {//在noteon的範圍

			onpin[i] = d[i] + 35; //to center C (60)  	if 25 is the first key
			if ((d[i] == 26)) {
				vel[i] = d[i + 1] * 7 + 57;
				if (vel[i] > 127) {
					vel[i] = 127;
				}
			}
			else if ((d[i] == 34) || (d[i] == 27)) {
				vel[i] = d[i + 1] * 5 + 57;
				if (vel[i] > 127) {
					vel[i] = 127;
				}
			}
			else {
				vel[i] = d[i + 1] * 4 + 35;
			}


			d[i] = 0;
			onnum = i + 1;
			//  Serial.println(onpin[i]);
			lcd.setCursor(0, 0);
			lcd.print(vel[i]);
			lcd.print("              ");
		}

		if (d[i] > 127) {//noteoff的範圍
			offpin[i] = d[i] - 68;
			d[i] = 0;
			offnum = i + 1;
//   Serial.println(offpin[i]);
		}

	}

}

void sendnote() {//傳輸給midi chip


	for (int a = 0; a < onnum; a++) {
		for (int j = 0; j < 6; j++) {
			if (chennalenable[j] == 1) {
				noteOn(j, onpin[a] + 12 * (tuneNum[j] - 4), vel[a]);
			}
		}


		onpin[a] = 0;
		vel[a] = 0;

	}

	for (int a = 0; a < offnum; a++) {
		for (int j = 0; j < 6; j++) {
			if (chennalenable[j] == 1) {
				noteOff(j, onpin[a] + 12 * (tuneNum[j] - 4), 0);
			}
		}
		offpin[a] = 0;



	}



}





int lcd_key     = 0;
int adc_key_in  = 0;
int read_LCD_buttons()
{
	adc_key_in = analogRead(0);
// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
// we add approx 50 to those values and check to see if we are close
	if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
// For V1.1 us this threshold

	if (adc_key_in < 50)   return btnRIGHT;
	if (adc_key_in < 195)  return btnUP;
	if (adc_key_in < 380)  return btnDOWN;
	if (adc_key_in < 555)  return btnLEFT;
	if (adc_key_in < 790)  return btnSELECT;
	return btnNONE;  // when all others fail, return this...
}











void lcdControl() {

	if (read_LCD_buttons() != 5) { changectrl = true;  }
	while (changectrl) {
		printscreen();

		if (ctrl == 0) {layer0() ;}
		if (ctrl == 1) {layer1();}
		if (ctrl == 2) {layer2();}
		delay(100);
	}

}

void layer0() {
	switch (read_LCD_buttons()) {
	case 0:
		ctrl = 1;
		delay(1000);
		break;
	case 1://up
		if (fucNum[0] > 0) { fucNum[0]--;   }

		break;
	case 2://down
		if (fucNum[0] < 5) {fucNum[0]++; }
		break;
	case 3:
		lcd.setCursor(0, 0);
		lcd.print("----Piano----");
		lcd.setCursor(0, 1);
		lcd.print("Press any  key");
		changectrl = false;
		break;
	}


}



void layer1() {
	switch (read_LCD_buttons()) {
	case 1:
		if (fucNum[1] > 0) {
			fucNum[1]--;
		}
		break;

	case 2:
		if (fucNum[1] < 4) {
			fucNum[1]++;
		}
		break;

	case 0://right
		ctrl = 2;
		delay(500);

		break;

	case 3://left
		ctrl = 0;
		delay(500);
		break;


	}
}

void layer2() {
	switch (fucNum[1]) {
	case 3 :
		switch (read_LCD_buttons()) {
		case  5:
			break;
		case 1:
			if (sustainnum[fucNum[0]] < 127) {
				sustainnum[fucNum[0]]++;
			}
			break;
		case 2:
			if (sustainnum[fucNum[0]] > 0) {sustainnum[fucNum[0]]--;}
			else {sustainnum[fucNum[0]] = 0;}
			break;
		case 3:
			//sustain(0,sustainnum[fucNum[0]]);
			ctrl = 1;
			delay(500);
			break;
		}
		break;
	case 2:
		switch (read_LCD_buttons()) {
		case  5:
			break;
		case 1:
			if (programNum[fucNum[0]] < 127) {
				programNum[fucNum[0]]++;
			}
			break;
		case 2:
			if (programNum[fucNum[0]] > 0) {programNum[fucNum[0]]--;}
			else {programNum[fucNum[0]] = 0;}
			break;
		case 3:
			changeProgram(fucNum[0], programNum[fucNum[0]]);
			ctrl = 1;
			delay(500);
			break;
		}
		break;
	case 0:
		switch (read_LCD_buttons()) {
		case  5:
			break;
		case 1:
			chennalenable[fucNum[0]] = 1;
			break;
		case 2:
			chennalenable[fucNum[0]] = 0;
			break;
		case 3:
			ctrl = 1;
			delay(500);
			break;
		}
		break;
//--------------------------------Tune------------------------
	case 1:
		switch (read_LCD_buttons()) {
		case 1:
			if (tuneNum[fucNum[0]] < 9) {tuneNum[fucNum[0]]++;}
			break;
		case 2:
			if (tuneNum[fucNum[0]] > -1) {tuneNum[fucNum[0]]--;}
			break;
		case 3:
			ctrl = 1;
			delay(500);
			break;
		}


		break;
//-------------------------------------Volume-----------
	case 4:
		switch (read_LCD_buttons()) {
		case 1:
			if (Volume[fucNum[0]] < 127) {Volume[fucNum[0]]++;}
			break;
		case 2:
			if (Volume[fucNum[0]] > 0) {Volume[fucNum[0]]--;}
			break;
		case 3:
			volumeChange(fucNum[0], Volume[fucNum[0]]);
			ctrl = 1;
			delay(500);
			break;



		}
		break;
	}
}


void printscreen() {
	switch (ctrl) {
	case 0://layer 0
		lcd.setCursor(0, 0);
		lcd.print("----Channel----");
		lcd.setCursor(0, 1);
		lcd.print("[*]Channel");
		lcd.print(fucNum[0]);
		lcd.print("               ");

		break;
	case 1://layer 1
		lcd.setCursor(0, 0);
		lcd.print("----Setting----");
		lcd.setCursor(0, 1);
		switch (fucNum[1]) {
		case 3:
			lcd.print("[*]Sustain");
			lcd.print("                 ");
			break;
		case 2:
			lcd.print("[*]Instrument");
			lcd.print("                 ");
			break;
		case 0:
			lcd.print("[*]Enable");
			lcd.print("                 ");
			break;
		case 4:
			lcd.print("[*]Volume");
			lcd.print("                 ");
			break;
		case 1:
			lcd.print("[*]Tune");
			lcd.print("                 ");
			break;
		}
		break;


	case 2://layer 2
		lcd.setCursor(0, 0);
		switch (fucNum[1]) {
		case 3:
			lcd.print("----Sustain----");
			lcd.setCursor(0, 1);
			lcd.print("Value:");
			lcd.print(sustainnum[fucNum[0]]);
			lcd.print("                 ");
			break;
		case 2:
			lcd.print("----Intsrument----");
			lcd.setCursor(0, 1);
			lcd.print("Value:");
			lcd.print(programNum[fucNum[0]]);
			lcd.print("                 ");
			break;
		case 0:
			lcd.print("----Enable----");
			lcd.setCursor(0, 1);
			if (chennalenable[fucNum[0]] == 1) {
				lcd.print("True                ");
			}
			else {
				lcd.print("False              ");
			}

			break;
		case 4:
			lcd.print("----Volume----");
			lcd.setCursor(0, 1);
			lcd.print("Volume: ");
			lcd.print(Volume[fucNum[0]] - 4);
			lcd.print("                 ");
			break;
		case 1:
			lcd.print("----Tune----");
			lcd.setCursor(0, 1);
			lcd.print("Value:");
			lcd.print(tuneNum[fucNum[0]]);
			lcd.print("                 ");
			break;
		}
		break;




	}
}
