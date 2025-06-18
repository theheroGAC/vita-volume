#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vitasdk.h>

#include "ctrl.h"
#include "debugScreen.h"

#define printf psvDebugScreenPrintf
#define clearScreen psvDebugScreenClear
#define setColor psvDebugScreenSetFgColor

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_YELLOW 0xFFFFFF00

char *lang = "en"; // default language
int selected = 0;
unsigned int keyUp = SCE_CTRL_UP;
unsigned int keyDown = SCE_CTRL_DOWN;
unsigned int keyEnter = SCE_CTRL_CROSS;

const char* _(const char* key) {
    if (strcmp(lang, "it") == 0) {
        if (strcmp(key, "title") == 0) return "--- Vita Volume di inthecatsdreams ---";
        if (strcmp(key, "vol_up") == 0) return "Aumenta Volume";
        if (strcmp(key, "vol_down") == 0) return "Diminuisci Volume";
        if (strcmp(key, "mute") == 0) return "Disattiva volume";
        if (strcmp(key, "shutdown") == 0) return "Spegni la console";
        if (strcmp(key, "disable_avls") == 0) return "Disabilita AVLS";
        if (strcmp(key, "reboot") == 0) return "Riavvia e applica";
        if (strcmp(key, "exit") == 0) return "Esci dall'app";
        if (strcmp(key, "language") == 0) return "Lingua: Italiano";
        if (strcmp(key, "vol_increased") == 0) return "Volume aumentato!";
        if (strcmp(key, "vol_decreased") == 0) return "Volume diminuito!";
        if (strcmp(key, "muted") == 0) return "Console muta.";
        if (strcmp(key, "already_max") == 0) return "Volume al massimo.";
        if (strcmp(key, "already_min") == 0) return "Volume al minimo.";
        if (strcmp(key, "rebooting") == 0) return "Riavvio in corso...";
        if (strcmp(key, "shutting_down") == 0) return "Spegnimento in corso...";
        if (strcmp(key, "exiting") == 0) return "Uscita dall'app...";
        if (strcmp(key, "avls_disabled") == 0) return "AVLS disabilitato.";
        if (strcmp(key, "keys_saved") == 0) return "Tasti salvati.";
    } else if (strcmp(lang, "es") == 0) {
        if (strcmp(key, "title") == 0) return "--- Vita Volume por inthecatsdreams ---";
        if (strcmp(key, "vol_up") == 0) return "Subir volumen";
        if (strcmp(key, "vol_down") == 0) return "Bajar volumen";
        if (strcmp(key, "mute") == 0) return "Silenciar volumen";
        if (strcmp(key, "shutdown") == 0) return "Apagar consola";
        if (strcmp(key, "disable_avls") == 0) return "Desactivar AVLS";
        if (strcmp(key, "reboot") == 0) return "Reiniciar y aplicar";
        if (strcmp(key, "exit") == 0) return "Salir de la app";
        if (strcmp(key, "language") == 0) return "Idioma: Espa\xC3\xB1ol";
        if (strcmp(key, "vol_increased") == 0) return "\xC2\xA1Volumen aumentado!";
        if (strcmp(key, "vol_decreased") == 0) return "\xC2\xA1Volumen reducido!";
        if (strcmp(key, "muted") == 0) return "Consola silenciada.";
        if (strcmp(key, "already_max") == 0) return "Volumen m\xC3\xA1ximo alcanzado.";
        if (strcmp(key, "already_min") == 0) return "Volumen m\xC3\xADnimo alcanzado.";
        if (strcmp(key, "rebooting") == 0) return "Reiniciando...";
        if (strcmp(key, "shutting_down") == 0) return "Apagando...";
        if (strcmp(key, "exiting") == 0) return "Saliendo...";
        if (strcmp(key, "avls_disabled") == 0) return "AVLS desactivado.";
        if (strcmp(key, "keys_saved") == 0) return "Teclas guardadas.";
    } else {
        if (strcmp(key, "title") == 0) return "--- Vita Volume by inthecatsdreams ---";
        if (strcmp(key, "vol_up") == 0) return "Increase Volume";
        if (strcmp(key, "vol_down") == 0) return "Decrease Volume";
        if (strcmp(key, "mute") == 0) return "Mute volume";
        if (strcmp(key, "shutdown") == 0) return "Shutdown console";
        if (strcmp(key, "disable_avls") == 0) return "Disable AVLS";
        if (strcmp(key, "reboot") == 0) return "Reboot and apply";
        if (strcmp(key, "exit") == 0) return "Exit app";
        if (strcmp(key, "language") == 0) return "Language: English";
        if (strcmp(key, "vol_increased") == 0) return "Volume increased!";
        if (strcmp(key, "vol_decreased") == 0) return "Volume decreased!";
        if (strcmp(key, "muted") == 0) return "Console muted.";
        if (strcmp(key, "already_max") == 0) return "Already at maximum volume.";
        if (strcmp(key, "already_min") == 0) return "Already at minimum volume.";
        if (strcmp(key, "rebooting") == 0) return "Rebooting...";
        if (strcmp(key, "shutting_down") == 0) return "Shutting down...";
        if (strcmp(key, "exiting") == 0) return "Exiting app...";
        if (strcmp(key, "avls_disabled") == 0) return "AVLS disabled.";
        if (strcmp(key, "keys_saved") == 0) return "Keys saved.";
    }
    return "";
}

void loadSettings() {
    FILE* f = fopen("ux0:data/vita_volume.cfg", "r");
    if (f) {
        char buffer[8];
        fgets(buffer, sizeof(buffer), f);
        if (strncmp(buffer, "it", 2) == 0) lang = "it";
        else if (strncmp(buffer, "es", 2) == 0) lang = "es";
        else lang = "en";

        fread(&selected, sizeof(int), 1, f);
        fread(&keyUp, sizeof(unsigned int), 1, f);
        fread(&keyDown, sizeof(unsigned int), 1, f);
        fread(&keyEnter, sizeof(unsigned int), 1, f);
        fclose(f);
    }
}

void saveSettings() {
    FILE* f = fopen("ux0:data/vita_volume.cfg", "w");
    if (f) {
        fputs(lang, f);
        fwrite(&selected, sizeof(int), 1, f);
        fwrite(&keyUp, sizeof(unsigned int), 1, f);
        fwrite(&keyDown, sizeof(unsigned int), 1, f);
        fwrite(&keyEnter, sizeof(unsigned int), 1, f);
        fclose(f);
    }
}

int getCurrentVolume() {
    int val = 0;
    sceRegMgrGetKeyInt("/CONFIG/SOUND/", "main_volume", &val);
    return val;
}

int getAVLSStatus() {
    int val = 0;
    sceRegMgrGetKeyInt("/CONFIG/SOUND/", "avls", &val);
    return val;
}

void setVolume(int vol) {
    sceRegMgrSetKeyInt("/CONFIG/SOUND/", "main_volume", vol);
}

void disableAVLS() {
    sceRegMgrSetKeyInt("/CONFIG/SOUND/", "avls", 0);
}

const char* getVolumeSymbol(int vol) {
    if (vol == 0) return "[🔇]";
    if (vol < 20) return "[🔉]";
    return "[🔊]";
}

const char* keyFromIndex(int i) {
    switch (i) {
        case 0: return "vol_up";
        case 1: return "vol_down";
        case 2: return "mute";
        case 3: return "disable_avls";
        case 4: return "reboot";
        case 5: return "shutdown";
        case 6: return "exit";
        case 7: return "language";
        default: return "";
    }
}

void drawMenu(int sel) {
    clearScreen();
    printf("%s\n\n", _("title"));
    for (int i = 0; i < 8; i++) {
        if (i == sel) {
            setColor(COLOR_YELLOW);
            printf("> %s\n", _(keyFromIndex(i)));
            setColor(COLOR_WHITE);
        } else {
            printf("  %s\n", _(keyFromIndex(i)));
        }
    }
    int vol = getCurrentVolume();
    printf("\nVolume: %d %s\n", vol, getVolumeSymbol(vol));
    printf("AVLS: %s\n", getAVLSStatus() ? "ON" : "OFF");
}

void showMessage(const char* msg) {
    printf("\n%s\n", msg);
    sceKernelDelayThread(2000000);
}

int main() {
    psvDebugScreenInit();
    loadSettings();
    drawMenu(selected);

    while (1) {
        int key = get_key(0);

        if (key & keyUp) {
            selected = (selected + 7) % 8;
            drawMenu(selected);
        } else if (key & keyDown) {
            selected = (selected + 1) % 8;
            drawMenu(selected);
        } else if (key & keyEnter) {
            int vol = getCurrentVolume();
            switch (selected) {
                case 0:
                    if (vol < 30) { setVolume(vol + 1); showMessage(_("vol_increased")); }
                    else showMessage(_("already_max"));
                    break;
                case 1:
                    if (vol > 0) { setVolume(vol - 1); showMessage(_("vol_decreased")); }
                    else showMessage(_("already_min"));
                    break;
                case 2:
                    setVolume(0); showMessage(_("muted"));
                    break;
                case 3:
                    disableAVLS(); showMessage(_("avls_disabled"));
                    break;
                case 4:
                    showMessage(_("rebooting")); scePowerRequestColdReset(); break;
                case 5:
                    showMessage(_("shutting_down")); scePowerRequestStandby(); break;
                case 6:
                    showMessage(_("exiting")); sceKernelExitProcess(0); break;
                case 7:
                    if (strcmp(lang, "it") == 0) lang = "en";
                    else if (strcmp(lang, "en") == 0) lang = "es";
                    else lang = "it";
                    saveSettings(); drawMenu(selected); break;
            }
            saveSettings();
            drawMenu(selected);
        }

        sceKernelDelayThread(100000);
    }

    return 0;
}
