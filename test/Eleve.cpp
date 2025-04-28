#pragma warning( disable : 4996 ) 

#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
using namespace std;

// touche P   : mets en pause
// touche ESC : ferme la fenêtre et quitte le jeu

struct Raquette
{
    V2 pos;
    V2 size;
    int IdSpriteNormal;

    Raquette()
    {
        pos = V2(300, 100);
        size = V2(100, 25);
    }

    void InitTextures()
    {
        IdSpriteNormal = G2D::ExtractTextureFromPNG(".//assets//raquette.png", Transparency::None);
    }
};

struct Balle
{
    V2 size;
    V2 BallPos = V2(300, 150);
    V2 BallMove;
    int BallRadius = 16;
    int IdSprite;
    vector<V2> PreviousPos;

    Balle()
    {
        size = V2(32, 32);
        PreviousPos.resize(50); // stocke les 50 dernières positions connues
        BallMove = V2(8, 12);
    }
    void InitTextures()
    {
        IdSprite = G2D::ExtractTextureFromPNG(".//assets//ball.png", Transparency::None);
    }
};

struct Brique
{
    V2 pos;
    V2 size;
    int IdSprite;
    bool isDestroyed; // Marque si la brique a été détruite
    int life;
    vector<V2>hitbox;
    Brique(V2 pPosition,int pLife)
    {
        pos = pPosition;
        size = V2(100, 30);
        hitbox = {pos,V2(pos.x+size.x,pos.y),V2(pos.x+size.x,pos.y+size.y),V2(pos.x,pos.y+size.y)};
        isDestroyed = false;
        life = pLife;
    }

    void InitTextures()
    {
        IdSprite = G2D::ExtractTextureFromPNG(".//assets//brique.png", Transparency::None);
    }
    void InitTextures(string pString)
    {
        IdSprite = G2D::ExtractTextureFromPNG(pString, Transparency::None);
    }
};

V2 Rebond(V2 V, V2 N)
{
    N.normalize();
    V2 T = V2(N.y, -N.x);  // rotation de 90° du vecteur n sens horaire
    float vt = prodScal(V, T);     // produit scalaire, vt est un nombre
    float vn = prodScal(V, N);     // produit scalaire, vn est un nombre
    V2 R = vt * T - vn * N; // * entre un flottant et un V2
    return R;
};

int CollisionSegCir(V2 A, V2 B, float r, V2 C)
{
    V2 AB = B - A;
    V2 T = AB;
    T.normalize();
    float d = prodScal(T, C - A);
    if (d > 0 && d < AB.norm())
    {
        V2 P = A + d * T; // proj de C sur [AB]
        V2 PC = C - P;
        if (PC.norm() < r) return 2;
        else               return 0;
    }
    if ((C - A).norm() < r) return 1;
    if ((C - B).norm() < r) return 3;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//     Données du jeu - structure instanciée dans le main

struct GameData
{
    int HeighPix = 800;   // hauteur de la fenêtre d'application
    int WidthPix = 600;   // largeur de la fenêtre d'application
    vector<V2> LP{ V2(0, 0), V2(0, 800), V2(600, 800), V2(600, 0) };
    Raquette X;
    Balle aBall;
    vector<Brique> briques; // Liste des briques
    int Idsprite_background;
    int score = 0;
    bool isPaused = false;

    GameData() {}

    void InitTextures()
    {
        Idsprite_background = G2D::ExtractTextureFromPNG(".//assets//background_blue.png", Transparency::None);
    }
};

// Fonction pour vérifier si toutes les briques sont détruites
bool AllBriquesDestroyed(const GameData& G)
{
    for (const Brique& brique : G.briques)
    {
        if (!brique.isDestroyed)
            return false;  // Si une brique n'est pas détruite, retourne false
    }
    return true;  // Si toutes les briques sont détruites, retourne true
}

// Fonction pour réinitialiser les briques et augmenter la vitesse de la balle
void ResetLevel(GameData& G)
{
    // Réinitialiser les briques
    int startX = (G.WidthPix - (5 * 110)) / 2; // Centrer horizontalement
    int startY = 600; // Placer les briques en haut de l'écran

    G.briques.clear(); // Supprimer les briques existantes

    // Ajouter de nouvelles briques et leur initialiser les textures
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            Brique newBrique(V2(startX + i * 110, startY + j * 40),2);
            newBrique.InitTextures();  // Initialiser le sprite de la brique
            G.briques.push_back(newBrique);
        }
    }

    // Augmenter légèrement la vitesse de la balle
    G.aBall.BallMove = G.aBall.BallMove * 1.1f; // Augmenter la vitesse de 10%
}


///////////////////////////////////////////////////////////////////////////////
//     fonction de rendu - reçoit en paramètre les données du jeu par référence

void Render(const GameData& G)
{
    G2D::clearScreen(Color::Black);

    if (G.isPaused)
    {
        string gameOverText = "Game Over!";
        string finalScoreText = "Final Score: " + to_string(G.score);

        // Afficher "Game Over" en gros
        G2D::drawStringFontMono(V2(G.WidthPix / 2 - 150, G.HeighPix / 2 - 50), gameOverText, 50, 5, Color::Red);

        // Afficher le score final juste en dessous
        G2D::drawStringFontMono(V2(G.WidthPix / 2 - 160, G.HeighPix / 2 + 50), finalScoreText, 30, 3, Color::Yellow);

        // Ne pas afficher les éléments du jeu quand le jeu est en pause
        G2D::Show();
        return;
    }

    for (int i = 0; i < G.WidthPix / 32 + 1; ++i)  // Dessiner sur toute la largeur
    {
        for (int j = 0; j < G.HeighPix / 32 + 1; ++j)  // Dessiner sur toute la hauteur
        {
            // Dessine le fond à la position (i*32, j*32)
            G2D::drawRectWithTexture(G.Idsprite_background, V2(i * 32, j * 32), V2(32, 32));
        }
    }

    string scoreText = "Score: " + to_string(G.score);
    G2D::drawStringFontMono(V2(10, 10), scoreText, 20, 3, Color::White);

    // Dessine la raquette
    int idsprite = G.X.IdSpriteNormal;
    G2D::drawRectWithTexture(idsprite, G.X.pos, G.X.size);

    // Dessine la balle
    int idsprite_ball = G.aBall.IdSprite;
    G2D::drawRectWithTexture(idsprite_ball, G.aBall.BallPos, G.aBall.size);

    for (const Brique& brique : G.briques)
    {
        if (!brique.isDestroyed)
        {
            int idsprite_brique = brique.IdSprite;
            G2D::drawRectWithTexture(idsprite_brique, brique.pos, brique.size);
        }
    }

    G2D::Show();
}

///////////////////////////////////////////////////////////////////////////////
//      Gestion de la logique du jeu - reçoit en paramètre les données du jeu par référence

void Logic(GameData& G)
{
    int speed = 12;
    if (G2D::isKeyPressed(Key::Q))  G.X.pos.x -= speed;
    if (G2D::isKeyPressed(Key::D)) G.X.pos.x += speed;

    // Limites de la raquette
    if (G.X.pos.x < 0) G.X.pos.x = 0;
    if (G.X.pos.x + G.X.size.x > G.WidthPix) G.X.pos.x = G.WidthPix - G.X.size.x;
    if (G.X.pos.y < 0) G.X.pos.y = 0;
    if (G.X.pos.y + G.X.size.y > G.HeighPix) G.X.pos.y = G.HeighPix - G.X.size.y;

    // futur deplacement de la balle
    V2 futur_pos = G.aBall.BallPos + G.aBall.BallMove;

    // Gestion des collisions avec les murs
    for (int i = 0; i <= G.LP.size() - 2; i++)
    {
        int collisionType = CollisionSegCir(G.LP[i], G.LP[i + 1], G.aBall.BallRadius, futur_pos);

        if (collisionType == 1)
        {
            G.aBall.BallMove = Rebond(G.aBall.BallMove, G.LP[i] - G.LP[i + 1]);
        }
        else if (collisionType == 2)
        {
            V2 AB = G.LP[i + 1] - G.LP[i];
            V2 AB_normal = V2(AB.y, -AB.x);
            G.aBall.BallMove = Rebond(G.aBall.BallMove, AB_normal);
        }
        else if (collisionType == 3)
        {
            G.aBall.BallMove = Rebond(G.aBall.BallMove, G.LP[i + 1] - G.LP[i]);
        }
    }

    // Collision avec les briques
    for (Brique& brique : G.briques) {
        if (!brique.isDestroyed) {
            for (int i = 0; i < brique.hitbox.size(); i++) {
                V2 A = brique.hitbox[i];
                V2 B = brique.hitbox[(i + 1) % brique.hitbox.size()];
                int collisionType = CollisionSegCir(A, B, G.aBall.BallRadius, futur_pos);
                if (collisionType == 1 ) { 
                    G.aBall.BallMove = Rebond(G.aBall.BallMove, A - B);
                    brique.life -= 1;
                    if (brique.life == 0)
                    {
                        brique.isDestroyed = true;
                        G.score += 10;
                    }                    
                    brique.InitTextures(".//assets//brique2.png");
                    break;
                }
                else if (collisionType == 2) {
                    V2 AB = B - A;
                    V2 AB_normal = V2(AB.y, -AB.x);  
                    G.aBall.BallMove = Rebond(G.aBall.BallMove, AB_normal);
                    brique.life -= 1;
                    if (brique.life == 0)
                    {
                        brique.isDestroyed = true;
                        G.score += 10;
                    }     
                    brique.InitTextures(".//assets//brique2.png");
                    break;
                }
                else if (collisionType == 3) { 
                    G.aBall.BallMove = Rebond(G.aBall.BallMove, B - A);
                    brique.life -= 1;
                    if (brique.life == 0)
                    {
                        brique.isDestroyed = true;
                        G.score += 10;
                    }
                    brique.InitTextures(".//assets//brique2.png");
                    break;
                }
            }
        }
    }


    // Collision avec la raquette
   // Collision avec la raquette
    for (int i = 0; i < 4; ++i) {  // Vérifier chaque côté de la raquette
        V2 A, B;
        if (i == 0) { // Côté supérieur
            A = G.X.pos;
            B = G.X.pos + V2(G.X.size.x, 0);
        }
        else if (i == 1) { // Côté droit
            A = G.X.pos + V2(G.X.size.x, 0);
            B = G.X.pos + G.X.size;
        }
        else if (i == 2) { // Côté inférieur
            A = G.X.pos + G.X.size;
            B = G.X.pos + V2(0, G.X.size.y);
        }
        else { // Côté gauche
            A = G.X.pos + V2(0, G.X.size.y);
            B = G.X.pos;
        }

        // Tester la collision entre la balle et ce segment de la raquette
        int collisionType = CollisionSegCir(A, B, G.aBall.BallRadius, futur_pos);

        if (collisionType == 1) { // Collision avec le point A
            G.aBall.BallMove = Rebond(G.aBall.BallMove, A - B);
        }
        else if (collisionType == 2) {
            if (i == 0) { // si collision avec le haut de la raquette
                // Calcul de l'angle personnalisé
                float relativeIntersectX = G.aBall.BallPos.x - (G.X.pos.x + G.X.size.x / 2);
                float normalizedRelativeIntersectionX = relativeIntersectX / (G.X.size.x / 2);
                float bounceAngle = normalizedRelativeIntersectionX * (75.0f * 3.14159f / 180.0f); // 75° max

                float speed = G.aBall.BallMove.norm();
                G.aBall.BallMove.x = speed * sin(bounceAngle);
                G.aBall.BallMove.y = speed * cos(bounceAngle);

                // s'assurer que la balle rebondit vers le haut
                if (G.aBall.BallMove.y > 0) G.aBall.BallMove.y = -G.aBall.BallMove.y;
            }
            else {
                // collision normale pour les autres côtés
                V2 AB = B - A;
                V2 AB_normal = V2(AB.y, -AB.x);
                G.aBall.BallMove = Rebond(G.aBall.BallMove, AB_normal);
            }
        }
        else if (collisionType == 3) { // Collision avec le point B
            G.aBall.BallMove = Rebond(G.aBall.BallMove, B - A);
        }
    }
    // Vérifier si toutes les briques ont été détruites
    if (AllBriquesDestroyed(G))
    {
        ResetLevel(G);  // Réinitialiser le niveau et augmenter la vitesse de la balle
    }

    // Mise à jour de la position de la balle
    G.aBall.BallPos = G.aBall.BallPos + G.aBall.BallMove;
    G.aBall.PreviousPos.push_back(G.aBall.BallPos);
    G.aBall.PreviousPos.erase(G.aBall.PreviousPos.begin());

    // Vérification si la balle sort par le bas
    if (G.aBall.BallPos.y + G.aBall.BallRadius < 0)
    {
        G.isPaused = true;  // Le jeu est mis en pause si la balle sort
    }
}


///////////////////////////////////////////////////////////////////////////////
//        Démarrage de l'application

int main(int argc, char* argv[])
{
    GameData G;   // instanciation de l'unique objet GameData qui sera passé aux fonctions render et logic

    // crée la fenêtre de l'application
    G2D::initWindow(V2(G.WidthPix, G.HeighPix), V2(20, 20), string("G2D DEMO"));


    int startX = (G.WidthPix - (5 * 110)) / 2; 
    int startY = 600;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            G.briques.push_back(Brique(V2(startX + i * 110, startY + j * 40),1));
        }
    }


    // Initialisation des textures pour la raquette, la balle et les briques
    G.InitTextures();
    G.X.InitTextures();
    G.aBall.InitTextures();
    for (Brique& brique : G.briques)
    {
        brique.InitTextures();
    }

    // nombre de fois où la fonction Logic est appelée par seconde
    int callToLogicPerSec = 50;

    // lance l'application en spécifiant les deux fonctions utilisées et l'instance de GameData
    G2D::Run(Logic, Render, G, callToLogicPerSec, true);

    // aucun code ici
}
