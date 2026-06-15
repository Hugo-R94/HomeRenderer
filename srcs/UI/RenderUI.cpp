// #include "HomeRenderer.hpp"

// /**
//  * @brief Orchestre le rendu de l'interface utilisateur complète
//  * 
//  * Layout fixe:
//  * ┌──────────────────────────────────────┐
//  * │ Menu Bar                             │
//  * ├─────┬──────────────┬────────────┬────┤
//  * │     │              │            │    │
//  * │ Out │  Viewport 1  │ Viewport 2 │Prop│
//  * │liner│              │            │ery │
//  * │     │              │            │    │
//  * └─────┴──────────────┴────────────┴────┘
//  * 
//  * @param window Pointeur vers la fenêtre GLFW (pour fermeture/événements)
//  * @param data Référence aux données application
//  */
// void RenderUI(GLFWwindow* window, Data& data)
// {
//     // Rendu des panneaux dans l'ordre: menu → outliner → properties → viewports
//     render_BarMenu(window);
//     render_outliner(data);
//     render_proprieties(data);
//     display_double_viewport(data);
// }