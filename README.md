Firmware ESP32 pour pilotage_VHX7000 par commande sérielle ou par bouton poussoir
ColdWay 2022/07/07 by Harold THIBAULT.
Envoyer la commande h sur le port sériel/USB pour afficher cet aide.
Envoyer la commande P sur le port sériel/USB pour prendre un cliché.
Envoyer la commande DBT xx sur le port sériel/USB pour indiquer le debounce_ime du poussoir en ms.
Envoyer la commande DL xx sur le port sériel/USB pour indiquer le delay_time entre les actions des relais de pilotage du VHX 7000 en ms.
Envoyer la commande S sur le port sériel/USB pour sauvegarder le paramètres.
Appuyer sur le bouton poussoir pour initier un cliché : porter à la masse la pin
La commande des relais \"Pause \" et \"save_to_file\" se fait en 3.3V depuis le µC. Un étage de sortie puissance est nécessaire pour piloter les bobines 12V des relais.
