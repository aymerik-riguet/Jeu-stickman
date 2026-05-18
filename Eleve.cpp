#pragma warning(disable : 4996)
#include <cstdlib>
#include <iostream>
#include <string>
#include <windows.h>
#include <cmath>
#include "G2D.h"

using namespace std;

// constantes du jeu
const float GRAVITY = -0.6f;
const float JUMP_FORCE = 14.0f;
const float VITESSE_JOUEUR = 7.0f;
const float LARGEUR_JOUEUR = 60.0f;
const float HAUTEUR_JOUEUR = 80.0f;
const int   VIES_MAX = 3;
const int   TEMPS_RESPAWN = 60;
const int   COOLDOWN_ATTAQUE = 25;
const int   DUREE_ATTAQUE = 10;
const int   DUREE_HITSTUN = 30;
const int   DEGATS_PAR_COUP = 13;
const float PORTEE_ATTAQUE = 45.0f;
const float SPRITE_W = 100.0f;
const float SPRITE_H = 130.0f;
// les etats du jeu
const int ETAT_INTRO = 0;
const int ETAT_MENU = 1;
const int ETAT_OPTIONS = 2;
const int ETAT_REGLES = 3;
const int ETAT_JEU = 4;
const int ETAT_GAMEOVER = 5;
const int ATTENTE_RIEN = 0;
const int ATTENTE_P1_GAUCHE = 1;
const int ATTENTE_P1_DROITE = 2;
const int ATTENTE_P1_SAUT = 3;
const int ATTENTE_P1_FRAPPE = 4;
const int ATTENTE_P2_GAUCHE = 5;
const int ATTENTE_P2_DROITE = 6;
const int ATTENTE_P2_SAUT = 7;
const int ATTENTE_P2_FRAPPE = 8;

// structure pour une plateforme
struct Plateforme
{
    float x, y, largeur, hauteur;
};

// structure pour un joueur
struct Joueur
{
    string nom;
    Color couleur = Color(1.0f, 1.0f, 1.0f);

    Key toucheGauche = Key::Unknown;
    Key toucheDroite = Key::Unknown;
    Key toucheSaut = Key::Unknown;
    Key toucheFrappe = Key::Unknown;
    float x = 0, y = 0;
    float velX = 0, velY = 0;
    float spawnX = 0, spawnY = 0;
    float largeur = LARGEUR_JOUEUR;
    float hauteur = HAUTEUR_JOUEUR;
    bool surSol = false;
    bool regardeDroite = true;
    bool estMort = false;
    bool partieTerminee = false;
    int vies = VIES_MAX;
    int timerRespawn = 0;
    int pourcentDegats = 0;
    int cooldownAttaque = 0;
    int timerAttaque = 0;
    int hitstun = 0;
    int timerAnimation = 0;
    bool spriteAlterne = false;
    int texImmobile = 0;
    int texAttaque = 0;
    int texMarche1 = 0;
    int texMarche2 = 0;
};

struct GameData
{
    int largeurEcran = 1920;
    int hauteurEcran = 1080;

    int textureCiel = 0;
    int textureGagnant = 0;

    Plateforme plateformes[10];
    int nbPlateformes = 0;

    Joueur joueur1;
    Joueur joueur2;

    int etatJeu = ETAT_INTRO;
    int attendTouche = ATTENTE_RIEN;
    int cooldownConfig = 0;

    Key p1Gauche = Key::Q;
    Key p1Droite = Key::D;
    Key p1Saut = Key::Z;
    Key p1Frappe = Key::S;

    Key p2Gauche = Key::LEFT;
    Key p2Droite = Key::RIGHT;
    Key p2Saut = Key::UP;
    Key p2Frappe = Key::DOWN;

    bool prevSourisDown = false;
    bool prevHautPresse = false;
    bool prevBasPresse = false;
    bool prevEntreePresse = false;

    int boutonMenuSelectionne = 0;

    string nomGagnant = "";
    Color couleurGagnant = Color(1.0f, 1.0f, 1.0f);
    int timerGameOver = 0;

    float logoIntroY = 0.0f;

    int vraieLargeurFenetre = 1920;
    int vraieHauteurFenetre = 1080;
};

// ===========================================================================
// FONCTIONS UTILITAIRES
// ===========================================================================

bool sourisSurRect(float mx, float my, float rx, float ry, float rw, float rh)
{
    if (mx >= rx && mx <= rx + rw && my >= ry && my <= ry + rh)
        return true;
    return false;
}

bool collision(float px, float py, float pw, float ph, float plx, float ply, float plw, float plh)
{
    if (px + pw > plx && px < plx + plw && py + ph > ply && py < ply + plh)
        return true;
    return false;
}

string nomTouche(Key k)
{
    if (k == Key::LEFT)    return "GAUCHE";
    if (k == Key::RIGHT)   return "DROITE";
    if (k == Key::UP)      return "HAUT";
    if (k == Key::DOWN)    return "BAS";
    if (k == Key::Unknown) return "SPACE";
    if (k == Key::Z) return "Z";
    if (k == Key::Q) return "Q";
    if (k == Key::S) return "S";
    if (k == Key::D) return "D";
    if (k == Key::A) return "A";
    if (k == Key::E) return "E";
    if (k == Key::R) return "R";
    if (k == Key::T) return "T";
    if (k == Key::Y) return "Y";
    if (k == Key::U) return "U";
    if (k == Key::I) return "I";
    if (k == Key::O) return "O";
    if (k == Key::P) return "P";
    if (k == Key::F) return "F";
    if (k == Key::G) return "G";
    if (k == Key::H) return "H";
    if (k == Key::J) return "J";
    if (k == Key::K) return "K";
    if (k == Key::L) return "L";
    if (k == Key::W) return "W";
    if (k == Key::X) return "X";
    if (k == Key::C) return "C";
    if (k == Key::V) return "V";
    if (k == Key::B) return "B";
    if (k == Key::N) return "N";
    if (k == Key::M) return "M";
    return "???";
}

void resetPosition(Joueur& j)
{
    j.x = j.spawnX;
    j.y = j.spawnY;
    j.velX = 0.0f;
    j.velY = 0.0f;
    j.estMort = false;
    j.timerRespawn = 0;
    j.pourcentDegats = 0;
    j.regardeDroite = true;
    j.cooldownAttaque = 0;
    j.timerAttaque = 0;
    j.hitstun = 0;
    j.timerAnimation = 0;
    j.spriteAlterne = false;
}

void resetJoueurComplet(Joueur& j)
{
    j.vies = VIES_MAX;
    j.partieTerminee = false;
    resetPosition(j);
}

Joueur creerJoueur(string nom, float sx, float sy, Key g, Key dr, Key sa, Key fr, Color coul)
{
    Joueur j;
    j.nom = nom;
    j.couleur = coul;
    j.toucheGauche = g;
    j.toucheDroite = dr;
    j.toucheSaut = sa;
    j.toucheFrappe = fr;
    j.spawnX = sx;
    j.spawnY = sy;
    j.largeur = LARGEUR_JOUEUR;
    j.hauteur = HAUTEUR_JOUEUR;
    j.surSol = false;
    j.regardeDroite = true;
    j.estMort = false;
    j.partieTerminee = false;
    j.vies = VIES_MAX;
    j.timerRespawn = 0;
    j.pourcentDegats = 0;
    j.cooldownAttaque = 0;
    j.timerAttaque = 0;
    j.hitstun = 0;
    j.x = sx;
    j.y = sy;
    j.velX = 0.0f;
    j.velY = 0.0f;
    j.timerAnimation = 0;
    j.spriteAlterne = false;
    j.texImmobile = 0;
    j.texAttaque = 0;
    j.texMarche1 = 0;
    j.texMarche2 = 0;
    return j;
}

void initJeu(GameData& G)
{
    G.nbPlateformes = 3;

    G.plateformes[0].x = 200.0f;
    G.plateformes[0].y = 300.0f;
    G.plateformes[0].largeur = G.largeurEcran - 400.0f;
    G.plateformes[0].hauteur = 30.0f;

    G.plateformes[1].x = 300.0f;
    G.plateformes[1].y = 450.0f;
    G.plateformes[1].largeur = 200.0f;
    G.plateformes[1].hauteur = 30.0f;

    G.plateformes[2].x = G.largeurEcran - 500.0f;
    G.plateformes[2].y = 450.0f;
    G.plateformes[2].largeur = 200.0f;
    G.plateformes[2].hauteur = 30.0f;

    G.joueur1 = creerJoueur("JOUEUR 1", 300.0f, 350.0f, G.p1Gauche, G.p1Droite, G.p1Saut, G.p1Frappe, Color::Blue);
    G.joueur2 = creerJoueur("JOUEUR 2", G.largeurEcran - 340.0f, 350.0f, G.p2Gauche, G.p2Droite, G.p2Saut, G.p2Frappe, Color::Red);

    // on reassigne les textures apres avoir recree les joueurs
    G.joueur1.texImmobile = G2D::ExtractTextureFromPNG(".//assets//position2b.png",255,255,255);
    G.joueur1.texAttaque = G2D::ExtractTextureFromPNG(".//assets//position3b.png",255,255,255);
    G.joueur1.texMarche1 = G2D::ExtractTextureFromPNG(".//assets//position1b.png",255,255,255);
    G.joueur1.texMarche2 = G2D::ExtractTextureFromPNG(".//assets//position4b.png",255,255,255);

    G.joueur2.texImmobile = G2D::ExtractTextureFromPNG(".//assets//position2r.png");
    G.joueur2.texAttaque = G2D::ExtractTextureFromPNG(".//assets//position3r.png");
    G.joueur2.texMarche1 = G2D::ExtractTextureFromPNG(".//assets//position1r.png");
    G.joueur2.texMarche2 = G2D::ExtractTextureFromPNG(".//assets//position4r.png");
}

void chargerTextures(GameData& G)
{
    // fond
    G.textureCiel = G2D::ExtractTextureFromPNG(".//assets//sky.png");

    // joueur 1
    G.joueur1.texImmobile = G2D::ExtractTextureFromPNG(".//assets//position2b.png",255,255,255);
    G.joueur1.texAttaque = G2D::ExtractTextureFromPNG(".//assets//position3b.png",255,255,255);
    G.joueur1.texMarche1 = G2D::ExtractTextureFromPNG(".//assets//position1b.png",255,255,255);
    G.joueur1.texMarche2 = G2D::ExtractTextureFromPNG(".//assets//position4b.png",255,255,255);

    // joueur 2
    G.joueur2.texImmobile = G2D::ExtractTextureFromPNG(".//assets//position2r.png");
    G.joueur2.texAttaque = G2D::ExtractTextureFromPNG(".//assets//position3r.png");
    G.joueur2.texMarche1 = G2D::ExtractTextureFromPNG(".//assets//position1r.png");
    G.joueur2.texMarche2 = G2D::ExtractTextureFromPNG(".//assets//position4r.png");
}
void getSourisCorrigee(const GameData& G, float& mx, float& my)
{
    int imx, imy;
    G2D::getMousePos(imx, imy);

    float ratioX = (float)G.largeurEcran / (float)G.vraieLargeurFenetre;
    float ratioY = (float)G.hauteurEcran / (float)G.vraieHauteurFenetre;

    mx = (float)imx * ratioX;
    my = (float)imy * ratioY;
}

// ===========================================================================
// FONCTIONS DE DESSIN COMMUNES
// ===========================================================================

void dessinerFond(const GameData& G)
{
    V2 pos(0.0f, (float)G.hauteurEcran);
    V2 taille((float)G.largeurEcran, (float)-G.hauteurEcran);
    G2D::drawRectWithTexture(G.textureCiel, pos, taille);
}

void dessinerTexteCenter(float centreX, float y, const string& texte, int taille, int epaisseur, Color couleur)
{
    float largTexte = texte.size() * taille * epaisseur * 0.6f;
    float debutX = centreX - largTexte / 2.0f;
    G2D::drawStringFontMono(V2(debutX, y), texte, (float)taille, (float)epaisseur, couleur);
}

void dessinerPanneau(float x, float y, float w, float h)
{
    G2D::drawRectangle(V2(x + 8.0f, y - 8.0f), V2(w, h), Color::Black, true);
    G2D::drawRectangle(V2(x, y), V2(w, h), Color::Black, true);
    G2D::drawRectangle(V2(x, y), V2(w, h), Color::White, false);
    G2D::drawRectangle(V2(x + 4.0f, y + 4.0f), V2(w - 8.0f, h - 8.0f), Color::Gray, false);
}

void dessinerBouton(float x, float y, float w, float h, const string& label, int taille, int epaisseur, bool selectionne, bool survol)
{
    Color couleurFond = Color::Gray;
    Color couleurTexte = Color::White;

    if (selectionne || survol)
    {
        couleurFond = Color::Yellow;
        couleurTexte = Color::Black;
    }

    G2D::drawRectangle(V2(x + 6.0f, y - 6.0f), V2(w, h), Color::Black, true);
    G2D::drawRectangle(V2(x, y), V2(w, h), couleurFond, true);
    G2D::drawRectangle(V2(x, y), V2(w, h), Color::White, false);

    dessinerTexteCenter(x + w / 2.0f, y + h / 2.0f - taille, label, taille, epaisseur, couleurTexte);
}

// ===========================================================================
// LOGIQUE INTRO
// ===========================================================================

void logiqueIntro(GameData& G)
{
    float cibleY = G.hauteurEcran / 2.0f + 100.0f;
    G.logoIntroY += (cibleY - G.logoIntroY) * 0.05f;

    bool entreePresse = G2D::isKeyPressed(Key::ENTER);
    bool sourisClique = G2D::isMouseLeftButtonPressed() && !G.prevSourisDown;

    if ((entreePresse && !G.prevEntreePresse) || sourisClique)
    {
        G.etatJeu = ETAT_MENU;
    }

    G.prevEntreePresse = entreePresse;
    G.prevSourisDown = G2D::isMouseLeftButtonPressed();
}

// ===========================================================================
// LOGIQUE MENU
// ===========================================================================

void logiqueMenu(GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    bool sourisDown = G2D::isMouseLeftButtonPressed();
    bool sourisClique = sourisDown && !G.prevSourisDown;

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float btnW = 500.0f;
    float btnH = 90.0f;
    float btnX = cx - btnW / 2.0f;

    float yJouer = cy + 150.0f;
    float yOptions = cy + 30.0f;
    float yRegles = cy - 90.0f;
    float yQuitter = cy - 210.0f;

    bool surJouer = sourisSurRect(mx, my, btnX, yJouer, btnW, btnH);
    bool surOptions = sourisSurRect(mx, my, btnX, yOptions, btnW, btnH);
    bool surRegles = sourisSurRect(mx, my, btnX, yRegles, btnW, btnH);
    bool surQuitter = sourisSurRect(mx, my, btnX, yQuitter, btnW, btnH);

    if (surJouer)   G.boutonMenuSelectionne = 0;
    if (surOptions) G.boutonMenuSelectionne = 1;
    if (surRegles)  G.boutonMenuSelectionne = 2;
    if (surQuitter) G.boutonMenuSelectionne = 3;

    bool hautPresse = G2D::isKeyPressed(Key::UP);
    bool basPresse = G2D::isKeyPressed(Key::DOWN);
    bool spacePresse = G2D::isKeyPressed(Key::Unknown);

    if (hautPresse && !G.prevHautPresse)
    {
        G.boutonMenuSelectionne = G.boutonMenuSelectionne - 1;
        if (G.boutonMenuSelectionne < 0) G.boutonMenuSelectionne = 3;
    }
    if (basPresse && !G.prevBasPresse)
    {
        G.boutonMenuSelectionne = G.boutonMenuSelectionne + 1;
        if (G.boutonMenuSelectionne > 3) G.boutonMenuSelectionne = 0;
    }

    bool confirmerJouer = (sourisClique && surJouer) || (spacePresse && !G.prevEntreePresse && G.boutonMenuSelectionne == 0);
    bool confirmerOptions = (sourisClique && surOptions) || (spacePresse && !G.prevEntreePresse && G.boutonMenuSelectionne == 1);
    bool confirmerRegles = (sourisClique && surRegles) || (spacePresse && !G.prevEntreePresse && G.boutonMenuSelectionne == 2);
    bool confirmerQuitter = (sourisClique && surQuitter) || (spacePresse && !G.prevEntreePresse && G.boutonMenuSelectionne == 3);

    if (confirmerJouer)
    {
        initJeu(G);
        G.etatJeu = ETAT_JEU;
    }
    if (confirmerOptions)
    {
        G.attendTouche = ATTENTE_RIEN;
        G.etatJeu = ETAT_OPTIONS;
    }
    if (confirmerRegles)
    {
        G.etatJeu = ETAT_REGLES;
    }
    if (confirmerQuitter)
    {
        exit(0);
    }

    G.prevHautPresse = hautPresse;
    G.prevBasPresse = basPresse;
    G.prevEntreePresse = spacePresse;
    G.prevSourisDown = sourisDown;
}

// ===========================================================================
// LOGIQUE REGLES
// ===========================================================================

void logiqueRegles(GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    bool sourisDown = G2D::isMouseLeftButtonPressed();
    bool sourisClique = sourisDown && !G.prevSourisDown;

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float panH = 750.0f;
    float panY = cy - panH / 2.0f + 50.0f;
    float btnW = 300.0f;
    float btnH = 70.0f;
    float retX = cx - btnW / 2.0f;
    float retY = panY + 25.0f;

    if (sourisClique && sourisSurRect(mx, my, retX, retY, btnW, btnH))
    {
        G.etatJeu = ETAT_MENU;
    }

    G.prevSourisDown = sourisDown;
}

// ===========================================================================
// LOGIQUE OPTIONS
// ===========================================================================

void logiqueOptions(GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    bool sourisDown = G2D::isMouseLeftButtonPressed();
    bool sourisClique = sourisDown && !G.prevSourisDown;

    if (G.attendTouche != ATTENTE_RIEN)
    {
        if (G.cooldownConfig > 0)
        {
            G.cooldownConfig--;
            G.prevSourisDown = sourisDown;
            return;
        }

        Key toutesLesTouches[] = {
            Key::LEFT, Key::RIGHT, Key::UP, Key::DOWN,
            Key::Z, Key::Q, Key::S, Key::D, Key::A, Key::E, Key::R, Key::T,
            Key::Y, Key::U, Key::I, Key::O, Key::P, Key::F, Key::G, Key::H,
            Key::J, Key::K, Key::L, Key::W, Key::X, Key::C, Key::V, Key::B,
            Key::N, Key::M, Key::Unknown
        };

        for (int i = 0; i < 31; i++)
        {
            if (!G2D::isKeyPressed(toutesLesTouches[i])) continue;

            Key k = toutesLesTouches[i];

            if (G.attendTouche == ATTENTE_P1_GAUCHE) G.p1Gauche = k;
            if (G.attendTouche == ATTENTE_P1_DROITE) G.p1Droite = k;
            if (G.attendTouche == ATTENTE_P1_SAUT)   G.p1Saut = k;
            if (G.attendTouche == ATTENTE_P1_FRAPPE) G.p1Frappe = k;
            if (G.attendTouche == ATTENTE_P2_GAUCHE) G.p2Gauche = k;
            if (G.attendTouche == ATTENTE_P2_DROITE) G.p2Droite = k;
            if (G.attendTouche == ATTENTE_P2_SAUT)   G.p2Saut = k;
            if (G.attendTouche == ATTENTE_P2_FRAPPE) G.p2Frappe = k;

            G.attendTouche = ATTENTE_RIEN;
            G.prevSourisDown = sourisDown;
            return;
        }

        G.prevSourisDown = sourisDown;
        return;
    }

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float panW = 1200.0f;
    float panH = 850.0f;
    float panX = cx - panW / 2.0f;
    float panY = cy - panH / 2.0f;

    float rowH = 75.0f;
    float startY = panY + panH - 90.0f;

    int attentes[8] = {
        ATTENTE_P1_GAUCHE, ATTENTE_P1_DROITE, ATTENTE_P1_SAUT, ATTENTE_P1_FRAPPE,
        ATTENTE_P2_GAUCHE, ATTENTE_P2_DROITE, ATTENTE_P2_SAUT, ATTENTE_P2_FRAPPE
    };

    for (int i = 0; i < 8; i++)
    {
        float rowY = startY - 40.0f - (i + 1) * rowH;
        if (sourisClique && sourisSurRect(mx, my, panX + 10.0f, rowY, panW - 20.0f, rowH))
        {
            G.attendTouche = attentes[i];
            G.cooldownConfig = 20;
            G.prevSourisDown = sourisDown;
            return;
        }
    }

    float retBtnW = 300.0f;
    float retBtnH = 70.0f;
    float retX = cx - retBtnW / 2.0f;
    float retY = panY + 25.0f;

    if (sourisClique && sourisSurRect(mx, my, retX, retY, retBtnW, retBtnH))
    {
        G.etatJeu = ETAT_MENU;
    }

    G.prevSourisDown = sourisDown;
}

// ===========================================================================
// LOGIQUE JEU
// ===========================================================================

void gererMouvements(Joueur& j)
{
    if (G2D::isKeyPressed(j.toucheGauche))
    {
        j.velX = -VITESSE_JOUEUR;
        j.regardeDroite = false;
    }
    else if (G2D::isKeyPressed(j.toucheDroite))
    {
        j.velX = VITESSE_JOUEUR;
        j.regardeDroite = true;
    }
    else
    {
        if (j.surSol)
            j.velX = j.velX * 0.7f;
        else
            j.velX = j.velX * 0.95f;
    }

    if (G2D::isKeyPressed(j.toucheSaut) && j.surSol)
    {
        j.velY = JUMP_FORCE;
        j.surSol = false;
    }
}

void mettreAJourAnimation(Joueur& j)
{
    bool estEnDeplacement = (j.velX > 1.0f || j.velX < -1.0f);

    if (estEnDeplacement)
    {
        j.timerAnimation++;
        if (j.timerAnimation >= 60)
        {
            j.timerAnimation = 0;
            j.spriteAlterne = !j.spriteAlterne;
        }
    }
    else
    {
        j.timerAnimation = 0;
        j.spriteAlterne = false;
    }
}

void gererHitstun(Joueur& j)
{
    if (j.surSol)
        j.velX = j.velX * 0.8f;
    else
        j.velX = j.velX * 0.98f;

    j.hitstun--;
}

void collisionsHorizontales(Joueur& j, GameData& G)
{
    float prochainX = j.x + j.velX;

    for (int i = 0; i < G.nbPlateformes; i++)
    {
        Plateforme& pl = G.plateformes[i];
        if (collision(prochainX, j.y, j.largeur, j.hauteur, pl.x, pl.y, pl.largeur, pl.hauteur))
        {
            prochainX = j.x;
            j.velX = 0.0f;
        }
    }
    j.x = prochainX;
}

void collisionsVerticales(Joueur& j, GameData& G)
{
    float prochainY = j.y + j.velY;
    j.surSol = false;

    for (int i = 0; i < G.nbPlateformes; i++)
    {
        Plateforme& pl = G.plateformes[i];
        if (!collision(j.x, prochainY, j.largeur, j.hauteur, pl.x, pl.y, pl.largeur, pl.hauteur)) continue;

        if (j.velY < 0.0f)
        {
            prochainY = pl.y + pl.hauteur;
            j.velY = 0.0f;
            j.surSol = true;
        }
        else
        {
            prochainY = pl.y - j.hauteur;
            j.velY = 0.0f;
        }
    }
    j.y = prochainY;
}

void attaquer(Joueur& attaquant, Joueur& cible)
{
    if (cible.estMort || cible.partieTerminee || cible.hitstun > 0) return;

    float atkX = attaquant.x + attaquant.largeur;
    if (!attaquant.regardeDroite)
        atkX = attaquant.x - PORTEE_ATTAQUE;

    float atkY = attaquant.y + 10.0f;
    float atkW = PORTEE_ATTAQUE;
    float atkH = attaquant.hauteur - 20.0f;

    if (collision(cible.x, cible.y, cible.largeur, cible.hauteur, atkX, atkY, atkW, atkH))
    {
        cible.pourcentDegats += DEGATS_PAR_COUP;
        cible.hitstun = DUREE_HITSTUN;

        int   nbCoups = cible.pourcentDegats / DEGATS_PAR_COUP;
        float forceX = 7.0f + nbCoups * 2.0f;
        float forceY = 6.0f + nbCoups * 1.5f;
        float direction = 1.0f;
        if (!attaquant.regardeDroite) direction = -1.0f;

        cible.velX = direction * forceX;
        cible.velY = forceY;
        cible.surSol = false;
    }
}

bool horsLimites(Joueur& j, GameData& G)
{
    if (j.x + j.largeur < 0) return true;
    if (j.x > G.largeurEcran) return true;
    if (j.y + j.hauteur < 0) return true;
    if (j.y > G.hauteurEcran) return true;
    return false;
}

void perdrUneVie(Joueur& j, Joueur& adversaire, GameData& G)
{
    j.vies--;

    if (j.vies <= 0)
    {
        j.partieTerminee = true;
        G.timerGameOver = 0;
        G.nomGagnant = adversaire.nom;
        G.couleurGagnant = adversaire.couleur;
    }
    else
    {
        j.estMort = true;
        j.timerRespawn = TEMPS_RESPAWN;
    }
}

void logiqueJeu(GameData& G)
{
    if (G2D::isOnPause()) return;

    if (G.joueur1.partieTerminee || G.joueur2.partieTerminee)
    {
        G.timerGameOver++;
        bool spacePresse = G2D::isKeyPressed(Key::Unknown);
        bool sourisClique = G2D::isMouseLeftButtonPressed() && !G.prevSourisDown;

        if (spacePresse || sourisClique)
        {
            G.etatJeu = ETAT_GAMEOVER;
        }

        G.prevSourisDown = G2D::isMouseLeftButtonPressed();
        return;
    }

    // mise a jour joueur 1
    if (!G.joueur1.partieTerminee)
    {
        if (G.joueur1.estMort)
        {
            G.joueur1.timerRespawn--;
            if (G.joueur1.timerRespawn <= 0)
                resetPosition(G.joueur1);
        }
        else
        {
            if (G.joueur1.cooldownAttaque > 0) G.joueur1.cooldownAttaque--;
            if (G.joueur1.timerAttaque > 0)    G.joueur1.timerAttaque--;

            if (G.joueur1.hitstun > 0)
                gererHitstun(G.joueur1);
            else
                gererMouvements(G.joueur1);

            G.joueur1.velY += GRAVITY;

            collisionsHorizontales(G.joueur1, G);
            collisionsVerticales(G.joueur1, G);

            mettreAJourAnimation(G.joueur1);

            if (G2D::isKeyPressed(G.joueur1.toucheFrappe) && G.joueur1.cooldownAttaque <= 0 && G.joueur1.hitstun <= 0)
            {
                G.joueur1.cooldownAttaque = COOLDOWN_ATTAQUE;
                G.joueur1.timerAttaque = DUREE_ATTAQUE;
                attaquer(G.joueur1, G.joueur2);
            }

            if (horsLimites(G.joueur1, G))
                perdrUneVie(G.joueur1, G.joueur2, G);
        }
    }

    // mise a jour joueur 2
    if (!G.joueur2.partieTerminee)
    {
        if (G.joueur2.estMort)
        {
            G.joueur2.timerRespawn--;
            if (G.joueur2.timerRespawn <= 0)
                resetPosition(G.joueur2);
        }
        else
        {
            if (G.joueur2.cooldownAttaque > 0) G.joueur2.cooldownAttaque--;
            if (G.joueur2.timerAttaque > 0)    G.joueur2.timerAttaque--;

            if (G.joueur2.hitstun > 0)
                gererHitstun(G.joueur2);
            else
                gererMouvements(G.joueur2);

            G.joueur2.velY += GRAVITY;

            collisionsHorizontales(G.joueur2, G);
            collisionsVerticales(G.joueur2, G);

            mettreAJourAnimation(G.joueur2);

            if (G2D::isKeyPressed(G.joueur2.toucheFrappe) && G.joueur2.cooldownAttaque <= 0 && G.joueur2.hitstun <= 0)
            {
                G.joueur2.cooldownAttaque = COOLDOWN_ATTAQUE;
                G.joueur2.timerAttaque = DUREE_ATTAQUE;
                attaquer(G.joueur2, G.joueur1);
            }

            if (horsLimites(G.joueur2, G))
                perdrUneVie(G.joueur2, G.joueur1, G);
        }
    }

    G.prevSourisDown = G2D::isMouseLeftButtonPressed();
}

// ===========================================================================
// LOGIQUE GAME OVER
// ===========================================================================

void logiqueGameOver(GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    bool sourisDown = G2D::isMouseLeftButtonPressed();
    bool sourisClique = sourisDown && !G.prevSourisDown;

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float btnW = 400.0f;
    float btnH = 80.0f;
    float btnX = cx - btnW / 2.0f;
    float btnY = cy - 250.0f;

    bool spacePresse = G2D::isKeyPressed(Key::Unknown);
    bool clicMenu = sourisClique && sourisSurRect(mx, my, btnX, btnY, btnW, btnH);

    if (clicMenu || (spacePresse && !G.prevEntreePresse))
    {
        G.etatJeu = ETAT_MENU;
        G.boutonMenuSelectionne = 0;
    }

    G.prevSourisDown = sourisDown;
    G.prevEntreePresse = spacePresse;
}

// ===========================================================================
// LOGIQUE DISPATCH
// ===========================================================================

void Logic(GameData& G)
{
    if (G.etatJeu == ETAT_INTRO)    logiqueIntro(G);
    if (G.etatJeu == ETAT_MENU)     logiqueMenu(G);
    if (G.etatJeu == ETAT_REGLES)   logiqueRegles(G);
    if (G.etatJeu == ETAT_OPTIONS)  logiqueOptions(G);
    if (G.etatJeu == ETAT_JEU)      logiqueJeu(G);
    if (G.etatJeu == ETAT_GAMEOVER) logiqueGameOver(G);
}

// ===========================================================================
// RENDU INTRO
// ===========================================================================

void renderIntro(const GameData& G)
{
    G2D::clearScreen(Color::Black);

    float cx = G.largeurEcran / 2.0f;
    G2D::drawStringFontMono(V2(350, G.logoIntroY), "STICKMAN FIGHT", 120, 4, Color::Yellow);

    float cibleY = G.hauteurEcran / 2.0f + 100.0f;
    bool logoArrive = abs(G.logoIntroY - cibleY) < 5.0f;

    if (logoArrive)
    {
        static int timer = 0;
        timer++;
        bool visible = (timer / 30) % 2 == 0;
        if (visible)
        {
            dessinerTexteCenter(cx + 100, G.hauteurEcran / 2.0f - 100.0f, "> Appuyez sur ENTRER <", 16, 2, Color::White);
        }
    }
}

// ===========================================================================
// RENDU MENU
// ===========================================================================

void renderMenu(const GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    G2D::clearScreen(Color::Black);
    dessinerFond(G);

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float panW = 900.0f;
    float panH = 750.0f;
    float panX = cx - panW / 2.0f;
    float panY = cy - panH / 2.0f + 50.0f;

    dessinerPanneau(panX, panY, panW, panH);

    G2D::drawStringFontMono(V2(600, panY + panH - 100.0f), "STICKMAN FIGHT", 72, 6, Color::Yellow);
    G2D::drawRectangle(V2(panX + 30.0f, panY + panH - 150.0f), V2(panW - 60.0f, 3.0f), Color::Yellow, true);

    float btnW = 500.0f;
    float btnH = 90.0f;
    float btnX = cx - btnW / 2.0f;

    float yJouer = cy + 150.0f;
    float yOptions = cy + 30.0f;
    float yRegles = cy - 90.0f;
    float yQuitter = cy - 210.0f;

    bool surJouer = sourisSurRect(mx, my, btnX, yJouer, btnW, btnH);
    bool surOptions = sourisSurRect(mx, my, btnX, yOptions, btnW, btnH);
    bool surRegles = sourisSurRect(mx, my, btnX, yRegles, btnW, btnH);
    bool surQuitter = sourisSurRect(mx, my, btnX, yQuitter, btnW, btnH);

    dessinerBouton(btnX, yJouer, btnW, btnH, "JOUER", 16, 2, G.boutonMenuSelectionne == 0, surJouer);
    dessinerBouton(btnX, yOptions, btnW, btnH, "OPTIONS", 16, 2, G.boutonMenuSelectionne == 1, surOptions);
    dessinerBouton(btnX, yRegles, btnW, btnH, "REGLES", 16, 2, G.boutonMenuSelectionne == 2, surRegles);
    dessinerBouton(btnX, yQuitter, btnW, btnH, "QUITTER", 16, 2, G.boutonMenuSelectionne == 3, surQuitter);

    G2D::drawRectangle(V2(panX + 30.0f, panY + 20.0f), V2(panW - 60.0f, 2.0f), Color::Gray, true);
    dessinerTexteCenter(cx, panY + 30.0f, "Cliquez ou utilisez HAUT/BAS + SPACE", 8, 2, Color::Gray);
}

// ===========================================================================
// RENDU REGLES
// ===========================================================================

void renderRegles(const GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    G2D::clearScreen(Color::Black);
    dessinerFond(G);

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float panW = 900.0f;
    float panH = 750.0f;
    float panX = cx - panW / 2.0f;
    float panY = cy - panH / 2.0f + 50.0f;

    dessinerPanneau(panX, panY, panW, panH);

    dessinerTexteCenter(cx, panY + panH - 80.0f, "REGLES DU JEU", 24, 3, Color::Yellow);
    G2D::drawRectangle(V2(panX + 30.0f, panY + panH - 120.0f), V2(panW - 60.0f, 3.0f), Color::Yellow, true);

    float espacement = 50.0f;
    float debutY = cy + 100.0f;

    G2D::drawStringFontMono(V2(cx - 380.0f, debutY), "> Frappez l'ennemi pour faire monter ses degats (%)", 14, 2, Color::White);
    G2D::drawStringFontMono(V2(cx - 380.0f, debutY - espacement), "> Poussez votre adversaire hors de l'ecran", 14, 2, Color::White);
    G2D::drawStringFontMono(V2(cx - 380.0f, debutY - espacement * 2), "> Chaque joueur a 3 vies", 14, 2, Color::White);
    G2D::drawStringFontMono(V2(cx - 380.0f, debutY - espacement * 3), "> Plus les degats sont hauts, plus vous decollez !", 14, 2, Color::White);
    G2D::drawStringFontMono(V2(cx - 380.0f, debutY - espacement * 4), "> Le dernier survivant remporte la partie", 14, 2, Color::White);

    float retBtnW = 300.0f;
    float retBtnH = 70.0f;
    float retX = cx - retBtnW / 2.0f;
    float retY = panY + 25.0f;
    bool  retHov = sourisSurRect(mx, my, retX, retY, retBtnW, retBtnH);

    dessinerBouton(retX, retY, retBtnW, retBtnH, "< RETOUR", 16, 2, false, retHov);
}

// ===========================================================================
// RENDU OPTIONS
// ===========================================================================

void renderOptions(const GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    G2D::clearScreen(Color::Black);
    dessinerFond(G);

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float panW = 1200.0f;
    float panH = 850.0f;
    float panX = cx - panW / 2.0f;
    float panY = cy - panH / 2.0f;

    dessinerPanneau(panX, panY, panW, panH);

    dessinerTexteCenter(cx, panY + panH - 65.0f, "CONFIG TOUCHES", 16, 2, Color::Yellow);
    G2D::drawRectangle(V2(panX + 20.0f, panY + panH - 85.0f), V2(panW - 40.0f, 2.0f), Color::Yellow, true);

    float colLabelX = panX + 150.0f;
    float colKeyX = cx + 80.0f;
    float keyBtnW = 320.0f;
    float keyBtnH = 50.0f;
    float rowH = 75.0f;
    float startY = panY + panH - 90.0f;

    dessinerTexteCenter(colLabelX, startY - 10.0f, "ACTION", 10, 2, Color::Gray);
    dessinerTexteCenter(colKeyX + keyBtnW / 2.0f, startY - 10.0f, "TOUCHE ASSIGNEE", 10, 2, Color::Gray);
    G2D::drawRectangle(V2(panX + 20.0f, startY - 22.0f), V2(panW - 40.0f, 2.0f), Color::Gray, true);

    string labelsActions[8] = {
        "JOUEUR 1  >  GAUCHE", "JOUEUR 1  >  DROITE",
        "JOUEUR 1  >  SAUT",   "JOUEUR 1  >  FRAPPE",
        "JOUEUR 2  >  GAUCHE", "JOUEUR 2  >  DROITE",
        "JOUEUR 2  >  SAUT",   "JOUEUR 2  >  FRAPPE"
    };

    Key touchesActuelles[8] = {
        G.p1Gauche, G.p1Droite, G.p1Saut, G.p1Frappe,
        G.p2Gauche, G.p2Droite, G.p2Saut, G.p2Frappe
    };

    Color couleurs[8] = {
        Color::Blue, Color::Blue, Color::Blue, Color::Blue,
        Color::Red,  Color::Red,  Color::Red,  Color::Red
    };

    int attentes[8] = {
        ATTENTE_P1_GAUCHE, ATTENTE_P1_DROITE, ATTENTE_P1_SAUT, ATTENTE_P1_FRAPPE,
        ATTENTE_P2_GAUCHE, ATTENTE_P2_DROITE, ATTENTE_P2_SAUT, ATTENTE_P2_FRAPPE
    };

    for (int i = 0; i < 8; i++)
    {
        float rowY = startY - 40.0f - (i + 1) * rowH;
        float btnY = rowY + (rowH - keyBtnH) / 2.0f;
        bool enAttente = (G.attendTouche == attentes[i]);
        bool survol = sourisSurRect(mx, my, panX + 10.0f, rowY, panW - 20.0f, rowH);

        if (survol && G.attendTouche == ATTENTE_RIEN)
        {
            G2D::drawRectangle(V2(panX + 10.0f, rowY), V2(panW - 20.0f, rowH - 4.0f), Color::Gray, true);
        }

        dessinerTexteCenter(colLabelX, rowY + rowH / 2.0f - 12.0f, labelsActions[i], 10, 2, couleurs[i]);

        string labelTouche = "[ " + nomTouche(touchesActuelles[i]) + " ]";
        Color couleurBouton = Color::Gray;
        Color couleurTexte = Color::White;

        if (enAttente)
        {
            labelTouche = "< APPUYEZ... >";
            couleurBouton = Color::Yellow;
            couleurTexte = Color::Black;
        }
        else if (survol)
        {
            couleurBouton = Color::Blue;
        }

        G2D::drawRectangle(V2(colKeyX + 4.0f, btnY - 4.0f), V2(keyBtnW, keyBtnH), Color::Black, true);
        G2D::drawRectangle(V2(colKeyX, btnY), V2(keyBtnW, keyBtnH), couleurBouton, true);
        G2D::drawRectangle(V2(colKeyX, btnY), V2(keyBtnW, keyBtnH), Color::White, false);
        dessinerTexteCenter(colKeyX + keyBtnW / 2.0f, btnY + keyBtnH / 2.0f - 10.0f, labelTouche, 10, 2, couleurTexte);
    }

    float retBtnW = 300.0f;
    float retBtnH = 70.0f;
    float retX = cx - retBtnW / 2.0f;
    float retY = panY + 25.0f;
    bool  retHov = sourisSurRect(mx, my, retX, retY, retBtnW, retBtnH);

    dessinerBouton(retX, retY, retBtnW, retBtnH, "< RETOUR", 16, 2, false, retHov);

    dessinerTexteCenter(cx, panY + panH + 15.0f, "Cliquez sur une ligne pour la modifier", 12, 2, Color::Gray);
}

// ===========================================================================
// RENDU JEU
// ===========================================================================

Color couleurDegats(int degats)
{
    if (degats < 50)  return Color::White;
    if (degats < 100) return Color::Yellow;
    return Color::Red;
}

Color couleurBarreVie(float ratio)
{
    if (ratio > 0.7f) return Color::Green;
    if (ratio > 0.4f) return Color::Yellow;
    return Color::Red;
}

void dessinerHUD(const Joueur& j, int index)
{
    float marge = 20.0f;
    float barW = 220.0f;
    float barH = 22.0f;
    float barX = marge + index * (barW + 100.0f);
    float barY = barH + marge + 30.0f;

    G2D::drawRectangle(V2(barX - 2.0f, barY - 2.0f), V2(barW + 4.0f, barH + 4.0f), Color::Black, true);
    G2D::drawRectangle(V2(barX, barY), V2(barW, barH), Color::Gray, true);

    float ratio = (float)j.vies / (float)VIES_MAX;
    if (ratio > 0.0f)
    {
        Color c = couleurBarreVie(ratio);
        G2D::drawRectangle(V2(barX, barY), V2(barW * ratio, barH), c, true);
    }

    G2D::drawRectangle(V2(barX, barY), V2(barW, barH), Color::White, false);

    dessinerTexteCenter(barX + barW / 2.0f, barY + 26.0f, j.nom, 13, 2, j.couleur);
    dessinerTexteCenter(barX + barW / 2.0f, barY + 3.0f, to_string(j.vies) + "/" + to_string(VIES_MAX), 10, 2, Color::Black);

    string txtDegats = to_string(j.pourcentDegats) + " %";
    Color  cDegats = couleurDegats(j.pourcentDegats);
    dessinerTexteCenter(barX + barW / 2.0f + 155, barY - 35.0f, txtDegats, 28, 4, cDegats);
}

void dessinerJoueur(const Joueur& j)
{
    if (j.estMort || j.partieTerminee) return;

    bool flashOn = (j.hitstun > 0) && ((j.hitstun / 4) % 2 == 0);

    if (flashOn)
    {
        G2D::drawRectangle(
            V2(j.x, j.y),
            V2(j.largeur, j.hauteur),
            Color::White,
            true
        );
        return;
    }

    int texADessiner = j.texImmobile;

    if (j.timerAttaque > 0)
    {
        texADessiner = j.texAttaque;
    }
    else if (j.velX > 1.0f || j.velX < -1.0f)
    {
        if (j.spriteAlterne)
            texADessiner = j.texMarche2;
        else
            texADessiner = j.texMarche1;
    }
    float drawX = j.x - (SPRITE_W - j.largeur) / 2.0f;
    float drawY = j.y - (SPRITE_H - j.hauteur) / 2.0f;

    if (j.regardeDroite)
    {
        G2D::drawRectWithTexture(
            texADessiner,
            V2(drawX + SPRITE_W, drawY + SPRITE_H),
            V2(-SPRITE_W, -SPRITE_H)
        );
    }
    else
    {
        G2D::drawRectWithTexture(
            texADessiner,
            V2(drawX, drawY + SPRITE_H),
            V2(SPRITE_W, -SPRITE_H)
        );
    }
}

void renderJeu(const GameData& G)
{
    G2D::clearScreen(Color::Black);
    dessinerFond(G);

    for (int i = 0; i < G.nbPlateformes; i++)
    {
        Plateforme pl = G.plateformes[i];
        G2D::drawRectangle(V2(pl.x, pl.y), V2(pl.largeur, pl.hauteur), Color::White, true);
    }

    dessinerJoueur(G.joueur1);
    dessinerJoueur(G.joueur2);

    dessinerHUD(G.joueur1, 0);
    dessinerHUD(G.joueur2, 1);

    if (G.joueur1.partieTerminee || G.joueur2.partieTerminee)
    {
        float cx = G.largeurEcran / 2.0f;
        float cy = G.hauteurEcran / 2.0f;
        G2D::drawRectangle(V2(cx - 320.0f, cy - 60.0f), V2(640.0f, 120.0f), Color::Black, true);
        G2D::drawRectangle(V2(cx - 320.0f, cy - 60.0f), V2(640.0f, 120.0f), Color::White, false);
        dessinerTexteCenter(cx, cy + 10.0f, "CLIQUEZ OU SPACE", 9, 2, Color::Yellow);
        dessinerTexteCenter(cx, cy - 40.0f, "FIN !", 12, 3, Color::White);
    }
}

// ===========================================================================
// RENDU GAME OVER
// ===========================================================================

void renderGameOver(const GameData& G)
{
    float mx, my;
    getSourisCorrigee(G, mx, my);

    G2D::clearScreen(Color::Black);
    dessinerFond(G);

    float cx = G.largeurEcran / 2.0f;
    float cy = G.hauteurEcran / 2.0f;
    float panW = 800.0f;
    float panH = 700.0f;
    float panX = cx - panW / 2.0f;
    float panY = cy - panH / 2.0f + 20.0f;

    G2D::drawRectangle(V2(panX + 12.0f, panY - 12.0f), V2(panW, panH), Color::Black, true);
    G2D::drawRectangle(V2(panX, panY), V2(panW, panH), Color::Black, true);
    G2D::drawRectangle(V2(panX, panY), V2(panW, panH), G.couleurGagnant, false);
    G2D::drawRectangle(V2(panX + 6.0f, panY + 6.0f), V2(panW - 12.0f, panH - 12.0f), Color::White, false);

    dessinerTexteCenter(cx, panY + panH - 80.0f, "FIN DE PARTIE", 20, 2, Color::Red);
    G2D::drawRectangle(V2(panX + 30.0f, panY + panH - 110.0f), V2(panW - 60.0f, 3.0f), Color::Red, true);

    string txtGagnant = G.nomGagnant + " GAGNE !";
    dessinerTexteCenter(cx, panY + panH - 180.0f, txtGagnant, 18, 2, G.couleurGagnant);

    float imgW = 256.0f;
    float imgH = 256.0f;
    G2D::drawRectWithTexture(G.textureGagnant, V2(cx - imgW / 2.0f, cy + imgH / 2.0f - 30.0f), V2(imgW, -imgH));

    float btnW = 400.0f;
    float btnH = 80.0f;
    float btnX = cx - btnW / 2.0f;
    float btnY = cy - 250.0f;
    bool survol = sourisSurRect(mx, my, btnX, btnY, btnW, btnH);

    dessinerBouton(btnX, btnY, btnW, btnH, "RETOUR AU MENU", 14, 2, false, survol);

    dessinerTexteCenter(cx, panY + 30.0f, "SPACE = Retour Menu", 8, 2, Color::Gray);
}

// ===========================================================================
// RENDER DISPATCH
// ===========================================================================

void Render(const GameData& G)
{
    if (G.etatJeu == ETAT_INTRO)    renderIntro(G);
    if (G.etatJeu == ETAT_MENU)     renderMenu(G);
    if (G.etatJeu == ETAT_REGLES)   renderRegles(G);
    if (G.etatJeu == ETAT_OPTIONS)  renderOptions(G);
    if (G.etatJeu == ETAT_JEU)      renderJeu(G);
    if (G.etatJeu == ETAT_GAMEOVER) renderGameOver(G);

    // curseur avec coordonnees corrigees
    float mx, my;
    getSourisCorrigee(G, mx, my);
    G2D::drawRectangle(V2(mx - 2.0f, my - 2.0f), V2(4.0f, 4.0f), Color::White, true);

    G2D::Show();
}

// ===========================================================================
// MAIN
// ===========================================================================

int main(int argc, char* argv[])
{
    // BUG 1 FIX : on declare le programme DPI-aware AVANT toute creation de fenetre
    // Cela empeche Windows de scaler automatiquement les coordonnees souris
    // et de tronquer la zone cliente sur les ecrans a haute densite ou avec scaling actif
    SetProcessDPIAware();

    GameData G;

    G.logoIntroY = G.hauteurEcran + 200.0f;

    initJeu(G);

    G2D::initWindow(V2((float)G.largeurEcran, (float)G.hauteurEcran), V2(0.0f, 0.0f), "Stickman Fight");

    // passage en plein ecran borderless
    HWND hwnd = FindWindowA(NULL, "Stickman Fight");
    if (hwnd)
    {
        // recupere les dimensions physiques du moniteur principal
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);

        // supprime tous les decorateurs de fenetre (titre, bordures, boutons)
        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX
            | WS_MAXIMIZEBOX | WS_SYSMENU | WS_BORDER);
        SetWindowLongPtr(hwnd, GWL_STYLE, style);

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE
            | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        // redimensionne et repositionne la fenetre en plein ecran
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, screenW, screenH,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOCOPYBITS);

        // force le redessin complet de la fenetre
        UpdateWindow(hwnd);

        // BUG 1 FIX : on attend que Windows ait fini d'ajuster la fenetre
        // avant de lire la vraie taille de la zone cliente
        // (sans ce delai, GetClientRect peut retourner les anciennes dimensions)
        Sleep(50);

        RECT rect;
        GetClientRect(hwnd, &rect);
        G.vraieLargeurFenetre = rect.right - rect.left;
        G.vraieHauteurFenetre = rect.bottom - rect.top;

        // securite : si Windows retourne 0 ou negatif, on garde les valeurs logiques
        if (G.vraieLargeurFenetre <= 0) G.vraieLargeurFenetre = G.largeurEcran;
        if (G.vraieHauteurFenetre <= 0) G.vraieHauteurFenetre = G.hauteurEcran;
    }

    chargerTextures(G);

    G2D::Run(Logic, Render, G, 60, true);
    return 0;
}
