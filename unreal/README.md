# SOLSTÍCIO 3D (Unreal Engine 5.8, C++)

Esqueleto do projeto 3D. Linguagem: **C++** (com Blueprints para níveis, UI e animações).

## Como abrir
1. Instala o Unreal Engine 5.8 e o Visual Studio 2022 (workload "Game development with C++").
2. Botão direito em `Solsticio.uproject` → *Generate Visual Studio project files*.
3. Compila `SolsticioEditor` (Development Editor) e abre o projeto.
4. Cria os mapas `Content/Maps/Mission01` … `Mission12` (um por missão; nomes usados em `Content/Data/missions.json`).

## O que já existe
- `SolsticioGameInstance`: carrega `missions.json`, guarda missão atual e ligação Tim–Íris, escolhe o final.
- `SolsticioArmorCharacter`: personagem em 3.ª pessoa com energia solar, propulsão (Espaço) e salto temporal (E).
- `SolsticioGameMode`: usa a armadura como pawn por defeito.
- Primeira pessoa dentro da armadura, 12 missões com todos os diálogos (`Content/Data/campaign.json`, gerado de `game/story.js`).
- Controlos: WASD + rato, Espaço propulsão, E salto temporal, Enter/clique avançar, 1–9 escolhas e sequências, Backspace desfaz, F9 salta missão.

## Próximos passos
Mapas e modelos 3D, UI de diálogos (UMG) a ler `Intro`, mini-jogos de cada missão, áudio e cutscenes (Sequencer). A versão web em `../game` serve de referência de história e mecânicas.


