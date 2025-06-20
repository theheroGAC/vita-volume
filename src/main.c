#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/power.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/registrymgr.h>
#include <debugScreen.h>
#include "ctrl.h"

#define printf psvDebugScreenPrintf
#define clearScreen psvDebugScreenClear
#define setColor psvDebugScreenSetFgColor

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_YELLOW 0xFFFFFF00
#define CONFIG_DIR "ux0:data/vitavolume/"
#define CONFIG_PATH CONFIG_DIR "vita_volume.cfg"

char *lang = "en";
int selected = 0;
unsigned int keyUp = SCE_CTRL_UP;
unsigned int keyDown = SCE_CTRL_DOWN;
unsigned int keyEnter = SCE_CTRL_CROSS;

void ensureDirectoryExists() {
    SceIoStat stat;
    if (sceIoGetstat(CONFIG_DIR, &stat) < 0) {
        sceIoMkdir(CONFIG_DIR, 0777);
    }
}

void saveSettings() {
    ensureDirectoryExists();
    SceUID fd = sceIoOpen(CONFIG_PATH, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 6);
    if (fd >= 0) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
            "language = %s\n"
            "selected = %d\n"
            "key_up = 0x%X\n"
            "key_down = 0x%X\n"
            "key_enter = 0x%X\n",
            lang, selected, keyUp, keyDown, keyEnter
        );
        sceIoWrite(fd, buffer, strlen(buffer));
        sceIoClose(fd);
    }
}

void loadSettings() {
    SceUID fd = sceIoOpen(CONFIG_PATH, SCE_O_RDONLY, 0);
    if (fd < 0) return;

    char buffer[256];
    int size = sceIoRead(fd, buffer, sizeof(buffer) - 1);
    sceIoClose(fd);
    if (size <= 0) return;

    buffer[size] = '\0';

    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        if (strncmp(line, "language =", 9) == 0) {
            char* val = strchr(line, '=');
            if (val) {
                val++;
                while (*val == ' ') val++;
                if (strncmp(val, "it", 2) == 0) lang = "it";
                else if (strncmp(val, "es", 2) == 0) lang = "es";
                else lang = "en";
            }
        } else if (strncmp(line, "selected =", 9) == 0) {
            sscanf(line + 9, "%d", &selected);
        } else if (strncmp(line, "key_up =", 8) == 0) {
            sscanf(line + 8, "%x", &keyUp);
        } else if (strncmp(line, "key_down =", 10) == 0) {
            sscanf(line + 10, "%x", &keyDown);
        } else if (strncmp(line, "key_enter =", 11) == 0) {
            sscanf(line + 11, "%x", &keyEnter);
        }

        line = strtok(NULL, "\n");
    }
}

const char* _(const char* key) {
    if (strcmp(lang, "it") == 0) {
        if (strcmp(key, "title") == 0) return "--- Vita Volume di inthecatsdreams ---";
        if (strcmp(key, "vol_up") == 0) return "Aumenta Volume";
        if (strcmp(key, "vol_down") == 0) return "Diminuisci Volume";
        if (strcmp(key, "mute") == 0) return "Disattiva volume";
        if (strcmp(key, "shutdown") == 0) return "Spegni la console";
        if (strcmp(key, "disable_avls") == 0) return "Disabilita AVLS";
        if (strcmp(key, "reboot") == 0) return "Riavvia e applica";
        if (strcmp(key, "exit") == 0) return "Esci";
        if (strcmp(key, "language") == 0) return "Lingua: Italiano";
        if (strcmp(key, "saved") == 0) return "Configurazione salvata!";
    } else if (strcmp(lang, "es") == 0) {
        if (strcmp(key, "title") == 0) return "--- Vita Volume por inthecatsdreams ---";
        if (strcmp(key, "vol_up") == 0) return "Subir volumen";
        if (strcmp(key, "vol_down") == 0) return "Bajar volumen";
        if (strcmp(key, "mute") == 0) return "Silenciar volumen";
        if (strcmp(key, "shutdown") == 0) return "Apagar consola";
        if (strcmp(key, "disable_avls") == 0) return "Desactivar AVLS";
        if (strcmp(key, "reboot") == 0) return "Reiniciar";
        if (strcmp(key, "exit") == 0) return "Salir";
        if (strcmp(key, "language") == 0) return "Idioma: Español";
        if (strcmp(key, "saved") == 0) return "¡Configuración guardada!";
    } else {
        if (strcmp(key, "title") == 0) return "--- Vita Volume by inthecatsdreams ---";
        if (strcmp(key, "vol_up") == 0) return "Increase Volume";
        if (strcmp(key, "vol_down") == 0) return "Decrease Volume";
        if (strcmp(key, "mute") == 0) return "Mute volume";
        if (strcmp(key, "shutdown") == 0) return "Shutdown";
        if (strcmp(key, "disable_avls") == 0) return "Disable AVLS";
        if (strcmp(key, "reboot") == 0) return "Reboot";
        if (strcmp(key, "exit") == 0) return "Exit";
        if (strcmp(key, "language") == 0) return "Language: English";
        if (strcmp(key, "saved") == 0) return "Settings saved!";
    }
    return "";
}

const char* getVolumeSymbol(int vol) {
    if (vol == 0) return "[🔇]";
    if (vol < 20) return "[🔉]";
    return "[🔊]";
}

void showMessage(const char* msg) {
    printf("\n%s\n", msg);
    sceKernelDelayThread(2000000);
}

int getCurrentVolume() {
    int val = 0;
    sceRegMgrGetKeyInt("/CONFIG/SOUND/", "main_volume", &val);
    return val;
}

void setVolume(int vol) {
    sceRegMgrSetKeyInt("/CONFIG/SOUND/", "main_volume", vol);
}

int getAVLSStatus() {
    int val = 0;
    sceRegMgrGetKeyInt("/CONFIG/SOUND/", "avls", &val);
    return val;
}

void disableAVLS() {
    sceRegMgrSetKeyInt("/CONFIG/SOUND/", "avls", 0);
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
                    if (vol < 30) setVolume(vol + 1);
                    break;
                case 1:
                    if (vol > 0) setVolume(vol - 1);
                    break;
                case 2:
                    setVolume(0);
                    break;
                case 3:
                    disableAVLS();
                    break;
                case 4:
                    showMessage(_("reboot")); scePowerRequestColdReset(); break;
                case 5:
                    showMessage(_("shutdown")); scePowerRequestStandby(); break;
                case 6:
                    showMessage(_("exit")); sceKernelExitProcess(0); break;
                case 7:
                    if (strcmp(lang, "it") == 0) lang = "en";
                    else if (strcmp(lang, "en") == 0) lang = "es";
                    else lang = "it";
                    showMessage(_("saved"));
                    break;
            }
            saveSettings();
            drawMenu(selected);
        }
        sceKernelDelayThread(100000);
    }

    return 0;
}
