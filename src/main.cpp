/**
 * permet d'envoyer un code, notamment pour enregistrer une nouvelle télécommande.
 * pour se faire, envoyer sur Serial une commande commencant par 'S' et suivi de la valeur numérique, sans charactère de fin
 * exemple avec cutecom, choisir "no line end" et envoyer "S3544968"
 *
 * ensuite envoyer 'T' pour arrêter la transmission
 *
 *
 * tuto télécommande : https://www.youtube.com/watch?v=RM_xgNsP4sQ
 * Pour reset la télécommande appuyer sur A+B jusqu'à ce que la LED clignotte.
 * Ensuite pour enregistrer un bouton, lancer l'envoi du code depuis le arduino puis
 * placer le côté gauche (au niveau du bouton C, un peu plus bas) de la télécommande contre l'émetteur et
 * maintenir appuyé le bouton à programmer jusqu'à ce que la led clignotte.
 *
 */

#include <Arduino.h>
#include <output.h>
#include <RCSwitch.h>

RCSwitch rcSwitch = RCSwitch();

// pin which support interrupt, for more detail see https://www.arduino.cc/en/Reference/AttachInterrupt
const int rfReceiverPin    = 2;
const int rfTransmitterPin = 10;

const int rfProtocol            = 1;
const int rfPulseLength         = 275;
const unsigned int rfCodeLength = 24;

unsigned long signalToSend = 0;

void setup() {
  Serial.begin(115200);
  rcSwitch.enableReceive(digitalPinToInterrupt(rfReceiverPin)); // Receiver on interrupt 0 => that is pin #2

  rcSwitch.enableTransmit(rfTransmitterPin);
  rcSwitch.setRepeatTransmit(1);
  rcSwitch.setProtocol(rfProtocol, rfPulseLength);
}

void loop() {
  if (signalToSend) {
    rcSwitch.send(signalToSend, rfCodeLength);
  } else {
    // read from receiver
    if (rcSwitch.available()) {
      output(rcSwitch.getReceivedValue(), rcSwitch.getReceivedBitlength(),
        rcSwitch.getReceivedDelay(), rcSwitch.getReceivedRawdata(), rcSwitch.getReceivedProtocol());
      rcSwitch.resetAvailable();
    }
  }

  if (Serial.available() > 0) {
    char rc = Serial.read();
    if ('S' == rc) {
      Serial.setTimeout(50);
      char receivedChars[32];
      byte len = Serial.readBytes(receivedChars, 32);
      signalToSend = strtoul(receivedChars, NULL, 10);
      Serial.print(F("Serial received string is "));
      Serial.print(receivedChars);
      Serial.print(F(" (len="));
      Serial.print(len);
      Serial.print(F(") ; corresponding long value is "));
      Serial.print(signalToSend);
      Serial.println(F(" -> send it with rcSwitch ..."));
    }  else if ('T' == rc) {
      Serial.println(F("sending ends"));
      signalToSend = 0;
    } else {
      Serial.print(F("Serial read ignored "));
      Serial.println(rc, BIN);
    }
  }
} // loop
