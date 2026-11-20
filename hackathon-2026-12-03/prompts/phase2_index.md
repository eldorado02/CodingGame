Tu es le sous-agent chargé de construire la LISTE CHRONOLOGIQUE des challenges CodinGame 2016 → 2026 qui serviront à la préparation du hackathon. Réponds en FRANÇAIS.

À FAIRE :
1. Réunir la liste la plus complète possible des challenges CodinGame (événements saisonniers : Spring/Summer/Fall/Winter Challenge, et challenges permanents notables) de 2016 à 2026 inclus. Sources possibles : API CodinGame (`https://www.codingame.com/api/v1/challenges` — ne renvoie que 20 résultats et ignore limit/offset, il faut compléter par le web), pages CodinGame, GitHub, wikis communautaires. Utilise webfetch / DuckDuckGo lite si besoin.
2. Pour chaque challenge : année, identifiant `pretty_id` utilisable dans l'API CodinGame, nom lisible. Trier par année croissante puis par date de début.
3. Vérifier autant que possible que le `pretty_id` est valide (ex. `fall-challenge-2026`, `summer-challenge-2026-back-track-king`).
4. Ne pas inclure les jeux purement « archive/puzzle classique » si ce ne sont pas des challenges de bot programming ou des défis algorithmiques pertinents pour un hackathon.

FICHIER À ÉCRIRE (chemin absolu) :
/home/arafaram/Projects/codingGame/hackathon-2026-12-03/challenges.json
Format JSON strict :
[
  {"year": 2016, "id": "spring-challenge-2016", "name": "Spring Challenge 2016", "start": "2016-05-..."},
  ...
]
Le fichier doit être du JSON valide, avec au moins 8 challenges couvrant 2016 à 2026. Ne modifie aucun autre fichier. Ne lance pas d'autre sous-agent. Ne lis jamais le token CodinGame.
