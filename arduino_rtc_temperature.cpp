/* 
## Fonctionnement des boutons ## :
Pour Parametrer l'horloge, on clique sur le bouton SET à gauche.
Premier Clique => Heure
Deuxieme => Minutes
...
Sixième clique => Sortir du mode config.
Quand on sort, on laisse juste l'horloge fonctionner seul. On attend un peu pour constater. 
*/

// Librairies
#include <LiquidCrystal.h>

// RS, EN, Db4, Db5, Db6, Db7
LiquidCrystal lcd(7, 6, 4, 3, 2, 1);

// Définition des boutons
const int boutonInc = 9;
const int boutonDec = 8;
const int boutonSet = 10;

// Variables de date et heure
int jour = 1, mois = 1, annee = 2025;
int heures = 12, minutes = 0, secondes = 0;

// Variables de gestion des boutons
int modeEdition = -1; // -1 = Pas en édition, 0 = Heures, 1 = Minutes, 2 = Jour, 3 = Mois, 4 = Année
const char* configElements[5] = {"Heure", "Minutes", "Jour", "Mois", "Annee"};
bool modeConfig = false;  // Mode configuration activé/désactivé
  
// Variables pour millis()
unsigned long previousMillis = 0;
const long interval = 1000; // 1 seconde

// Nombre de jours par mois
const int joursParMois[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void setup() {
  lcd.begin(16, 2);

  // Configuration des boutons en entrée
  pinMode(boutonInc, INPUT);
  pinMode(boutonDec, INPUT);
  pinMode(boutonSet, INPUT);

  lcd.print("UTB LIGNE 1");
  lcd.setCursor(0, 1);
  lcd.print("ABIDJAN - YAKRO");
  delay(3000);
  lcd.clear();
}

void loop() {
  gererBoutons();

  // On lit la temperature captée
  int sensorValue = analogRead(A0);

  // Logique de mise à jour automatique de l'horloge (si on n'est PAS en mode édition)
  if (modeEdition == -1) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      secondes++;

      if (secondes >= 60) {
        secondes = 0;
        minutes++;

        if (minutes >= 60) {
          minutes = 0;
          heures++;

          if (heures >= 24) {
            heures = 0;
            jour++;

            if (jour > joursParMois[mois - 1]) {
              jour = 1;
              mois++;

              if (mois > 12) {
                mois = 1;
                annee++;
              }
            }
          }
        }
      }
    }
  }

  // Affichage
  afficherHeure();
  afficherTemperature(sensorValue);
  afficherDate();

  delay(100);
}

// ----- GESTION DES BOUTONS -----
void gererBoutons() {
  
  if (digitalRead(boutonSet) == HIGH) {
    
    modeEdition = (modeEdition + 1) % 6; // Change l'élément à modifier (6 = retour au mode normal)
	modeConfig = true;
    
    if (modeEdition == 5) 
    {
      modeEdition = -1; // Sortie du mode édition
      modeConfig = false; 
    }
    delay(300);
    
    lcd.clear();
    
    if (modeConfig) {
      lcd.setCursor(0, 0);
      lcd.print("Mode Config");
      lcd.setCursor(0, 1);
      lcd.print(configElements[modeEdition]);
      delay(1000);
      lcd.clear();
    }else{
      lcd.setCursor(0, 0);
      lcd.print("Sortie du mode");
      delay(1000);
      lcd.clear();
    }
  }

  if (modeEdition != -1) { // En mode édition, on peut modifier les valeurs
    if (digitalRead(boutonInc) == HIGH) {
      incrementerValeur();
      delay(200);
    }
    if (digitalRead(boutonDec) == HIGH) {
      decrementerValeur();
      delay(200);
    }
  }
}

// ----- INCRÉMENTATION ET DÉCRÉMENTATION -----
void incrementerValeur() {
  switch (modeEdition) {
    case 0: heures = (heures + 1) % 24; break;
    case 1: minutes = (minutes + 1) % 60; break;
    case 2: jour = (jour % joursParMois[mois - 1]) + 1; break;
    case 3: mois = (mois % 12) + 1; break;
    case 4: annee++; break;
  }
}

void decrementerValeur() {
  switch (modeEdition) {
    case 0: heures = (heures - 1 + 24) % 24; break;
    case 1: minutes = (minutes - 1 + 60) % 60; break;
    case 2: jour = (jour - 1 == 0) ? joursParMois[mois - 1] : jour - 1; break;
    case 3: mois = (mois - 1 == 0) ? 12 : mois - 1; break;
    case 4: annee = (annee - 1 < 2020) ? 2020 : annee - 1; break;
  }
}

// ----- AFFICHAGE -----

void afficherHeure() {
  char heure[9];
  sprintf(heure, "%02d:%02d", heures, minutes);
  lcd.setCursor(2, 0);
  lcd.print(heure);
}

void afficherTemperature(int sensor_val) {
  float  temperature = (sensor_val * 5.0 / 1024.0 - 0.5 ) * 100.0;
  lcd.setCursor(10, 0);
  lcd.print(round(temperature));
  lcd.print(char(178));
  lcd.print("C");
}

void afficherDate() {
  char date[11];
  sprintf(date, "%02d/%02d/%04d", jour, mois, annee);
  lcd.setCursor(3, 1);
  lcd.print(date);
}
