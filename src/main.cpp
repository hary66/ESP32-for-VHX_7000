/*
  Commande VHX7000
  ColdWay 2022
  Harold THIBAULT

**/
#include <Arduino.h>
#include <SerialCommands.h>
#include <Preferences.h>

#define ON HIGH
#define OFF LOW

// ESP to STRIP case circuit wiring
const int pause_pin = 16;
const int save_to_file_pin = 4;
const int push_button = 0;

// Create a 32 bytes static buffer to be used exclusive by SerialCommands object.
// The size should accomodate command token, arguments, termination sequence and string delimeter \0 char.
char serial_command_buffer_[32];
// Creates SerialCommands object attached to Serial
// working buffer = serial_command_buffer_
// command delimeter: Cr & Lf
// argument delimeter: SPACE
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
void cmd_unrecognized(SerialCommands *sender, const char *cmd)
{
  sender->GetSerial()->print("ERROR: Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

Preferences preferences; // For the permanent storage in EEPROM
uint16_t default_delayTime = 0.2;
uint16_t default_debounceTime = 50;
uint16_t dl = 0;            //  To leave enough time to the VHX 7000 to take action into account !
uint16_t debounce_time = 0; // for the push button

void takePicture()
{
  digitalWrite(pause_pin, ON);
  delay(dl);
  digitalWrite(pause_pin, OFF);
  delay(dl);
  digitalWrite(save_to_file_pin, ON);
  delay(dl);
  digitalWrite(save_to_file_pin, OFF);
  delay(dl);
  digitalWrite(save_to_file_pin, ON);
  delay(dl);
  digitalWrite(save_to_file_pin, OFF);
  delay(dl);
  digitalWrite(pause_pin, ON);
  delay(dl);
  digitalWrite(pause_pin, OFF);
}

void takePictureByCommand(SerialCommands *sender)
{
  takePicture();
}

void takePictureByButton()
{
  takePicture();
}

void help()
{
  Serial.println("\nFirmware ESP32 pour pilotage_VHX7000 par commande sérielle ou par bouton poussoir");
  Serial.println("ColdWay 2022/07/07 by Harold THIBAULT");
  Serial.println("-h pour afficher cet aide.");
  Serial.println("Envoyer la commande P + CR + LF sur le port sériel/USB pour prendre un cliché");
  Serial.println("Envoyer la commande DBT xx + CR + LF sur le port sériel/USB pour indiquer le debounce_ime du poussoir en ms");
  Serial.println("Envoyer la commande DL xx + CR + LF sur le port sériel/USB pour indiquer le delay_time entre les actions des relais de pilotage du VHX 7000 en ms");
  Serial.println("Envoyer la commande S + CR + LF sur le port sériel/USB pour sauvegarder le paramètres");
  Serial.println("Appuyer sur le bouton poussoir : porter à la masse la pin " + push_button);
}

void helpByCommand(SerialCommands *sender)
{
  help();
}

void debouceTimeAdjust(SerialCommands *sender)
{
  char *m_str = sender->Next(); // Note: Every call to Next moves the pointer to next parameter
  uint16_t m = atoi(m_str);
  sender->GetSerial()->println("\ndebounceTime = " + debounce_time);
  sender->GetSerial()->print("\ndebounceTime modified from  " + debounce_time);
  debounce_time = m;
  sender->GetSerial()->println(" to  " + debounce_time);
}

void delayTimeAdjust(SerialCommands *sender)
{
  char *m_str = sender->Next(); // Note: Every call to Next moves the pointer to next parameter
  uint16_t m = atoi(m_str);
  sender->GetSerial()->println("\ndelayTime = " + dl);
  sender->GetSerial()->print("\ndelayTime modified from  " + dl);
  dl = m;
  sender->GetSerial()->println(" to  " + dl);
}

void Save_parameters(SerialCommands *sender)
{
  sender->GetSerial()->println("\nAttention ! Sauvegarde des paramètres de calibration dans l'EEPROM");
  preferences.end();                   //   closing preferences permanent_storage to reopen it in R/W mode
  preferences.begin("storage", false); // Opening permanent_storage in R/W mode
  preferences.putUShort("dl", dl);
  preferences.putUShort("debouce_time", debounce_time);
  preferences.end();
  sender->GetSerial()->println("\nLes valeurs de calibrations ont été sauvegardées dans l'EEPROM");
}

SerialCommand takePicture_("P", takePictureByCommand);
//SerialCommand help_("--help", helpByCommand);
SerialCommand help_("-h", helpByCommand);
SerialCommand debouceTimeAdjust_("DBT", debouceTimeAdjust);
SerialCommand delayTimeAdjust_("DL", delayTimeAdjust);
SerialCommand Save_parameters_("S", Save_parameters);

void IRAM_ATTR ISR() //  Adding "IRAM_ATTR" in front of the ISR store it in the
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > debounce_time)
  {
    takePictureByButton();
  }
  last_interrupt_time = interrupt_time;
}

void setup()
{
  Serial.begin(115200);
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  serial_commands_.AddCommand(&takePicture_);
  serial_commands_.AddCommand(&help_);
  serial_commands_.AddCommand(&debouceTimeAdjust_);
  serial_commands_.AddCommand(&delayTimeAdjust_);

  pinMode(pause_pin, OUTPUT);
  pinMode(save_to_file_pin, OUTPUT);
  pinMode(push_button, INPUT_PULLUP);
  help();

  preferences.begin("storage", true); // EEPROM storage space space opened in read_only mode
  dl = preferences.getUShort("dl", default_delayTime);
  debounce_time = preferences.getUShort("debounce_time", default_debounceTime);
  preferences.end(); // EEPROM storage space closed

  attachInterrupt(push_button, ISR, CHANGE);
}

void loop()
{
  serial_commands_.ReadSerial(); //  Wait for command from serial port (USB)
}
