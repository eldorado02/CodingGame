Tu es le sous-agent d'IMPLÉMENTATION du challenge CODINGAME {YEAR} — {CHALLENGE_NAME} (`{CHALLENGE_ID}`). Réponds et commente en FRANÇAIS.

BASE : lis statement.md, research.md, plan.md et critique.md dans /home/arafaram/Projects/codingGame/hackathon-2026-12-03/{CHALLENGE_DIR}/ , ainsi que le socle DSA dans /home/arafaram/Projects/codingGame/hackathon-2026-12-03/dsa/ . Applique les recommandations de la critique.

À FAIRE : écrire 4 implémentations COMPLÈTES et IDIOMATIQUES, chacune selon les conventions de son langage (pas de traduction mot à mot) :
- C : structs + fonctions, gestion explicite des erreurs et de la mémoire, compilation avec `gcc -O2 -std=c11`.
- C++ : STL, RAII, `const`, types forts, compilation avec `g++ -O2 -std=c++20`.
- Rust : ownership, `Result`, erreurs typées, Cargo.toml, `cargo build --release`.
- Python : PEP 8, `typing`, `if __name__ == "__main__"`, Python 3.11+.

Chaque solution doit lire exactement les entrées de l'énoncé depuis stdin et écrire exactement les sorties attendues sur stdout. Réutiliser le socle DSA (copier localement le code nécessaire si l'import n'est pas praticable en Compilation CodinGame, en gardant la même qualité). Ajouter un petit jeu de tests locaux.

FICHIERS À ÉCRIRE (chemins absolus, sous /home/arafaram/Projects/codingGame/hackathon-2026-12-03/{CHALLENGE_DIR}/) :
- solutions/main.c
- solutions/main.cpp
- solutions/main.rs (+ Cargo.toml si besoin)
- solutions/main.py
- tests/README.md (comment lancer les tests + cas couverts)

Vérifie que chaque fichier est non vide et syntaxiquement valide. Ne modifie aucun autre fichier. Ne lance pas d'autre sous-agent. Ne soumets rien à CodinGame. Ne lis jamais le token.
