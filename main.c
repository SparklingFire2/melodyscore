#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>
#include <stdbool.h>
#include <wchar.h>
#include <sys/stat.h>
#include <windows.h>
#include <Lmcons.h>

#define GetCurrentDir _getcwd
#define NB_BUNDLED_TRACKS 6


typedef struct{
    char ligne[FILENAME_MAX]; //A copier tel quel
    double pourcentage; //Pour le tri
}Donnee;

int ordreDecroissantPourcentage_function(const void *a, const void *b) {
    Donnee *x = (Donnee *) a;
    Donnee *y = (Donnee *) b;
    if(x->pourcentage > y->pourcentage) return -1;
    else if(x->pourcentage < y->pourcentage) return 1;
    return 0;
}

bool isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
       return false;
    return S_ISDIR(statbuf.st_mode);
}

int main()
{
    char username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName(username, &username_len);

    char bundledTracks[6][41] = {"Shirobon - City Patrol (Stage B)",
                                 "Shirobon - Cyber Party ft Radix",
                                 "Shirobon - Immune ft Camden Cox",
                                 "Shirobon - Perfect Machine",
                                 "Shirobon - Regain Control",
                                 "Shirobon - Running My Head ft Camden Cox"};

    wchar_t wideBundledTrack[41];

    /* Même si Relaxing et Medium ont toujours les mêmes scores, je préfère les garder séparées
    pour pouvoir mettre à jour simplement le code si le créateur du jeu décide un jour d'ajouter
    d'autres morceaux où il ferait des pistes différentes pour ces 2 modes.
    Les scores ont été trouvé en ouvrant l'exécutable du jeu avec HxD (éditeur hexadécimal)
    en recherchant par exemple la chaîne "18:". Il faut cocher la case Chaine Unicode */

    int scoresBundledTracks[NB_BUNDLED_TRACKS][4] = { {48712, 48712, 48712, 71106},
                                      {62896, 62896, 62896, 80920},
                                      {46402, 46402, 69966, 58578},
                                      {95184, 95184, 95184, 116728},
                                      {61512, 61512, 61512, 79956},
                                      {52278, 52278, 72660, 64416} };

    int score = 0, combo = 0, multiplicateur = 1, longueurTrainee, base;
    int type, symbole;
    int positionDernierCaractereExtension, difficulte, scorePerso;

    FILE *fichier;
    FILE *fichierDifficulte[5];
    FILE *fichierScore;

    Donnee *donneeDifficulte;
    int nbLignes[6] = {0};
    int ligneActuelle[5] = {0};

    char dossierJeu[FILENAME_MAX];
    char dossierTracks[FILENAME_MAX];
    char cheminFichier[FILENAME_MAX];
    char cheminFichierTemp[FILENAME_MAX];
    char nomFichierMusique[FILENAME_MAX];
    wchar_t wideNomFichierMusique[FILENAME_MAX];
    char nomFichierScore[9] = "hs_x.mel";
    wchar_t ligne[10000];
    GetCurrentDir(dossierJeu, FILENAME_MAX);
    char lecteur = dossierJeu[0];
    sprintf(dossierJeu, "%c:/Users/%s/AppData/Roaming/MelodyEscape", lecteur, username);
    sprintf(dossierTracks, "%s/TracksCache", dossierJeu);

    struct dirent *dir;
    DIR *d = opendir(dossierTracks);

    for(int i = 0; i < 5; i++){
        int temp = nbLignes[i];
        ligneActuelle[i] = temp;
        nomFichierScore[3] = i + '0';
        sprintf(cheminFichier, "%s/%s", dossierJeu, nomFichierScore);

        fichierScore = fopen(cheminFichier, "a, ccs=UTF-16LE");
        fclose(fichierScore);
        fichierScore = fopen(cheminFichier, "r, ccs=UTF-16LE");
        while(!feof(fichierScore)){
            fgetws(ligne, FILENAME_MAX, fichierScore);
            nbLignes[i]++;
        }

        nbLignes[i+1] = nbLignes[i];
        nbLignes[i] = temp;
        fclose(fichierScore);
    }

    donneeDifficulte = malloc(nbLignes[5] * sizeof(Donnee));

    //Tempo case 3, EP case 4
    wchar_t customDifficultyCode[9] = L":      :";
    bool cacheTrackData = false;

    sprintf(cheminFichier, "%s/settings.xml", dossierJeu);
    fichier = fopen(cheminFichier, "r");

    for(int i = 0; i < 25; i++)
        fgetws(ligne, 200, fichier);

    while(fgetc(fichier) != '>');

    if(getc(fichier) == 't')
        cacheTrackData = true;

    for(int i = 0; i < 23; i++)
        fgetws(ligne, 200, fichier);

    for(int i = 1; i < 7; i++){
        while(fgetc(fichier) != '>');

        customDifficultyCode[i] = (wchar_t)fgetc(fichier);
        fgetws(ligne, 200, fichier);
    }

    fclose(fichier);

    // a me permet ici de créer le fichier s'il n'existe pas déjà, r+ ne fonctionne que sur les fichiers qui existent déjà
    fichier = fopen("CurrentCustomDifficulty.txt", "a, ccs=UTF-16LE");
    fclose(fichier);
    fichier = fopen("CurrentCustomDifficulty.txt", "r+, ccs=UTF-16LE");

    wchar_t previousCustomDifficultyCode[9];
    if(fgetws(previousCustomDifficultyCode, 9, fichier) == NULL){
        fwprintf(fichier, L"%s", customDifficultyCode);
    }
    else if(wcscmp(previousCustomDifficultyCode, customDifficultyCode) != 0 ){
        fseek(fichier, 2, SEEK_SET);
        sprintf(cheminFichierTemp, "%s/hs_t.mel", dossierJeu);
        FILE *fichierScoreTemp = fopen(cheminFichierTemp, "w, ccs=UTF-16LE");
        /* J'avais plein de problèmes bizarres pour ça que j'ai ouvert le fichier en mode binaire,
        sinon ça ne marchait pas genre je demandais à fseek de reculer et il avançait... */
        sprintf(cheminFichier, "%s/hs_4.mel", dossierJeu);
        fichierScore = fopen(cheminFichier, "rb, ccs=UTF-16LE");

        wchar_t wc;

        fseek(fichierScore, 2, SEEK_SET);
        while((wc = fgetwc(fichierScore)) != WEOF){
            fseek(fichierScore, -2, SEEK_CUR);
            fgetws(ligne, FILENAME_MAX, fichierScore);
            ligne[wcslen(ligne) - 2] = L'\0';

            if(wc == L':'){
                wchar_t temp = ligne[8];
                ligne[8] = L'\0';

                int offset = 0;

                if(wcscmp(ligne, customDifficultyCode) == 0)
                    offset = 8;

                ligne[8] = temp;

                fwprintf(fichierScoreTemp, L"%s\n", ligne + offset);
            }
            else{
                fwprintf(fichierScoreTemp, L"%s", previousCustomDifficultyCode);
                fwprintf(fichierScoreTemp, L"%s\n", ligne);
            }
        }

        fclose(fichierScore);
        fclose(fichierScoreTemp);

        remove(cheminFichier);
        rename(cheminFichierTemp, cheminFichier);
        fwprintf(fichier, L"%s", customDifficultyCode);
    }

    fclose(fichier);

    int customDifficultyTEMPO = customDifficultyCode[3] - L'0';
    int customDifficultyEP = customDifficultyCode[4] - L'0' + 1;

    for(int i = 0; i < 5; i++){
        nomFichierScore[3] = i + '0';
        sprintf(cheminFichier, "%s/%s", dossierJeu, nomFichierScore);
        difficulte = i;
        if(i == 4)
            difficulte = customDifficultyTEMPO;
        fichierScore = fopen(cheminFichier, "r, ccs=UTF-16LE");
        for(int j = 0; j < NB_BUNDLED_TRACKS; j++){
            /* On place le curseur à 2 après le début du fichier car les fichiers encodés en UTF-16LE
            commencent par un caractère spécial (large donc 2 et non pas 1) qu'on ne veut pas lire.
            Note : Lorsqu'on ouvre un fichier en mode UTF-16LE, le curseur est directement placé au bon endroit
            donc pas besoin de fseek si on ne lit le fichier qu'une fois */
            fseek(fichierScore, 2, SEEK_SET);

            //On convertit la chaîne de char en chaîne de wchar_t pour pouvoir comparer avec ligne
            mbstowcs(wideBundledTrack , bundledTracks[j], FILENAME_MAX);

            int caractereFinBundledTrack = wcslen(wideBundledTrack);

            while(!feof(fichierScore)){
                if(fgetws(ligne, FILENAME_MAX, fichierScore) == NULL){
                    break;
                }
                /* On ne met un '\0' que si c'est un ; sinon on risquerait de récupérer le score d'un autre morceau dont le nom
                commence de la même manière */
                if(ligne[caractereFinBundledTrack] == L';')
                    ligne[caractereFinBundledTrack] = L'\0';

                //Fonction de compraison des chaînes de caractères de type wchar_t
                if(wcscmp(wideBundledTrack, ligne) == 0){
                    swscanf(ligne + caractereFinBundledTrack + 1, L"%d", &scorePerso);
                    int nbCoeurs;
                    int caractere = caractereFinBundledTrack + 2;
                    while(ligne[caractere] != L';')
                        caractere++;
                    swscanf(ligne + caractere + 1, L"%d", &nbCoeurs);

                    donneeDifficulte[ligneActuelle[i]].pourcentage = scorePerso * 100 / (double)scoresBundledTracks[j][difficulte];
                    sprintf(donneeDifficulte[ligneActuelle[i]].ligne, "%s : %d / %d : %.2f%% (%d)\n", bundledTracks[j], scorePerso, scoresBundledTracks[j][difficulte], donneeDifficulte[ligneActuelle[i]].pourcentage, nbCoeurs);

                    ligneActuelle[i]++;

                    break;
                }
            }
        }
        fclose(fichierScore);
    }

    if(d){
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] != '.'){
                sprintf(cheminFichier, "%s/%s", dossierTracks, dir->d_name);
                if(!isDirectory(cheminFichier)){
                    fichier = fopen(cheminFichier, "r");
                    for(int i = 0; i < 3; i++){
                        fgetws(ligne, 10000, fichier);
                    }

                    fgetc(fichier);

                    if(!feof(fichier))
                        fseek(fichier, -1, SEEK_CUR);
                    else{
                        fclose(fichier);
                        continue;
                    }

                    while(!feof(fichier)){
                        while(fgetc(fichier) != ':');

                        type = fgetc(fichier);

                        if(type == '2' || type == '4' || type == '6' || type == '8')
                            base = 50;
                        else
                            base = 100;

                        symbole = fgetc(fichier);

                        if(symbole == '-'){
                            fscanf(fichier, "%d", &longueurTrainee);
                            score += (multiplicateur * (base + 2 * longueurTrainee) );

                            combo += 2;
                        }
                        else{
                            score += (multiplicateur * base);

                            if(type == '2' || type == '4' || type == '6' || type == '8')
                                combo++;
                            else
                                combo += 2;
                        }

                        multiplicateur = combo / 6 + 1;
                        if(multiplicateur > 3)
                            multiplicateur = 3;
                    }

                    fclose(fichier);

                    bool custom = false;

                    if(dir->d_name[0] == 'E'){
                        if(dir->d_name[3] == '1')
                            difficulte = 5;
                        else
                            difficulte = dir->d_name[3] - '0';
                        strcpy(nomFichierMusique, dir->d_name + 5);
                        positionDernierCaractereExtension = strlen(nomFichierMusique) - 7;
                        if(customDifficultyEP == dir->d_name[3] - '0')
                            custom = true;
                    }
                    else{
                        difficulte = dir->d_name[6] - '0' - 1;
                        strcpy(nomFichierMusique, dir->d_name + 8);
                        positionDernierCaractereExtension = strlen(nomFichierMusique) - 7;
                        if(customDifficultyTEMPO == difficulte)
                            custom = true;
                    }

                    int caractereFinNomFichierMusique = positionDernierCaractereExtension;
                    while(nomFichierMusique[caractereFinNomFichierMusique] != '.')
                        caractereFinNomFichierMusique--;

                    nomFichierMusique[caractereFinNomFichierMusique] = '\0';

                    //On convertit la chaîne de char en chaîne de wchar_t pour pouvoir comparer avec ligne
                    mbstowcs(wideNomFichierMusique , nomFichierMusique, FILENAME_MAX);

                    if(difficulte != 5){
                        nomFichierScore[3] = difficulte + '0';
                    }
                    else{
                        nomFichierScore[3] = '0';
                    }

                    while(true){
                        sprintf(cheminFichier, "%s/%s", dossierJeu, nomFichierScore);
                        fichierScore = fopen(cheminFichier, "r, ccs=UTF-16LE");
                        while(!feof(fichierScore)){
                            /* Si on a rien lu, on quitte la boucle while.
                            Exemple montrant ce qui pourrait arriver si on ne faisait pas ça : EP_1_02 Jóga.flac.track
                            hs_0.mel : 02 Jóga;89472;6
                            hs_1.mel :
                            Dans cet exemple hs_1.mel ne contient rien donc ligne vaut toujours 02 Jóga donc le score réalisé
                            en mode Relaxing va aussi être écrit dans le fichier
                            2 - Medium alors qu'on a jamais joué ce morceau en Medium !!! */
                            if(fgetws(ligne, FILENAME_MAX, fichierScore) == NULL)
                                break;

                            /* On ne met un '\0' que si c'est un ; sinon on risquerait de récupérer le score d'un autre morceau dont le nom
                            commence de la même manière */
                            if(ligne[caractereFinNomFichierMusique] == L';')
                                ligne[caractereFinNomFichierMusique] = L'\0';

                            //Fonction de compraison des chaînes de caractères de type wchar_t
                            if(wcscmp(wideNomFichierMusique, ligne) == 0){
                                int numDiff = nomFichierScore[3] - '0';
                                //Comme sscanf mais pour wchar_t, il faut toujours mettre un L devant le début des guillemets
                                swscanf(ligne + caractereFinNomFichierMusique + 1, L"%d", &scorePerso);

                                int nbCoeurs;
                                int caractere = caractereFinNomFichierMusique + 2;
                                while(ligne[caractere] != L';')
                                    caractere++;
                                swscanf(ligne + caractere + 1, L"%d", &nbCoeurs);

                                donneeDifficulte[ligneActuelle[numDiff]].pourcentage = scorePerso * 100 / (double)score;
                                sprintf(donneeDifficulte[ligneActuelle[numDiff]].ligne, "%s : %d / %d : %.2f%% (%d)\n", nomFichierMusique, scorePerso, score, donneeDifficulte[ligneActuelle[numDiff]].pourcentage, nbCoeurs);
                                ligneActuelle[numDiff]++;

                                break;
                            }
                        }

                        fclose(fichierScore);

                        if(difficulte == 5 && nomFichierScore[3] == '0'){
                            nomFichierScore[3] = '1';
                            continue;
                        }
                        else if(custom){
                            nomFichierScore[3] = '4';
                            custom = false;
                            continue;
                        }
                        else
                            break;
                    }

                    score = 0;
                    combo = 0;
                    multiplicateur = 1;
                }
            }
        }
        closedir(d);
    }

    fichierDifficulte[0] = fopen("1 - Relaxing.txt", "w");
    fichierDifficulte[1] = fopen("2 - Medium.txt", "w");
    fichierDifficulte[2] = fopen("3 - Intense.txt", "w");
    fichierDifficulte[3] = fopen("4 - Overload.txt", "w");
    fichierDifficulte[4] = fopen("5 - Custom.txt", "w");

    for(int i = 0; i < 5; i++){
        qsort(donneeDifficulte + nbLignes[i], ligneActuelle[i] - nbLignes[i], sizeof(Donnee), ordreDecroissantPourcentage_function);
        for(int j = nbLignes[i]; j < ligneActuelle[i]; j++){
            fprintf(fichierDifficulte[i], "%s", donneeDifficulte[j].ligne);
        }
        fclose(fichierDifficulte[i]);
    }

    free(donneeDifficulte);

    printf("Done!\n");
    if(!cacheTrackData)
        printf("Cache track data is currently disabled!\nSet it to Yes in Melody's Escape or this software won't work properly!\n");

    getchar();

    return 0;
}
