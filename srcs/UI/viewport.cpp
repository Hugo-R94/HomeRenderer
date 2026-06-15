// #include "HomeRenderer.hpp"
// #include <imgui.h>
// #include <GL/gl.h>
// #include <cstdint>
// #include <iostream>

// // ============================================================
// // CONSTANTS & UTILITIES
// // ============================================================

// namespace
// {
//     // Constantes pour la disposition des panneaux
//     constexpr float PANEL_WIDTH_RATIO = 1.0f / 9.6f;  // ~10.4% de largeur écran
//     constexpr float VIEWPORT_MIN_SPLIT = 0.1f;        // Split minimum 10%
//     constexpr float VIEWPORT_MAX_SPLIT = 0.9f;        // Split maximum 90%
//     constexpr float BG_CLEAR_R = 0.12f;               // Teinte gris-bleu
//     constexpr float BG_CLEAR_G = 0.30f;
//     constexpr float BG_CLEAR_B = 0.40f;
//     constexpr float BG_CLEAR_A = 1.00f;

//     /**
//      * @brief Clamp une valeur entre min et max
//      */
//     inline float clampf(float v, float min, float max)
//     {
//         return (v < min) ? min : (v > max) ? max : v;
//     }
// }

// // ============================================================
// // GEOMETRY CREATION
// // ============================================================

// /**
//  * @brief Crée un carré 2D centré à une position donnée
//  * 
//  * @param size Taille du carré (largeur/hauteur)
//  * @param center Position du centre
//  * @return unique_ptr vers le carré créé
//  */
// std::unique_ptr<Square> create_square(float size, Vec2 center)
// {
//     auto square = std::make_unique<Square>();
    
//     float half_size = size / 2.0f;
//     square->vertex1 = Vec2(center.x - half_size, center.y - half_size);
//     square->vertex2 = Vec2(center.x - half_size, center.y + half_size);
//     square->vertex3 = Vec2(center.x + half_size, center.y + half_size);
//     square->vertex4 = Vec2(center.x + half_size, center.y - half_size);
    
//     return square;
// }

// // ============================================================
// // RENDERING
// // ============================================================

// /**
//  * @brief Dessine un carré avec dégradé de couleurs
//  * 
//  * @param square Référence au carré à dessiner
//  * @param center Position du centre (pour debug)
//  */
// void draw_square_rotate(const Square& square, Vec2 center)
// {
//     // Supprime avertissement paramètre inutilisé
//     (void)center;
    
//     glBegin(GL_QUADS);
    
//     // Sommet 1 - Orange
//     glColor3f(1.0f, 0.4f, 0.2f);
//     glVertex2f(square.vertex1.x, square.vertex1.y);
    
//     // Sommet 2 - Vert
//     glColor3f(0.0f, 0.4f, 0.2f);
//     glVertex2f(square.vertex2.x, square.vertex2.y);
    
//     // Sommet 3 - Bleu foncé
//     glColor3f(0.0f, 0.4f, 0.0f);
//     glVertex2f(square.vertex3.x, square.vertex3.y);
    
//     // Sommet 4 - Bleu clair
//     glColor3f(0.2f, 0.4f, 0.8f);
//     glVertex2f(square.vertex4.x, square.vertex4.y);
    
//     glEnd();  // ✅ Fermer la primitive
// }
// /**
//  * @brief Rendu simple du viewport principal
//  * 
//  * Initialise la projection orthographique et dessine un carré
//  */
// void render_viewport_simple()
// {
//     // Configuration de la projection
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();
    
//     // Créer et dessiner un carré
//     auto square = create_square(0.2f, {0.0f, 0.0f});
//     draw_square_rotate(*square, {0.0f, 0.0f});
    
//     // square est automatiquement libéré (unique_ptr)
// }

// /**
//  * @brief Redimensionne le FBO si les dimensions ont changé
//  * 
//  * NOTE: Cette fonction utilise glBindFramebuffer qui provient de GLAD.
//  *       Elle est commentée car le FBO n'est pas initialisé actuellement.
//  * 
//  * @param vp Référence au viewport à redimensionner
//  * @param w Nouvelle largeur
//  * @param h Nouvelle hauteur
//  */
// void resize_if_needed(ViewportRender& vp, int w, int h)
// {
//     if (vp.width == w && vp.height == h)
//         return;  // Pas de changement
    
//     vp.width = w;
//     vp.height = h;
    
//     // Configurer la texture
//     glBindTexture(GL_TEXTURE_2D, vp.texture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//     // Attacher au framebuffer
//     // NOTE: glBindFramebuffer provient de GLAD
//     // glBindFramebuffer(GL_FRAMEBUFFER, vp.fbo);
//     // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vp.texture, 0);
//     // glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }

// /**
//  * @brief Rendu offscreen via FBO
//  * 
//  * NOTE: Cette fonction n'est pas utilisée actuellement car le FBO
//  *       n'est pas initialisé. À activer quand le rendu offscreen sera implémenté.
//  * 
//  * @param vp Référence au ViewportRender contenant le FBO
//  */
// void render_viewport(ViewportRender& vp)
// {
//     if (vp.fbo == 0)
//         return;  // FBO non initialisé
    
//     // NOTE: glBindFramebuffer provient de GLAD
//     // glBindFramebuffer(GL_FRAMEBUFFER, vp.fbo);
//     // glViewport(0, 0, vp.width, vp.height);
//     // glClearColor(BG_CLEAR_R, BG_CLEAR_G, BG_CLEAR_B, BG_CLEAR_A);
//     // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
//     // TODO: Code de rendu de la scène 3D ici
//     // render_viewport_simple();
    
//     // glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }

// /**
//  * @brief Affiche un seul viewport (hérité, peut être obsolète)
//  * 
//  * @param data Référence aux données application
//  */
// void display_viewport(Data& data)
// {
//     ImGuiViewport* vp = ImGui::GetMainViewport();

//     // Position et taille du viewport
//     if (data.is_outliner_visible)
//     {
//         ImGui::SetNextWindowPos(ImVec2(vp->Size.x * PANEL_WIDTH_RATIO, 20), ImGuiCond_Always);
//         ImGui::SetNextWindowSize(
//             ImVec2((vp->Size.x - (vp->Size.x * PANEL_WIDTH_RATIO * 2)) / 2, vp->Size.y - 20),
//             ImGuiCond_Always
//         );
//     }
//     else
//     {
//         ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
//         ImGui::SetNextWindowSize(
//             ImVec2(vp->Size.x / 2, vp->Size.y - 20),
//             ImGuiCond_Always
//         );
//     }

//     ImGui::Begin("Viewport", nullptr,
//                  ImGuiWindowFlags_NoCollapse |
//                  ImGuiWindowFlags_NoMove |
//                  ImGuiWindowFlags_NoResize);

//     ImVec2 size = ImGui::GetContentRegionAvail();
//     glViewport(1, 0, (int)size.x, (int)size.y);
//     glClearColor(BG_CLEAR_R, BG_CLEAR_G, BG_CLEAR_B, BG_CLEAR_A);
//     glClear(GL_COLOR_BUFFER_BIT);

//     ImGui::Image((void*)(intptr_t)0, size);
//     ImGui::End();
// }


// /**
//  * @brief Affiche deux viewports côte à côte avec splitter redimensionnable
//  * 
//  * Layout:
//  * ┌──────────────────────────────────┐
//  * │ Viewport 1 │ Splitter │ Viewport 2 │
//  * └──────────────────────────────────┘
//  * 
//  * @param data Référence aux données application
//  */
// void display_double_viewport(Data& data)
// {
//     ImGuiViewport* vp = ImGui::GetMainViewport();
    
//     // Calcul des dimensions
//     float screen_w = vp->Size.x;
//     float screen_h = vp->Size.y - 20.0f;
//     float offset_x = data.is_outliner_visible ? screen_w * PANEL_WIDTH_RATIO : 0.0f;
//     float width_available = screen_w - 2.0f * offset_x;
    
//     // Position de référence
//     ImVec2 pos(offset_x, 20.0f);
    
//     // ========== SPLITTER STATE ==========
//     static float split = 0.7f;  // 70% pour VP1, 30% pour VP2
//     split = clampf(split, VIEWPORT_MIN_SPLIT, VIEWPORT_MAX_SPLIT);
    
//     const float SPLITTER_WIDTH = 4.0f;
//     float width_vp1 = width_available * split;
//     float width_vp2 = width_available - width_vp1 - SPLITTER_WIDTH;
    
//     // ========== VIEWPORT 1 ==========
//     ImGui::SetNextWindowPos(pos);
//     ImGui::SetNextWindowSize(ImVec2(width_vp1, screen_h));
//     ImGui::Begin("Viewport 1", nullptr,
//                  ImGuiWindowFlags_NoMove |
//                  ImGuiWindowFlags_NoResize |
//                  ImGuiWindowFlags_NoCollapse |
//                  ImGuiWindowFlags_NoScrollbar |
//                  ImGuiWindowFlags_NoScrollWithMouse);
    
//     ImVec2 avail1 = ImGui::GetContentRegionAvail();
    
//     // Callback OpenGL pour rendu personnalisé
//     ImDrawList* draw_list = ImGui::GetWindowDrawList();
//     draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd*) {
//         glPushAttrib(GL_ALL_ATTRIB_BITS);
//         glDisable(GL_SCISSOR_TEST);
//         render_viewport_simple();
//         glPopAttrib();
//     }, nullptr);
    
//     // Réserver l'espace
//     draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
//     ImGui::Dummy(avail1);
    
//     ImGui::End();
    
//     // ========== SPLITTER (Draggable) ==========
//     ImGui::SetNextWindowPos(ImVec2(pos.x + width_vp1, 20.0f));
//     ImGui::SetNextWindowSize(ImVec2(SPLITTER_WIDTH, screen_h));
//     ImGui::Begin("##splitter", nullptr,
//                  ImGuiWindowFlags_NoMove |
//                  ImGuiWindowFlags_NoCollapse |
//                  ImGuiWindowFlags_NoScrollbar |
//                  ImGuiWindowFlags_NoScrollWithMouse |
//                  ImGuiWindowFlags_NoTitleBar |
//                  ImGuiWindowFlags_AlwaysAutoResize);
    
//     // Splitter button transparent
//     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
//     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
    
//     if (ImGui::Button("##drag", ImVec2(SPLITTER_WIDTH, screen_h - 8)))
//     {
//         // Le bouton recevra le drag automatiquement
//     }
    
//     // Gestion du drag
//     if (ImGui::IsItemActive())
//     {
//         float delta = ImGui::GetIO().MouseDelta.x;
//         split += delta / width_available;
//         split = clampf(split, VIEWPORT_MIN_SPLIT, VIEWPORT_MAX_SPLIT);
//     }
    
//     ImGui::PopStyleColor(2);
//     ImGui::End();
    
//     // ========== VIEWPORT 2 ==========
//     ImGui::SetNextWindowPos(ImVec2(pos.x + width_vp1 + SPLITTER_WIDTH, 20.0f));
//     ImGui::SetNextWindowSize(ImVec2(width_vp2, screen_h));
//     ImGui::Begin("Viewport 2", nullptr,
//                  ImGuiWindowFlags_NoMove |
//                  ImGuiWindowFlags_NoResize |
//                  ImGuiWindowFlags_NoCollapse |
//                  ImGuiWindowFlags_NoScrollbar |
//                  ImGuiWindowFlags_NoScrollWithMouse);
    
//     ImVec2 avail2 = ImGui::GetContentRegionAvail();
    
//     // Callback OpenGL pour VP2 (actuellement vide, à implémenter)
//     draw_list = ImGui::GetWindowDrawList();
//     draw_list->AddCallback([](const ImDrawList*, const ImDrawCmd*) {
//         // TODO: Implémenter rendu VP2
//         glClearColor(BG_CLEAR_R * 0.8f, BG_CLEAR_G * 0.8f, BG_CLEAR_B * 0.8f, BG_CLEAR_A);
//         glClear(GL_COLOR_BUFFER_BIT);
//     }, nullptr);
    
//     draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
//     ImGui::Dummy(avail2);
    
//     ImGui::End();
// }