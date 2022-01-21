// Kirjasto I2C-väylän kanssa kommunikointiin
#include <Wire.h>
 // Kirjastot led-matriisin hallintaa varten:
#include "Adafruit_LEDBackpack.h"

#include "Adafruit_GFX.h"
 // TimerOne kirjasto:
#include "TimerOne.h"
 // EEPROM-kirjasto
#include <EEPROM.h>
 // Otetaan led-matriisi matrix-muuttujaan:
Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
// Laitetaan nappeihin kytketyt pinnit interrupteja varten:
const byte interruptPinLeft = 2;
const byte interruptPinRight = 3;
// Alustetaan tulos ja pelin nopeus:
int score = 0;
int myDelay = 1000;
// Alustetaan luku vihollisten ilmestymisen satunnaisuudelle:
long randNumber;
// Alustetaan pelitilanne, jonka mukaan ledimatriisia ohjataan:
int gameState[8][8] = {
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
    {
        0,0,0,0,0,0,0,0
    },
};
// Alustetaan pelaajan sijainta
int playerState[8] = {
    0,0,0,1,0,0,0,0
};
void setup() {
    // Otetaan randomSeed tyhjän analogipinnin värinästä:
    randomSeed(analogRead(0));
    // Alustetaan ajastin. Viiden sekunnin välein ajetaan pelin nopeutus-funktio:
    Timer1.initialize(5000000);
    Timer1.attachInterrupt(nopeutus);
    Serial.begin(9600);
    // Alustetaan matriisin osoite väylässä:
    matrix.begin(0x70);
    // Kiinnitetään nappipinneihin funktiot napin painalluksesta:
    pinMode(interruptPinLeft, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPinLeft), left, FALLING);
    pinMode(interruptPinRight, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPinRight), right, FALLING);
    // EEPROM oli aluksi 255, alustetaan se nollaan:
    if (EEPROM.read(0) == 255) {
        EEPROM.write(0, 0);
    }
}
void loop() {
    // Kasvatetaan tulosta jokaisella pelikierroksella:
    score++;
    // Tyhjennetään matriisi:
    matrix.clear();
    // Kirjoitetaan matriisiin gameStaten ja playerStaten mukaan:
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (gameState[x][y] == 1) {
                matrix.drawPixel(x, y, LED_ON); // write the data out to the matrix
            }
        }
    }
    for (int x = 0; x < 8; x++) {
        if (playerState[x] == 1) {
            matrix.drawPixel(7, x, LED_ON); // write the data out to the matrix
            if (gameState[7][x] == 1) {
                endGame();
            }
        }
    }
    //Sytytetään kirjoitetut ledit:
    matrix.writeDisplay();
    // Vihollisten lähestymisnopeus / kierrosnopeus:
    delay(myDelay);
    // Pudotetaan jokaista vihollisriviä yksi rivi alaspäin. Viimeiseltä riviltä katoavat.
    for (int x = 0; x < 7; x++) {
        for (int y = 0; y < 8; y++) {
            gameState[7 - x][y] = gameState[7 - (x + 1)][y];
        }
    }
    // Luodaan uusi vihollinen satunnaiseen kohtaan yläriville:
    randNumber = random(0, 8);
    for (int x = 0; x < 8; x++) {
        if (randNumber == x) {
            gameState[0][x] = 1;
        } else {
            gameState[0][x] = 0;
        }
    }
}
// Vasemman napin painalluksesta laukeava funktio. noInterrupts() yrittää estää tuplapainallukset.
// Siirtää pelaajaa yhden pykälän vasemmalle.
void left() {
    noInterrupts();
    Serial.println("Moi");
    for (int x = 1; x < 8; x++) {
        if (playerState[x] == 1) {
            playerState[x - 1] = 1;
            playerState[x] = 0;
            interrupts();
            return;
        }
    }
    interrupts();
}
// Sama, mutta oikealle.
void right() {
    noInterrupts();
    Serial.println("Hei");
    for (int x = 0; x < 7; x++) {
        if (playerState[x] == 1) {
            playerState[x + 1] = 1;
            playerState[x] = 0;
            interrupts();
            return;
        }
    }
    interrupts();
}
//Loppurutiinit, ajetaan kun pelaaja osuu viholliseen:
void endGame() {
    //Tallenetaan tulos EEPROMiin jos paras tulos ylittyy. Jaetaan kahdella, että EEPROMin mahtuu
    huipputulos.
    if(score / 2 > EEPROM.read(0)) {
        EEPROM.write(0, score / 2);
    }
    //Alustetaan matriisi scrollaavaa tekstiä varten, kirjoitetaan yrityksen pisteet ja huippupisteet:
    matrix.setTextSize(1);
    matrix.setTextWrap(false);
    matrix.setTextColor(LED_ON);
    for (int8_t x = 0; x >= -36; x--) {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print(" YOU: ");
        matrix.writeDisplay();
        delay(150);
    }
    for (int8_t x = 0; x >= -36; x--) {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print(" ");
        matrix.print(score / 2);
        matrix.writeDisplay();
        delay(150);
    }
    for (int8_t x = 0; x >= -36; x--) {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print(" HIGH: ");
        matrix.writeDisplay();
        delay(150);
    }
    for (int8_t x = 0; x >= -36; x--) {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print(" ");
        matrix.print(EEPROM.read(0));
        matrix.writeDisplay();
        delay(150);
    }
    //Alustetaan yrityspisteet taas nollaan ja pelin nopeus alkunopeudeksi.
    //Kytketään nopeutus takaisin, jos se on edellisessä ajossa poistettu.
    //Alustetaan pelaajan sijainti alkusijainniksi ja gameState tyhjäksi.
    score = 0;
    myDelay = 1000;
    Timer1.attachInterrupt(nopeutus);
    for (int x = 0; x < 8; x++) {
        playerState[x] = 0;
        if (x == 3) {
            playerState[x] = 1;
        }
        for (int y = 0; y < 8; y++) {
            gameState[x][y] = 0;
        }
    }
}
// Pelin nopeuttaja-funktio. Jos nopeus laskee tosi pieneksi, kytketään irti timerista -> peli ei enää nopeudu:
void nopeutus() {
    myDelay = myDelay - 50;
    Serial.println("Yeaa!");
    if (myDelay == 50) {
        Timer1.detachInterrupt();
    }
}