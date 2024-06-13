#include "mbed.h"
#include "max31865.h"


#define temperature       0x54
#define humidite          0x48
#define pH                0x50
#define CO2               0x43
#define viscosite         0x56


//initialisation de l'I/O
Serial pc(SERIAL_TX, SERIAL_RX);
Serial Mbed(PB_6,PB_7);
max31865 PT100(PB_5, PB_4, PB_3, PA_11); // MOSI, MISO, SCLK, CS - D11, D12, D13, D10
PwmOut PwmRelay(PA_8);//pinout relay PWM  /!\ le relay est normalement ouvert -> PwmRelay = 1 -> circuit fermé
Timer timer;


// Déclaration des variables
float Kp = 0.01, Ki = 0, Kd = 0,Temperature_consigne = 40, temps_1, temps_2, erreur_1, erreur_2, I, D;


// Déclaration des fonctions
float Temperature(void);
float Puissance_chauffe(void);
float Deriver(float temps_1, float temps_2, float erreur_1, float erreur_2);
float Integrale(float temps_1, float temps_2, float erreur_1, float S);
void Envoie_Donners(char type, float donner);// les donner sont le nombre a envoyé, il sera envoyé comme ça --,-


//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

//Début du programme
int main(void)
{
//Mise en place des paramètres non changent
    PT100.Begin(MAX31865_3WIRE);
    PwmRelay.period(1);
    timer.start();
//On affiche nos coef pour les tests de paliers
    pc.printf("Kp = %f;Ki = %f;Kd = %f\n\r",Kp,Ki,Kd);

    while(1) {
        
//On définie la puissance de chauffe
        PwmRelay = Puissance_chauffe();

//On récupère un premier couple Température/temps pour les calcules de dériver|Intégrale
        erreur_2 = Temperature_consigne - Temperature();
        temps_2 = timer.read();

//On calcule la dériver/intégrale
        D = Deriver(temps_1, temps_2, erreur_1, erreur_2);
        I = Integrale(temps_1, temps_2, erreur_1, I);

//On fait les affichages (Temperature_consigne, Temperature_mesurée, Puissance_chauffe, temps, Dériver et Intégrale)
       // pc.printf("Temperature_consigne = %.2f;Temperature_mesuree = %.2f;On chauffe a %.2f;temps(s) = %.2f;Erreur = %.2f;Deriver = %.2f;Integrale = %.2f\n\r",Temperature_consigne,(-1.0*erreur_1 + Temperature_consigne),PwmRelay.read()*100,temps_1,erreur_1,D,I);

//On gère les tempsattend une secondes entre nos prises de mesures
        thread_sleep_for(1*1000);

//On récupère un second couple Température/temps pour les calcules de dériver|Intégrale
        temps_1 = temps_2;
        erreur_1 = erreur_2;

        
        Envoie_Donners(temperature, Temperature());
        
    }
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

float Temperature(void)
{
//On récupère la température depuis la lecture de la différence de résistance des cables de la Pt100
    float ratio, Resistance_mesuree, Temperature_mesuree, Resistance_referfance = 430.0;
    
//Le conditionneur retourne la valeur binaire non signée du ratio entre la Resistance_mesuree et Resistance_referfance
//On a donc ratio = Resistance_mesuree / Resistance_referfance
//On récupere la ratio
    ratio = PT100.ReadRTD(); 
    
//L'information est envoye par le conditionneur sur les MSB on les remet donc sur les LSB pour les traiter
    ratio /= 32768;
    
//On calcule Resistance_mesuree a partire du ratio
    Resistance_mesuree = Resistance_referfance*ratio;
    
//On calcule la temperature grace a la loi d'etalonnage qu'on a etablie : R = 0.365T + 100
    Temperature_mesuree = ((Resistance_mesuree - 100)/0.365); 
    
    return Temperature_mesuree;
}

float Puissance_chauffe(void)
{
//On calcule la puissance d'allimentation de la plaque chauffante
    float erreur = Temperature_consigne - Temperature();
    if (erreur < 0 ) {
        return 0;
    } else {
        return erreur*Kp + Ki*I + Kd*D;
    }
}

float Deriver(float temps_1, float temps_2, float erreur_1, float erreur_2)
{
//On dérive l'erreur
    return (erreur_2-erreur_1)/(temps_2-temps_1);
}

float Integrale(float temps_1, float temps_2, float erreur_1, float S)
{
//On intègre l'erreur
    return (temps_2-temps_1)*erreur_1 + 0*S;
}

void Envoie_Donners(char type, float donner)
{
//On envoie les donner a l'autre microcontrolleur
//On isole la dixaine, l'unité et le dixième
    int c3 = int(donner*10) %10;
    int c2 = int(donner) % 10;
    int c1 = int(donner) / 10;
//On envoie notre trame 
    Mbed.putc(0xAA);
    Mbed.putc(type);
    Mbed.putc(c1+0x30);
    Mbed.putc(c2+0x30);
    Mbed.putc(c3+0x30);
    Mbed.putc(0xF0);
}
